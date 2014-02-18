

#include <windows.h>
#include "..\\VssEx\\CCString2.h"
#include "resource.h"
#include <Shlobj.h>
#include <objbase.h>
#include <htmlhelp.h>
#include <shlwapi.h>

HICON g_hIcon;
HINSTANCE g_hInstance;

#define VSSEX_NOTIFICATION_CLASS L"VSSEX_Notification Window"
#define VSSM_RESETDB WM_USER + 100

INT_PTR CALLBACK AboutDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
        {
			// Set the font to courier.
			SendMessage(GetDlgItem(hWnd, IDC_EULA), WM_SETFONT, (WPARAM)GetStockObject(ANSI_FIXED_FONT), FALSE);

            // Put the eula in the edit box.
            PWSTR wszEula = NULL;
            DWORD br;
            //WCHAR wszFile[MAX_PATH];
            HANDLE hFile = NULL;

            //GetModuleFileName(g_hInstance, wszFile, MAX_PATH);
            //*PathFindFileName(wszFile) = 0;
            //PathAppend(wszFile, L"eula.txt");

            hFile = CreateFile(L"Eula.txt", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL, OPEN_EXISTING, NULL, NULL);

            if(hFile == INVALID_HANDLE_VALUE) return TRUE;

            DWORD dwSize = GetFileSize(hFile, NULL);

            wszEula = (PWSTR)malloc(dwSize + sizeof(WCHAR));
            ZeroMemory(wszEula, dwSize + sizeof(WCHAR));

            ReadFile(hFile, wszEula, dwSize, &br, NULL);

            SetDlgItemText(hWnd, IDC_EULA, wszEula);

            free(wszEula);

            CloseHandle(hFile);

            return TRUE;
        }
    case WM_CLOSE:
        {
            EndDialog(hWnd, 0);
            return TRUE;
        }
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDOK:
                {
                    EndDialog(hWnd, 0);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

void OnAbout(HWND hWnd)
{
    DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_ABOUT), 0, AboutDlgProc, 0);
}

void OnHelp(HWND hWnd)
{
    HtmlHelp(GetDesktopWindow(), L"vssex.chm", HH_DISPLAY_TOPIC,NULL);
}


void OnApply(HWND hWnd)
{
    // Set all those registry keys.
    HKEY hKey;
    WCHAR wszIni[MAX_PATH];
    WCHAR wszUser[MAX_PATH];
    WCHAR wszRoot[MAX_PATH];
    WCHAR wszPass[MAX_PATH];

    GetDlgItemText(hWnd, IDC_DATABASE, wszIni, MAX_PATH);
    GetDlgItemText(hWnd, IDC_USER, wszUser, MAX_PATH);
    GetDlgItemText(hWnd, IDC_LOCALROOT, wszRoot, MAX_PATH);
    GetDlgItemText(hWnd, IDC_PASSWORD, wszPass, MAX_PATH);

    // Open the database.
    if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER,
        L"Software\\Carl Corcoran\\VSSEx", 0, 0,
        0, KEY_WRITE, NULL, &hKey, NULL))
    {
        // Failed to get the key.
        MessageBox(hWnd, L"Couldn't open the registry", 0, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    if(ERROR_SUCCESS != RegSetValueEx(hKey, L"Ini", 0, REG_SZ, (PBYTE)wszIni, (wcslen(wszIni) + 1) * sizeof(WCHAR)))
    {
        MessageBox(hWnd, L"Error setting the Ini", 0, MB_OK | MB_ICONEXCLAMATION);
    }

    if(ERROR_SUCCESS != RegSetValueEx(hKey, L"Root", 0, REG_SZ, (PBYTE)wszRoot, (wcslen(wszRoot) + 1) * sizeof(WCHAR)))
    {
        MessageBox(hWnd, L"Error setting the Root", 0, MB_OK | MB_ICONEXCLAMATION);
    }

    // Encrypt the password.
    DATA_BLOB DataIn;
    DataIn.cbData = sizeof(WCHAR) * (wcslen(wszPass) + 1);
    DataIn.pbData = (PBYTE)wszPass;

    DATA_BLOB DataOut;

    CryptProtectData(&DataIn, L"VSSEx", NULL, NULL, NULL, CRYPTPROTECT_LOCAL_MACHINE, &DataOut);

    if(ERROR_SUCCESS != RegSetValueEx(hKey, L"Pass", 0, REG_BINARY, (PBYTE)DataOut.pbData, DataOut.cbData))
    {
        MessageBox(hWnd, L"Error setting the Pass", 0, MB_OK | MB_ICONEXCLAMATION);
    }

    LocalFree(DataOut.pbData);

    if(ERROR_SUCCESS != RegSetValueEx(hKey, L"User", 0, REG_SZ, (PBYTE)wszUser, (wcslen(wszUser) + 1) * sizeof(WCHAR)))
    {
        MessageBox(hWnd, L"Error setting the User", 0, MB_OK | MB_ICONEXCLAMATION);
    }

    BOOL bCascade = IsDlgButtonChecked(hWnd, IDC_CASCADE) == BST_CHECKED ? TRUE : FALSE;
    if(ERROR_SUCCESS != RegSetValueEx(hKey, L"Cascade", 0, REG_SZ, (PBYTE)&bCascade, sizeof(bCascade)))
    {
        MessageBox(hWnd, L"Error setting the Cascade state.", 0, MB_OK | MB_ICONEXCLAMATION);
    }

    RegCloseKey(hKey);

    // Now try to get the shell extension's notification window and tell it to reset.
    HWND hNotification = NULL;
    int i = 0;

    while(1)
    {
        hNotification = 
            FindWindowEx(HWND_MESSAGE, hNotification, VSSEX_NOTIFICATION_CLASS, VSSEX_NOTIFICATION_CLASS);

        if(hNotification == NULL)
        {
            break;
        }

        i++;
        PostMessage(hNotification, VSSM_RESETDB, 0, 0);
    }

    CCString Report;
    Report.FormatW(L"%d sessions were updated.", i);
    MessageBox(0, Report, L"VSSManager", MB_OK | MB_ICONASTERISK);

    return;
}


INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
	case WM_DESTROY:
		{
			DeleteObject(g_hIcon);
			return TRUE;
		}
    case WM_CLOSE:
        {
            EndDialog(hWnd, 0);
            return TRUE;
        }
    case WM_INITDIALOG:
        {
            HKEY hKey;
            WCHAR wszIni[MAX_PATH];
            WCHAR wszUser[MAX_PATH];
            WCHAR wszRoot[MAX_PATH];
//            WCHAR wszPass[MAX_PATH];
            DWORD dwSize;

            // Open the database.
            if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER,
                L"Software\\Carl Corcoran\\VSSEx", 0, 0,
                0, KEY_QUERY_VALUE, NULL, &hKey, NULL))
            {
                // Failed to get the key.
                MessageBox(hWnd, L"Couldn't open the registry", 0, MB_OK | MB_ICONEXCLAMATION);
                return TRUE;
            }

            dwSize = MAX_PATH * sizeof(WCHAR);
            if(ERROR_SUCCESS == RegQueryValueEx(hKey, L"Ini", 0, NULL, (PBYTE)wszIni, &dwSize))
            {
                SetDlgItemText(hWnd, IDC_DATABASE, wszIni);
            }

            dwSize = MAX_PATH * sizeof(WCHAR);
            if(ERROR_SUCCESS == RegQueryValueEx(hKey, L"Root", 0, NULL, (PBYTE)wszRoot, &dwSize))
            {
                SetDlgItemText(hWnd, IDC_LOCALROOT, wszRoot);
            }

            dwSize = MAX_PATH * sizeof(WCHAR);
            if(ERROR_SUCCESS == RegQueryValueEx(hKey, L"User", 0, NULL, (PBYTE)wszUser, &dwSize))
            {
                SetDlgItemText(hWnd, IDC_USER, wszUser);
            }

            if(ERROR_SUCCESS == RegQueryValueEx(hKey, L"Pass", 0, NULL, NULL, &dwSize))
            {
                PBYTE pPass = (PBYTE)malloc(dwSize);
                if(ERROR_SUCCESS == RegQueryValueEx(hKey, L"Pass", 0, NULL, (PBYTE)pPass, &dwSize))
                {
                    // Decrypt it.
                    DATA_BLOB DataIn;
                    DATA_BLOB DataOut;

                    DataIn.cbData = dwSize;
                    DataIn.pbData = pPass;

                    CryptUnprotectData(&DataIn, NULL, NULL, NULL, NULL, 0, &DataOut);

                    SetDlgItemText(hWnd, IDC_PASSWORD, (PWSTR)DataOut.pbData);
                    LocalFree(DataOut.pbData);
                }
            }

            BOOL bCascade = TRUE;
            dwSize = sizeof(bCascade);
            if(ERROR_SUCCESS == RegQueryValueEx(hKey, L"Cascade", 0, NULL, (PBYTE)&bCascade, &dwSize))
            {
                CheckDlgButton(hWnd, IDC_CASCADE, bCascade == TRUE ? BST_CHECKED : BST_UNCHECKED);
            }

            RegCloseKey(hKey);

			g_hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON1));
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)g_hIcon);

            return TRUE;
        }
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDM_ABOUT:
                {
                    OnAbout(hWnd);
                    return TRUE;
                }
            case IDM_HELP:
                {
                    OnHelp(hWnd);
                    return TRUE;
                }
            case IDC_BROWSEROOT:
                {
                    WCHAR wszFileName[MAX_PATH];
                    LPITEMIDLIST pidl = 0;
                    IMalloc* pMalloc = 0;

                    BROWSEINFOW bi;
                    bi.hwndOwner = hWnd;
                    bi.iImage = 0;
                    bi.lParam = 0;
                    bi.lpfn = 0;
                    bi.lpszTitle = 0;
                    bi.pidlRoot = 0;
                    bi.pszDisplayName = 0;
                    bi.ulFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
                    bi.ulFlags |= BIF_NEWDIALOGSTYLE;
                    //bi.ulFlags |= BIF_USENEWUI;
                    pidl = SHBrowseForFolder(&bi);

                    if(pidl)
                    {
                        // Get the filename.
                        if(SHGetPathFromIDList(pidl, wszFileName) == TRUE)
                        {
                            // Success.
                            SetDlgItemText(hWnd, IDC_LOCALROOT, wszFileName);
                        }

                        SHGetMalloc(&pMalloc);
                        pMalloc->Free(pidl);
                        pMalloc->Release();
                        pMalloc = NULL;
                    }

                    break;
                }
            case IDC_BROWSEDATABASE:
                {
                    OPENFILENAME ofn;       // common dialog box structure
                    WCHAR wszFile[MAX_PATH] = {0};       // buffer for file name

                    // Initialize OPENFILENAME
                    ZeroMemory(&ofn, sizeof(OPENFILENAME));
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hWnd;
                    ofn.lpstrFile = wszFile;
                    ofn.nMaxFile = sizeof(wszFile);
                    ofn.lpstrFilter = L"All\0*.*\0Ini Files\0*.ini\0";
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    // Display the Open dialog box. 

                    if (GetOpenFileName(&ofn)==TRUE)
                    {
                        // use it.
                        SetDlgItemText(hWnd, IDC_DATABASE, wszFile);
                    }

                    break;
                }
            case IDC_APPLY:
                {
                    OnApply(hWnd);
                    return TRUE;
                }
            case IDOK:
                {
                    OnApply(hWnd);
                    EndDialog(hWnd, 0);
                    return TRUE;
                }
            case IDCANCEL:
                {
                    EndDialog(hWnd, 0);
                    return TRUE;
                }
            }
            break;
        }
    }

    return FALSE;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    g_hInstance = hInstance;

    CoInitialize(0);
    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MANAGER), 0, DlgProc, 0);
    CoUninitialize();
    return 0;
}


