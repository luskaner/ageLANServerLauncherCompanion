#include "pch.h"
#include "fakehostresolver.h"
#include "hostreader.h"

int (WINAPI* Real_GetAddrInfo)(
    PCSTR pNodeName,
    PCSTR pServiceName,
    const ADDRINFOA* pHints,
    PADDRINFOA* ppResult
    ) = getaddrinfo;

int WINAPI Mine_GetAddrInfo(
    PCSTR pNodeName,
    PCSTR pServiceName,
    const ADDRINFOA* pHints,
    PADDRINFOA* ppResult
) {
    int ret = Real_GetAddrInfo(pNodeName, pServiceName, pHints, ppResult);
    std::string hostName = pNodeName;
    std::transform(hostName.begin(), hostName.end(), hostName.begin(), ::tolower);

    auto it = HostIpMap.find(hostName);
    if (it != HostIpMap.end()) {
        std::string ipString = it->second;        
        if (ret != 0) {
            return ret;
        }
        if (*ppResult != nullptr && (*ppResult)->ai_family == AF_INET) {
            in_addr ia;
            if (inet_pton(AF_INET, ipString.c_str(), &ia) != 1) {
                return ret;
            }
            struct sockaddr_in* sockaddr_v4 = reinterpret_cast<struct sockaddr_in*>((*ppResult)->ai_addr);
            sockaddr_v4->sin_addr = ia;
            if ((*ppResult)->ai_next != NULL) {
                freeaddrinfo((*ppResult)->ai_next);
                (*ppResult)->ai_next = NULL;
            }            
        }
        return 0;

    }
    return ret;
}

void FakeHostResolverAttach() {
    DetourAttach(&(PVOID&)Real_GetAddrInfo, Mine_GetAddrInfo);
}

void FakeHostResolverDetach() {
    DetourDetach(&(PVOID&)Real_GetAddrInfo, Mine_GetAddrInfo);
}
