

#pragma once
#include <string>
#include <vector>
#include "configuration.h"
#include "testframework.hpp"
#include "test.hpp"
#include "../libcc/libcc/parse.hpp"


struct TestParams
{
	Path ntfs;
	Path fat;
	bool verbose;
};


namespace GlitchenSyncAutomatedTests
{
	struct DirEntryLine
	{
		DirEntryLine() :
			secondsExists(false),
			contentsExists(false),
			rhsPathExists(false),
			verify(false),
			verifyExists(false),
			verifyNotExists(false),
			delete_(false)
		{
		}
		std::wstring path;

		bool verify;
		bool verifyExists;
		bool verifyNotExists;
		bool delete_;

		int seconds;
		bool secondsExists;

		std::wstring contents;
		bool contentsExists;

		std::wstring rhsPath;
		bool rhsPathExists;

		std::wstring scriptContext;
	};

	struct DirStructure
	{
		std::vector<DirEntryLine> lines;
	};

	struct TestSubscriber
	{
		TestSubscriber() :
			isSource(false),
			isDestination(false)
		{
		}

		std::wstring id;
		std::wstring path;
		bool isSource;
		bool isDestination;
		std::vector<std::wstring> sourceFilterIncludes;
		std::vector<std::wstring> sourceFilterExcludes;
		std::vector<std::wstring> destinationFilterIncludes;
		std::vector<std::wstring> destinationFilterExcludes;
	};

	struct RunSync
	{
		RunSync() :
			expectedAnalysisModified(-1),
			expectedAnalysisDeleted(-1),
			expectedAnalysisChangedToDirectory(-1),
			expectedAnalysisChangedToFile(-1),
			expectedActionsCopied(-1),
			expectedActionsDeleted(-1),
			comparisonType(::SyncScript::ModifiedDateUTC),
			nextSubscriberID(1)
		{
		}

		::SyncScript::ComparisonType comparisonType;

		std::vector<TestSubscriber> subscribers;
		std::map<std::wstring, TestSubscriber> storedSubscribers;

		int expectedAnalysisModified;
		int expectedAnalysisDeleted;
		int expectedAnalysisChangedToDirectory;
		int expectedAnalysisChangedToFile;

		int expectedActionsCopied;
		int expectedActionsDeleted;

		LibCC::Parse::ScriptCursor expectedAnalysisModifiedContext;
		LibCC::Parse::ScriptCursor expectedAnalysisDeletedContext;
		LibCC::Parse::ScriptCursor expectedAnalysisChangedToDirectoryContext;
		LibCC::Parse::ScriptCursor expectedAnalysisChangedToFileContext;

		LibCC::Parse::ScriptCursor expectedActionsCopiedContext;
		LibCC::Parse::ScriptCursor expectedActionsDeletedContext;

		int nextSubscriberID;
	};

	struct Command
	{
		enum
		{
			TUnknown,
			TDirStructure,
			TRunSync,
			TBreak
		}
		type;

		Command() :
			type(TUnknown)
		{
		}

		DirStructure dirStruct;
		RunSync runSync;

		std::wstring scriptContext;
	};

	struct Test
	{
		Test() :
			solo(false),
			enabled(true)
		{
		}
		bool solo;
		bool enabled;
		std::wstring name;
		std::vector<Command> commands;

		std::map<std::wstring, RunSync> storedRunSync;
	};

	struct Script
	{
		std::vector<Test> m_tests;
	};


	bool ParseScript(const std::wstring& script, Script& s);


	void RunAutomatedTests(LibCC::TestHarness& t, TestParams& params);
}

