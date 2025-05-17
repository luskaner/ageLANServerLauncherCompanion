#include "pch.h"
#include "hostreader.h"

std::unordered_map<std::string, std::string> HostIpMap;

int ReadHostsFile() {
    LPWSTR commandLine = GetCommandLineW();
    if (commandLine != nullptr) {
        int argc;
        LPWSTR* argv = CommandLineToArgvW(commandLine, &argc);        
        if (argv != nullptr && argc > 1) {
			for (int i = 1; i < argc; ++i) {
                if (wcsncmp(argv[i], L"--overrideHosts=", wcslen(L"--overrideHosts=")) == 0) {
                    std::wstring hostsFilePath = argv[i] + 16;
                    std::string hostsFilePathStr(hostsFilePath.begin(), hostsFilePath.end());
                    LocalFree(argv);
                    std::ifstream hostsFile(hostsFilePathStr);                    
                    if (hostsFile.is_open()) {
                        std::string line;
                        while (std::getline(hostsFile, line)) {
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
                                    while (iss >> host) {
                                        std::string lowerHost = host;
                                        std::transform(lowerHost.begin(), lowerHost.end(), lowerHost.begin(), ::tolower);
                                        HostIpMap[lowerHost] = ip;
                                    }
                                }
                            }
                        }
                        hostsFile.close();
                    }
                    break;
                }
			}            
        }
    }    
    if (HostIpMap.empty()) {
        return 1;
    }
    return 0;
}
