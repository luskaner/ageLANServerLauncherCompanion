#include "pch.h"
#include "debuglog.h"
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
    DEBUG_LOG("Enum::Next enter celt=%lu out=%p fetched=%p", static_cast<unsigned long>(celt), (void*)rgelt, (void*)pceltFetched);
    HRESULT res = m_pOriginal->Next(celt, rgelt, pceltFetched);
    ULONG fetched = (pceltFetched != nullptr) ? *pceltFetched : 0;
    INetwork* raw = (SUCCEEDED(res) && rgelt != nullptr) ? *rgelt : nullptr;
    DEBUG_LOG("Enum::Next exit hr=0x%08lx fetched=%lu raw=%p", static_cast<unsigned long>(res), static_cast<unsigned long>(fetched), (void*)raw);
    if (FAILED(res) || rgelt == nullptr || *rgelt == nullptr) {
        DEBUG_LOG("Enum::Next: nothing to wrap");
        return res;
    }
    INetwork* pOriginalINetwork = *rgelt;
    FakeNetwork* proxy = new FakeNetwork(pOriginalINetwork);
    *rgelt = proxy;
    DEBUG_LOG("Enum::Next: wrapped %p -> %p", (void*)pOriginalINetwork, (void*)proxy);
    return res;
}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::Skip(ULONG celt)  {
    HRESULT hr = m_pOriginal->Skip(celt);
    DEBUG_LOG("Enum::Skip celt=%lu hr=0x%08lx", static_cast<unsigned long>(celt), static_cast<unsigned long>(hr));
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::Reset()  {
    HRESULT hr = m_pOriginal->Reset();
    DEBUG_LOG("Enum::Reset hr=0x%08lx", static_cast<unsigned long>(hr));
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::Clone(IEnumNetworks** ppenum)  {
    HRESULT hr = m_pOriginal->Clone(ppenum);
    DEBUG_LOG("Enum::Clone hr=0x%08lx out=%p", static_cast<unsigned long>(hr), (void*)(ppenum != nullptr ? *ppenum : nullptr));
    return hr;
}

HRESULT STDMETHODCALLTYPE FakeEnumNetworks::get__NewEnum(IEnumVARIANT** ppEnum)  {
    HRESULT hr = m_pOriginal->get__NewEnum(ppEnum);
    DEBUG_LOG("Enum::get__NewEnum hr=0x%08lx out=%p", static_cast<unsigned long>(hr), (void*)(ppEnum != nullptr ? *ppEnum : nullptr));
    return hr;
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