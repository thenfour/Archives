// August 8, 2001

#pragma once

#include <windows.h>
#include "SSAuto.h"
#include "CCString2.h"


// Utils
void MakeVSSPathPretty(PWSTR wsz);
void VSSPathRemoveFileSpec(CCString& s);
void VSSPathGetOnePart(CCString sPath, CCString* pOut);
void VSSPathAdvanceNextPart(CCString sPath, CCString* pOut);
BOOL IsDirectory(CCString Path);


class VSSDB
{
public:

    VSSDB();
    ~VSSDB();

    HRESULT Open(CCString Ini, CCString User, CCString Pass);
    HRESULT Close();

    HRESULT SetRoot(CCString Path);

	// Quick Utils
    BOOL    IsWithinDomain(CCString Path);
	BOOL    IsIntegrated(CCString LocalSpec);
    BOOL    IsCheckedOut(CCString LocalSpec);
    HRESULT Diff(CCString Path);
	HRESULT ViewDbVersion(CCString Path);

    HRESULT CheckOut(CCString LocalSpec, CCString Comment, BOOL bRecurse);
	HRESULT Sync(CCString Path, BOOL bRecurse, BOOL bClobber);
    HRESULT Revert(CCString Path, BOOL bRecurse, BOOL bClobber);

    HRESULT CheckIn(CCString LocalSpec, CCString Comment, BOOL bRecurse, BOOL bRemoveLocal);
    HRESULT Rename(CCString Path, CCString NewFileNameOnly);

    HRESULT Add(CCString Path, CCString Comment, BOOL bRecurse, CCString Exclusions);
    HRESULT Remove(CCString Path, BOOL bDestroy);

private:

    HRESULT _EnsurePathExists(CCString LocalSpec);
    HRESULT _PathToSpec(PCWSTR wszLocalSpec, CCString* pDbSpec);
    HRESULT _SpecToPath(CCString DbSpec, CCString* pLocalSpec);
    HRESULT _SpecExists(CCString DbSpec);
    HRESULT _GetItemFromSpec(CCString DbSpec, IVSSItem** ppItem);
    HRESULT _GetItemFromFile(CCString DbSpec, IVSSItem** ppItem);

    HRESULT _Sync(CCString DbSpec, CCString Path, BOOL bRecurse);
    HRESULT _Add(IVSSItem* pParent, CCString ParentDir, WIN32_FIND_DATA* pfdd, CCString Comment);

    IVSSDatabase* pDb;

    WCHAR wszLocalRoot[MAX_PATH];
    CCString sExclusions;

};

