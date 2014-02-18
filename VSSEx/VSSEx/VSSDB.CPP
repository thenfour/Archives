#include "VSSDB.h"
#include <shlwapi.h>


#define ERRMSG                                                            \
{                                                                         \
    CCString ErrMsg;                                                      \
    ErrMsg.FormatW(L"    ** ERROR: %S Line:%d\n", __FILE__, __LINE__);    \
    OutputDebugString(ErrMsg);                                            \
}


BOOL IsDirectory(CCString Path)
{
    DWORD dw = GetFileAttributes(Path);
    CCString s;
    s.FormatW(L"IS %s DIRECTORY: %s\n", (dw & FILE_ATTRIBUTE_DIRECTORY) == 0 ? L"NOT" : L"", (PCWSTR)Path);
    return (dw & FILE_ATTRIBUTE_DIRECTORY) == 0 ? FALSE : TRUE;
}

VSSDB::VSSDB()
{
    CoInitialize(0);
    this->wszLocalRoot[0] = 0;
    this->pDb = NULL;
}


VSSDB::~VSSDB()
{
    this->Close();
    CoUninitialize();
}


HRESULT VSSDB::Close()
{
//    CCString s;
//    s.FormatW(L"**** VSSDB::Close(): %08x\n", this->pDb);
//    OutputDebugString(s);

    if(this->pDb)
    {
//        OutputDebugString(L"    Releasing the database\n");
        try
        {
            this->pDb->Release();
        }
        catch(...)
        {
        }

        this->pDb = NULL;
    }

    this->wszLocalRoot[0] = 0;

    return S_OK;
}


HRESULT VSSDB::Open(CCString Ini, CCString User, CCString Pass)
{
//    CCString s;
//    s.FormatW(L"**** VSSDB::Open(\"%s\", \"%s\", \"%s\")\n", (PCWSTR)Ini, (PCWSTR)User, (PCWSTR)Pass);
//    OutputDebugString(s);

    IVSSItem* pItem = NULL;

    // Make sure it's closed first
    this->Close();

    if(FAILED(CoCreateInstance(
        CLSID_VSSDatabase, NULL, CLSCTX_SERVER, IID_IVSSDatabase, (PVOID*)&this->pDb)))
    {
        ERRMSG
        return E_FAIL;
    }

    if(FAILED(this->pDb->Open(Ini.bstr(), User.bstr(), Pass.bstr())))
    {
        ERRMSG
        this->pDb->Release();
        this->pDb = NULL;

        return E_FAIL;
    }

    // Get the local root directory.
    // To do this, we get the first
    CCString Spec = L"$/";

    if(FAILED(pDb->get_VSSItem(Spec.bstr(), FALSE, &pItem)))
    {
        // Couldn't get root node
        ERRMSG
        this->pDb->Release();
        this->pDb = NULL;

        return E_FAIL;
    }

    // We have an item - let's get its local spec.
    BSTR wsz = NULL;

    if(FAILED(pItem->get_LocalSpec(&wsz)))
    {
        ERRMSG
        this->pDb->Release();
        this->pDb = NULL;
        return E_FAIL;
    }

    wcsncpy(this->wszLocalRoot, wsz, MAX_PATH);
    wcslwr(this->wszLocalRoot);

    //CCString s;
    //s.FormatW(L"\n\n"
    //    L"    ** Got the local root for the database.** \n"
    //    L"    Ini : %s\n"
    //    L"    User: %s\n"
    //    L"    Pass: %s\n"
    //    L"    Root: %s\n\n",
    //    (PCWSTR)Ini, (PCWSTR)User, (PCWSTR)Pass, wsz);
    //OutputDebugString(s);

    SysFreeString(wsz);

    pItem->Release();
    pItem = NULL;

    //s.FormatW(L"**VSSDB::Open() Succeeded: Root: %s\n", this->wszLocalRoot);
    //OutputDebugString(s);

    return S_OK;
}


HRESULT VSSDB::_PathToSpec(PCWSTR wszLocalSpec, CCString* pDbSpec)
{
    if(this->IsWithinDomain(wszLocalSpec) == FALSE) return E_FAIL;
    if(this->pDb == NULL) return E_FAIL;

    // Replace the local root with $/
    // So if the local spec is d:\\root\\source,
    // the db spec will be $/source
    WCHAR wszLocal[MAX_PATH];
    wcscpy(wszLocal, wszLocalSpec);
    wcslwr(wszLocal);
    int i = 0;

    while(1)
    {
        if(this->wszLocalRoot[i] == 0) break;

        if(wszLocal[i] != this->wszLocalRoot[i]) return E_FAIL;
        i++;
    }

    // We know that the path is in the source safe database
    // And we know where it ends.
    //*pDbSpec = this->wszLocalRoot;
    *pDbSpec = L"$/";

    pDbSpec->cat(wszLocal + i);

    MakeVSSPathPretty((PWSTR)(PCWSTR)*pDbSpec);

    return S_OK;
}


HRESULT VSSDB::_SpecToPath(CCString DbSpec, CCString* pLocalSpec)
{
    IVSSItem* pItem = NULL;

    // Validation
    if(this->pDb == NULL) return E_FAIL;
    if(pLocalSpec == NULL) return E_FAIL;

    if(FAILED(this->pDb->get_VSSItem(DbSpec.bstr(), FALSE, &pItem)))
    {
        ERRMSG
        return E_FAIL;
    }

    PWSTR wsz = NULL;

    if(FAILED(pItem->get_LocalSpec(&wsz)))
    {
        pItem->Release();
        pItem = NULL;
        ERRMSG
        return E_FAIL;
    }

    *pLocalSpec = wsz;

    SysFreeString(wsz);

    pItem->Release();
    pItem = NULL;

    return S_OK;
}


BOOL VSSDB::IsCheckedOut(CCString LocalSpec)
{
    IVSSItem* pItem = NULL;
    long lStatus;

    if(FAILED(this->_GetItemFromFile(LocalSpec, &pItem)))
    {
        //OutputDebugString(L"VSSDB::IsCheckedOut FAILED - Couldn't get item from local spec.\n");
        return FALSE;
    }

    // Now that we have the item, determine whether it's checked out or not.

    if(FAILED(pItem->get_IsCheckedOut(&lStatus)))
    {
        pItem->Release();
        pItem = NULL;
        //OutputDebugString(L"VSSDB::IsCheckedOut FAILED - Couldn't perform pItem->get_IsCheckedOut\n");
        return FALSE;
    }

    //CCString s;
    //s.FormatW(L"VSSDB::IsCheckedOut Succeeded; lStatus=%d", lStatus);
    //OutputDebugString(s);

    pItem->Release();
    pItem = NULL;

    return lStatus == VSSFILE_NOTCHECKEDOUT ? FALSE : TRUE;
}


BOOL VSSDB::IsIntegrated(CCString LocalSpec)
{
    IVSSItem* pItem = NULL;

    if(FAILED(this->_GetItemFromFile(LocalSpec, &pItem)))
    {
        return FALSE;
    }

    pItem->Release();
    pItem = NULL;

    return TRUE;
}


void MakeVSSPathPretty(PWSTR wsz)
{
    // Remove double-slashes, turn all back-slashes into forward-slashes.
    int iSrc = 0;
    int iDest = 0;

    while(1)
    {
        if(wsz[iSrc] == 0)
        {
            wsz[iDest] = 0;
            break;
        }

        if(wsz[iSrc] == '/' || wsz[iSrc] == '\\')
        {
            // Skip multiples
            while(1)
            {
                if(wsz[iSrc] != '/' && wsz[iSrc] != '\\')
                {
                    iSrc --;
                    wsz[iSrc] = '/';
                    break;
                }
                iSrc ++;
            }
        }

        wsz[iDest] = wsz[iSrc];
        iDest ++;
        iSrc ++;
    }

    return;
}


HRESULT VSSDB::CheckOut(CCString LocalSpec, CCString Comment, BOOL bRecurse)
{
    IVSSItem* pItem = NULL;
    int iType = 0;

    if(FAILED(this->_GetItemFromFile(LocalSpec, &pItem)))
    {
        return FALSE;
    }

    if(FAILED(pItem->get_Type(&iType)))
    {
        pItem->Release();
        pItem = NULL;
        ERRMSG
        return E_FAIL;
    }

    if(FAILED(pItem->Checkout(Comment.bstr(), LocalSpec.bstr(),
        VSSFLAG_FORCEDIRNO | (bRecurse == TRUE ? VSSFLAG_RECURSYES : VSSFLAG_RECURSNO)
        )))
    {
        pItem->Release();
        pItem = NULL;
        ERRMSG
        return E_FAIL;
    }

    pItem->Release();
    pItem = NULL;

    return S_OK;
}


/*
    This is the same as CheckOut, but doesn't actually check it out... just gets the files.
*/
HRESULT VSSDB::Sync(CCString Path, BOOL bRecurse, BOOL bClobber)
{
    CCString DbSpec;
    IVSSItem* pItem = NULL;
    long iType = 0;

    if(this->pDb == 0) return E_FAIL;

    if(FAILED(this->_GetItemFromFile(Path, &pItem)))
    {
        return FALSE;
    }

    BSTR wsz = Path.bstr();

    pItem->Get(&wsz, VSSFLAG_FORCEDIRNO | 
        (bRecurse == TRUE ? VSSFLAG_RECURSYES : VSSFLAG_RECURSNO) |
        (bClobber == TRUE ? VSSFLAG_REPSKIP : VSSFLAG_REPREPLACE)
        );

    pItem->Release();
    pItem = NULL;
    
    return S_OK;
}


HRESULT VSSDB::CheckIn(CCString LocalSpec, CCString Comment, BOOL bRecurse, BOOL bRemoveLocal)
{
    IVSSItem* pItem = NULL;
    long iType = 0;

    if(this->pDb == 0) return E_FAIL;

    if(FAILED(this->_GetItemFromFile(LocalSpec, &pItem)))
    {
        ERRMSG
        return E_FAIL;
    }

    if(FAILED(pItem->Checkin(Comment.bstr(), LocalSpec.bstr(), VSSFLAG_FORCEDIRNO |
        (bRecurse == TRUE ? VSSFLAG_RECURSYES : VSSFLAG_RECURSNO) |
        (bRemoveLocal == TRUE ? VSSFLAG_DELYES : VSSFLAG_DELNO)
        )))
    {
        ERRMSG
    }

    pItem->Release();
    pItem = NULL;
    
    return S_OK;
}


HRESULT VSSDB::Rename(CCString Path, CCString NewName)
{
    IVSSItem* pItem = NULL;
    long iType = 0;

    if(this->pDb == 0) return E_FAIL;

    if(FAILED(this->_GetItemFromFile(Path, &pItem)))
    {
        ERRMSG
        return E_FAIL;
    }

    if(FAILED(pItem->put_Name(NewName.bstr())))
    {
        pItem->Release();
        pItem = NULL;
        ERRMSG
        return E_FAIL;
    }

    pItem->Release();
    pItem = NULL;
    
    return S_OK;
}


HRESULT VSSDB::Add(CCString Path, CCString Comment, BOOL bRecurse, CCString Exclusions)
{
    IVSSItem* pParent = 0;
    IVSSItem* pNew = 0;
    CCString sParentDir = Path;
    CCString Name;
    CCString ChildFullPath;

    this->sExclusions = Exclusions;

    if(wcsicmp(PathFindExtension(Path), L"SCC") == 0) return S_OK;

    //CCString x;
    //x.FormatW(L"VSSDB::Add(\"%s\",\"%s\",%s)\n", (PCWSTR)Path, (PCWSTR)Comment,
    //    bRecurse == TRUE ? L"TRUE" : L"FALSE");
    //OutputDebugString(x);

    if(this->pDb == 0)
    {
        ERRMSG
        return E_FAIL;
    }

    // Get a parent first.

    PathRemoveFileSpec((PWSTR)((PCWSTR)sParentDir));

    if(FAILED(this->_EnsurePathExists(sParentDir)))
    {
        ERRMSG
        return E_FAIL;
    }

    if(FAILED(this->_GetItemFromFile(sParentDir, &pParent)))
    {
        ERRMSG
        return E_FAIL;
    }

    // Now add it under the parent.
    if(IsDirectory(Path) == TRUE)
    {
        Name = Path;
        PathStripPath((PWSTR)(PCWSTR)Name);

        if(this->sExclusions.len() != 0)
        {
            if(PathMatchSpec(Name, this->sExclusions) == TRUE) return S_OK;
        }

        if(FAILED(pParent->NewSubproject(Name.bstr(), Comment.bstr(), &pNew)))
        {
            //CCString s;
            //s.FormatW(L"*** Failed to create subproject: %s underneath %s\n", (PCWSTR)Name, (PCWSTR)sParentDir);
            //OutputDebugString(s);

            pParent->Release();
            pParent = NULL;
            return E_FAIL;
        }

        /*
            If we must recurse subfolders, then enumerate the stuff underneath this
            folder.
        */
        if(bRecurse == TRUE)
        {
            WIN32_FIND_DATA fdd;
            HANDLE hFind;

            CCString FindString;
            FindString = Path;
            FindString.Path_Append(L"*.*");

            if((hFind = FindFirstFile(FindString, &fdd)) == INVALID_HANDLE_VALUE)
            {
                // Error...
                //x.FormatW(L"FindFirstFile(%s) failed.\n", (PCWSTR)FindString);
                //OutputDebugString(x);
                pNew->Release();
                pNew = NULL;
                return E_FAIL;
            }

            do
            {
                if(wcscmp(fdd.cFileName, L".") != 0 && wcscmp(fdd.cFileName, L"..") != 0)
                {
                    this->_Add(pNew, Path, &fdd, Comment);
                }
            }
            while(FindNextFile(hFind, &fdd));

            CloseHandle(hFind);
        }

        pNew->Release();
        pNew = NULL;
    }
    else
    {
        // add it as a file.
        if(this->sExclusions.len() != 0)
        {
            if(PathMatchSpec(Path, this->sExclusions) == TRUE) return S_OK;
        }

        if(FAILED(pParent->Add(Path.bstr(), Comment.bstr(), VSSFLAG_DELNO, &pNew)))
        {
            //CCString s;
            //s.FormatW(L"*** Failed to create new file: %s underneath %s\n", (PCWSTR)Path, (PCWSTR)sParentDir);
            //OutputDebugString(s);

            pParent->Release();
            pParent = NULL;
            return E_FAIL;
        }

        pNew->Release();
        pNew = NULL;
    }

    pParent->Release();
    pParent = NULL;

    //CCString y;
    //y.FormatW(L"*** Successful ADD(): %s underneath %s\n", (PCWSTR)Path, (PCWSTR)sParentDir);
    //OutputDebugString(y);

    return S_OK;
}


/*
    This function makes sure that the LocalSpec directory exists.  If LocalSpec is a file,
    its parent directory is the one that we focus on.
*/
HRESULT VSSDB::_EnsurePathExists(CCString LocalSpec)
{
    CCString sDir = LocalSpec;  // D:\\root\\citycraft\\help
    CCString sDb;               // $/citycraft/help
    CCString sDbTemp;           // $/citycraft
    CCString OnePath;
    CCString Comment = L"Autogenerated";
    CCString CurrentProject;
    CCString CurrentLocalProject;
    CCString AnotherTemp;
    CCString sTheRest;
    CCString sTemp;
    int nDirs = 0;// The number of directories we'll have to create.
    int i = 0;
    IVSSItem* pItem = NULL;
    IVSSItem* pNewItem = NULL;

    if(this->pDb == 0) return E_FAIL;

    if(IsDirectory(sDir) == FALSE)
    {
        PathRemoveFileSpec((PWSTR)((PCWSTR)sDir));
    }

    // We have the directory that we need to focus on.
    if(FAILED(this->_PathToSpec(sDir, &sDb)))
    {
        ERRMSG
        return E_FAIL;
    }

    sDbTemp = sDb;

    // Keep chopping off sDbTemp until we have the path.
    while(1)
    {
        if(this->_SpecExists(sDbTemp) == S_OK)
        {
            // We're ok; let's go from here.
            break;
        }

        VSSPathRemoveFileSpec(sDbTemp);

        if(sDbTemp.len() == 0)
        {
            // We have an error - even the $/ project is gone.
            ERRMSG
            return E_FAIL;
        }

        nDirs ++;
    }

    // We now have two items:  the part of the path that exists and the entire path
    // to create.  Get the REST of the path to create.
    if(nDirs == 0) return S_OK;

    if(sDbTemp.len() == 2)
    {
        sDb.Mid(2, sTheRest);
        CurrentProject = sDbTemp;
    }
    else
    {
        sDb.Mid(sDbTemp.len() + 1, sTheRest);
        CurrentProject = sDbTemp;
    }


    // Get the current project (the parent project - the one that already exists).
    if(FAILED(this->_GetItemFromSpec(CurrentProject, &pItem)))
    {
        ERRMSG
        return E_FAIL;// This would be really weird - probably a bug on my part.
    }

    for(i=0;i<nDirs;i++)
    {
        VSSPathGetOnePart(sTheRest, &OnePath);
        // Append that to the part we already have.
        sDbTemp.cat(OnePath);

        // And advance sTheRest to the next part.
        VSSPathAdvanceNextPart(sTheRest, &sTemp);
        sTheRest = sTemp;

        //this->_SpecToPath(sDbTemp, &CurrentLocalProject);

        if(FAILED(pItem->NewSubproject(OnePath.bstr(), Comment.bstr(), &pNewItem)))
        {
            // Major screw up.
            ERRMSG
            return E_FAIL;
        }

        pItem->Release();
        pItem = pNewItem;
    }

    pNewItem->Release();

    return S_OK;
}


void VSSPathRemoveFileSpec(CCString& s)
{
    if(s.len() == 0) return;

    int i = s.len() - 1;

    while(1)
    {
        if(s[i] == '/')
        {
            if(i == 1) i = 2;
            s.SetAt(i, 0);
            return;
        }

        if(i == 0) break;
        i--;
    }

    s = L"";

    return;
}


HRESULT VSSDB::_SpecExists(CCString DbSpec)
{
    if(this->pDb == NULL) return E_FAIL;

    IVSSItem* pItem;

    if(FAILED(this->pDb->get_VSSItem(DbSpec.bstr(), FALSE, &pItem)))
    {
        return S_FALSE;
    }

    pItem->Release();
    pItem = NULL;

    return S_OK;
}


HRESULT VSSDB::_GetItemFromSpec(CCString DbSpec, IVSSItem** ppItem)
{
    if(this->pDb == NULL) return E_FAIL;

    if(FAILED(this->pDb->get_VSSItem(DbSpec.bstr(), FALSE, ppItem)))
    {
//        OutputDebugString(L"VSSDB::_GetItemFromFile() pDb->get_VSSItem failed.\n");
        return E_FAIL;
    }

    return S_OK;
}


HRESULT VSSDB::_GetItemFromFile(CCString Path, IVSSItem** ppItem)
{
    CCString DbSpec;

    if(this->pDb == NULL) return E_FAIL;

    if(FAILED(this->_PathToSpec(Path, &DbSpec)))
    {
//        OutputDebugString(L"VSSDB::_GetItemFromFile() Couldn't get spec from path.\n");
        return E_FAIL;
    }

    return this->_GetItemFromSpec(DbSpec, ppItem);
}


void VSSPathGetOnePart(CCString sPath, CCString* pOut)
{
    int iNextPart = sPath.Find(L'/', 0);

    if(iNextPart == -1)
    {
        *pOut = sPath;
        return;
    }

    sPath.Left(iNextPart - 1, *pOut);
    return;
}


void VSSPathAdvanceNextPart(CCString sPath, CCString* pOut)
{
    int iNextPart = sPath.Find(L'/', 0);

    if(iNextPart == -1)
    {
        *pOut = "";
        return;
    }

    sPath.Mid(iNextPart + 1, *pOut);
    return;
}


HRESULT VSSDB::SetRoot(CCString Path)
{
    //CCString s;
    //s.FormatW(L"VSSDB::SetRoot(\"%s\")\n", (PCWSTR)Path);
    //OutputDebugString(s);

    if(this->pDb == 0) return FALSE;

    wcscpy(this->wszLocalRoot, Path);
    wcslwr(this->wszLocalRoot);

    IVSSItem* pItem = NULL;

    if(FAILED(this->_GetItemFromSpec(L"$/", &pItem)))
    {
        //OutputDebugString(L"Get item from spec failed.\n");
        return E_FAIL;
    }

    if(FAILED(pItem->put_LocalSpec(Path.bstr())))
    {
        //OutputDebugString(L"put_LocalSpec failed.\n");
        pItem->Release();
        pItem = NULL;
        return E_FAIL;
    }

    //OutputDebugString(L"Success!\n");

    pItem->Release();
    pItem = NULL;

    return S_OK;
}


HRESULT VSSDB::Diff(CCString Path)
{
    CCString TempFile;
    CCString DbSpec;
    CCString CmdLine;

    if(this->pDb == 0) return E_FAIL;

    // Get the DB spec.
    this->_PathToSpec(Path, &DbSpec);

    // Create a temporary filename
    TempFile.GetTempFileName();

    // We have to extract the db version in order to diff it.
    if(FAILED(this->_Sync(DbSpec, TempFile, TRUE)))
    {
        // Couldn't grab files from SourceSafe to do a diff.
        ERRMSG
        return E_FAIL;
    }

    // Diff them by first constructing a command line
    CmdLine.FormatW(L"windiff.exe %s %s", (PCWSTR)Path, (PCWSTR)TempFile);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    GetStartupInfo(&si);
    CreateProcess(NULL, (PWSTR)(PCWSTR)CmdLine, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
    CloseHandle(pi.hProcess);

    return S_OK;
}


HRESULT VSSDB::ViewDbVersion(CCString Path)
{
    return S_OK;
}


HRESULT VSSDB::_Sync(CCString DbSpec, CCString Path, BOOL bRecurse)
{
    if(this->pDb == 0) return E_FAIL;

    IVSSItem* pItem = NULL;

    if(FAILED(this->_GetItemFromSpec(DbSpec, &pItem)))
    {
        return FALSE;
    }

    BSTR wsz = Path.bstr();

    if(FAILED(pItem->Get(&wsz, VSSFLAG_FORCEDIRNO | 
        (bRecurse == TRUE ? VSSFLAG_RECURSYES : VSSFLAG_RECURSNO) |
        VSSFLAG_REPSKIP
        )))
    {
        ERRMSG
        pItem->Release();
        pItem = NULL;
        return E_FAIL;
    }

    pItem->Release();
    pItem = NULL;
    
    return S_OK;
}


HRESULT VSSDB::Revert(CCString Path, BOOL bRecurse, BOOL bClobber)
{
    if(this->pDb == 0) return E_FAIL;

    IVSSItem* pItem = NULL;

    if(FAILED(this->_GetItemFromFile(Path, &pItem)))
    {
        ERRMSG
        return E_FAIL;
    }

    if(FAILED(pItem->UndoCheckout(Path.bstr(), 
        (bRecurse == TRUE ? VSSFLAG_RECURSYES : VSSFLAG_RECURSNO) |
        (bClobber == TRUE ? VSSFLAG_REPSKIP : 0)
        )))
    {
        ERRMSG
        pItem->Release();
        pItem = NULL;
        return E_FAIL;
    }

    pItem->Release();
    pItem = NULL;

    return S_OK;
}


BOOL VSSDB::IsWithinDomain(CCString Path)
{
//    CCString s;
//    s.FormatW(L"    IsWithinDomain(%s)\n    ROOT: %s\n", (PCWSTR)Path, this->wszLocalRoot);
//    OutputDebugString(s);
    if(this->pDb == 0) return FALSE;

    int i;

    if(Path.FindI(this->wszLocalRoot, 0) == 0)
    {
        i = wcslen(this->wszLocalRoot);
        if(Path[i] == '\\' || Path[i] == 0) return TRUE;
    }

    return FALSE;
}


HRESULT VSSDB::_Add(IVSSItem* pParent, CCString ParentDir, WIN32_FIND_DATA* pfdd, CCString Comment)
{
    // For our own find operations.
    HANDLE hFind;
    WIN32_FIND_DATA fdd;
    CCString x;
    CCString FindString;

    CCString FullPath;
    IVSSItem* pItem = NULL;

    //x.FormatW(L"VSSDB::_Add(%s, %s)\n", (PCWSTR)ParentDir, pfdd->cFileName);
    //OutputDebugString(x);
    if(this->sExclusions.len() != 0)
    {
        if(PathMatchSpec(pfdd->cFileName, this->sExclusions) == TRUE) return S_OK;
    }

    // Don't include SCC files into source code control.
    if(wcsicmp(PathFindExtension(pfdd->cFileName), L"SCC") == 0) return S_OK;

    if(pfdd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        // Add it as a new project.
        x = pfdd->cFileName;

        if(FAILED(pParent->NewSubproject(x.bstr(), Comment.bstr(), &pItem)))
        {
            // Failed.
            return E_FAIL;
        }

        // Now do our own recursion.
        FindString = ParentDir;
        FindString.Path_Append(pfdd->cFileName);
        FindString.Path_Append(L"*.*");

        hFind = FindFirstFile(FindString, &fdd);
        if(hFind == INVALID_HANDLE_VALUE)
        {
            // Error...
            //x.FormatW(L"VSSDB::_Add::FindFirstFile(%s) failed.\n", (PCWSTR)FindString);
            //OutputDebugString(x);
            pItem->Release();
            pItem = NULL;
            return E_FAIL;
        }

        do
        {
            //x.FormatW(L"VSSDB::_Add::Examining(%s).\n", fdd.cFileName);
            //OutputDebugString(x);

            if(wcscmp(fdd.cFileName, L".") != 0 && wcscmp(fdd.cFileName, L"..") != 0)
            {
                FindString = ParentDir;
                FindString.Path_Append(pfdd->cFileName);
                this->_Add(pItem, FindString, &fdd, Comment);
            }
        }
        while(FindNextFile(hFind, &fdd));

        CloseHandle(hFind);

        pItem->Release();
        pItem = NULL;
    }
    else
    {
        FullPath = ParentDir;
        FullPath.Path_Append(pfdd->cFileName);

        // Add it as a new file.
        if(FAILED(pParent->Add(FullPath.bstr(), Comment.bstr(), VSSFLAG_DELNO, &pItem)))
        {
            // Failed.
            //x.FormatW(L"FAILED: pParent->Add(%s, %s, , %08x)\n", (PCWSTR)FullPath, (PCWSTR)Comment, pItem);
            //OutputDebugString(x);
            return E_FAIL;
        }

        pItem->Release();
        pItem = NULL;
    }

    //x.FormatW(L"SUCCEEDED: _Add(%s, %s)\n", (PCWSTR)ParentDir, pfdd->cFileName);
    //OutputDebugString(x);
    return S_OK;
}


HRESULT VSSDB::Remove(CCString Path, BOOL bDestroy)
{
    if(this->pDb == NULL) return E_FAIL;

    IVSSItem* pItem = NULL;

    if(FAILED(this->_GetItemFromFile(Path, &pItem)))
    {
        ERRMSG
        return E_FAIL;
    }

    if(bDestroy == TRUE)
    {
        if(FAILED(pItem->Destroy()))
        {
            ERRMSG
            pItem->Release();
            pItem = NULL;
            return E_FAIL;
        }
    }
    else
    {
        if(FAILED(pItem->put_Deleted(TRUE)))
        {
            ERRMSG
            pItem->Release();
            pItem = NULL;
            return E_FAIL;
        }
    }

    pItem->Release();
    pItem = NULL;

    return S_OK;
}
