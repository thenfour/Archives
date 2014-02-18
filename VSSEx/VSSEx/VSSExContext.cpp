

#include "VSSDB.h"
#include "VSSExContext.h"
#include "CCString2.h"
#include "Main.h"
#include "resource.h"
#include <process.h>
#include "AutoComplete.h"


extern HINSTANCE g_hInstance;
extern VSSDB g_Db;


#define IDM_SYNC        0x00000000
#define IDM_CHECKIN     0x00000001
#define IDM_CHECKOUT    0x00000002
#define IDM_DIFF        0x00000003
#define IDM_ADD         0x00000004
#define IDM_REVERT      0x00000005
#define IDM_REMOVE      0x00000006
#define IDM_RENAME      0x00000007
#define IDM_MAX         0x00000008


void AddExclusion(PCWSTR wszEntry)
{
    // Determine if the string already exists.
    int nIndex = 0;
    HKEY hKey = NULL;
    WCHAR wsz[MAX_PATH];
    DWORD dwSize;
    CCString ValueName;
    CCString s;

   // s.FormatW(L"wszEntry = %s\n", wszEntry);
    //OutputDebugString(s);


    if(ERROR_SUCCESS != 
        RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Carl Corcoran\\VSSEx\\ExclusionList",
        0, KEY_WRITE, &hKey))
    {
        //OutputDebugString(L"Opening the exclusion list REG key failed.\n");
        return;
    }

    while(1)
    {
        ValueName.FromInt(nIndex, 10);
        dwSize = MAX_PATH * sizeof(WCHAR);
        if(RegQueryValueEx(hKey, ValueName, 0, NULL, (PBYTE)wsz, &dwSize) != ERROR_SUCCESS)
        {
            //s.FormatW(L"Failed to query value %d\n", nIndex);
            //OutputDebugString(s);
            break;
        }

        if(wcsicmp(wsz, wszEntry) == 0)
        {
            RegCloseKey(hKey);
            //OutputDebugString(L"Matched entry; exiting\n");
            return;
        }

        nIndex ++;
    }

    if(ERROR_SUCCESS != RegSetValueEx(hKey, ValueName, 0, REG_SZ, (PBYTE)wszEntry, (wcslen(wszEntry) + 1) * sizeof(WCHAR)))
    {
        //s.FormatW(L"RegSetValueEx(%08x, %s, 0, REG_SZ, %s, %d) FAILED gle=%d\n",
        //    hKey, (PCWSTR)ValueName, wszEntry, (wcslen(wszEntry) + 1) * sizeof(WCHAR), GetLastError());
        //OutputDebugString(s);
    }

    RegCloseKey(hKey);
}



CVSSExContext::CVSSExContext()
{
    //OutputDebugString(L"CVSSExContext::CVSSExContext()\n");

    this->pDataObj = NULL;
    this->hContextBmp = NULL;
    this->dwRefCount = 1;
    DllAddRef();
}


CVSSExContext::~CVSSExContext()
{
    //OutputDebugString(L"CVSSExContext::~CVSSExContext()\n");
    if(this->hContextBmp) DeleteObject(this->hContextBmp);
    DllRelease();
}


STDMETHODIMP CVSSExContext::QueryInterface(REFIID riid, PVOID* ppOut)
{
    *ppOut = NULL;

    if(IsEqualIID(riid, IID_IUnknown))
    {
        *ppOut = this;
    }

    else if(IsEqualIID(riid, IID_IShellExtInit))
    {
        *ppOut = (IShellExtInit*)this;
    }

    else if(IsEqualIID(riid, IID_IContextMenu))
    {
        *ppOut = (IContextMenu*)this;
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


STDMETHODIMP_(DWORD) CVSSExContext::AddRef()
{
    return ++this->dwRefCount;
}


STDMETHODIMP_(DWORD) CVSSExContext::Release()
{
    if(--this->dwRefCount == 0)
    {
        delete this;
        return 0;
    }
   
    return this->dwRefCount;
}



#define INSERTMENUITEM(id, text) \
InsertMenu(hVSSMenu, indexMenu, MF_STRING | MF_BYPOSITION, idCmdFirst + id, text); \
SetMenuItemBitmaps(hVSSMenu, idCmdFirst + id, MF_BYCOMMAND, this->hContextBmp, this->hContextBmp); \
n++;


STDMETHODIMP CVSSExContext::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
    //OutputDebugString(L"CVSSExContext::QueryContextMenu()\n");
    HMENU hVSSMenu;
    BOOL bCascade = TRUE;

    EnsureDbOpened();
    int n = 1;

    if(!this->hContextBmp)
    {
        this->hContextBmp = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_CONTEXTIMAGE));
    }

    HKEY hKey;

    bCascade = TRUE;

    if(ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER,
        L"Software\\Carl Corcoran\\VSSEx", 0, 0,
        0, KEY_QUERY_VALUE, NULL, &hKey, NULL))
    {
        OutputDebugString(L"Successfully created key.\n");
        DWORD dwSize = sizeof(bCascade);

        if(ERROR_SUCCESS != RegQueryValueEx(hKey, L"Cascade", 0, NULL, (PBYTE)&bCascade, &dwSize))
        {
            OutputDebugString(L"Failed to QUERY key.\n");
        }

        CCString s;
        s.FormatW(L"Cascade = %d\n", bCascade);
        OutputDebugString(s);

        RegCloseKey(hKey);
    }

    if(bCascade == TRUE)
    {
        hVSSMenu = CreateMenu();
    }
    else
    {
        hVSSMenu = hMenu;
    }

    if(!(CMF_DEFAULTONLY & uFlags))
    {
        if(this->bSync == TRUE) INSERTMENUITEM(IDM_SYNC, L"Sync");
        if(this->bCheckIn == TRUE) INSERTMENUITEM(IDM_CHECKIN, L"CheckIn");
        if(this->bCheckOut == TRUE) INSERTMENUITEM(IDM_CHECKOUT, L"CheckOut");
        if(this->bRevert == TRUE) INSERTMENUITEM(IDM_REVERT, L"Revert");
        if(this->bAdd == TRUE) INSERTMENUITEM(IDM_ADD, L"Add");
        if(this->bDiff == TRUE) INSERTMENUITEM(IDM_DIFF, L"Diff");
        if(this->bRemove == TRUE) INSERTMENUITEM(IDM_REMOVE, L"Remove");
        if(this->bRename == TRUE) INSERTMENUITEM(IDM_RENAME, L"Rename");

        // Now add the submenu to the parent menu.
        if(bCascade == TRUE)
        {
            InsertMenu(hMenu, indexMenu, MF_POPUP | MF_STRING | MF_BYPOSITION, (UINT)hVSSMenu, L"VSS Commands");
            SetMenuItemBitmaps(hMenu, indexMenu, MF_BYPOSITION, this->hContextBmp, this->hContextBmp);
        }

        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(n));
    }

    //OutputDebugString(L"    Returning no menu items.\n");
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
}


#define COPYCOMMANDSTRING(a) \
{\
    switch(uFlags) \
    { \
        case GCS_HELPTEXTA: \
            lstrcpynA(pszName, #a, cchMax); \
            break;  \
        case GCS_HELPTEXTW:  \
            lstrcpynW((LPWSTR)pszName, L#a, cchMax);  \
            break;  \
        case GCS_VERBA: \
            lstrcpynA(pszName, #a, cchMax);  \
            break;  \
        case GCS_VERBW: \
            lstrcpynW((LPWSTR)pszName, L#a, cchMax);  \
            break;  \
    }\
}

STDMETHODIMP CVSSExContext::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
    //OutputDebugString(L"CVSSExContext::GetCommandString()\n");
    EnsureDbOpened();

    switch(idCmd)
    {
    case IDM_DIFF:
        COPYCOMMANDSTRING(View differences between local files and SourceSafe files.)
        break;
    case IDM_ADD:
        COPYCOMMANDSTRING(Add new files to the SourceSafe database.)
        break;
    case IDM_REVERT:
        COPYCOMMANDSTRING(Revert modified files to the version stored in SourceSafe.)
        break;
    case IDM_SYNC:
        COPYCOMMANDSTRING(Synchronize local files with the SourceSafe database.)
        break;
    case IDM_CHECKIN:
        COPYCOMMANDSTRING(Check files into SourceSafe.)
        break;
    case IDM_CHECKOUT:
        COPYCOMMANDSTRING(Check files out of SourceSafe.)
        break;
    case IDM_REMOVE:
        COPYCOMMANDSTRING(Remove files from source control.)
        break;
    case IDM_RENAME:
        COPYCOMMANDSTRING(Rename a file both on disk and in SourceSafe.)
        break;
    }

    return S_OK;
}


STDMETHODIMP CVSSExContext::Initialize(LPCITEMIDLIST pIDFolder, IDataObject *pDataObj, HKEY hKey)
{
    //OutputDebugString(L"CVSSExContext::Initialize()\n");
    //CCString s;
    //s.FormatW(L"CVSSExContext::Initialize(%08x, %08x, %08x)\n", pIDFolder, pDataObj, hKey);
    //OutputDebugString(s);
    
    WCHAR wsz[MAX_PATH];
    EnsureDbOpened();
    UINT        uCount;

    BOOL bAnyCheckedOut = FALSE;
    BOOL bAnyCheckedIn = FALSE;
    BOOL bAnyIntegrated = FALSE;
    BOOL bAnyNotIntegrated = FALSE;
    BOOL bAnyFolders = FALSE;
    BOOL bAnyFiles = FALSE;

    // If a data object pointer was passed in, save it and
    // extract the file name. 
    if(this->pDataObj) this->pDataObj->Release();

    this->bAdd = FALSE;
    this->bCheckIn = FALSE;
    this->bCheckOut = FALSE;
    this->bRevert = FALSE;
    this->bSync = FALSE;
    this->bRemove = FALSE;
    this->bDiff = FALSE;
    this->bRename = FALSE;

    this->pDataObj = pDataObj;
    this->pDataObj->AddRef();

    if(pDataObj) 
    {
        STGMEDIUM   medium;
        FORMATETC   fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, 
		                  TYMED_HGLOBAL};

        if(SUCCEEDED(pDataObj->GetData(&fe, &medium)))
        {
            // Get the file name from the CF_HDROP.
            uCount = DragQueryFile((HDROP)medium.hGlobal, (UINT)-1, NULL, 0);

            for(UINT i = 0;i<uCount;i++)
            {
                DragQueryFile((HDROP)medium.hGlobal, i, wsz, MAX_PATH);

                // If it's not in the domain of the database, forget it.
                if(g_Db.IsWithinDomain(wsz) == FALSE)
                {
                    this->bAdd = FALSE;
                    this->bCheckIn = FALSE;
                    this->bCheckOut = FALSE;
                    this->bDiff = FALSE;
                    this->bRevert = FALSE;
                    this->bSync = FALSE;
                    this->bRemove = FALSE;
                    //s.FormatW(L"    %NOT IN THE DOMAIN: 02d: %s\n", i, wsz);
                    //OutputDebugString(s);
                    break;
                }

                //s.FormatW(L"    IN THE DOMAIN: %02d: %s\n", i, wsz);
                //OutputDebugString(s);

                if(IsDirectory(wsz) == TRUE)
                {
                    bAnyFolders = TRUE;
                }
                else
                {
                    bAnyFiles = TRUE;
                }

                if(g_Db.IsIntegrated(wsz) == TRUE)
                {
                    bAnyIntegrated = TRUE;

                    if(g_Db.IsCheckedOut(wsz) == TRUE)
                    {
                        bAnyCheckedOut = TRUE;
                    }
                    else
                    {
                        bAnyCheckedIn = TRUE;
                    }
                }
                else
                {
                    bAnyNotIntegrated = TRUE;
                }
            }

            ReleaseStgMedium(&medium);
        }
    }

    if(bAnyCheckedOut == TRUE)
    {
        this->bCheckIn = TRUE;
        this->bRevert = TRUE;
        this->bDiff = TRUE;
    }

    if(bAnyCheckedIn == TRUE)
    {
        this->bSync = TRUE;
        this->bCheckOut = TRUE;
    }

    if(bAnyNotIntegrated == TRUE)
    {
        this->bAdd = TRUE;
    }

    if(bAnyIntegrated)
    {
        this->bRename = TRUE;
        this->bRemove = TRUE;
    }

    if(bAnyFolders == TRUE)
    {
        this->bDiff = TRUE;
        this->bCheckIn = TRUE;
        this->bCheckOut = TRUE;
        this->bSync = TRUE;
        this->bRevert = TRUE;
        this->bAdd = TRUE;
    }

    if(uCount > 1)
    {
        this->bRename = FALSE;
        this->bDiff = FALSE;
    }

    return S_OK; 
}


STDMETHODIMP CVSSExContext::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
    //OutputDebugString(L"CVSSExContext::Initialize()\n");

    this->Folder = lpcmi->lpDirectory;

    STGMEDIUM medium;
    FORMATETC fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, 
		              TYMED_HGLOBAL};
    WCHAR wsz[MAX_PATH];
    UINT uCount;
    UINT idCmd;
    CCString s;

    this->hWnd = lpcmi->hwnd;

    if(!HIWORD(lpcmi->lpVerb))
    {
        idCmd = LOWORD(lpcmi->lpVerb);

        // Create a list of all the filenames.
        if(SUCCEEDED(pDataObj->GetData(&fe, &medium)))
        {
            // Get the file name from the CF_HDROP.
            uCount = DragQueryFile((HDROP)medium.hGlobal, (UINT)-1, NULL, 0);

            this->pFiles = new CCString[uCount];
            if(this->pFiles == NULL) return E_FAIL;
            this->nCount = (int)uCount;

            for(UINT i = 0;i<uCount;i++)
            {
                DragQueryFile((HDROP)medium.hGlobal, i, wsz, MAX_PATH);
                this->pFiles[i].cpy(wsz);
                s.FormatW(L"*** Adding file %02d: \"%s\"\n", i, (PCWSTR)this->pFiles[i]);
                OutputDebugString(s);
            }

            ReleaseStgMedium(&medium);

            switch (idCmd)
            {
            case IDM_SYNC:
                this->OnSync();
                break;
            case IDM_REMOVE:
                this->OnRemove();
                break;
            case IDM_ADD:
                this->OnAdd();
                break;
            case IDM_REVERT:
                this->OnRevert();
                break;
            case IDM_CHECKIN:
                this->OnCheckIn();
                break;
            case IDM_CHECKOUT:
                this->OnCheckOut();
                break;
            case IDM_DIFF:
                this->OnDiff();
                break;
            case IDM_RENAME:
                this->OnRename();
                break;
            }

            delete [] this->pFiles;
            this->pFiles = NULL;
        }
    }

    return S_OK;
}


INT_PTR CALLBACK CVSSExContext::AddProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CVSSExContext* pThis = (CVSSExContext*)GetWindowLongPtr(hWnd, GWL_USERDATA);

    switch(uMsg)
    {
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDOK:
                {
                    pThis->StartProgress();
                    CCString ExclusionList;

                    // Do the action.
                    BOOL bRecurse;
                    CCString s;
                    PWSTR wszComment = NULL;
                    PWSTR wszExclusion = NULL;
                    int nSize;
                    WCHAR wszFile[MAX_PATH];
                    int i;
                    LVITEM lvi;

                    /*
                        Grab autocomplete information.
                    */
                    pThis->pAC->Release();

                    nSize = GetWindowTextLength(GetDlgItem(hWnd, IDC_EXCLUDE));
                    if(nSize != 0)
                    {
                        wszExclusion = (PWSTR)malloc((nSize + 1) * sizeof(WCHAR));

                        GetWindowText(GetDlgItem(hWnd, IDC_EXCLUDE), wszExclusion, nSize + 1);

                        AddExclusion(wszExclusion);
                    }
                    else
                    {
                        wszExclusion = (PWSTR)malloc((100) * sizeof(WCHAR));
                        wcscpy(wszExclusion, L"");
                    }

                    // Get other settings.

                    bRecurse = IsDlgButtonChecked(hWnd, IDC_RECURSIVE) == BST_CHECKED ? TRUE : FALSE;

                    nSize = GetWindowTextLength(GetDlgItem(hWnd, IDC_COMMENT));
                    wszComment = (PWSTR)malloc((nSize + 1) * sizeof(WCHAR));

                    GetWindowText(GetDlgItem(hWnd, IDC_COMMENT), wszComment, nSize + 1);

                    // Go through the list and perform the operation.
                    for(i=0;i<ListView_GetItemCount(GetDlgItem(hWnd, IDC_SUMMARY));i++)
                    {
                        // Get the item text.
                        lvi.mask = LVIF_TEXT;
                        lvi.iItem = i;
                        lvi.iSubItem = 0;
                        lvi.cchTextMax = MAX_PATH;
                        lvi.pszText = wszFile;

                        ListView_GetItem(GetDlgItem(hWnd, IDC_SUMMARY), &lvi);

                        // Now perform the operation.
                        if(FAILED(g_Db.Add(wszFile, wszComment, bRecurse, wszExclusion)))
                        {
                            //s.FormatW(L"**** FAILED ADD: \"%s\"\n", wszFile);
                        }
                        else
                        {
                            //s.FormatW(L"**** SUCCESSFUL ADD: \"%s\"\n", wszFile);
                        }
                        //OutputDebugString(s);
                    }

                    free(wszComment);
                    free(wszExclusion);

                    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH, (PCWSTR)pThis->Folder, NULL);

                    EndDialog(hWnd, 0);

                    pThis->SignalProgressFinished();

                    return TRUE;
                }
            case IDCANCEL:
                {
                    EndDialog(hWnd, 0);
                    return TRUE;
                }
            }
            return TRUE;
        }
    case WM_CLOSE:
        {
            EndDialog(hWnd, 0);
            return TRUE;
        }
    case WM_INITDIALOG:
        {
            SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)lParam);
            pThis = (CVSSExContext*)lParam;

            CheckDlgButton(hWnd, IDC_RECURSIVE, BST_CHECKED);

            LVCOLUMN lvc;
            lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
            lvc.fmt = LVCFMT_LEFT;
            lvc.cx = 500;
            lvc.pszText = L"Filename";
            lvc.iSubItem = 0;
            lvc.iOrder = 0;
            SendMessage(GetDlgItem(hWnd, IDC_SUMMARY), LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);

            /*
                Initialize the autocomplete source.
            */
            CoCreateInstance(CLSID_AutoComplete, NULL, CLSCTX_INPROC_SERVER,
                             IID_IAutoComplete, (PVOID*)&pThis->pAC);

            IUnknown* pSource = (IUnknown*)new CVSSEnumString;

            IAutoComplete2 *pac2;
            if (SUCCEEDED(pThis->pAC->QueryInterface(IID_IAutoComplete2, (LPVOID*)&pac2)))
            {
                pac2->SetOptions(ACO_AUTOSUGGEST);
                pac2->Release();
            }

            pThis->pAC->Init(GetDlgItem(hWnd, IDC_EXCLUDE), pSource, NULL, NULL);

            // Fill the list with the good stuff
            int i;
            LVITEM lvi = {0};
            for(i=0;i<pThis->nCount;i++)
            {
                // Make sure that the files are not integrated
                if(g_Db.IsIntegrated(pThis->pFiles[i]) == FALSE ||
                    IsDirectory(pThis->pFiles[i]) == TRUE
                    )
                {
                    // Add it to the listview.
                    lvi.mask = LVIF_TEXT;
                    lvi.iItem = 65535;
                    lvi.iSubItem = 0;
                    lvi.pszText = (PWSTR)((PCWSTR)pThis->pFiles[i]);

                    ListView_InsertItem(GetDlgItem(hWnd, IDC_SUMMARY), &lvi);
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}


void CVSSExContext::OnAdd()
{
    DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_ADD), this->hWnd,
        CVSSExContext::AddProc, (LONG)this);
}


void CVSSExContext::OnDiff()
{
    g_Db.Diff(this->pFiles[0]);
}


INT_PTR CALLBACK CVSSExContext::CheckInProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CVSSExContext* pThis = (CVSSExContext*)GetWindowLongPtr(hWnd, GWL_USERDATA);

    switch(uMsg)
    {
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDOK:
                {
                    pThis->StartProgress();

                    // Do the action.
                    BOOL bRecurse;
                    BOOL bRemoveLocal;
                    CCString s;
                    PWSTR wszComment = NULL;
                    int nSize;
                    WCHAR wszFile[MAX_PATH];
                    int i;
                    LVITEM lvi;

                    bRecurse = IsDlgButtonChecked(hWnd, IDC_RECURSIVE) == BST_CHECKED ? TRUE : FALSE;
                    bRemoveLocal = IsDlgButtonChecked(hWnd, IDC_REMOVELOCAL) == BST_CHECKED ? TRUE : FALSE;

                    nSize = GetWindowTextLength(GetDlgItem(hWnd, IDC_COMMENT));
                    wszComment = (PWSTR)malloc((nSize + 1) * sizeof(WCHAR));

                    GetWindowText(GetDlgItem(hWnd, IDC_COMMENT), wszComment, nSize + 1);

                    // Go through the list and perform the operation.
                    for(i=0;i<ListView_GetItemCount(GetDlgItem(hWnd, IDC_SUMMARY));i++)
                    {
                        // Get the item text.
                        lvi.mask = LVIF_TEXT;
                        lvi.iItem = i;
                        lvi.iSubItem = 0;
                        lvi.cchTextMax = MAX_PATH;
                        lvi.pszText = wszFile;

                        ListView_GetItem(GetDlgItem(hWnd, IDC_SUMMARY), &lvi);

                        // Now perform the operation.
                        if(FAILED(g_Db.CheckIn(wszFile, wszComment, bRecurse, bRemoveLocal)))
                        {
                            //s.FormatW(L"**** FAILED CHECK IN: \"%s\"\n", wszFile);
                        }
                        else
                        {
                            //s.FormatW(L"**** CHECKED IN: \"%s\"\n", wszFile);
                        }
                        //OutputDebugString(s);
                    }

                    free(wszComment);

                    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH, (PCWSTR)pThis->Folder, NULL);

                    EndDialog(hWnd, 0);

                    pThis->SignalProgressFinished();

                    return TRUE;
                }
            case IDCANCEL:
                {
                    EndDialog(hWnd, 0);
                    return TRUE;
                }
            }
            return TRUE;
        }
    case WM_CLOSE:
        {
            EndDialog(hWnd, 0);
            return TRUE;
        }
    case WM_INITDIALOG:
        {
            SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)lParam);
            pThis = (CVSSExContext*)lParam;

            CheckDlgButton(hWnd, IDC_RECURSIVE, BST_CHECKED);
            CheckDlgButton(hWnd, IDC_REMOVELOCAL, BST_UNCHECKED);

            LVCOLUMN lvc;
            lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
            lvc.fmt = LVCFMT_LEFT;
            lvc.cx = 500;
            lvc.pszText = L"Filename";
            lvc.iSubItem = 0;
            lvc.iOrder = 0;
            SendMessage(GetDlgItem(hWnd, IDC_SUMMARY), LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);

            // Fill the list with the good stuff
            int i;
            LVITEM lvi = {0};
            for(i=0;i<pThis->nCount;i++)
            {
                // Make sure that the files are checked out
                if(g_Db.IsCheckedOut(pThis->pFiles[i]) == TRUE ||
                    IsDirectory(pThis->pFiles[i]) == TRUE
                    )
                {
                    // Add it to the listview.
                    lvi.mask = LVIF_TEXT;
                    lvi.iItem = 65535;
                    lvi.iSubItem = 0;
                    lvi.pszText = (PWSTR)((PCWSTR)pThis->pFiles[i]);

                    ListView_InsertItem(GetDlgItem(hWnd, IDC_SUMMARY), &lvi);
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}


void CVSSExContext::OnCheckIn()
{
    DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_CHECKIN), this->hWnd,
        CVSSExContext::CheckInProc, (LONG)this);
}


INT_PTR CALLBACK CVSSExContext::CheckOutProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CVSSExContext* pThis = (CVSSExContext*)GetWindowLongPtr(hWnd, GWL_USERDATA);

    switch(uMsg)
    {
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDOK:
                {
                    pThis->StartProgress();

                    // Do the action.
                    BOOL bRecurse;
                    CCString s;
                    PWSTR wszComment = NULL;
                    int nSize;
                    WCHAR wszFile[MAX_PATH];
                    int i;
                    LVITEM lvi;

                    bRecurse = IsDlgButtonChecked(hWnd, IDC_RECURSIVE) == BST_CHECKED ? TRUE : FALSE;

                    nSize = GetWindowTextLength(GetDlgItem(hWnd, IDC_COMMENT));
                    wszComment = (PWSTR)malloc((nSize + 1) * sizeof(WCHAR));

                    GetWindowText(GetDlgItem(hWnd, IDC_COMMENT), wszComment, nSize + 1);

                    // Go through the list and perform the operation.
                    for(i=0;i<ListView_GetItemCount(GetDlgItem(hWnd, IDC_SUMMARY));i++)
                    {
                        // Get the item text.
                        lvi.mask = LVIF_TEXT;
                        lvi.iItem = i;
                        lvi.iSubItem = 0;
                        lvi.cchTextMax = MAX_PATH;
                        lvi.pszText = wszFile;

                        ListView_GetItem(GetDlgItem(hWnd, IDC_SUMMARY), &lvi);

                        // Now perform the operation.
                        if(FAILED(g_Db.CheckOut(wszFile, wszComment, bRecurse)))
                        {
                            //s.FormatW(L"**** FAILED CHECK OUT: \"%s\"\n", wszFile);
                        }
                        else
                        {
                            //s.FormatW(L"**** CHECKED OUT: \"%s\"\n", wszFile);
                        }
                        //OutputDebugString(s);
                    }

                    free(wszComment);

                    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH, (PCWSTR)pThis->Folder, NULL);

                    EndDialog(hWnd, 0);

                    pThis->SignalProgressFinished();

                    return TRUE;
                }
            case IDCANCEL:
                {
                    EndDialog(hWnd, 0);
                    return TRUE;
                }
            }
            return TRUE;
        }
    case WM_CLOSE:
        {
            EndDialog(hWnd, 0);
            return TRUE;
        }
    case WM_INITDIALOG:
        {
            SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)lParam);
            pThis = (CVSSExContext*)lParam;

            CheckDlgButton(hWnd, IDC_RECURSIVE, BST_CHECKED);
            CheckDlgButton(hWnd, IDC_REMOVELOCAL, BST_UNCHECKED);

            LVCOLUMN lvc;
            lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
            lvc.fmt = LVCFMT_LEFT;
            lvc.cx = 500;
            lvc.pszText = L"Filename";
            lvc.iSubItem = 0;
            lvc.iOrder = 0;
            SendMessage(GetDlgItem(hWnd, IDC_SUMMARY), LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);

            // Fill the list with the good stuff
            int i;
            LVITEM lvi = {0};
            for(i=0;i<pThis->nCount;i++)
            {
                // Make sure that the files are not checked out
                if(g_Db.IsCheckedOut(pThis->pFiles[i]) == FALSE ||
                    IsDirectory(pThis->pFiles[i]) == TRUE
                    )
                {
                    // Add it to the listview.
                    lvi.mask = LVIF_TEXT;
                    lvi.iItem = 65535;
                    lvi.iSubItem = 0;
                    lvi.pszText = (PWSTR)((PCWSTR)pThis->pFiles[i]);

                    ListView_InsertItem(GetDlgItem(hWnd, IDC_SUMMARY), &lvi);
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}

void CVSSExContext::OnCheckOut()
{
    DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_CHECKOUT), this->hWnd,
        CVSSExContext::CheckOutProc, (LONG)this);
}


INT_PTR CALLBACK CVSSExContext::SyncProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CVSSExContext* pThis = (CVSSExContext*)GetWindowLongPtr(hWnd, GWL_USERDATA);

    switch(uMsg)
    {
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDOK:
                {
                    pThis->StartProgress();

                    // Do the action.
                    BOOL bRecurse;
                    BOOL bClobber;
                    CCString s;
                    WCHAR wszFile[MAX_PATH];
                    int i;
                    LVITEM lvi;

                    bRecurse = IsDlgButtonChecked(hWnd, IDC_RECURSIVE) == BST_CHECKED ? TRUE : FALSE;
                    bClobber = IsDlgButtonChecked(hWnd, IDC_CLOBBER) == BST_CHECKED ? TRUE : FALSE;

                    // Go through the list and perform the operation.
                    for(i=0;i<ListView_GetItemCount(GetDlgItem(hWnd, IDC_SUMMARY));i++)
                    {
                        // Get the item text.
                        lvi.mask = LVIF_TEXT;
                        lvi.iItem = i;
                        lvi.iSubItem = 0;
                        lvi.cchTextMax = MAX_PATH;
                        lvi.pszText = wszFile;

                        ListView_GetItem(GetDlgItem(hWnd, IDC_SUMMARY), &lvi);

                        // Now perform the operation.
                        if(FAILED(g_Db.Sync(wszFile, bRecurse, bClobber)))
                        {
                            //s.FormatW(L"**** FAILED CHECK OUT: \"%s\"\n", wszFile);
                        }
                        else
                        {
                            //s.FormatW(L"**** CHECKED OUT: \"%s\"\n", wszFile);
                        }
                        //OutputDebugString(s);
                    }

                    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH, (PCWSTR)pThis->Folder, NULL);

                    EndDialog(hWnd, 0);

                    pThis->SignalProgressFinished();

                    return TRUE;
                }
            case IDCANCEL:
                {
                    EndDialog(hWnd, 0);
                    return TRUE;
                }
            }
            return TRUE;
        }
    case WM_CLOSE:
        {
            EndDialog(hWnd, 0);
            return TRUE;
        }
    case WM_INITDIALOG:
        {
            SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)lParam);
            pThis = (CVSSExContext*)lParam;

            CheckDlgButton(hWnd, IDC_RECURSIVE, BST_CHECKED);
            CheckDlgButton(hWnd, IDC_CLOBBER, BST_UNCHECKED);

            LVCOLUMN lvc;
            lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
            lvc.fmt = LVCFMT_LEFT;
            lvc.cx = 500;
            lvc.pszText = L"Filename";
            lvc.iSubItem = 0;
            lvc.iOrder = 0;
            SendMessage(GetDlgItem(hWnd, IDC_SUMMARY), LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);

            // Fill the list with the good stuff
            int i;
            LVITEM lvi = {0};
            for(i=0;i<pThis->nCount;i++)
            {
                // Make sure that the files are not checked out
                if(g_Db.IsCheckedOut(pThis->pFiles[i]) == FALSE ||
                    IsDirectory(pThis->pFiles[i]) == TRUE
                    )
                {
                    // Add it to the listview.
                    lvi.mask = LVIF_TEXT;
                    lvi.iItem = 65535;
                    lvi.iSubItem = 0;
                    lvi.pszText = (PWSTR)((PCWSTR)pThis->pFiles[i]);

                    ListView_InsertItem(GetDlgItem(hWnd, IDC_SUMMARY), &lvi);
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}


void CVSSExContext::OnSync()
{
    DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_SYNC), this->hWnd,
        CVSSExContext::SyncProc, (LONG)this);
}


INT_PTR CALLBACK CVSSExContext::RevertProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CVSSExContext* pThis = (CVSSExContext*)GetWindowLongPtr(hWnd, GWL_USERDATA);

    switch(uMsg)
    {
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDOK:
                {
                    pThis->StartProgress();

                    // Do the action.
                    BOOL bRecurse;
                    BOOL bClobber;
                    CCString s;
                    WCHAR wszFile[MAX_PATH];
                    int i;
                    LVITEM lvi;

                    bRecurse = IsDlgButtonChecked(hWnd, IDC_RECURSIVE) == BST_CHECKED ? TRUE : FALSE;
                    bClobber = IsDlgButtonChecked(hWnd, IDC_CLOBBER) == BST_CHECKED ? TRUE : FALSE;

                    // Go through the list and perform the operation.
                    for(i=0;i<ListView_GetItemCount(GetDlgItem(hWnd, IDC_SUMMARY));i++)
                    {
                        // Get the item text.
                        lvi.mask = LVIF_TEXT;
                        lvi.iItem = i;
                        lvi.iSubItem = 0;
                        lvi.cchTextMax = MAX_PATH;
                        lvi.pszText = wszFile;

                        ListView_GetItem(GetDlgItem(hWnd, IDC_SUMMARY), &lvi);

                        // Now perform the operation.
                        if(FAILED(g_Db.Revert(wszFile, bRecurse, bClobber)))
                        {
                            //s.FormatW(L"**** FAILED CHECK OUT: \"%s\"\n", wszFile);
                        }
                        else
                        {
                            //s.FormatW(L"**** CHECKED OUT: \"%s\"\n", wszFile);
                        }
                        //OutputDebugString(s);
                    }

                    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH, (PCWSTR)pThis->Folder, NULL);

                    EndDialog(hWnd, 0);

                    pThis->SignalProgressFinished();

                    return TRUE;
                }
            case IDCANCEL:
                {
                    EndDialog(hWnd, 0);
                    return TRUE;
                }
            }
            return TRUE;
        }
    case WM_CLOSE:
        {
            EndDialog(hWnd, 0);
            return TRUE;
        }
    case WM_INITDIALOG:
        {
            SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)lParam);
            pThis = (CVSSExContext*)lParam;

            CheckDlgButton(hWnd, IDC_RECURSIVE, BST_CHECKED);
            CheckDlgButton(hWnd, IDC_CLOBBER, BST_UNCHECKED);

            LVCOLUMN lvc;
            lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
            lvc.fmt = LVCFMT_LEFT;
            lvc.cx = 500;
            lvc.pszText = L"Filename";
            lvc.iSubItem = 0;
            lvc.iOrder = 0;
            SendMessage(GetDlgItem(hWnd, IDC_SUMMARY), LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);

            // Fill the list with the good stuff
            int i;
            LVITEM lvi = {0};
            for(i=0;i<pThis->nCount;i++)
            {
                // Make sure that the files are not checked out
                if(g_Db.IsCheckedOut(pThis->pFiles[i]) == TRUE ||
                    IsDirectory(pThis->pFiles[i]) == TRUE
                    )
                {
                    // Add it to the listview.
                    lvi.mask = LVIF_TEXT;
                    lvi.iItem = 65535;
                    lvi.iSubItem = 0;
                    lvi.pszText = (PWSTR)((PCWSTR)pThis->pFiles[i]);

                    ListView_InsertItem(GetDlgItem(hWnd, IDC_SUMMARY), &lvi);
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}


void CVSSExContext::OnRevert()
{
    DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_REVERT), this->hWnd,
        CVSSExContext::RevertProc, (LONG)this);
}


void CVSSExContext::StartProgress()
{
    // Create an event to be signaled when we finish the job
    // AKA when the call to SignalProgressFinished() happens.

    this->hEventCompleted = CreateEvent(NULL, TRUE, FALSE, NULL);

    _beginthread(CVSSExContext::ProgressThreadProc, 0, this);
}


void CVSSExContext::SignalProgressFinished()
{
    SetEvent(this->hEventCompleted);
}


void CDECL CVSSExContext::ProgressThreadProc(PVOID pParam)
{
    CVSSExContext* pThis = (CVSSExContext*)pParam;

    HWND hDlg;
    
    // Create the dialog box
    hDlg = CreateDialogParam(g_hInstance, MAKEINTRESOURCE(IDD_PROGRESS), pThis->hWnd,
        CVSSExContext::ProgressProc, (LONG)pThis);

    MSG msg;
    while(1)
    {
        if(WaitForSingleObject(pThis->hEventCompleted, 33) == WAIT_OBJECT_0) break;

        if(PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
        {
            GetMessage(&msg, 0, 0, 0);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    EndDialog(hDlg, 0);

    return;
}

INT_PTR CALLBACK CVSSExContext::ProgressProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_CLOSE:
        EndDialog(hWnd, 0);
        return TRUE;
    }

    return FALSE;
}




INT_PTR CALLBACK CVSSExContext::RemoveProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CVSSExContext* pThis = (CVSSExContext*)GetWindowLongPtr(hWnd, GWL_USERDATA);

    switch(uMsg)
    {
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDOK:
                {
                    pThis->StartProgress();

                    // Do the action.
                    BOOL bDelete;
                    CCString s;
                    WCHAR wszFile[MAX_PATH];
                    int i;
                    LVITEM lvi;

                    bDelete = IsDlgButtonChecked(hWnd, IDC_DESTROY) == BST_CHECKED ? TRUE : FALSE;

                    // Go through the list and perform the operation.
                    for(i=0;i<ListView_GetItemCount(GetDlgItem(hWnd, IDC_SUMMARY));i++)
                    {
                        // Get the item text.
                        lvi.mask = LVIF_TEXT;
                        lvi.iItem = i;
                        lvi.iSubItem = 0;
                        lvi.cchTextMax = MAX_PATH;
                        lvi.pszText = wszFile;

                        ListView_GetItem(GetDlgItem(hWnd, IDC_SUMMARY), &lvi);

                        // Now perform the operation.
                        if(FAILED(g_Db.Remove(wszFile, bDelete)))
                        {
                            //s.FormatW(L"**** FAILED REMOVE: \"%s\"\n", wszFile);
                        }
                        else
                        {
                            //s.FormatW(L"**** REMOVED: \"%s\"\n", wszFile);
                        }
                        //OutputDebugString(s);
                    }

                    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH, (PCWSTR)pThis->Folder, NULL);

                    EndDialog(hWnd, 0);

                    pThis->SignalProgressFinished();

                    return TRUE;
                }
            case IDCANCEL:
                {
                    EndDialog(hWnd, 0);
                    return TRUE;
                }
            }
            return TRUE;
        }
    case WM_CLOSE:
        {
            EndDialog(hWnd, 0);
            return TRUE;
        }
    case WM_INITDIALOG:
        {
            SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)lParam);
            pThis = (CVSSExContext*)lParam;

            CheckDlgButton(hWnd, IDC_DESTROY, BST_UNCHECKED);

            LVCOLUMN lvc;
            lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
            lvc.fmt = LVCFMT_LEFT;
            lvc.cx = 500;
            lvc.pszText = L"Filename";
            lvc.iSubItem = 0;
            lvc.iOrder = 0;
            SendMessage(GetDlgItem(hWnd, IDC_SUMMARY), LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);

            // Fill the list with the good stuff
            int i;
            LVITEM lvi = {0};
            for(i=0;i<pThis->nCount;i++)
            {
                // Make sure that the files are checked out
                if(g_Db.IsIntegrated(pThis->pFiles[i]) == TRUE)
                {
                    // Add it to the listview.
                    lvi.mask = LVIF_TEXT;
                    lvi.iItem = 65535;
                    lvi.iSubItem = 0;
                    lvi.pszText = (PWSTR)((PCWSTR)pThis->pFiles[i]);

                    ListView_InsertItem(GetDlgItem(hWnd, IDC_SUMMARY), &lvi);
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}


void CVSSExContext::OnRemove()
{
    DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_REMOVE), this->hWnd,
        CVSSExContext::RemoveProc, (LONG)this);
}


INT_PTR CALLBACK CVSSExContext::RenameProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CVSSExContext* pThis = (CVSSExContext*)GetWindowLongPtr(hWnd, GWL_USERDATA);

    switch(uMsg)
    {
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
                    int nSize;
                    PWSTR wszFileName;

                    nSize = GetWindowTextLength(GetDlgItem(hWnd, IDC_NEWNAME));
                    wszFileName = (PWSTR)malloc((nSize + 1) * sizeof(WCHAR));

                    GetWindowText(GetDlgItem(hWnd, IDC_NEWNAME), wszFileName, nSize + 1);

                    // Make sure it doesn't contain
                    //                    |<>*?\/:
                    if(wcspbrk(wszFileName, L"|\\/?><*:") != 0)
                    {
                        MessageBox(hWnd,
                            L"Filenames cannot contain the following characters:\n"
                            L"/\\?*:><|", L"VSSEx", MB_OK | MB_ICONEXCLAMATION);
                        free(wszFileName);
                        return TRUE;
                    }

                    CCString NewFullPath;
                    NewFullPath = pThis->pFiles[0];
                    NewFullPath.Path_StripToPath();
                    NewFullPath.Path_Append(wszFileName);

                    MoveFile(pThis->pFiles[0], NewFullPath);
                    g_Db.Rename(pThis->pFiles[0], wszFileName);

                    free(wszFileName);

                    EndDialog(hWnd, 0);
                    return TRUE;
                }
            case IDCANCEL:
                {
                    EndDialog(hWnd, 0);
                    return TRUE;
                }
            }
            return FALSE;
        }
    case WM_INITDIALOG:
        {
            SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)lParam);
            pThis = (CVSSExContext*)lParam;

            // Put the filename in the old path dialog box
            CCString File = pThis->pFiles[0];
            File.Path_StripToFilename();

            SetDlgItemText(hWnd, IDC_OLDNAME, File);
            SetDlgItemText(hWnd, IDC_NEWNAME, File);

            SetFocus(GetDlgItem(hWnd, IDC_NEWNAME));

            return TRUE;
        }
    }

    return FALSE;
}


void CVSSExContext::OnRename()
{
    DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_RENAME), this->hWnd,
        CVSSExContext::RenameProc, (LONG)this);
}


