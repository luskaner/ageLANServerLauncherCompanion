#include "pch.h"
#include "debuglog.h"
#include "fakecomresolver.h"
#include <iostream>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpReserved*/
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DebugLogInit(hModule);
        DEBUG_LOG("DLL_PROCESS_ATTACH pid=%lu tid=%lu", GetCurrentProcessId(), GetCurrentThreadId());
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        COMDllProcessAttach();
        DetourTransactionCommit();
        DEBUG_LOG("detours attached");
		break;
    case DLL_PROCESS_DETACH:
        DEBUG_LOG("DLL_PROCESS_DETACH");
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        COMDllProcessDetach();
        DetourTransactionCommit();
        DEBUG_LOG("detours detached");
        DebugLogShutdown();
        break;
    }
    return TRUE;
}

