#include "pch.h"
#include "debuglog.h"
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
    const char* nodeLog = pNodeName != nullptr ? pNodeName : "(null)";
    const char* svcLog = pServiceName != nullptr ? pServiceName : "(null)";
    int hintFamily = pHints != nullptr ? pHints->ai_family : -1;
    int hintFlags = pHints != nullptr ? pHints->ai_flags : 0;
    int hintSock = pHints != nullptr ? pHints->ai_socktype : 0;
    int hintProto = pHints != nullptr ? pHints->ai_protocol : 0;
    DEBUG_LOG("getaddrinfo enter node='%s' svc='%s' hints(f=%d flags=0x%x sock=%d proto=%d) map=%llu",
        nodeLog, svcLog, hintFamily, hintFlags, hintSock, hintProto,
        static_cast<unsigned long long>(HostIpMap.size()));
    int ret = Real_GetAddrInfo(pNodeName, pServiceName, pHints, ppResult);
    DEBUG_LOG("getaddrinfo real ret=%d%s", ret, ret != 0 ? gai_strerrorA(ret) : "");
    if (pNodeName == nullptr) {
        DEBUG_LOG("getaddrinfo node=null passthrough ret=%d", ret);
        return ret;
    }
    std::string hostName = pNodeName;
    std::transform(hostName.begin(), hostName.end(), hostName.begin(),
        [](char c) { return static_cast<char>(::tolower(static_cast<unsigned char>(c))); });

    auto it = HostIpMap.find(hostName);
    if (it == HostIpMap.end()) {
        DEBUG_LOG("getaddrinfo miss host='%s' (lower='%s') passthrough ret=%d", pNodeName, hostName.c_str(), ret);
        return ret;
    }
    {
        std::string ipString = it->second;
        DEBUG_LOG("getaddrinfo hit host='%s' override='%s' realRet=%d", pNodeName, ipString.c_str(), ret);
        if (ret != 0) {
            DEBUG_LOG("getaddrinfo hit but real failed, keep error ret=%d", ret);
            return ret;
        }
        if (ppResult == nullptr || *ppResult == nullptr) {
            DEBUG_LOG("getaddrinfo hit but ppResult null, keep ret=0");
            return 0;
        }
        int family = (*ppResult)->ai_family;
        if (family != AF_INET) {
            DEBUG_LOG("getaddrinfo hit but family=%d (not AF_INET), no patch", family);
            return 0;
        }
        char origIp[INET_ADDRSTRLEN] = "";
        if ((*ppResult)->ai_addr != nullptr && (*ppResult)->ai_addrlen >= static_cast<size_t>(sizeof(sockaddr_in))) {
            const sockaddr_in* orig = reinterpret_cast<const sockaddr_in*>((*ppResult)->ai_addr);
            inet_ntop(AF_INET, &orig->sin_addr, origIp, sizeof(origIp));
        }
        in_addr ia{};
        if (inet_pton(AF_INET, ipString.c_str(), &ia) != 1) {
            DEBUG_LOG("getaddrinfo hit but inet_pton failed ip='%s' err=%lu", ipString.c_str(), WSAGetLastError());
            return ret;
        }
        struct sockaddr_in* sockaddr_v4 = reinterpret_cast<struct sockaddr_in*>((*ppResult)->ai_addr);
        sockaddr_v4->sin_addr = ia;
        bool hadNext = (*ppResult)->ai_next != NULL;
        if (hadNext) {
            freeaddrinfo((*ppResult)->ai_next);
            (*ppResult)->ai_next = NULL;
        }
        DEBUG_LOG("getaddrinfo patched '%s': '%s' -> '%s' (trimmedNext=%d)", pNodeName,
            origIp[0] != '\0' ? origIp : "?", ipString.c_str(), hadNext ? 1 : 0);
        return 0;
    }
}

void FakeHostResolverAttach() {
    DEBUG_LOG("attach getaddrinfo hook");
    DetourAttach(&(PVOID&)Real_GetAddrInfo, Mine_GetAddrInfo);
}

void FakeHostResolverDetach() {
    DEBUG_LOG("detach getaddrinfo hook");
    DetourDetach(&(PVOID&)Real_GetAddrInfo, Mine_GetAddrInfo);
}
