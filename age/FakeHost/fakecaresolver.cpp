#include "pch.h"
#include "fakecaresolver.h"
#include "certreader.h"
#include "hostreader.h"
#include <array>

// The game's HTTP layer (rlink) is libcurl on the Schannel backend, which does
// not support CURLOPT_SSL_CTX_FUNCTION, so server certificates fall back to
// Schannel's automatic validation against the system stores - and the LAN
// server's CA is not there. Forcing SCH_CRED_MANUAL_CRED_VALIDATION at
// AcquireCredentialsHandle time stops auto-validation, so the self-signed LAN
// server certificate is accepted. Nothing outside the process is modified.
//
// WinHTTP (libHttpClient, Xbox Live/PlayFab) validates inside the handshake in
// WinHttpSendRequest; the in-process lever is WINHTTP_OPTION_SECURITY_FLAGS,
// set on the request handle right before the handshake - only for hosts in
// --overrideHosts (or the loopback the game rewrites them to); every other
// host keeps full validation.
//
// Security note: while loaded, certificates signed by the override cert are
// not verified and Schannel-side validation is off for the game process. Keep
// real validation by installing the CA in the OS store and not injecting this
// DLL.

typedef SECURITY_STATUS(SEC_ENTRY* PFN_AcquireCredentialsHandleW)(
    LPWSTR pszPrincipal, LPWSTR pszPackage, unsigned long fCredentialUse,
    void* pvLogonId, void* pAuthData, SEC_GET_KEY_FN pGetKeyFn,
    void* pvGetKeyArgument, PCredHandle phCredential, PTimeStamp ptsExpiry);
typedef SECURITY_STATUS(SEC_ENTRY* PFN_AcquireCredentialsHandleA)(
    LPSTR pszPrincipal, LPSTR pszPackage, unsigned long fCredentialUse,
    void* pvLogonId, void* pAuthData, SEC_GET_KEY_FN pGetKeyFn,
    void* pvGetKeyArgument, PCredHandle phCredential, PTimeStamp ptsExpiry);

static PFN_AcquireCredentialsHandleW Real_AcquireCredentialsHandleW = nullptr;
static PFN_AcquireCredentialsHandleA Real_AcquireCredentialsHandleA = nullptr;

typedef BOOL(WINAPI* PFN_CertVerifyCertificateChainPolicy)(
    LPCSTR pszPolicyOID,
    PCCERT_CHAIN_CONTEXT pChainContext,
    PCERT_CHAIN_POLICY_PARA pPolicyPara,
    PCERT_CHAIN_POLICY_STATUS pPolicyStatus);

static PFN_CertVerifyCertificateChainPolicy Real_CertVerifyCertificateChainPolicy = nullptr;

typedef BOOL(WINAPI* PFN_WinHttpSendRequest)(
    HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional,
    DWORD dwOptionalLength, DWORD dwTotalLength, DWORD_PTR dwContext);
typedef BOOL(WINAPI* PFN_WinHttpSetOption)(
    HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength);
typedef BOOL(WINAPI* PFN_WinHttpQueryOption)(
    HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer, LPDWORD lpdwBufferLength);
typedef BOOL(WINAPI* PFN_WinHttpCrackUrl)(
    LPCWSTR pszUrl, DWORD dwUrlLength, DWORD dwFlags, LPURL_COMPONENTSW lpUrlComponents);

static PFN_WinHttpSendRequest Real_WinHttpSendRequest = nullptr;
static PFN_WinHttpSetOption Real_WinHttpSetOption = nullptr;
static PFN_WinHttpQueryOption Real_WinHttpQueryOption = nullptr;
static PFN_WinHttpCrackUrl Real_WinHttpCrackUrl = nullptr;

static bool ResolveFunctions() {
    // sspicli may not be loaded yet at DLL_PROCESS_ATTACH; load it so the
    // detour targets resolve.
    HMODULE sspi = LoadLibraryW(L"sspicli.dll");
    if (sspi == nullptr) {
        return false;
    }
    Real_AcquireCredentialsHandleW = reinterpret_cast<PFN_AcquireCredentialsHandleW>(
        GetProcAddress(sspi, "AcquireCredentialsHandleW"));
    Real_AcquireCredentialsHandleA = reinterpret_cast<PFN_AcquireCredentialsHandleA>(
        GetProcAddress(sspi, "AcquireCredentialsHandleA"));
    if (Real_AcquireCredentialsHandleW == nullptr || Real_AcquireCredentialsHandleA == nullptr) {
        return false;
    }
    HMODULE crypt32 = GetModuleHandleW(L"crypt32.dll");
    if (crypt32 == nullptr) {
        crypt32 = LoadLibraryW(L"crypt32.dll");
    }
    if (crypt32 == nullptr) {
        return false;
    }
    Real_CertVerifyCertificateChainPolicy = reinterpret_cast<PFN_CertVerifyCertificateChainPolicy>(
        GetProcAddress(crypt32, "CertVerifyCertificateChainPolicy"));
    if (Real_CertVerifyCertificateChainPolicy == nullptr) {
        return false;
    }
    HMODULE winhttp = GetModuleHandleW(L"winhttp.dll");
    if (winhttp == nullptr) {
        winhttp = LoadLibraryW(L"winhttp.dll");
    }
    if (winhttp == nullptr) {
        return false;
    }
    Real_WinHttpSendRequest = reinterpret_cast<PFN_WinHttpSendRequest>(
        GetProcAddress(winhttp, "WinHttpSendRequest"));
    Real_WinHttpSetOption = reinterpret_cast<PFN_WinHttpSetOption>(
        GetProcAddress(winhttp, "WinHttpSetOption"));
    Real_WinHttpQueryOption = reinterpret_cast<PFN_WinHttpQueryOption>(
        GetProcAddress(winhttp, "WinHttpQueryOption"));
    Real_WinHttpCrackUrl = reinterpret_cast<PFN_WinHttpCrackUrl>(
        GetProcAddress(winhttp, "WinHttpCrackUrl"));
    return Real_WinHttpSendRequest != nullptr && Real_WinHttpSetOption != nullptr
        && Real_WinHttpQueryOption != nullptr && Real_WinHttpCrackUrl != nullptr;
}

namespace {
    // The game rewrites redirected service URLs to a loopback address in
    // 127.0.0.0/8 (observed: 127.1.33.7), so match the whole range - it is the
    // local machine by definition.
    bool IsLoopbackHost(const std::wstring& host) noexcept {
        return host.rfind(L"127.", 0) == 0 || host == L"::1" || host == L"localhost"
            || host == L"[::1]";
    }

    // True when the host is one of the --overrideHosts entries (HostIpMap keys
    // are stored lowercase) or a loopback rewrite.
    bool IsOverrideHost(const std::wstring& host) noexcept {
        if (HostIpMap.empty()) {
            // HostIpMap is empty, meaning override any hosts.
            return true;
        }
        std::wstring lower(host);
        std::transform(lower.begin(), lower.end(), lower.begin(),
            [](wchar_t c) { return static_cast<wchar_t>(::towlower(c)); });
        std::string narrow;
        narrow.reserve(lower.size());
        for (wchar_t c : lower) {
            narrow.push_back(static_cast<char>(c));
        }
        return HostIpMap.find(narrow) != HostIpMap.end() || IsLoopbackHost(host);
    }

    // Applies the security ignore flags to a request whose URL points at an
    // override host (or its loopback rewrite).
    void BlessRequestHandle(HINTERNET hRequest) {
        wchar_t url[2048] = L"";
        DWORD urlLen = sizeof(url);
        if (Real_WinHttpQueryOption == nullptr
            || Real_WinHttpQueryOption(hRequest, WINHTTP_OPTION_URL, url, &urlLen) == FALSE) {
            return;
        }
        // WinHttpQueryOption reports lengths in bytes and does not guarantee a
        // null terminator when the buffer fills exactly; enforce one before
        // WinHttpCrackUrl reads the string.
        url[ARRAYSIZE(url) - 1] = L'\0';
        URL_COMPONENTSW parts = {};
        parts.dwStructSize = sizeof(parts);
        wchar_t host[256] = L"";
        parts.lpszHostName = host;
        parts.dwHostNameLength = ARRAYSIZE(host);
        if (Real_WinHttpCrackUrl == nullptr
            || Real_WinHttpCrackUrl(url, 0, 0, &parts) == FALSE
            || parts.lpszHostName == nullptr
            || parts.dwHostNameLength == 0) {
            return;
        }
        std::wstring hostName(parts.lpszHostName, parts.dwHostNameLength);
        if (!IsOverrideHost(hostName)) {
            return;
        }
        DWORD flags = 0;
        DWORD flagsSize = sizeof(flags);
        if (Real_WinHttpQueryOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &flags, &flagsSize) == FALSE) {
            flags = 0;
        }
        flags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
        Real_WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &flags, sizeof(flags));
    }
}

// The TLS handshake happens inside WinHttpSendRequest; set the flags right
// before calling through.
static BOOL WINAPI Mine_WinHttpSendRequest(
    HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional,
    DWORD dwOptionalLength, DWORD dwTotalLength, DWORD_PTR dwContext) noexcept {
    BlessRequestHandle(hRequest);
    return Real_WinHttpSendRequest(
        hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength,
        dwTotalLength, dwContext);
}

namespace {
    // The SDK declares SCH_CREDENTIALS only under SCHANNEL_USE_BLACKLISTS;
    // mirror the documented v5 layout (only dwVersion/dwFlags are accessed).
    struct SCH_CREDENTIALS_MIRROR {
        DWORD dwVersion;
        DWORD dwCredFormat;
        DWORD cCreds;
        PCCERT_CONTEXT* paCred;
        HCERTSTORE hRootStore;
        DWORD cMappers;
        struct _HMAPPER** aphMappers;
        DWORD dwSessionLifespan;
        DWORD dwFlags;
        DWORD cTlsParameters;
        void* pTlsParameters;
    };

    // Matches Schannel credential packages only; other packages must stay
    // untouched.
    bool IsSchannelPackage(const wchar_t* package) {
        return package != nullptr
            && (_wcsicmp(package, L"Microsoft Unified Security Protocol Provider") == 0
                || _wcsicmp(package, L"Microsoft Schannel") == 0
                || _wcsnicmp(package, L"TLS", 3) == 0);
    }

    // Switches Schannel client credentials from automatic to manual certificate
    // validation. SCHANNEL_CRED and SCH_CREDENTIALS share the dwFlags semantics.
    void RelaxCredentialValidation(void* pAuthData, const wchar_t* package) {
        if (pAuthData == nullptr || !IsSchannelPackage(package)) {
            return;
        }
        DWORD version = *static_cast<DWORD*>(pAuthData);
        DWORD* flags = nullptr;
        if (version == SCHANNEL_CRED_VERSION) {
            flags = &static_cast<SCHANNEL_CRED*>(pAuthData)->dwFlags;
        }
        else if (version == SCH_CREDENTIALS_VERSION) {
            flags = &static_cast<SCH_CREDENTIALS_MIRROR*>(pAuthData)->dwFlags;
        }
        else {
            return;
        }
        *flags = (*flags | SCH_CRED_MANUAL_CRED_VALIDATION) & ~SCH_CRED_AUTO_CRED_VALIDATION;
    }

    // True when the cert's SHA-256 thumbprint matches one of the
    // --overrideCerts certificates. Pinning the thumbprint ties trust to the
    // exact cert bytes we shipped, not to forgeable issuer/subject strings.
    bool IsOverrideCert(PCCERT_CONTEXT cert) {
        if (cert == nullptr) {
            return false;
        }
        CertThumbprint thumbprint{};
        DWORD length = static_cast<DWORD>(thumbprint.size());
        if (CertGetCertificateContextProperty(cert, CERT_SHA256_HASH_PROP_ID,
                thumbprint.data(), &length) == FALSE || length != thumbprint.size()) {
            return false;
        }
        for (const auto& pinned : CertThumbprintList) {
            if (pinned == thumbprint) {
                return true;
            }
        }
        return false;
    }

    // Scoped pass-through: accept a failing chain only when its leaf or root
    // matches an override certificate. Internet certificates keep full
    // validation.
    BOOL WINAPI Mine_CertVerifyCertificateChainPolicy(
        LPCSTR pszPolicyOID,
        PCCERT_CHAIN_CONTEXT pChainContext,
        PCERT_CHAIN_POLICY_PARA pPolicyPara,
        PCERT_CHAIN_POLICY_STATUS pPolicyStatus) {
        BOOL result = Real_CertVerifyCertificateChainPolicy(
            pszPolicyOID, pChainContext, pPolicyPara, pPolicyStatus);
        if (result != FALSE || pPolicyStatus == nullptr
            || pPolicyStatus->dwError == S_OK
            || pChainContext == nullptr || pChainContext->cChain == 0
            || pChainContext->rgpChain == nullptr
            || pChainContext->rgpChain[0] == nullptr
            || pChainContext->rgpChain[0]->cElement == 0
            || pChainContext->rgpChain[0]->rgpElement == nullptr) {
            return result;
        }
        PCERT_SIMPLE_CHAIN chain = pChainContext->rgpChain[0];
        PCCERT_CONTEXT leaf = chain->rgpElement[0]->pCertContext;
        PCCERT_CONTEXT root = chain->rgpElement[chain->cElement - 1]->pCertContext;
        if (IsOverrideCert(leaf) || IsOverrideCert(root)) {
            pPolicyStatus->dwError = S_OK;
            return TRUE;
        }
        return result;
    }
}

static SECURITY_STATUS SEC_ENTRY Mine_AcquireCredentialsHandleW(
    LPWSTR pszPrincipal, LPWSTR pszPackage, unsigned long fCredentialUse,
    void* pvLogonId, void* pAuthData, SEC_GET_KEY_FN pGetKeyFn,
    void* pvGetKeyArgument, PCredHandle phCredential, PTimeStamp ptsExpiry) noexcept {
    RelaxCredentialValidation(pAuthData, pszPackage);
    return Real_AcquireCredentialsHandleW(
        pszPrincipal, pszPackage, fCredentialUse, pvLogonId, pAuthData,
        pGetKeyFn, pvGetKeyArgument, phCredential, ptsExpiry);
}

static SECURITY_STATUS SEC_ENTRY Mine_AcquireCredentialsHandleA(
    LPSTR pszPrincipal, LPSTR pszPackage, unsigned long fCredentialUse,
    void* pvLogonId, void* pAuthData, SEC_GET_KEY_FN pGetKeyFn,
    void* pvGetKeyArgument, PCredHandle phCredential, PTimeStamp ptsExpiry) noexcept {
    // The A variant only carries ANSI package names; convert for the check.
    // Size the conversion from the input so over-length names cannot fail
    // silently and skip relaxation.
    if (pszPackage != nullptr) {
        int wideLen = MultiByteToWideChar(CP_ACP, 0, pszPackage, -1, nullptr, 0);
        if (wideLen > 0) {
            std::vector<wchar_t> widePackage(static_cast<size_t>(wideLen));
            if (MultiByteToWideChar(CP_ACP, 0, pszPackage, -1, widePackage.data(), wideLen) > 0) {
                RelaxCredentialValidation(pAuthData, widePackage.data());
            }
        }
    }
    return Real_AcquireCredentialsHandleA(
        pszPrincipal, pszPackage, fCredentialUse, pvLogonId, pAuthData,
        pGetKeyFn, pvGetKeyArgument, phCredential, ptsExpiry);
}

void FakeCAResolverAttach() {
    DetourAttach(&(PVOID&)Real_AcquireCredentialsHandleW, Mine_AcquireCredentialsHandleW);
    DetourAttach(&(PVOID&)Real_AcquireCredentialsHandleA, Mine_AcquireCredentialsHandleA);
    DetourAttach(&(PVOID&)Real_CertVerifyCertificateChainPolicy, Mine_CertVerifyCertificateChainPolicy);
    DetourAttach(&(PVOID&)Real_WinHttpSendRequest, Mine_WinHttpSendRequest);
}

void FakeCAResolverDetach() {
    DetourDetach(&(PVOID&)Real_AcquireCredentialsHandleW, Mine_AcquireCredentialsHandleW);
    DetourDetach(&(PVOID&)Real_AcquireCredentialsHandleA, Mine_AcquireCredentialsHandleA);
    DetourDetach(&(PVOID&)Real_CertVerifyCertificateChainPolicy, Mine_CertVerifyCertificateChainPolicy);
    DetourDetach(&(PVOID&)Real_WinHttpSendRequest, Mine_WinHttpSendRequest);
}

bool FakeCAResolverInit() {
    return ResolveFunctions();
}
