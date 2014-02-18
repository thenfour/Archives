

#include "COMUtil.h"
#include "stdio.h"
#include "shlwapi.h"


HRESULT GetRegistryValue(HKEY hRootKey, PCWSTR wszSubKey, PCWSTR wszValue, CCString* s)
{
    //CCString s2;
    //s2.FormatW(L"GetRegistryValue(): %s\n", wszSubKey);
    //OutputDebugString(s2);

    HKEY hKey = NULL;

    if(ERROR_SUCCESS != RegCreateKeyEx(hRootKey, wszSubKey, 0, 0, 0, KEY_READ, NULL, &hKey, NULL))
    {
        return E_FAIL;
    }

    DWORD dwSize = 0;

    if(RegQueryValueEx(hKey, wszValue, 0, NULL, NULL, &dwSize) != ERROR_SUCCESS)
    {
        return E_FAIL;
    }

    PWSTR wsz = (PWSTR)malloc(sizeof(WCHAR) * (dwSize + 1));

    if(RegQueryValueEx(hKey, wszValue, 0, NULL, (PBYTE)wsz, &dwSize) != ERROR_SUCCESS)
    {
        return E_FAIL;
    }

    *s = wsz;
    free(wsz);

    RegCloseKey(hKey);

    return S_OK;
}


HRESULT SetRegistryValue(HKEY hRootKey, PCWSTR wszSubKey, PCWSTR wszValue, PCWSTR wszData)
{
    HKEY hKey = NULL;
    DWORD dwDisp = 0;

    //WCHAR wszBuf[400];
    //swprintf(wszBuf, L"HKEY_CLASSES_ROOT\\%s\n    '%s'=%s\n    ...", wszSubKey, wszValue, wszData);
    //OutputDebugString(wszBuf);

    if(ERROR_SUCCESS != RegCreateKeyEx(hRootKey, wszSubKey, 0, 0, 0, KEY_WRITE, NULL, &hKey, NULL))
    {
        //OutputDebugString(L"FAIL Creation\n");
        return E_FAIL;
    }

    if(ERROR_SUCCESS != RegSetValueEx(hKey, wszValue, 0, REG_SZ, (PBYTE)wszData, (wcslen(wszData) + 1) * sizeof(WCHAR)))
    {
        //OutputDebugString(L"FAIL SetValue\n");
        RegCloseKey(hKey);
        return E_FAIL;
    }

    //OutputDebugString(L"SUCCESS\n");
    RegCloseKey(hKey);
    return S_OK;
}


HRESULT UnregisterCOMClass(const GUID* pGuid)
{
    CCString sGuid;
    CCString sTemp;
    CCString sVIProgId;
    CCString sVDProgId;
    HRESULT hr = S_OK;

    CCString sKey;

    sGuid.FromGuid((GUID*)pGuid);

    sKey.FormatW(L"CLSID\\%s\\ProgId", (PCWSTR)sGuid);
    if(FAILED(GetRegistryValue(HKEY_CLASSES_ROOT, sKey, L"", &sVDProgId)))
    {
//        OutputDebugString(L"Couldn't get VD ProdId registry value\n");
        hr = E_FAIL;
    }

    sKey.FormatW(L"CLSID\\%s\\VersionIndependentProgId", (PCWSTR)sGuid);

    if(FAILED(GetRegistryValue(HKEY_CLASSES_ROOT, sKey, L"", &sVIProgId)))
    {
//        OutputDebugString(L"Couldn't get registry value\n");
        hr = E_FAIL;
    }

    sKey.FormatW(L"CLSID\\%s", (PCWSTR)sGuid);

    SHDeleteKey(HKEY_CLASSES_ROOT, sKey);

    SHDeleteKey(HKEY_CLASSES_ROOT, sVIProgId);
    SHDeleteKey(HKEY_CLASSES_ROOT, sVDProgId);

    return S_OK;
}
/*
    HKEY_CLASSES_ROOT\CLSID\{37B03544-A4C8-11D2-B634-00C04F79498E}
    @="Description"

    HKEY_CLASSES_ROOT\CLSID\{37B03544-A4C8-11D2-B634-00C04F79498E}\InProcServer32
    @="c:\\stuff\component.dll"
    ThreadingModel="Apartment"

    HKEY_CLASSES_ROOT\CLSID\{37B03544-A4C8-11D2-B634-00C04F79498E}\ProgId
    @="SdEx.Test.1"

    HKEY_CLASSES_ROOT\CLSID\{37B03544-A4C8-11D2-B634-00C04F79498E}\VersionIndependentProgID
    @="SdEx.Test"

    HKEY_CLASSES_ROOT\SdEx.Test
    @="Description"

    HKEY_CLASSES_ROOT\SdEx.Test\CLSID
    @="{37B03544-A4C8-11D2-B634-00C04F79498E}"

    HKEY_CLASSES_ROOT\SdEx.Test\CurVer
    @="SdEx.Test.1"

    HKEY_CLASSES_ROOT\SdEx.Test.1
    @="Description"

    HKEY_CLASSES_ROOT\SdEx.Test.1\CLSID
    @="{37B03544-A4C8-11D2-B634-00C04F79498E}"
*/
HRESULT RegisterCOMClass(
    PCWSTR wszVIProgId, DWORD dwVersion, PCWSTR wszDesc, const GUID* clsid, PCWSTR wszDll)
{
    WCHAR wszGuid[50];
    WCHAR wszClass[100];
    WCHAR wszTemp[200];
    PWSTR wszVDProgId = NULL;
    HRESULT hr = S_OK;

    // Validate.
    if(wszVIProgId == NULL) return E_FAIL;
    if(dwVersion > 10000) return E_FAIL;
    if(dwVersion == 0) return E_FAIL;
    if(wszDesc == NULL) return E_FAIL;
    if(clsid == NULL) return E_FAIL;
    if(wszDll == NULL) return E_FAIL;
    if(PathFileExists(wszDll) == FALSE) return E_FAIL;

    // Create a version-dependent prog ID
    wszVDProgId = (PWSTR)malloc(sizeof(WCHAR) * (wcslen(wszVIProgId) + 10));
    if(wszVDProgId == NULL) return E_FAIL;
    swprintf(wszVDProgId, L"%s.%d", wszVIProgId, dwVersion);

    // Get the guid to a string.
    StringFromGUID2(*clsid, wszGuid, 50 * sizeof(WCHAR));

    // Create the class key name
    swprintf(wszClass, L"CLSID\\%s", wszGuid);

    if(FAILED(SetRegistryValue(HKEY_CLASSES_ROOT, wszClass, L"", wszDesc)))
    {
        hr = E_FAIL;
    }

    swprintf(wszTemp, L"%s\\InProcServer32", wszClass);
    if(FAILED(SetRegistryValue(HKEY_CLASSES_ROOT, wszTemp, L"", wszDll)))
    {
        hr = E_FAIL;
    }

    if(FAILED(SetRegistryValue(HKEY_CLASSES_ROOT, wszTemp, L"ThreadingModel", L"Apartment")))
    {
        hr = E_FAIL;
    }

    swprintf(wszTemp, L"%s\\ProgId", wszClass);
    if(FAILED(SetRegistryValue(HKEY_CLASSES_ROOT, wszTemp, L"", wszVDProgId)))
    {
        hr = E_FAIL;
    }

    swprintf(wszTemp, L"%s\\VersionIndependentProgID", wszClass);
    if(FAILED(SetRegistryValue(HKEY_CLASSES_ROOT, wszTemp, L"", wszVIProgId)))
    {
        hr = E_FAIL;
    }

    if(FAILED(SetRegistryValue(HKEY_CLASSES_ROOT, wszVIProgId, L"", wszDesc)))
    {
        hr = E_FAIL;
    }

    swprintf(wszTemp, L"%s\\CLSID", wszVIProgId);
    if(FAILED(SetRegistryValue(HKEY_CLASSES_ROOT, wszTemp, L"", wszGuid)))
    {
        hr = E_FAIL;
    }

    swprintf(wszTemp, L"%s\\CurVer", wszVIProgId);
    if(FAILED(SetRegistryValue(HKEY_CLASSES_ROOT, wszTemp, L"", wszVDProgId)))
    {
        hr = E_FAIL;
    }

    if(FAILED(SetRegistryValue(HKEY_CLASSES_ROOT, wszVDProgId, L"", wszDesc)))
    {
        hr = E_FAIL;
    }

    swprintf(wszTemp, L"%s\\CLSID", wszVDProgId);
    if(FAILED(SetRegistryValue(HKEY_CLASSES_ROOT, wszTemp, L"", wszGuid)))
    {
        hr = E_FAIL;
    }

    return hr;
}
