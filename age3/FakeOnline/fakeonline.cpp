#include "pch.h"
#include "debuglog.h"
#include "fakeonline.h"
#include "fakeenumnetworks.h"
#include <string>

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
    HRESULT hr = original->get_IsConnected(pbIsConnected);
    short val = (SUCCEEDED(hr) && pbIsConnected != nullptr) ? *pbIsConnected : 0;
    DEBUG_LOG("NLM::get_IsConnectedToInternet hr=0x%08lx val=%d", static_cast<unsigned long>(hr), (int)val);
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::get_IsConnected(VARIANT_BOOL* pbIsConnected) {
    HRESULT hr = original->get_IsConnected(pbIsConnected);
    short val = (SUCCEEDED(hr) && pbIsConnected != nullptr) ? *pbIsConnected : 0;
    DEBUG_LOG("NLM::get_IsConnected hr=0x%08lx val=%d (passthrough)", static_cast<unsigned long>(hr), (int)val);
	return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::QueryInterface(REFIID riid, void** ppvObject) {
    HRESULT hr = original->QueryInterface(riid, ppvObject);
    DEBUG_LOG("NLM::QueryInterface iid=%s hr=0x%08lx out=%p", DbgGuid(riid).c_str(), static_cast<unsigned long>(hr), (void*)(ppvObject != nullptr ? *ppvObject : nullptr));
    return hr;
}

ULONG STDMETHODCALLTYPE FakeNetworkListManager::AddRef() {
    return original->AddRef();
}

ULONG STDMETHODCALLTYPE FakeNetworkListManager::Release() {
	return original->Release();
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::GetNetworks(NLM_ENUM_NETWORK Flags, IEnumNetworks** ppEnumNetwork) {
    HRESULT hr = original->GetNetworks(Flags, ppEnumNetwork);
    IEnumNetworks* raw = (SUCCEEDED(hr) && ppEnumNetwork != nullptr) ? *ppEnumNetwork : nullptr;
    DEBUG_LOG("NLM::GetNetworks flags=0x%x hr=0x%08lx raw=%p", static_cast<unsigned>(Flags), static_cast<unsigned long>(hr), (void*)raw);
    if (FAILED(hr) || ppEnumNetwork == nullptr || *ppEnumNetwork == nullptr) {
        DEBUG_LOG("NLM::GetNetworks: no enum to wrap");
        return hr;
    }
	IEnumNetworks* pOriginalEnumNetwork = *ppEnumNetwork; 
    FakeEnumNetworks* pProxy = new FakeEnumNetworks(pOriginalEnumNetwork);
	*ppEnumNetwork = pProxy;
    DEBUG_LOG("NLM::GetNetworks: wrapped %p -> %p", (void*)pOriginalEnumNetwork, (void*)pProxy);
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::GetNetwork(GUID gdNetworkId, INetwork** ppNetwork) {
    HRESULT hr = original->GetNetwork(gdNetworkId, ppNetwork);
    DEBUG_LOG("NLM::GetNetwork id=%s hr=0x%08lx out=%p", DbgGuid(gdNetworkId).c_str(), static_cast<unsigned long>(hr), (void*)(ppNetwork != nullptr ? *ppNetwork : nullptr));
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::GetNetworkConnections(IEnumNetworkConnections** ppEnum) {
    HRESULT hr = original->GetNetworkConnections(ppEnum);
    DEBUG_LOG("NLM::GetNetworkConnections hr=0x%08lx out=%p", static_cast<unsigned long>(hr), (void*)(ppEnum != nullptr ? *ppEnum : nullptr));
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::GetNetworkConnection(GUID gdNetworkConnectionId, INetworkConnection** ppNetworkConnection) {
    HRESULT hr = original->GetNetworkConnection(gdNetworkConnectionId, ppNetworkConnection);
    DEBUG_LOG("NLM::GetNetworkConnection id=%s hr=0x%08lx out=%p", DbgGuid(gdNetworkConnectionId).c_str(), static_cast<unsigned long>(hr), (void*)(ppNetworkConnection != nullptr ? *ppNetworkConnection : nullptr));
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::GetConnectivity(NLM_CONNECTIVITY* pConnectivity) {
    HRESULT hr = original->GetConnectivity(pConnectivity);
    unsigned val = (SUCCEEDED(hr) && pConnectivity != nullptr) ? static_cast<unsigned>(*pConnectivity) : 0;
    DEBUG_LOG("NLM::GetConnectivity hr=0x%08lx val=0x%x", static_cast<unsigned long>(hr), val);
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::SetSimulatedProfileInfo(NLM_SIMULATED_PROFILE_INFO* pSimulatedInfo) {
    return original->SetSimulatedProfileInfo(pSimulatedInfo);
}

HRESULT STDMETHODCALLTYPE FakeNetworkListManager::ClearSimulatedProfileInfo() {
    return original->ClearSimulatedProfileInfo();
}