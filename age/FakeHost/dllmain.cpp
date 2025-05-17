#include "pch.h"
#include "hostreader.h"
#include "fakehostresolver.h"

BOOL APIENTRY DllMain( HMODULE /*hModule*/,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpReserved*/
                     )
{
	auto attached = false;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		if (ReadHostsFile() == 0) {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            FakeHostResolverAttach();
            DetourTransactionCommit();
			attached = true;			
		}
        break;
    case DLL_PROCESS_DETACH:
		if (attached) {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            FakeHostResolverDetach();
            DetourTransactionCommit();
		}        
        break;
    }
    return TRUE;
}   

