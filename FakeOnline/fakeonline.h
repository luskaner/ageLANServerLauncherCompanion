#pragma once
#include "pch.h"

class FakeNetworkListManager : public INetworkListManager
{
public:
    FakeNetworkListManager(INetworkListManager* original);

    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) override;

    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;

    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;

    HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;

    HRESULT STDMETHODCALLTYPE get_IsConnectedToInternet(VARIANT_BOOL* pbIsConnected) override;

    HRESULT STDMETHODCALLTYPE get_IsConnected(VARIANT_BOOL* pbIsConnected) override;

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;

    ULONG STDMETHODCALLTYPE AddRef() override;

    ULONG STDMETHODCALLTYPE Release() override;

    HRESULT STDMETHODCALLTYPE GetNetworks(NLM_ENUM_NETWORK Flags, IEnumNetworks** ppEnumNetwork) override;

    HRESULT STDMETHODCALLTYPE GetNetwork(GUID gdNetworkId, INetwork** ppNetwork) override;

    HRESULT STDMETHODCALLTYPE GetNetworkConnections(IEnumNetworkConnections** ppEnum) override;

    HRESULT STDMETHODCALLTYPE GetNetworkConnection(GUID gdNetworkConnectionId, INetworkConnection** ppNetworkConnection) override;

    HRESULT STDMETHODCALLTYPE GetConnectivity(NLM_CONNECTIVITY* pConnectivity) override;

    HRESULT STDMETHODCALLTYPE SetSimulatedProfileInfo(NLM_SIMULATED_PROFILE_INFO* pSimulatedInfo) override;

    HRESULT STDMETHODCALLTYPE ClearSimulatedProfileInfo() override;

    virtual ~FakeNetworkListManager() = default;
private:
    INetworkListManager* original;
};