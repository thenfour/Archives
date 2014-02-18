

#pragma once


#include <windows.h>
#include "guids.h"

#define VSSEX_NOTIFICATION_CLASS L"VSSEX_Notification Window"
#define VSSM_RESETDB WM_USER + 100

/*
    For keeping track of the DLL's global ref count.
*/
DWORD DllAddRef();
DWORD DllRelease();

HRESULT EnsureDbOpened();
HRESULT ResetDb();

