#include "pch.h"
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
        return 1;
    }
    int argc;
    LPWSTR* argv = CommandLineToArgvW(commandLine, &argc);
    if (argv == nullptr) {
        return 1;
    }

    // CommandLineToArgvW allocates with LocalAlloc; release it on every exit
    // path once we are done reading argv.
    std::unique_ptr<LPWSTR, decltype(&LocalFree)> argvGuard(argv, &LocalFree);
    if (argc <= 1) {
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
                return 1;
            }

            std::string line;
            std::string pemBlock;
            bool inCert = false;
            while (std::getline(certFile, line)) {
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                if (line.rfind("-----BEGIN CERTIFICATE-----", 0) == 0) {
                    pemBlock.clear();
                    inCert = true;
                }
                else if (line.rfind("-----END CERTIFICATE-----", 0) == 0) {
                    std::vector<BYTE> der = Base64Decode(pemBlock);
                    if (!der.empty()) {
                        CertThumbprint thumbprint{};
                        if (CalcCertThumbprint(der, thumbprint)) {
                            CertThumbprintList.push_back(thumbprint);
                        }
                    }
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
            break;
        }
    }

    if (!found || CertThumbprintList.empty()) {
        return 1;
    }
    return 0;
}
