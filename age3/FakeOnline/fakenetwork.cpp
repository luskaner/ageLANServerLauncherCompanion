#include "pch.h"
#include "debuglog.h"
#include "fakenetwork.h"

FakeNetwork::FakeNetwork(INetwork* pOriginal) : m_pOriginal(pOriginal) {}

HRESULT STDMETHODCALLTYPE FakeNetwork::QueryInterface(REFIID riid, void** ppvObject) {
    if (riid == __uuidof(IUnknown) || riid == __uuidof(IDispatch) || riid == __uuidof(INetwork)) {
        *ppvObject = static_cast<INetwork*>(this);
        AddRef();
        return S_OK;
    }
    return m_pOriginal->QueryInterface(riid, ppvObject);
}

ULONG STDMETHODCALLTYPE FakeNetwork::AddRef() {
    return m_pOriginal->AddRef();
}

ULONG STDMETHODCALLTYPE FakeNetwork::Release() {
    ULONG count = m_pOriginal->Release();
    if (count == 0) {
        delete this;
    }
    return count;
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetNetworkId(GUID* pgdGuidNetworkId) {
    HRESULT hr = m_pOriginal->GetNetworkId(pgdGuidNetworkId);
    DEBUG_LOG("Network::GetNetworkId hr=0x%08lx id=%s", static_cast<unsigned long>(hr),
        (SUCCEEDED(hr) && pgdGuidNetworkId != nullptr) ? DbgGuid(*pgdGuidNetworkId).c_str() : "(null)");
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetName(BSTR* pszNetworkName) {
    HRESULT hr = m_pOriginal->GetName(pszNetworkName);
    DEBUG_LOG("Network::GetName hr=0x%08lx name='%s'", static_cast<unsigned long>(hr),
        (SUCCEEDED(hr) && pszNetworkName != nullptr) ? DbgNarrowW(*pszNetworkName).c_str() : "(null)");
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetwork::SetName(BSTR szNetworkNewName) {
    DEBUG_LOG("Network::SetName name='%s'", DbgNarrowW(szNetworkNewName).c_str());
    return m_pOriginal->SetName(szNetworkNewName);
}

HRESULT FakeNetwork::GetDomainType(
    NLM_DOMAIN_TYPE* pNetworkType
) {
    HRESULT hr = m_pOriginal->GetDomainType(pNetworkType);
    int val = (SUCCEEDED(hr) && pNetworkType != nullptr) ? (int)*pNetworkType : -1;
    DEBUG_LOG("Network::GetDomainType hr=0x%08lx val=%d", static_cast<unsigned long>(hr), val);
    return hr;
}

HRESULT FakeNetwork::GetConnectivity(
    NLM_CONNECTIVITY* pConnectivity
) {
    HRESULT hr = m_pOriginal->GetConnectivity(pConnectivity);
    unsigned val = (SUCCEEDED(hr) && pConnectivity != nullptr) ? static_cast<unsigned>(*pConnectivity) : 0;
    DEBUG_LOG("Network::GetConnectivity hr=0x%08lx val=0x%x", static_cast<unsigned long>(hr), val);
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetDescription(BSTR* pszDescription) {
    HRESULT hr = m_pOriginal->GetDescription(pszDescription);
    DEBUG_LOG("Network::GetDescription hr=0x%08lx desc='%s'", static_cast<unsigned long>(hr),
        (SUCCEEDED(hr) && pszDescription != nullptr) ? DbgNarrowW(*pszDescription).c_str() : "(null)");
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetwork::SetDescription(BSTR szDescription) {
    DEBUG_LOG("Network::SetDescription desc='%s'", DbgNarrowW(szDescription).c_str());
    return m_pOriginal->SetDescription(szDescription);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetNetworkConnections(IEnumNetworkConnections** ppEnumNetworkConnections) {
    HRESULT hr = m_pOriginal->GetNetworkConnections(ppEnumNetworkConnections);
    DEBUG_LOG("Network::GetNetworkConnections hr=0x%08lx out=%p", static_cast<unsigned long>(hr), (void*)(ppEnumNetworkConnections != nullptr ? *ppEnumNetworkConnections : nullptr));
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetTimeCreatedAndConnected(DWORD* pdwLowDateTimeCreated,
    DWORD* pdwHighDateTimeCreated,
    DWORD* pdwLowDateTimeConnected,
    DWORD* pdwHighDateTimeConnected) {
    HRESULT hr = m_pOriginal->GetTimeCreatedAndConnected(pdwLowDateTimeCreated, pdwHighDateTimeCreated, pdwLowDateTimeConnected, pdwHighDateTimeConnected);
    DEBUG_LOG("Network::GetTimeCreatedAndConnected hr=0x%08lx", static_cast<unsigned long>(hr));
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetwork::get_IsConnected(VARIANT_BOOL* pbIsConnected) {
    HRESULT hr = m_pOriginal->get_IsConnected(pbIsConnected);
    short val = (SUCCEEDED(hr) && pbIsConnected != nullptr) ? *pbIsConnected : 0;
    DEBUG_LOG("Network::get_IsConnected hr=0x%08lx val=%d", static_cast<unsigned long>(hr), (int)val);
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetwork::get_IsConnectedToInternet(VARIANT_BOOL* pbIsConnected) {
    HRESULT hr = m_pOriginal->get_IsConnected(pbIsConnected);
    short val = (SUCCEEDED(hr) && pbIsConnected != nullptr) ? *pbIsConnected : 0;
    DEBUG_LOG("Network::get_IsConnectedToInternet hr=0x%08lx val=%d (mapped to IsConnected)", static_cast<unsigned long>(hr), (int)val);
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetCategory(NLM_NETWORK_CATEGORY* pCategory) {
    HRESULT hr = m_pOriginal->GetCategory(pCategory);
    int val = (SUCCEEDED(hr) && pCategory != nullptr) ? (int)*pCategory : -1;
    DEBUG_LOG("Network::GetCategory hr=0x%08lx val=%d", static_cast<unsigned long>(hr), val);
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeNetwork::SetCategory(NLM_NETWORK_CATEGORY NewCategory) {
    DEBUG_LOG("Network::SetCategory val=%d", (int)NewCategory);
    return m_pOriginal->SetCategory(NewCategory);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetTypeInfoCount(UINT* pctinfo) {
    return m_pOriginal->GetTypeInfoCount(pctinfo);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) {
    return m_pOriginal->GetTypeInfo(iTInfo, lcid, ppTInfo);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) {
    return m_pOriginal->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) {
    return m_pOriginal->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}