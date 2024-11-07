#pragma once
#include "pch.h"

class FakeNetwork : public INetwork {
private:
    INetwork* m_pOriginal;

public:
    FakeNetwork(INetwork* pOriginal);
    virtual ~FakeNetwork() = default;

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;

    ULONG STDMETHODCALLTYPE AddRef() override;

    ULONG STDMETHODCALLTYPE Release() override;

    HRESULT STDMETHODCALLTYPE GetNetworkId(GUID* pgdGuidNetworkId) override;

    HRESULT STDMETHODCALLTYPE GetName(BSTR* pszNetworkName) override;

    HRESULT STDMETHODCALLTYPE SetName(BSTR szNetworkNewName) override;

    HRESULT GetDomainType(
        NLM_DOMAIN_TYPE* pNetworkType
    )override;

    HRESULT GetConnectivity(
        NLM_CONNECTIVITY* pConnectivity
    )override;

    HRESULT STDMETHODCALLTYPE GetDescription(BSTR* pszDescription) override;

    HRESULT STDMETHODCALLTYPE SetDescription(BSTR szDescription) override;

    HRESULT STDMETHODCALLTYPE GetNetworkConnections(IEnumNetworkConnections** ppEnumNetworkConnections) override;

    HRESULT STDMETHODCALLTYPE GetTimeCreatedAndConnected(DWORD* pdwLowDateTimeCreated,
        DWORD* pdwHighDateTimeCreated,
        DWORD* pdwLowDateTimeConnected,
        DWORD* pdwHighDateTimeConnected) override;

    HRESULT STDMETHODCALLTYPE get_IsConnected(VARIANT_BOOL* pbIsConnected) override;

    HRESULT STDMETHODCALLTYPE get_IsConnectedToInternet(VARIANT_BOOL* pbIsConnected) override;

    HRESULT STDMETHODCALLTYPE GetCategory(NLM_NETWORK_CATEGORY* pCategory) override;

    HRESULT STDMETHODCALLTYPE SetCategory(NLM_NETWORK_CATEGORY NewCategory) override;

    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) override;

    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;

    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;

    HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;
};