

#include "AutoComplete.h"


CVSSEnumString::CVSSEnumString()
{
    int nIndex = 0;
    this->hKey = NULL;

    this->dwRefCount = 1;
}


CVSSEnumString::~CVSSEnumString()
{
    // Make sure the key is closed
    if(this->hKey)
    {
        RegCloseKey(hKey);
    }
}


STDMETHODIMP CVSSEnumString::QueryInterface(REFIID riid, PVOID* ppOut)
{
    *ppOut = NULL;

    if(IsEqualIID(riid, IID_IUnknown))
    {
        *ppOut = this;
    }

    else if(IsEqualIID(riid, IID_IEnumString))
    {
        *ppOut = (IEnumString*)this;
    }

    /*
        Add a reference for the new interface
    */
    if(*ppOut)
    {
        (*(LPUNKNOWN*)ppOut)->AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}


STDMETHODIMP_(DWORD) CVSSEnumString::AddRef()
{
    return ++this->dwRefCount;
}


STDMETHODIMP_(DWORD) CVSSEnumString::Release()
{
    if(--this->dwRefCount == 0)
    {
        delete this;
        return 0;
    }
   
    return this->dwRefCount;
}


STDMETHODIMP CVSSEnumString::Next(ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched)
{
    WCHAR wszName[1];
    DWORD cbName;
    WCHAR wszData[MAX_PATH];
    DWORD cbData;

    this->_EnsureKeyOpen();

    for(ULONG i=0;i<celt;i++)
    {
        cbName = 1 * sizeof(WCHAR);
        cbData = MAX_PATH * sizeof(WCHAR);

        if(ERROR_SUCCESS != RegEnumValue(hKey, this->nIndex, wszName, &cbName, 0,
            NULL, (PBYTE)wszData, &cbData))
        {
            *pceltFetched = i;
            return E_FAIL;
        }

        rgelt[i] = SysAllocString(wszData);

        this->nIndex++;
    }

    *pceltFetched = celt;
    return S_OK;
}


STDMETHODIMP CVSSEnumString::Skip(ULONG celt)
{
    this->nIndex += celt;
    return S_OK;
}


STDMETHODIMP CVSSEnumString::Reset()
{
    this->nIndex = 0;
    return S_OK;
}


STDMETHODIMP CVSSEnumString::Clone(IEnumString ** ppenum)
{
    CVSSEnumString* pNew = new CVSSEnumString;
    pNew->nIndex = this->nIndex;
    pNew->QueryInterface(IID_IEnumString, (PVOID*)ppenum);
    pNew->Release();

    return S_OK;
}


HRESULT CVSSEnumString::_EnsureKeyOpen()
{
    if(this->hKey) return S_OK;

    // Open the key.
    if(ERROR_SUCCESS != 
        RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Carl Corcoran\\VSSEx\\ExclusionList",
        0, KEY_QUERY_VALUE, &this->hKey))
    {
        return E_FAIL;
    }

    return S_OK;
}

