#include "pch.h"
#include "debuglog.h"
#include "certreader.h"

std::vector<CertThumbprint> CertThumbprintList;

namespace {
    std::vector<BYTE> Base64Decode(const std::string& input) {
        std::vector<BYTE> output;
        if (input.empty()) {
            return output;
        }
        DWORD size = 0;
        if (CryptStringToBinaryA(
            input.c_str(),
            static_cast<DWORD>(input.size()),
            CRYPT_STRING_BASE64,
            nullptr,
            &size,
            nullptr,
            nullptr) == FALSE) {
            return output;
        }
        output.resize(size);
        if (CryptStringToBinaryA(
            input.c_str(),
            static_cast<DWORD>(input.size()),
            CRYPT_STRING_BASE64,
            output.data(),
            &size,
            nullptr,
            nullptr) == FALSE) {
            output.clear();
            return output;
        }
        output.resize(size);
        return output;
    }

    bool CalcCertThumbprint(const std::vector<BYTE>& der, CertThumbprint& out) {
        // Use the DER bytes directly; SHA-256 over them is the certificate's
        // SHA-256 thumbprint.
        HCRYPTPROV prov = 0;
        HCRYPTHASH hash = 0;
        BOOL ok = CryptAcquireContextW(&prov, nullptr, nullptr, PROV_RSA_AES,
            CRYPT_VERIFYCONTEXT);
        if (ok) {
            ok = CryptCreateHash(prov, CALG_SHA_256, 0, 0, &hash);
        }
        if (ok) {
            ok = CryptHashData(hash, der.data(), static_cast<DWORD>(der.size()), 0);
        }
        DWORD length = static_cast<DWORD>(out.size());
        if (ok) {
            ok = CryptGetHashParam(hash, HP_HASHVAL, out.data(), &length, 0);
        }
        if (hash != 0) {
            CryptDestroyHash(hash);
        }
        if (prov != 0) {
            CryptReleaseContext(prov, 0);
        }
        return ok == TRUE && length == out.size();
    }
}

int ReadCertsFile() {
    LPWSTR commandLine = GetCommandLineW();
    if (commandLine == nullptr) {
        DEBUG_LOG("ReadCertsFile: GetCommandLineW=null");
        return 1;
    }
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(commandLine, &argc);
    if (argv == nullptr) {
        DEBUG_LOG("ReadCertsFile: CommandLineToArgvW=null err=%lu", GetLastError());
        return 1;
    }

    // CommandLineToArgvW allocates with LocalAlloc; release it on every exit
    // path once we are done reading argv.
    std::unique_ptr<LPWSTR, decltype(&LocalFree)> argvGuard(argv, &LocalFree);
    if (argc <= 1) {
        DEBUG_LOG("ReadCertsFile: no args (argc=%d)", argc);
        return 1;
    }

    bool found = false;
    for (int i = 1; i < argc; ++i) {
        if (wcsncmp(argv[i], L"--overrideCerts=", wcslen(L"--overrideCerts=")) == 0) {
            found = true;
            std::wstring certFilePath = argv[i] + 16;

            // Open via the wide path so non-ASCII characters survive; a narrow
            // ifstream would reinterpret UTF-8 bytes as ANSI (CP_ACP) and fail.
            std::ifstream certFile{std::filesystem::path(certFilePath)};
            if (!certFile.is_open()) {
                DEBUG_LOG("ReadCertsFile: open failed path='%s' err=%lu", DbgNarrowWS(certFilePath).c_str(), GetLastError());
                return 1;
            }
            DEBUG_LOG("ReadCertsFile: opened path='%s'", DbgNarrowWS(certFilePath).c_str());

            std::string line;
            std::string pemBlock;
            bool inCert = false;
            unsigned long lineNo = 0;
            unsigned long certIndex = 0;
            while (std::getline(certFile, line)) {
                ++lineNo;
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                if (line.rfind("-----BEGIN CERTIFICATE-----", 0) == 0) {
                    pemBlock.clear();
                    inCert = true;
                    DEBUG_LOG("ReadCertsFile: BEGIN cert #%lu line %lu", certIndex, lineNo);
                }
                else if (line.rfind("-----END CERTIFICATE-----", 0) == 0) {
                    std::vector<BYTE> der = Base64Decode(pemBlock);
                    DEBUG_LOG("ReadCertsFile: END cert #%lu line %lu b64=%llu der=%llu",
                        certIndex, lineNo,
                        static_cast<unsigned long long>(pemBlock.size()),
                        static_cast<unsigned long long>(der.size()));
                    if (!der.empty()) {
                        CertThumbprint thumbprint{};
                        if (CalcCertThumbprint(der, thumbprint)) {
                            CertThumbprintList.push_back(thumbprint);
                            DEBUG_LOG("ReadCertsFile: cert #%lu sha256=%s", certIndex,
                                DbgThumbHex(thumbprint.data(), thumbprint.size()).c_str());
                        }
                        else {
                            DEBUG_LOG("ReadCertsFile: cert #%lu thumbprint calc failed", certIndex);
                        }
                    }
                    else {
                        DEBUG_LOG("ReadCertsFile: cert #%lu base64 decode failed b64len=%llu",
                            certIndex, static_cast<unsigned long long>(pemBlock.size()));
                    }
                    ++certIndex;
                    pemBlock.clear();
                    inCert = false;
                }
                else if (inCert) {
                    // Only accumulate base64 body while inside a certificate
                    // block; headers, comments, or blank lines outside the
                    // BEGIN/END markers must not pollute pemBlock.
                    pemBlock += line;
                }
            }
            certFile.close();
            DEBUG_LOG("ReadCertsFile: parsed %lu lines, %lu blocks", lineNo, certIndex);
            break;
        }
    }

    if (!found || CertThumbprintList.empty()) {
        DEBUG_LOG("ReadCertsFile: no certs (found=%d count=%llu)", found ? 1 : 0,
            static_cast<unsigned long long>(CertThumbprintList.size()));
        return 1;
    }
    DEBUG_LOG("ReadCertsFile: total %llu pinned", static_cast<unsigned long long>(CertThumbprintList.size()));
#ifdef _DEBUG
    for (size_t i = 0; i < CertThumbprintList.size(); ++i) {
        DEBUG_LOG("cert pinned #%llu sha256=%s", static_cast<unsigned long long>(i),
            DbgThumbHex(CertThumbprintList[i].data(), CertThumbprintList[i].size()).c_str());
    }
#endif
    return 0;
}
