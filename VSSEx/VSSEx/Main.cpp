

#include "Guids.h"
#include "ClassFactory.h"
#include "VSSExIconOverlayCheckedOut.h"
#include "VSSExIconOverlayCheckedIn.h"
#include "VSSExContext.h"
#include "VSSDB.h"
#include "COMUtil.h"
#include "Main.h"
#include "shlwapi.h"


HINSTANCE g_hInstance;
DWORD g_DllRefCount;
VSSDB g_Db;
BOOL g_bDbOpened;
HWND g_hNotification;


LPARAM CALLBACK NotificationProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case VSSM_RESETDB:
        {
            ResetDb();
            return 0;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


HRESULT ResetDb()
{
    if(g_bDbOpened == TRUE)
    {
        //OutputDebugString(L"VSSEX: ResetDb(); - The database was open; we're closing it.\n");
        g_Db.Close();
        g_bDbOpened = FALSE;
            //OutputDebugString(L"g_DbOpened changed to FALSE\n");
        return S_OK;
    }

    //OutputDebugString(L"VSSEX: ResetDb(); - The database wasn't open.\n");
    return S_OK;
}

HRESULT EnsureDbOpened()
{
    DATA_BLOB Password;
    HKEY hKey;
    WCHAR wszIni[MAX_PATH];
    WCHAR wszUser[MAX_PATH];
    WCHAR wszRoot[MAX_PATH];
    DWORD dwSize;

    //CCString s;
    //s.FormatW(L"VSSEX::EnsureDbOpened() The database is currently: %s\n",
    //    g_bDbOpened == TRUE ? L"TRUE" : L"FALSE");
    //OutputDebugString(s);

    if(g_bDbOpened == FALSE)
    {   
        // Open the database.
        if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER,
            L"Software\\Carl Corcoran\\VSSEx", 0, 0,
            0, KEY_QUERY_VALUE, NULL, &hKey, NULL))
        {
            // Failed to get the key.
            return E_FAIL;
        }

        dwSize = MAX_PATH * sizeof(WCHAR);
        if(ERROR_SUCCESS != RegQueryValueEx(hKey, L"Ini", 0, NULL, (PBYTE)wszIni, &dwSize))
        {
            RegCloseKey(hKey);
            return E_FAIL;
        }

        dwSize = MAX_PATH * sizeof(WCHAR);
        if(ERROR_SUCCESS != RegQueryValueEx(hKey, L"User", 0, NULL, (PBYTE)wszUser, &dwSize))
        {
            RegCloseKey(hKey);
            return E_FAIL;
        }

        dwSize = MAX_PATH * sizeof(WCHAR);
        if(ERROR_SUCCESS != RegQueryValueEx(hKey, L"Root", 0, NULL, (PBYTE)wszRoot, &dwSize))
        {
            RegCloseKey(hKey);
            return E_FAIL;
        }

        if(ERROR_SUCCESS == RegQueryValueEx(hKey, L"Pass", 0, NULL, NULL, &dwSize))
        {
            PBYTE pPass = (PBYTE)malloc(dwSize);
            if(ERROR_SUCCESS == RegQueryValueEx(hKey, L"Pass", 0, NULL, (PBYTE)pPass, &dwSize))
            {
                // Decrypt it.
                DATA_BLOB DataIn;

                DataIn.cbData = dwSize;
                DataIn.pbData = pPass;

                CryptUnprotectData(&DataIn, NULL, NULL, NULL, NULL, 0, &Password);
            }
        }

        RegCloseKey(hKey);

        if(FAILED(g_Db.Open(wszIni, wszUser, (PCWSTR)Password.pbData)))
        {
            return E_FAIL;
        }

        LocalFree(Password.pbData);

        g_bDbOpened = TRUE;

        // Set the working folder.
        if(FAILED(g_Db.SetRoot(wszRoot)))
        {
            return E_FAIL;
        }
    }

    // Make sure our notification window is open.
    if(g_hNotification == NULL)
    {
        WNDCLASSEX wcx = {0};
        wcx.cbSize = sizeof(wcx);
        wcx.hInstance = g_hInstance;
        wcx.lpfnWndProc = NotificationProc;
        wcx.lpszClassName = VSSEX_NOTIFICATION_CLASS;
        RegisterClassEx(&wcx);

        g_hNotification = CreateWindowEx(0, VSSEX_NOTIFICATION_CLASS, VSSEX_NOTIFICATION_CLASS,
            0, 0, 0, 0, 0, HWND_MESSAGE, 0, g_hInstance, 0);
    }

    //OutputDebugString(L"VSSEX: EnsureDbOpened() Succeeded.\n");
    return S_OK;
}


DWORD DllAddRef()
{
    return ++g_DllRefCount;
}


DWORD DllRelease()
{
    return --g_DllRefCount;
}


BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, DWORD dwReserved)
{
    switch(fdwReason)
    {
    case DLL_PROCESS_DETACH:
        {
            if(g_hNotification)
            {
                DestroyWindow(g_hNotification);
                UnregisterClass(VSSEX_NOTIFICATION_CLASS, g_hInstance);
            }

            g_Db.Close();
            g_bDbOpened = FALSE;
            //OutputDebugString(L"g_DbOpened changed to FALSE\n");
        }
    case DLL_PROCESS_ATTACH:
        {
            //OutputDebugString(L"VSSEX: DllMain, Process attach\n");
            g_hInstance = hInstance;
            g_hNotification = NULL;
            g_bDbOpened = FALSE;
            //OutputDebugString(L"g_DbOpened changed to FALSE\n");
            g_DllRefCount = 0;
        }
    }

    return TRUE;
}


/*
    Returns S_OK if this DLL can unload.
*/
STDAPI DllCanUnloadNow()
{
    return g_DllRefCount == 0 ? S_OK : S_FALSE;
}


/*
    Returns a newly allocated Class Factory.
*/
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    //CCString sPassedIn;
    //sPassedIn.FromGuid((GUID*)&rclsid);
    //CCString s;
    //s.FormatW(L"VSSEX: DllGetClassObject(%s)\n", (PCWSTR)sPassedIn);
    //OutputDebugString(s);

    HRESULT hr = E_FAIL;
    *ppv = NULL;

    /*
        Make sure they're asking for either unknown or classfactory.
    */
    if(!IsEqualIID(riid, IID_IUnknown) &&
       !IsEqualIID(riid, IID_IClassFactory))
    {
        return E_NOINTERFACE;
    }

    if(
        IsEqualCLSID(rclsid, CLSID_VSSExIconOverlayCheckedOut) ||
        IsEqualCLSID(rclsid, CLSID_VSSExContext)  ||
        IsEqualCLSID(rclsid, CLSID_VSSExIconOverlayCheckedIn)
        )
    {
        // They're asking for the right thing; let's make it...
        //OutputDebugString(L"VSSEX: DllGetClassObject CLSID is valid... creating IClassFactory\n");

        CClassFactory* pCF = new CClassFactory;
        pCF->SetCLSID((GUID*)&rclsid);

        if(pCF == NULL)
        {
            return E_OUTOFMEMORY;
        }

        hr = pCF->QueryInterface(riid, ppv);

        if(SUCCEEDED(hr))
        {
            pCF->Release();
        }

        return hr;
    }

    return E_NOINTERFACE;
}


/*
    HKEY_
*/
STDAPI DllRegisterServer(void)
{
    WCHAR wszFileName[MAX_PATH];
    WCHAR wszGuid[100];

    GetModuleFileName(g_hInstance, wszFileName, MAX_PATH);

    /*
        CVSSExIconOverlayCheckedIn
    */
    StringFromGUID2(CLSID_VSSExIconOverlayCheckedIn, wszGuid, 100 * sizeof(WCHAR));

    if(FAILED(RegisterCOMClass(
        L"VSSExIconOverlayCheckedIn",
        1,
        L"VSS IconOverlay Checked-In Class",
        &CLSID_VSSExIconOverlayCheckedIn, wszFileName
        )))
    {
        return E_FAIL;
    }

    // Register the overlay classes
    if(FAILED(SetRegistryValue(
        HKEY_LOCAL_MACHINE,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers\\VSSExIconOverlayCheckedIn",
        L"",
        wszGuid
        )))
    {
        return E_FAIL;
    }

    /*
        CVSSExIconOverlayCheckedOut
    */
    StringFromGUID2(CLSID_VSSExIconOverlayCheckedOut, wszGuid, 100 * sizeof(WCHAR));

    if(FAILED(RegisterCOMClass(
        L"VSSExIconOverlayCheckedOut",
        1,
        L"VSS IconOverlay Checked-Out Class",
        &CLSID_VSSExIconOverlayCheckedOut, wszFileName
        )))
    {
        return E_FAIL;
    }

    // Register the overlay classes
    if(FAILED(SetRegistryValue(
        HKEY_LOCAL_MACHINE,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers\\VSSExIconOverlayCheckedOut",
        L"",
        wszGuid
        )))
    {
        return E_FAIL;
    }

    /*
        CVSSExContext
    */
    StringFromGUID2(CLSID_VSSExContext, wszGuid, 100 * sizeof(WCHAR));

    if(FAILED(RegisterCOMClass(
        L"VSSExContext",
        1,
        L"VSS Context Menu Class",
        &CLSID_VSSExContext, wszFileName
        )))
    {
        return E_FAIL;
    }

    // Register the context menu classes
    if(FAILED(SetRegistryValue(
        HKEY_CLASSES_ROOT,
        L"AllFileSystemObjects\\shellex\\ContextMenuHandlers\\VSSExContext",
        L"",
        wszGuid
        )))
    {
        return E_FAIL;
    }

    // Add it to the approved extensions:
    CCString Approved;
    Approved.FormatW(L"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved\\%s", wszGuid);

    if(FAILED(SetRegistryValue(
        HKEY_LOCAL_MACHINE,
        Approved,
        L"",
        L"VSSExContext"
        )))
    {
        return E_FAIL;
    }

    return S_OK;
}


STDAPI DllUnregisterServer(void)
{
    CCString sGuid;
    CCString s;

    UnregisterCOMClass(&CLSID_VSSExIconOverlayCheckedIn);
    UnregisterCOMClass(&CLSID_VSSExIconOverlayCheckedOut);
    UnregisterCOMClass(&CLSID_VSSExContext);

    SHDeleteKey(HKEY_LOCAL_MACHINE,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\"
        L"Explorer\\ShellIconOverlayIdentifiers\\VSSExIconOverlayCheckedIn");

    SHDeleteKey(HKEY_LOCAL_MACHINE,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\"
        L"Explorer\\ShellIconOverlayIdentifiers\\VSSExIconOverlayCheckedOut");


    sGuid.FromGuid((GUID*)&CLSID_VSSExContext);
    s.FormatW(L"Software\\Microsoft\\Windows\\CurrentVersion\\"
        L"Shell Extensions\\Approved\\%s", (PCWSTR)sGuid);

    SHDeleteKey(HKEY_LOCAL_MACHINE, s);

    SHDeleteKey(HKEY_CLASSES_ROOT,
        L"AllFileSystemObjects\\shellex\\ContextMenuHandlers\\VSSExContext");

    return S_OK;
}

