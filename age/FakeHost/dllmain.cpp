#include "pch.h"
#include "debuglog.h"
#include "hostreader.h"
#include "certreader.h"
#include "fakehostresolver.h"
#include "fakecaresolver.h"

namespace {
    bool g_attached = false;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpReserved*/
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        DebugLogInit(hModule);
        DEBUG_LOG("DLL_PROCESS_ATTACH pid=%lu tid=%lu", GetCurrentProcessId(), GetCurrentThreadId());
        // Host resolution (all games) and certificate trust (--overrideCerts)
        // are independent: attach whichever side initialized successfully.
        bool hostsReady = ReadHostsFile() == 0;
        bool certsReady = ReadCertsFile() == 0 && FakeCAResolverInit();
        DEBUG_LOG("hostsReady=%d certsReady=%d", hostsReady ? 1 : 0, certsReady ? 1 : 0);
        if (hostsReady || certsReady) {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            if (hostsReady) {
                FakeHostResolverAttach();
            }
            if (certsReady) {
                FakeCAResolverAttach();
            }
            DetourTransactionCommit();
            g_attached = true;
            DEBUG_LOG("detours attached");
        }
        break;
    }
    case DLL_PROCESS_DETACH:
        DEBUG_LOG("DLL_PROCESS_DETACH g_attached=%d", g_attached ? 1 : 0);
        if (g_attached) {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            FakeHostResolverDetach();
            FakeCAResolverDetach();
            DetourTransactionCommit();
            g_attached = false;
            DEBUG_LOG("detours detached");
        }
        DebugLogShutdown();
        break;
    }
    return TRUE;
}
