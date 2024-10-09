#include "pch.h"
#include "fakecomresolver.h"
#include "fakeonline.h"

HRESULT(WINAPI* Real_CoCreateInstance)(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv) = CoCreateInstance;

HRESULT WINAPI Mine_CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv) {
    HRESULT hr = Real_CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
    if (SUCCEEDED(hr) && riid == IID_INetworkListManager) {
        INetworkListManager* original = (INetworkListManager*)(*ppv);
        *ppv = new FakeNetworkListManager(original);
    }
    return hr;
}

void COMDllProcessAttach() {
    DetourAttach(&(PVOID&)Real_CoCreateInstance, Mine_CoCreateInstance);
}

void COMDllProcessDetach() {
    DetourDetach(&(PVOID&)Real_CoCreateInstance, Mine_CoCreateInstance);
}