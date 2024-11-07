#include "pch.h"
#include "fakeonline.h"

FakeNetworkListManager::FakeNetworkListManager(INetworkListManager* original) : original(original) {}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::GetTypeInfoCount(UINT* pctinfo) {
    return original->GetTypeInfoCount(pctinfo);
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) {
    return original->GetTypeInfo(iTInfo, lcid, ppTInfo);
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) {
    return original->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) {
    return original->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::get_IsConnectedToInternet(VARIANT_BOOL* pbIsConnected) {
    return original->get_IsConnected(pbIsConnected);
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::get_IsConnected(VARIANT_BOOL* pbIsConnected) {
	return original->get_IsConnected(pbIsConnected);
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::QueryInterface(REFIID riid, void** ppvObject) {
    return original->QueryInterface(riid, ppvObject);
}

ULONG STDMETHODCALLTYPE FakeNetworkListManager::AddRef() {
    return original->AddRef();
}

ULONG STDMETHODCALLTYPE FakeNetworkListManager::Release() {
	return original->Release();
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::GetNetworks(NLM_ENUM_NETWORK Flags, IEnumNetworks** ppEnumNetwork) {
    return original->GetNetworks(Flags, ppEnumNetwork);
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::GetNetwork(GUID gdNetworkId, INetwork** ppNetwork) {
    return original->GetNetwork(gdNetworkId, ppNetwork);
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::GetNetworkConnections(IEnumNetworkConnections** ppEnum) {
    return original->GetNetworkConnections(ppEnum);
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::GetNetworkConnection(GUID gdNetworkConnectionId, INetworkConnection** ppNetworkConnection) {
    return original->GetNetworkConnection(gdNetworkConnectionId, ppNetworkConnection);
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::GetConnectivity(NLM_CONNECTIVITY* pConnectivity) {
    return original->GetConnectivity(pConnectivity);
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::SetSimulatedProfileInfo(NLM_SIMULATED_PROFILE_INFO* pSimulatedInfo) {
    return original->SetSimulatedProfileInfo(pSimulatedInfo);
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::ClearSimulatedProfileInfo() {
    return original->ClearSimulatedProfileInfo();
}