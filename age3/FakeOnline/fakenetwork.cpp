#include "pch.h"
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
    return m_pOriginal->GetNetworkId(pgdGuidNetworkId);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetName(BSTR* pszNetworkName) {
    return m_pOriginal->GetName(pszNetworkName);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::SetName(BSTR szNetworkNewName) {
    return m_pOriginal->SetName(szNetworkNewName);
}

HRESULT FakeNetwork::GetDomainType(
    NLM_DOMAIN_TYPE* pNetworkType
) {
    return m_pOriginal->GetDomainType(pNetworkType);
}

HRESULT FakeNetwork::GetConnectivity(
    NLM_CONNECTIVITY* pConnectivity
) {
    return m_pOriginal->GetConnectivity(pConnectivity);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetDescription(BSTR* pszDescription) {
    return m_pOriginal->GetDescription(pszDescription);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::SetDescription(BSTR szDescription) {
    return m_pOriginal->SetDescription(szDescription);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetNetworkConnections(IEnumNetworkConnections** ppEnumNetworkConnections) {
    return m_pOriginal->GetNetworkConnections(ppEnumNetworkConnections);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetTimeCreatedAndConnected(DWORD* pdwLowDateTimeCreated,
    DWORD* pdwHighDateTimeCreated,
    DWORD* pdwLowDateTimeConnected,
    DWORD* pdwHighDateTimeConnected) {
    return m_pOriginal->GetTimeCreatedAndConnected(pdwLowDateTimeCreated, pdwHighDateTimeCreated, pdwLowDateTimeConnected, pdwHighDateTimeConnected);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::get_IsConnected(VARIANT_BOOL* pbIsConnected) {
    return m_pOriginal->get_IsConnected(pbIsConnected);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::get_IsConnectedToInternet(VARIANT_BOOL* pbIsConnected) {
    return m_pOriginal->get_IsConnected(pbIsConnected);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::GetCategory(NLM_NETWORK_CATEGORY* pCategory) {
    return m_pOriginal->GetCategory(pCategory);
}

HRESULT STDMETHODCALLTYPE FakeNetwork::SetCategory(NLM_NETWORK_CATEGORY NewCategory) {
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