/*
Glitchen Sync, (c) 2008 Trident Loop
Examining "c:\root" 25,000 files found
Examining "c:\root2" 25,000 files found
Analyzing (95% done)
	45 files added
	34 files deleted
	etc...
Generating sync plan (95% done)
	Conflicted:
		\locale.txt
		\blah
	50 files to be copied
	1 file to be deleted


*/

#include "stdafx.h"
#include "md5.hpp"
#include "base64.hpp"
#include "configuration.h"
#include "threadpool.h"
#include "resource.h"
#include <list>
#include "..\libcc\libcc/Log.hpp"
#include "filedatabase.hpp"
#include "filecollection.hpp"
#include "test.hpp"
#include "utility.hpp"
#include "xversion.h"


HINSTANCE g_hInstance = 0;
LibCC::Log* gslog = 0;

int g_stdOutDisappearingTextLength = 0;

extern bool RunSyncScript(ConfigFile& configFile, SyncScript& script, bool verbose);

int DisplayUsage()
{
	std::wstring usage = LoadTextFileResource(g_hInstance, MAKEINTRESOURCE(IDR_TEXT2), L"TEXT");
	std::wcout << usage;
	return 1;
}

int main2(std::vector<std::wstring>& args)
{
	///////////////////////////////////////////
	// FOR DEBUGGING PURPOSES, OVERRIDE ARGS
	{
		args.clear();
		args.push_back(L"/verbose");
		args.push_back(L"/test");
		Path ntfsSandbox(L"c:\\temp\\sandbox");
		Path fatSandbox(L"f:\\sandbox");

		if(!ntfsSandbox.CreateDirectory_())
		{
			MessageBox(0, LibCC::Format(L"the ntfs sandbox % could not be created.")(ntfsSandbox.GetPathW()).CStr(), L"GlitchenSync", MB_OK);
			return 1;
		}
		args.push_back(ntfsSandbox.GetPathW());
		if(fatSandbox.CreateDirectory_())
		{
			args.push_back(fatSandbox.GetPathW());
		}
	}
	///////////////////////////////////////////

	Version ver;
	ver.FromFile(LibCC::GetModuleFilenameW(g_hInstance));
	gslog->Message(LibCC::FormatW(L"% %, %")(ver.GetProductName())(ver.GetFileVersionString())(ver.GetCopyright()));

	int argc = args.size();

	// parse command line.
	if(argc == 0)
	{
		return DisplayUsage();
	}

	bool test = false;
	bool verbose = false;
	bool md5 = false;
	std::wstring ntfsSandbox;
	std::wstring fatSandbox;
	std::vector<std::wstring> otherPaths;

	for(std::vector<std::wstring>::const_iterator it = args.begin(); it != args.end(); ++ it)
	{
		if(LibCC::StringEqualsI(*it, L"/test"))
		{
			if(test)
			{
				std::cout << "Error parsing the command line: You already specified /test." << std::endl;
				return DisplayUsage();
			}
			test = true;

			++ it;
			if(it == args.end())
			{
				std::cout << "Error parsing the command line: You need to specify at least an NTFS sandbox for testing." << std::endl;
				return DisplayUsage();
			}

			ntfsSandbox = *it;

			++ it;
			if(it == args.end())
			{
				// no problem; you don't need a FAT32 sandbox.
				break;
			}
		}
		else if(LibCC::StringEqualsI(*it, L"/verbose"))
		{
			verbose = true;
		}
		else if(LibCC::StringEqualsI(*it, L"/md5"))
		{
			md5 = true;
		}
		else
		{
			// it's just a path. later i'll decide whether it's a config file or a sync target.
			otherPaths.push_back(*it);
		}
	}

	// some error trapping
	if(md5 && test)
	{
		std::cout << "Error parsing the command line: /md5 and /test are not compatible." << std::endl;
		return DisplayUsage();
	}

	// and execute the results.
	if(test)
	{
		GlitchenSyncTest(ntfsSandbox, fatSandbox, verbose);
		return 0;
	}
	else if(otherPaths.size() > 1)
	{
		// they are sync targets. create a default sync script
	}
	else if(otherPaths.size() == 1)
	{
		ConfigFile f;
		LibCC::Result r = f.Load(otherPaths[0]);
		if(r.Failed())
		{
			std::wcout << r.str();
			return 1;
		}
		else
		{
			return RunSyncScript(f, f.scripts[0], verbose) ? 0 : 1;
		}
	}
	else
	{
		// it's not /test, and there are no paths specified.
		return DisplayUsage();
	}

	//MessageBox(0, L"", L"", MB_OK);
	return 0;
}


int wmain(int argc, const wchar_t* argv[])
{
	int ret = 0;
	g_hInstance = GetModuleHandle(0);
	gslog = new LibCC::Log(L"GlitchenSync.log", g_hInstance, false, true, true, true, true, false);
	argc --;
	argv ++;
	std::vector<std::wstring> args;

	for(int i = 0; i < argc; ++ i)
	{
		args.push_back(argv[i]);
	}

	ret = main2(args);

	delete gslog;

	return ret;
}

