#include "pch.h"
#include "fakeenumnetworks.h"
#include "fakenetwork.h"
#include <string>

FakeEnumNetworks::FakeEnumNetworks(IEnumNetworks* pOriginal) : m_pOriginal(pOriginal) {}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::QueryInterface(REFIID riid, void** ppvObject)  {
    if (riid == __uuidof(IUnknown) || riid == __uuidof(IDispatch) || riid == __uuidof(IEnumNetworks)) {
        *ppvObject = static_cast<IEnumNetworks*>(this);
        AddRef();
        return S_OK;
    }
    return m_pOriginal->QueryInterface(riid, ppvObject);
}

ULONG STDMETHODCALLTYPE FakeEnumNetworks::AddRef()  {
    return m_pOriginal->AddRef();
}

ULONG STDMETHODCALLTYPE FakeEnumNetworks::Release()  {
    ULONG count = m_pOriginal->Release();
    if (count == 0) {
        delete this;
    }
    return count;
}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::Next(ULONG celt, INetwork** rgelt, ULONG* pceltFetched)  {
    HRESULT res = m_pOriginal->Next(celt, rgelt, pceltFetched);
    INetwork* pOriginalINetwork = *rgelt;
    FakeNetwork* proxy = new FakeNetwork(pOriginalINetwork);
    *rgelt = proxy;
    return res;
}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::Skip(ULONG celt)  {
    return m_pOriginal->Skip(celt);
}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::Reset()  {
    return m_pOriginal->Reset();
}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::Clone(IEnumNetworks** ppenum)  {
    return m_pOriginal->Clone(ppenum);
}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::get__NewEnum(IEnumVARIANT** ppEnum)  {
    return m_pOriginal->get__NewEnum(ppEnum);
}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::GetTypeInfoCount(UINT* pctinfo)  {
    return m_pOriginal->GetTypeInfoCount(pctinfo);
}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)  {
    return m_pOriginal->GetTypeInfo(iTInfo, lcid, ppTInfo);
}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)  {
    return m_pOriginal->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)  {
    return m_pOriginal->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}