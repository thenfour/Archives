

#include "VSSExIconOverlayCheckedIn.h"
#include <shlwapi.h>
#include "Main.h"
#include <stdio.h>
#include "VSSDB.h"

extern HINSTANCE g_hInstance;
extern VSSDB g_Db;


CVSSExIconOverlayCheckedIn::CVSSExIconOverlayCheckedIn()
{
    this->dwRefCount = 1;
    EnsureDbOpened();
    DllAddRef();
}


CVSSExIconOverlayCheckedIn::~CVSSExIconOverlayCheckedIn()
{
    DllRelease();
}


// IUnknown methods
STDMETHODIMP CVSSExIconOverlayCheckedIn::QueryInterface(REFIID riid, PVOID* ppOut)
{
    *ppOut = NULL;

    if(IsEqualIID(riid, IID_IUnknown))
    {
        *ppOut = (IUnknown*)this;
    }

    else if(IsEqualIID(riid, IID_IShellIconOverlayIdentifier))
    {
        *ppOut = (IShellIconOverlayIdentifier*)this;
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


STDMETHODIMP_(DWORD) CVSSExIconOverlayCheckedIn::AddRef()
{
    return ++this->dwRefCount;
}


STDMETHODIMP_(DWORD) CVSSExIconOverlayCheckedIn::Release()
{
    if(--this->dwRefCount == 0)
    {
        delete this;
        return 0;
    }

    return this->dwRefCount;
}


STDMETHODIMP CVSSExIconOverlayCheckedIn::GetOverlayInfo(PWSTR wszPath, int cchMax, int* pnIcon, DWORD* pfFlags)
{
    //OutputDebugString(L"VSSEX: CVSSExIconOverlayCheckedIn GetOverlayInfo.\n");
    GetModuleFileName(g_hInstance, wszPath, cchMax);

    //CCString s;
    //s.FormatW(L"    Returning icon %d from %s\n", 1, wszPath);
    //OutputDebugString(s);

    *pnIcon = 1;
    *pfFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;

    return S_OK;
}


STDMETHODIMP CVSSExIconOverlayCheckedIn::GetPriority(int* pPri)
{
    *pPri = 50;
    return S_OK;
}


STDMETHODIMP CVSSExIconOverlayCheckedIn::IsMemberOf(PCWSTR wszFileName, DWORD dwAttrib)
{
    EnsureDbOpened();

    // We'll only use the overlay if the file does not have read-only attributes.
    //WCHAR wszBuf[200];
    //swprintf(wszBuf, L"VSSEX(CheckedIn)::IsMemberOf(%s, 0x%08x)\n", wszFileName, dwAttrib);
    //OutputDebugString(wszBuf);

    //if(dwAttrib & FILE_ATTRIBUTE_DIRECTORY) return S_FALSE;

    if(g_Db.IsIntegrated(wszFileName) == TRUE)
    {
        //OutputDebugString(L"    The file is integrated\n");
        if(g_Db.IsCheckedOut(wszFileName) == FALSE)
        {
            //OutputDebugString(L"    And it's not checked out.  Returning S_OK\n");
            return S_OK;
        }

        //OutputDebugString(L"    And it's checked out.. Returning S_FALSE.\n");
        return S_FALSE;
    }

    //OutputDebugString(L"    The file is not integrated.  Returning S_FALSE\n");
    return S_FALSE;
}


