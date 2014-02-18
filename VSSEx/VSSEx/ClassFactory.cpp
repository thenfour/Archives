/*
    The only thing about this that needs to be changed is:

    1) Include appropriate header files for objects
    2) Add CREATEINSTANCE macros for the implemented interfaces.
*/

#include "ClassFactory.h"

#include "VSSExIconOverlayCheckedOut.h"
#include "VSSExIconOverlayCheckedIn.h"
#include "VSSExContext.h"

#include "Main.h"
#include "CCString2.h"


CClassFactory::CClassFactory()
{
    this->dwRefCount = 1;
    DllAddRef();
}


CClassFactory::~CClassFactory()
{
    DllRelease();
}


STDMETHODIMP CClassFactory::QueryInterface(REFIID riid, PVOID* ppOut)
{
    *ppOut = NULL;

    if(IsEqualIID(riid, IID_IUnknown))
    {
        *ppOut = (IUnknown*)this;
    }

    else if(IsEqualIID(riid, IID_IClassFactory))
    {
        *ppOut = (IClassFactory*)this;
    }   

    /*
        Add a reference for the new interface
    */
    if(*ppOut)
    {
        (*(PUNKNOWN*)ppOut)->AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}


STDMETHODIMP_(DWORD) CClassFactory::AddRef()
{
    return ++this->dwRefCount;
}


STDMETHODIMP_(DWORD) CClassFactory::Release()
{
    if(--this->dwRefCount == 0)
    {
        delete this;
        return 0;
    }

    return this->dwRefCount;
}

#define CREATEINSTANCE(clsname, guid)                                      \
if(IsEqualIID(this->clsid, guid))                                          \
{                                                                          \
    clsname* pTemp_XXX;                                                      \
    pTemp_XXX = (clsname*)new clsname;                                         \
    if(pTemp_XXX == 0) return E_OUTOFMEMORY;                                   \
    hr = pTemp_XXX->QueryInterface(riid, ppOut);                               \
    pTemp_XXX->Release();                                                      \
    return hr;                                                             \
}

STDMETHODIMP CClassFactory::CreateInstance(PUNKNOWN pUnknown, REFIID riid, LPVOID* ppOut)
{
    HRESULT  hr = E_FAIL;
    PVOID pTemp = NULL;

    //CCString sPassedIn;
    //sPassedIn.FromGuid((GUID*)&riid);
    //CCString s;
    //s.FormatW(L"VSSEX: CClassFactory::CreateInstance(%s)\n", (PCWSTR)sPassedIn);
    //OutputDebugString(s);

    *ppOut = NULL;

    if(pUnknown != NULL)
    {
        //OutputDebugString(L"VSSEX: No aggregation!\n");
        return CLASS_E_NOAGGREGATION;
    }

    CREATEINSTANCE(CVSSExIconOverlayCheckedOut, CLSID_VSSExIconOverlayCheckedOut);
    CREATEINSTANCE(CVSSExIconOverlayCheckedIn, CLSID_VSSExIconOverlayCheckedIn);
    CREATEINSTANCE(CVSSExContext, CLSID_VSSExContext);

    return hr;
}


STDMETHODIMP CClassFactory::LockServer(BOOL)
{
    return E_NOTIMPL;
}


void CClassFactory::SetCLSID(CLSID* pGuid)
{
    memcpy(&this->clsid, pGuid, sizeof(CLSID));
}
