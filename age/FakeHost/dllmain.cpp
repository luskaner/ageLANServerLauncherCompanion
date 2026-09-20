#include "pch.h"
#include "hostreader.h"
#include "certreader.h"
#include "fakehostresolver.h"
#include "fakecaresolver.h"

namespace {
    bool g_attached = false;
}

BOOL APIENTRY DllMain( HMODULE /*hModule*/,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpReserved*/
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        // Host resolution (all games) and certificate trust (--overrideCerts)
        // are independent: attach whichever side initialized successfully.
        bool hostsReady = ReadHostsFile() == 0;
        bool certsReady = ReadCertsFile() == 0 && FakeCAResolverInit();
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
        }
        break;
    }
    case DLL_PROCESS_DETACH:
        if (g_attached) {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            FakeHostResolverDetach();
            FakeCAResolverDetach();
            DetourTransactionCommit();
            g_attached = false;
        }
        break;
    }
    return TRUE;
}
