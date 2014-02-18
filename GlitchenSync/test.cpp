

#include "stdafx.h"
#include "test.hpp"
#include "fileDatabase.hpp"
#include "automatedTests.hpp"
#include "resource.h"

extern HINSTANCE g_hInstance;


void BasicFileDatabaseTest(LibCC::TestHarness& t, TestParams& params)
{
	t.TestAssert(params.ntfs["testdb.db3"].Delete());
	FileDatabase db(params.ntfs["testdb.db3"].GetPathW(), LoadTextFileResource(g_hInstance, MAKEINTRESOURCE(IDR_TEXT1), L"TEXT"));

	SYSTEMTIME st;
	FILETIME now;
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &now);
	MD5Result md5 = MD5Encode("abcd", 4);
	std::wstring fileName = L"test\\test.test";
	UINT64 size = 0xf0f0f0f0f0f0f0f0;

	t.TestAssert(db.UpdateFile(fileName, false, false, md5, now, size));
	FileDatabase::FileIterator it(db, fileName);

	t.TestAssert(it.Read());
	t.TestAssertEq(it.RelativePath(), fileName);
	t.TestAssertEq(it.Size(), size);
	t.TestAssert(it.ModifiedDate() == now);
	t.TestAssertEq(it.IsDirectory(), false);
}


void AllTests(LibCC::TestHarness& t, TestParams& params)
{
	//t.Run(BasicFileDatabaseTest, L"BasicFileDatabaseTest", params);
	t.Run(GlitchenSyncAutomatedTests::RunAutomatedTests, L"AutomatedTests", params);
}

void GlitchenSyncTest(const std::wstring& _ntfsSandbox, const std::wstring& _fatSandbox, bool verbose)
{
	TestParams params;
	params.ntfs = Path(_ntfsSandbox);
	params.fat = Path(_fatSandbox);
	params.verbose = verbose;

	LibCC::TestHarness t;
	t.Run(AllTests, L"All tests", params);
	
	MessageBox(0, L"", L"", MB_OK);
}


