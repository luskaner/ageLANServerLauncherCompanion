#pragma once
#include "pch.h"

class FakeEnumNetworks : public IEnumNetworks {
private:
    IEnumNetworks* m_pOriginal;

public:
    FakeEnumNetworks(IEnumNetworks* pOriginal);
    virtual ~FakeEnumNetworks() = default;

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;

    ULONG STDMETHODCALLTYPE AddRef() override;

    ULONG STDMETHODCALLTYPE Release() override;

    HRESULT STDMETHODCALLTYPE Next(ULONG celt, INetwork** rgelt, ULONG* pceltFetched) override;

    HRESULT STDMETHODCALLTYPE Skip(ULONG celt) override;

    HRESULT STDMETHODCALLTYPE Reset() override;

    HRESULT STDMETHODCALLTYPE Clone(IEnumNetworks** ppenum) override;

    HRESULT STDMETHODCALLTYPE get__NewEnum(IEnumVARIANT** ppEnum) override;

    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) override;

    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;

    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;

    HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;
};