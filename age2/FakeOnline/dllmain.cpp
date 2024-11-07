#include "pch.h"
#include "fakecomresolver.h"
#include <iostream>

BOOL APIENTRY DllMain( HMODULE /*hModule*/,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpReserved*/
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        COMDllProcessAttach();
        DetourTransactionCommit();
		break;
    case DLL_PROCESS_DETACH:
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        COMDllProcessDetach();
        DetourTransactionCommit();
        break;
    }
    return TRUE;
}

