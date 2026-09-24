#include "pch.h"
#include "debuglog.h"
#include "fakecomresolver.h"
#include "fakeonline.h"

HRESULT(WINAPI* Real_CoCreateInstance)(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv) = CoCreateInstance;

HRESULT WINAPI Mine_CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv) {
    DEBUG_LOG("CoCreateInstance enter cls=%s iid=%s ctx=0x%lx outer=%p",
        DbgGuid(rclsid).c_str(), DbgGuid(riid).c_str(), static_cast<unsigned long>(dwClsContext), (void*)pUnkOuter);
    HRESULT hr = Real_CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
    bool isNlm = (riid == IID_INetworkListManager);
    DEBUG_LOG("CoCreateInstance exit hr=0x%08lx ppv=%p isNlm=%d", static_cast<unsigned long>(hr), (void*)(ppv != nullptr ? *ppv : nullptr), isNlm ? 1 : 0);
    if (SUCCEEDED(hr) && isNlm) {
        if (ppv == nullptr || *ppv == nullptr) {
            DEBUG_LOG("CoCreateInstance: NLM ok but ppv null, skip wrap");
            return hr;
        }
        INetworkListManager* original = (INetworkListManager*)(*ppv);
        *ppv = new FakeNetworkListManager(original);
        DEBUG_LOG("CoCreateInstance: INetworkListManager wrapped orig=%p proxy=%p", (void*)original, (void*)*ppv);
    }
    return hr;
}

void COMDllProcessAttach() {
    DEBUG_LOG("attach CoCreateInstance hook");
    DetourAttach(&(PVOID&)Real_CoCreateInstance, Mine_CoCreateInstance);
}

void COMDllProcessDetach() {
    DEBUG_LOG("detach CoCreateInstance hook");
    DetourDetach(&(PVOID&)Real_CoCreateInstance, Mine_CoCreateInstance);
}