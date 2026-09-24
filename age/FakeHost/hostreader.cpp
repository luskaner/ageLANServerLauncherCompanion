#include "pch.h"
#include "debuglog.h"
#include "hostreader.h"

std::unordered_map<std::string, std::string> HostIpMap;

int ReadHostsFile() {
    LPWSTR commandLine = GetCommandLineW();
    if (commandLine == nullptr) {
        DEBUG_LOG("ReadHostsFile: GetCommandLineW=null");
        return 1;
    }
    int argc = 0;
    LPWSTR* argvRaw = CommandLineToArgvW(commandLine, &argc);
    if (argvRaw == nullptr) {
        DEBUG_LOG("ReadHostsFile: CommandLineToArgvW=null err=%lu", GetLastError());
        return 1;
    }
    std::unique_ptr<LPWSTR, decltype(&LocalFree)> argvGuard(argvRaw, &LocalFree);
    LPWSTR* argv = argvRaw;
    if (argc <= 1) {
        DEBUG_LOG("ReadHostsFile: no args (argc=%d)", argc);
    }
    else {
        for (int i = 1; i < argc; ++i) {
                if (wcsncmp(argv[i], L"--overrideHosts=", wcslen(L"--overrideHosts=")) == 0) {
                    std::wstring hostsFilePath = argv[i] + 16;
                    DEBUG_LOG("ReadHostsFile: path='%s'", DbgNarrowWS(hostsFilePath).c_str());
                    // Open via the wide path so non-ASCII characters survive;
                    // a narrow ifstream would reinterpret UTF-8 bytes as ANSI
                    // (CP_ACP) and fail to open.
                    std::ifstream hostsFile{std::filesystem::path(hostsFilePath)};
                    if (hostsFile.is_open()) {
                        DEBUG_LOG("ReadHostsFile: opened ok");
                        std::string line;
                        unsigned long lineNo = 0;
                        unsigned long mapped = 0;
                        while (std::getline(hostsFile, line)) {
                            ++lineNo;
                            size_t commentPos = line.find('#');
                            std::string lineWithoutComment;

                            if (commentPos != std::string::npos) {
                                lineWithoutComment = line.substr(0, commentPos);
                            }
                            else {
                                lineWithoutComment = line;
                            }

                            size_t firstNonSpace = lineWithoutComment.find_first_not_of(" \t");
                            if (std::string::npos != firstNonSpace) {
                                lineWithoutComment = lineWithoutComment.substr(firstNonSpace);
                            }
                            size_t lastNonSpace = lineWithoutComment.find_last_not_of(" \t");
                            if (std::string::npos != lastNonSpace) {
                                lineWithoutComment = lineWithoutComment.substr(0, lastNonSpace + 1);
                            }

                            if (!lineWithoutComment.empty()) {
                                std::istringstream iss(lineWithoutComment);
                                std::string ip, host;
                                if (iss >> ip) {
                                    bool anyHost = false;
                                    while (iss >> host) {
                                        anyHost = true;
                                        std::string lowerHost = host;
                                        std::transform(lowerHost.begin(), lowerHost.end(), lowerHost.begin(),
                                            [](char c) { return static_cast<char>(::tolower(static_cast<unsigned char>(c))); });
                                        auto prev = HostIpMap.find(lowerHost);
                                        if (prev != HostIpMap.end() && prev->second != ip) {
                                            DEBUG_LOG("hosts line %lu: '%s' override '%s'->'%s'",
                                                lineNo, lowerHost.c_str(), prev->second.c_str(), ip.c_str());
                                        }
                                        else {
                                            DEBUG_LOG("hosts line %lu: '%s' -> '%s'",
                                                lineNo, lowerHost.c_str(), ip.c_str());
                                        }
                                        HostIpMap[lowerHost] = ip;
                                        ++mapped;
                                    }
                                    if (!anyHost) {
                                        DEBUG_LOG("hosts line %lu: ip '%s' without host, skipped", lineNo, ip.c_str());
                                    }
                                }
                                else {
                                    DEBUG_LOG("hosts line %lu: no ip token, skipped", lineNo);
                                }
                            }
                        }
                        hostsFile.close();
                        DEBUG_LOG("ReadHostsFile: parsed %lu lines, %lu mappings", lineNo, mapped);
                    }
                    else {
                        DEBUG_LOG("ReadHostsFile: open failed err=%lu", GetLastError());
                    }
                    break;
                }
			}
        }
    if (HostIpMap.empty()) {
        DEBUG_LOG("ReadHostsFile: no hosts loaded (map empty)");
        return 1;
    }
    DEBUG_LOG("ReadHostsFile: total %llu entries", static_cast<unsigned long long>(HostIpMap.size()));
#ifdef _DEBUG
    for (const auto& [host, ip] : HostIpMap) {
        DEBUG_LOG("hosts entry: '%s' -> '%s'", host.c_str(), ip.c_str());
    }
#endif
    return 0;
}
