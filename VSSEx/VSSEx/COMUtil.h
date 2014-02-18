

#pragma once


#include <Windows.h>
#include "CCString2.h"


HRESULT RegisterCOMClass(PCWSTR, DWORD, PCWSTR, const GUID*, PCWSTR);
HRESULT UnregisterCOMClass(const GUID*);
HRESULT SetRegistryValue(HKEY hKey, PCWSTR wszSubKey, PCWSTR wszValue, PCWSTR wszData);
HRESULT GetRegistryValue(HKEY hRootKey, PCWSTR wszSubKey, PCWSTR wszValue, CCString* s);


