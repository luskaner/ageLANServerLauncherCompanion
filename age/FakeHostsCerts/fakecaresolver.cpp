#include "pch.h"
#include "debuglog.h"
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
        DEBUG_LOG("ResolveFunctions: LoadLibrary sspicli failed err=%lu", GetLastError());
        return false;
    }
    Real_AcquireCredentialsHandleW = reinterpret_cast<PFN_AcquireCredentialsHandleW>(
        GetProcAddress(sspi, "AcquireCredentialsHandleW"));
    Real_AcquireCredentialsHandleA = reinterpret_cast<PFN_AcquireCredentialsHandleA>(
        GetProcAddress(sspi, "AcquireCredentialsHandleA"));
    DEBUG_LOG("ResolveFunctions: AHCW=%p AHCA=%p", (void*)Real_AcquireCredentialsHandleW, (void*)Real_AcquireCredentialsHandleA);
    if (Real_AcquireCredentialsHandleW == nullptr || Real_AcquireCredentialsHandleA == nullptr) {
        DEBUG_LOG("ResolveFunctions: sspi procs missing");
        return false;
    }
    HMODULE crypt32 = GetModuleHandleW(L"crypt32.dll");
    if (crypt32 == nullptr) {
        crypt32 = LoadLibraryW(L"crypt32.dll");
    }
    if (crypt32 == nullptr) {
        DEBUG_LOG("ResolveFunctions: crypt32 load failed err=%lu", GetLastError());
        return false;
    }
    Real_CertVerifyCertificateChainPolicy = reinterpret_cast<PFN_CertVerifyCertificateChainPolicy>(
        GetProcAddress(crypt32, "CertVerifyCertificateChainPolicy"));
    DEBUG_LOG("ResolveFunctions: CertVerify=%p", (void*)Real_CertVerifyCertificateChainPolicy);
    if (Real_CertVerifyCertificateChainPolicy == nullptr) {
        DEBUG_LOG("ResolveFunctions: CertVerify proc missing");
        return false;
    }
    HMODULE winhttp = GetModuleHandleW(L"winhttp.dll");
    if (winhttp == nullptr) {
        winhttp = LoadLibraryW(L"winhttp.dll");
    }
    if (winhttp == nullptr) {
        DEBUG_LOG("ResolveFunctions: winhttp load failed err=%lu", GetLastError());
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
    DEBUG_LOG("ResolveFunctions: Send=%p Set=%p Query=%p Crack=%p",
        (void*)Real_WinHttpSendRequest, (void*)Real_WinHttpSetOption,
        (void*)Real_WinHttpQueryOption, (void*)Real_WinHttpCrackUrl);
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
        if (Real_WinHttpQueryOption == nullptr) {
            DEBUG_LOG("BlessRequest: QueryOption=null, skip req=%p", hRequest);
            return;
        }
        if (Real_WinHttpQueryOption(hRequest, WINHTTP_OPTION_URL, url, &urlLen) == FALSE) {
            DEBUG_LOG("BlessRequest: QueryOption URL failed req=%p err=%lu", hRequest, GetLastError());
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
            DEBUG_LOG("BlessRequest: CrackUrl failed url='%s' err=%lu", DbgNarrowW(url).c_str(), GetLastError());
            return;
        }
        std::wstring hostName(parts.lpszHostName, parts.dwHostNameLength);
        if (!IsOverrideHost(hostName)) {
            DEBUG_LOG("BlessRequest: skip url='%s' host='%s' (not override, map=%llu)",
                DbgNarrowW(url).c_str(), DbgNarrowWS(hostName).c_str(),
                static_cast<unsigned long long>(HostIpMap.size()));
            return;
        }
        DWORD flags = 0;
        DWORD flagsSize = sizeof(flags);
        BOOL gotFlags = Real_WinHttpQueryOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &flags, &flagsSize);
        if (gotFlags == FALSE) {
            DEBUG_LOG("BlessRequest: QueryOption flags failed req=%p err=%lu, use 0", hRequest, GetLastError());
            flags = 0;
        }
        DWORD oldFlags = flags;
        flags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
        BOOL setOk = Real_WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &flags, sizeof(flags));
        DEBUG_LOG("BlessRequest: url='%s' host='%s' flags 0x%08lx->0x%08lx set=%d err=%lu",
            DbgNarrowW(url).c_str(), DbgNarrowWS(hostName).c_str(),
            static_cast<unsigned long>(oldFlags), static_cast<unsigned long>(flags),
            setOk ? 1 : 0, setOk ? 0 : GetLastError());
    }
}

// The TLS handshake happens inside WinHttpSendRequest; set the flags right
// before calling through.
static BOOL WINAPI Mine_WinHttpSendRequest(
    HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional,
    DWORD dwOptionalLength, DWORD dwTotalLength, DWORD_PTR dwContext) noexcept {
    DEBUG_LOG("WinHttpSendRequest enter req=%p hdrLen=%lu optLen=%lu total=%lu ctx=%llu",
        hRequest, static_cast<unsigned long>(dwHeadersLength),
        static_cast<unsigned long>(dwOptionalLength), static_cast<unsigned long>(dwTotalLength),
        static_cast<unsigned long long>(dwContext));
    BlessRequestHandle(hRequest);
    BOOL ok = Real_WinHttpSendRequest(
        hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength,
        dwTotalLength, dwContext);
    DEBUG_LOG("WinHttpSendRequest exit req=%p ok=%d err=%lu", hRequest, ok ? 1 : 0, ok ? 0 : GetLastError());
    return ok;
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
        if (pAuthData == nullptr) {
            DEBUG_LOG("RelaxCred: pAuthData=null pkg='%s', skip", DbgNarrowW(package).c_str());
            return;
        }
        if (!IsSchannelPackage(package)) {
            DEBUG_LOG("RelaxCred: pkg='%s' not schannel, skip", DbgNarrowW(package).c_str());
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
            DEBUG_LOG("RelaxCred: pkg='%s' unknown ver=%lu, skip", DbgNarrowW(package).c_str(), static_cast<unsigned long>(version));
            return;
        }
        DWORD old = *flags;
        *flags = (*flags | SCH_CRED_MANUAL_CRED_VALIDATION) & ~SCH_CRED_AUTO_CRED_VALIDATION;
        DEBUG_LOG("RelaxCred: pkg='%s' ver=%lu flags 0x%08lx->0x%08lx",
            DbgNarrowW(package).c_str(), static_cast<unsigned long>(version),
            static_cast<unsigned long>(old), static_cast<unsigned long>(*flags));
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
        DWORD errBefore = (pPolicyStatus != nullptr) ? pPolicyStatus->dwError : 0;
        const char* oidLog = pszPolicyOID != nullptr ? pszPolicyOID : "(null)";
        if (result != FALSE || pPolicyStatus == nullptr
            || pPolicyStatus->dwError == S_OK
            || pChainContext == nullptr || pChainContext->cChain == 0
            || pChainContext->rgpChain == nullptr
            || pChainContext->rgpChain[0] == nullptr
            || pChainContext->rgpChain[0]->cElement == 0
            || pChainContext->rgpChain[0]->rgpElement == nullptr) {
            DEBUG_LOG("CertVerify: policy=%s result=%d err=0x%08lx passthrough (no chain or ok)",
                oidLog, result ? 1 : 0, static_cast<unsigned long>(errBefore));
            return result;
        }
        PCERT_SIMPLE_CHAIN chain = pChainContext->rgpChain[0];
        PCCERT_CONTEXT leaf = chain->rgpElement[0]->pCertContext;
        PCCERT_CONTEXT root = chain->rgpElement[chain->cElement - 1]->pCertContext;
        bool leafHit = IsOverrideCert(leaf);
        bool rootHit = IsOverrideCert(root);
        DEBUG_LOG("CertVerify: policy=%s result=%d err=0x%08lx chains=%lu elems=%lu leafHit=%d rootHit=%d pinned=%llu",
            oidLog, result ? 1 : 0, static_cast<unsigned long>(errBefore),
            static_cast<unsigned long>(pChainContext->cChain),
            static_cast<unsigned long>(chain->cElement),
            leafHit ? 1 : 0, rootHit ? 1 : 0,
            static_cast<unsigned long long>(CertThumbprintList.size()));
        if (leafHit || rootHit) {
            DEBUG_LOG("CertVerify: accept override chain policy=%s", oidLog);
            pPolicyStatus->dwError = S_OK;
            return TRUE;
        }
        DEBUG_LOG("CertVerify: reject policy=%s err=0x%08lx", oidLog, static_cast<unsigned long>(errBefore));
        return result;
    }
}

static SECURITY_STATUS SEC_ENTRY Mine_AcquireCredentialsHandleW(
    LPWSTR pszPrincipal, LPWSTR pszPackage, unsigned long fCredentialUse,
    void* pvLogonId, void* pAuthData, SEC_GET_KEY_FN pGetKeyFn,
    void* pvGetKeyArgument, PCredHandle phCredential, PTimeStamp ptsExpiry) noexcept {
    DEBUG_LOG("AHCW enter pkg='%s' use=%lu auth=%p", DbgNarrowW(pszPackage).c_str(), static_cast<unsigned long>(fCredentialUse), pAuthData);
    RelaxCredentialValidation(pAuthData, pszPackage);
    SECURITY_STATUS st = Real_AcquireCredentialsHandleW(
        pszPrincipal, pszPackage, fCredentialUse, pvLogonId, pAuthData,
        pGetKeyFn, pvGetKeyArgument, phCredential, ptsExpiry);
    DEBUG_LOG("AHCW exit pkg='%s' st=0x%08lx", DbgNarrowW(pszPackage).c_str(), static_cast<unsigned long>(st));
    return st;
}

static SECURITY_STATUS SEC_ENTRY Mine_AcquireCredentialsHandleA(
    LPSTR pszPrincipal, LPSTR pszPackage, unsigned long fCredentialUse,
    void* pvLogonId, void* pAuthData, SEC_GET_KEY_FN pGetKeyFn,
    void* pvGetKeyArgument, PCredHandle phCredential, PTimeStamp ptsExpiry) noexcept {
    DEBUG_LOG("AHCA enter pkg='%s' use=%lu auth=%p", pszPackage != nullptr ? pszPackage : "(null)", static_cast<unsigned long>(fCredentialUse), pAuthData);
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
            else {
                DEBUG_LOG("AHCA: package conv failed err=%lu", GetLastError());
            }
        }
        else {
            DEBUG_LOG("AHCA: package conv size failed err=%lu", GetLastError());
        }
    }
    SECURITY_STATUS st = Real_AcquireCredentialsHandleA(
        pszPrincipal, pszPackage, fCredentialUse, pvLogonId, pAuthData,
        pGetKeyFn, pvGetKeyArgument, phCredential, ptsExpiry);
    DEBUG_LOG("AHCA exit pkg='%s' st=0x%08lx", pszPackage != nullptr ? pszPackage : "(null)", static_cast<unsigned long>(st));
    return st;
}

void FakeCAResolverAttach() {
    DEBUG_LOG("attach CA hooks");
    DetourAttach(&(PVOID&)Real_AcquireCredentialsHandleW, Mine_AcquireCredentialsHandleW);
    DetourAttach(&(PVOID&)Real_AcquireCredentialsHandleA, Mine_AcquireCredentialsHandleA);
    DetourAttach(&(PVOID&)Real_CertVerifyCertificateChainPolicy, Mine_CertVerifyCertificateChainPolicy);
    DetourAttach(&(PVOID&)Real_WinHttpSendRequest, Mine_WinHttpSendRequest);
}

void FakeCAResolverDetach() {
    DEBUG_LOG("detach CA hooks");
    DetourDetach(&(PVOID&)Real_AcquireCredentialsHandleW, Mine_AcquireCredentialsHandleW);
    DetourDetach(&(PVOID&)Real_AcquireCredentialsHandleA, Mine_AcquireCredentialsHandleA);
    DetourDetach(&(PVOID&)Real_CertVerifyCertificateChainPolicy, Mine_CertVerifyCertificateChainPolicy);
    DetourDetach(&(PVOID&)Real_WinHttpSendRequest, Mine_WinHttpSendRequest);
}

bool FakeCAResolverInit() {
    bool ok = ResolveFunctions();
    DEBUG_LOG("CA resolver init=%d", ok ? 1 : 0);
    return ok;
}
