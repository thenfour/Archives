

#pragma once


#include <Windows.h>
#include <shlobj.h>
#include "Guids.h"


class CVSSExIconOverlayCheckedIn : public IShellIconOverlayIdentifier
{
public:

    CVSSExIconOverlayCheckedIn();
    ~CVSSExIconOverlayCheckedIn();

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID, PVOID*);
    STDMETHODIMP_(DWORD) AddRef();
    STDMETHODIMP_(DWORD) Release();

    // IShellIconOverlayIdentifier methods
    STDMETHODIMP GetOverlayInfo(PWSTR, int, int*, DWORD*);
    STDMETHODIMP GetPriority(int*);
    STDMETHODIMP IsMemberOf(PCWSTR, DWORD);

private:

    DWORD dwRefCount;
};


