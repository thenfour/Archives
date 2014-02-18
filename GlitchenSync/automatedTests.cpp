
#include "stdafx.h"

/*
	Test script syntax:
	
	TestName [ CommandName [ ... ] ]
	
	For DirStructure commands, enter symbols in the form:
		path ["contents of file", 1]
	where 1 is a number of seconds used to generate a modified date for the file.
	Put NTFS or FAT32 as the root directory ALWAYS.
	you can omit any combination of stuff there - only path is required.

	NOTE: Directories must end in a backslash.
	
	To verify a file, use either syntax:
		path = path
		path = ["contents", 1]
	This compares files, or checks that a file has the given contents & calculated modified date.

	also:

	path = [exists]   - check if the path exists
	path = [missing]  - check if the path does not exist
	path [delete]     - delete the path
	break             - triggers a breakpoint in the debugger.

	break can also be used as a command in between DirStructure and RunSync
	
	For RunSync commands, the syntax is:
		RunSync(a->b)
		[
			ModifiedDateUTC
			Subscriber
			[
				"path"
				source
				destination
				SourceFilterInclude = ...
				SourceFilterExclude = ...
				DestinationFilterInclude = ...
				DestinationFilterExclude = ...
			]
			ExpectedAnalysis
			[
				modified = 3
				deleted = 3
				changedToDirectory = 3
				changedToFile = 4
			]
			ExpectedActions
			[
				copied = 3,
				deleted = 3,
			]
		]
	Specifying 'source', 'destination' indicates that the subscriber is a sync source / destination.
	ModifiedDateUTC or MD5 are supported.
	
	a->b refers to storing the results of this specification somewhere. RunSync(->x) will store these parameters in 'x'.
	To re-run with exactly the same parameters, you can simply do RunSync(x)[].
	To recall parameters AND assign your modified values somewhere, RunSync(a->b)[ ... ]

	Each test will be run in a self-contained directory structure.
	ntfsSandbox\\AutomatedTests\\TestName\\...

	Members of expected analysis / actions are optional
*/

#include "utility.hpp"
#include "resource.h"
#include "..\libcc\libcc/stringutil.hpp"
#include "automatedTests.hpp"
#include "DateTime.h"
#include "glitchenSync.hpp"
#include "../libcc/libcc/parse.hpp"

using namespace LibCC::Parse;

namespace GlitchenSyncAutomatedTests
{
	Parser Identifier(std::wstring& out)
	{
		return CharOfI(L"abcdefghijklmnopqrstuvwxyz_", CharToStringOutput(out))
				+ ZeroOrMore(CharOfI(L"0123456789abcdefghijklmnopqrstuvwxyz_", CharToStringOutput(out)));
	}

	template<typename Toutput>
	struct StringQuotesOptionalT : public ParserWithOutput<Toutput, StringQuotesOptionalT<Toutput> >
	{
		std::wstring GetParserName() const
		{
			return L"StringQuotesOptional";
		}

		StringQuotesOptionalT(const Toutput& output_) : ParserWithOutput(output_) { }
		bool Parse(ParseResult& result, ScriptReader& input)
		{
			std::wstring out;

			Parser unQuoted = 
				OneOrMore
				(
					Not(CharOfI(L"[],\"'") | Space())
					+ Char(0, CharToStringOutput(out))
				);

			bool ret = (unQuoted | StringParser(out)).ParseRetainingStateOnError(result, input);
			if(!ret)
				return false;

			output.Save(input, out);
			return true;
		}
	};

	template<typename Toutput>
	StringQuotesOptionalT<Toutput> StringQuotesOptional(const Toutput& output_)
	{
		return StringQuotesOptionalT<Toutput>(output_);
	}

	inline std::wstring ScriptCursorToString(const ScriptCursor& input)
	{
		return LibCC::FormatW(L"(tests.txt, line:{0} col:{1})")(input.line)(input.column).Str();
	}

	inline std::wstring GetScriptContext(ScriptReader& input)
	{
		return ScriptCursorToString(input.GetCursorCopy());
	}

	template<typename Toutput>
	struct BreakCommandParserT : public ParserWithOutput<Toutput, BreakCommandParserT<Toutput> >
	{
		std::wstring GetParserName() const
		{
			return L"BreakCommandParser";
		}

		BreakCommandParserT(const Toutput& output_) : ParserWithOutput(output_) { }
		bool Parse(ParseResult& result, ScriptReader& input)
		{
			if(!(StrI(L"break").ParseRetainingStateOnError(result, input)))
				return false;
			Command c;
			c.type = Command::TBreak;
			c.scriptContext = GetScriptContext(input);
			output.Save(input, c);
			return true;
		}
	};

	template<typename Toutput>
	BreakCommandParserT<Toutput> BreakCommandParser(const Toutput& output_)
	{
		return BreakCommandParserT<Toutput>(output_);
	}

	/*
		path
		path [contents, date]
		path = [contents, date]
		path = [exists|delete|missing]
		path = path
	*/
	template<typename Toutput>
	struct DirEntryLineParserT : public ParserWithOutput<Toutput, DirEntryLineParserT<Toutput> >
	{
		std::wstring GetParserName() const
		{
			return L"DirEntryLineParser";
		}

		DirEntryLineParserT(const Toutput& output_) : ParserWithOutput(output_) { }
		bool Parse(ParseResult& result, ScriptReader& input)
		{
			DirEntryLine temp;
			wchar_t ch;// throwaway
			std::wstring ws;// throwaway
			std::wstring specialCommand;// exists / delete / missing

			temp.scriptContext = GetScriptContext(input);

			// set up grammar
			Parser specialCommandP =
				(
					Char('=', ExistsOutput(ch, temp.verify)) + Char('[') +
					(
						StrI(L"exists", ExistsOutput(ws, temp.verifyExists)) | StrI(L"missing", ExistsOutput(ws, temp.verifyNotExists))
					)
					+ Char(']')
				)
				|(
					Char('[') + StrI(L"delete", ExistsOutput(ws, temp.delete_)) + Char(']')
				);

			Parser contentsP =
				Char('[') +
				(
					FalseMsg(L"Contents string failed to parse", StringQuotesOptional(RefOutput(temp.contents)))
					+ Optional(Char(',', ExistsOutput(ch, temp.secondsExists)) + CInteger2(temp.seconds))
				)
				+ Char(']');

			Parser p = 
				Not(Char(']')) +
				FalseMsg(L"Failed to parse dir entry line",
					StringQuotesOptional(RefOutput(temp.path)) +
					Optional
					(
						FalseMsg(L"=specialCommandP failed to parse", specialCommandP)
						| FalseMsg(L"Contents failed to parse", contentsP)
						| FalseMsg(L"=Contents failed to parse", (Char('=', ExistsOutput(ch, temp.verify)) + contentsP))
						| FalseMsg(L"=path failed to parse", (Char('=', ExistsOutput(ch, temp.verify)) + StringQuotesOptional(RefOutput(temp.rhsPath))))
					)
				);

			// parse
			if(!p.ParseRetainingStateOnError(result, input))
				return false;

			// correct a couple things
			temp.contentsExists = !temp.contents.empty();
			temp.rhsPathExists = !temp.rhsPath.empty();

			output.Save(input, temp);

			return true;
		}
	};

	template<typename Toutput>
	DirEntryLineParserT<Toutput> DirEntryLineParser(const Toutput& output_)
	{
		return DirEntryLineParserT<Toutput>(output_);
	}

	template<typename Toutput>
	struct DirStructureCommandParserT : public ParserWithOutput<Toutput, DirStructureCommandParserT<Toutput> >
	{
		std::wstring GetParserName() const
		{
			return L"DirStructureCommandParser";
		}

		DirStructureCommandParserT(const Toutput& output_) : ParserWithOutput(output_) { }
		bool Parse(ParseResult& result, ScriptReader& input)
		{
			Command c;
			c.type = Command::TDirStructure;
			c.scriptContext = GetScriptContext(input);
			Parser p = 
				StrI(L"DirStructure")
				+ FalseMsg(L"Missing '[' for DirStructure command", Char(L'['))
				+ ZeroOrMoreS(DirEntryLineParser(VectorOutput(c.dirStruct.lines)))
				+ FalseMsg(L"Missing ']' for DirStructure command", Char(L']'));
			if(!p.ParseRetainingStateOnError(result, input))
				return false;
			output.Save(input, c);
			return true;
		}
	};

	template<typename Toutput>
	DirStructureCommandParserT<Toutput> DirStructureCommandParser(const Toutput& output_)
	{
		return DirStructureCommandParserT<Toutput>(output_);
	}

	/*
		Subscriber(a->b)
		[
			"path"
			source
			destination
			SourceFilterInclude = ...
			SourceFilterExclude = ...
			DestinationFilterInclude = ...
			DestinationFilterExclude = ...
		]
	*/
	template<typename Toutput>
	struct SubscriberParserT : public ParserWithOutput<Toutput, SubscriberParserT<Toutput> >
	{
		std::wstring GetParserName() const
		{
			return L"SubscriberParser";
		}

		SubscriberParserT(const Toutput& output_) : ParserWithOutput(output_) { }
		bool Parse(ParseResult& result, ScriptReader& input)
		{
			RunSync rs = output.Value();
			std::wstring s;// throwaway for template deduction purposes
			wchar_t ch;//throwaway
			std::wstring recall;
			std::wstring store;
			
			// parse the subscriber name and recall / storage option.
			Parser p =
				StrI(L"Subscriber") +
				Optional
				(
					Char(L'(')
					+ Optional(Identifier(recall))
					+ Optional(Str(L"->") + Identifier(store))
					+ Char(L')')
				);

			if(!p.ParseRetainingStateOnError(result, input))
				return false;

			// grab original values if it's a recall.
			TestSubscriber temp;
			if(!recall.empty())
			{
				temp = rs.storedSubscribers[recall];
			}

			// default id value.
			temp.id = LibCC::Format()(rs.nextSubscriberID ++).Str();

			bool negativeSource = false;
			bool negativeDestination = false;

			// do the real parse
			p = *Space() + Char(L'[')
				+ ZeroOrMoreS(
					(StrI(L"SourceFilterInclude") + Char('=') + StringQuotesOptional(VectorOutput(temp.sourceFilterIncludes)))
					| (StrI(L"SourceFilterExclude") + Char('=') + StringQuotesOptional(VectorOutput(temp.sourceFilterExcludes)))
					| (StrI(L"DestinationFilterInclude") + Char('=') + StringQuotesOptional(VectorOutput(temp.destinationFilterIncludes)))
					| (StrI(L"DestinationFilterExclude") + Char('=') + StringQuotesOptional(VectorOutput(temp.destinationFilterExcludes)))
					| (Optional(Char('-', ExistsOutput(ch, negativeSource))) + StrI(L"source", ExistsOutput(s, temp.isSource)))
					| (Optional(Char('-', ExistsOutput(ch, negativeDestination))) + StrI(L"destination", ExistsOutput(s, temp.isDestination)))
					| (StrI(L"id") + Char('=') + StringQuotesOptional(RefOutput(temp.id)))
					| (Not(Char(']')) + StringQuotesOptional(RefOutput(temp.path)))
				)
				+ Char(L']');

			if(!p.ParseRetainingStateOnError(result, input))
				return false;

			if(negativeSource)
				temp.isSource = false;

			if(negativeDestination)
				temp.isDestination = false;

			// store it
			if(!store.empty())
			{
				rs.storedSubscribers[store] = temp;
			}

			rs.subscribers.push_back(temp);

			output.Save(input, rs);
			return true;
		}
	};

	template<typename Toutput>
	SubscriberParserT<Toutput> SubscriberParser(const Toutput& output_)
	{
		return SubscriberParserT<Toutput>(output_);
	}

	/*
		RunSync(a->b)
		[
			ModifiedDateUTC
			MD5
			Subscriber(a->b)
			[
				"path"
				source
				destination
				SourceFilterInclude = ...
				SourceFilterExclude = ...
				DestinationFilterInclude = ...
				DestinationFilterExclude = ...
			]
			ExpectedAnalysis
			[
				modified = 3
				deleted = 3
				changedToDirectory = 3
				changedToFile = 4
			]
			ExpectedActions
			[
				copied = 3,
				deleted = 3,
			]
		]
	*/
	template<typename Toutput>
	struct RunSyncCommandParserT : public ParserWithOutput<Toutput, RunSyncCommandParserT<Toutput> >
	{
		std::wstring GetParserName() const
		{
			return L"RunSyncCommandParser";
		}

		RunSyncCommandParserT(const Toutput& output_) : ParserWithOutput(output_) { }
		bool Parse(ParseResult& result, ScriptReader& input)
		{
			Test t = output.Value();
			std::wstring s;// throwaway
			std::wstring recall;
			std::wstring store;
			//int it;// throwaway
			
			// parse the subscriber name and recall / storage option.
			Parser p =
				StrI(L"RunSync") +
				Optional
				(
					FalseMsg(L"Error parsing recall / assignment for runsync command",
						Char(L'(')
						+ Optional(Identifier(recall))
						+ Optional(Str(L"->") + Identifier(store))
						+ Char(L')')
					)
				);

			if(!p.ParseRetainingStateOnError(result, input))
				return false;

			// grab original values if it's a recall.
			Command c;
			c.type = Command::TRunSync;
			c.scriptContext = GetScriptContext(input);

			RunSync& temp(c.runSync);
			if(!recall.empty())
			{
				temp = t.storedRunSync[recall];
			}

			// do the real parse
			Parser expectedAnalysisParser = 
				StrI(L"ExpectedAnalysis")
				+ Char(L'[') +
				ZeroOrMoreS(
					(StrI(L"modified", ContextOutput(s, temp.expectedAnalysisModifiedContext)) + Char(L'=') + CInteger2(temp.expectedAnalysisModified))
					| (StrI(L"deleted", ContextOutput(s, temp.expectedAnalysisDeletedContext)) + Char(L'=') + CInteger2(temp.expectedAnalysisDeleted))
					| (StrI(L"changedToDirectory", ContextOutput(s, temp.expectedAnalysisChangedToDirectoryContext)) + Char(L'=') + CInteger2(temp.expectedAnalysisChangedToDirectory))
					| (StrI(L"changedToFile", ContextOutput(s, temp.expectedAnalysisChangedToFileContext)) + Char(L'=') + CInteger2(temp.expectedAnalysisChangedToFile))
				)
				+ Char(L']');

			Parser expectedActionsParser = 
				StrI(L"ExpectedActions")
				+ Char(L'[') + 
				ZeroOrMoreS(
					(StrI(L"copied", ContextOutput(s, temp.expectedActionsCopiedContext)) + Char(L'=') + CInteger2(temp.expectedActionsCopied))
					| (StrI(L"deleted", ContextOutput(s, temp.expectedActionsDeletedContext)) + Char(L'=') + CInteger2(temp.expectedActionsDeleted))
				)
				+ Char(L']');

			p = *Space() + FalseMsg(L"Missing [ for runsync command.", Char(L'[')) +
				ZeroOrMoreS(
					StrI(L"ModifiedDateUTC", ExistsOutput(s, temp.comparisonType, SyncScript::ModifiedDateUTC))
					| StrI(L"MD5", ExistsOutput(s, temp.comparisonType, SyncScript::MD5))
					| SubscriberParser(RefOutput(temp))
					| expectedAnalysisParser
					| expectedActionsParser
				)
				+ Char(L']');

			ScriptCursor oldCursor = input.GetCursorCopy();
			std::wstring oldContext = _DebugCursorToString(oldCursor, input);

			if(!p.ParseRetainingStateOnError(result, input))
				return false;

			ScriptCursor newCursor = input.GetCursorCopy();
			std::wstring newContext = _DebugCursorToString(newCursor, input);

			// store it
			if(!store.empty())
			{
				t.storedRunSync[store] = temp;
			}

			t.commands.push_back(c);

			output.Save(input, t);
			return true;
		}
	};

	template<typename Toutput>
	RunSyncCommandParserT<Toutput> RunSyncCommandParser(const Toutput& output_)
	{
		return RunSyncCommandParserT<Toutput>(output_);
	}

	// someTest [ ]
	template<typename Toutput>
	struct TestParserT : public ParserWithOutput<Toutput, TestParserT<Toutput> >
	{
		std::wstring GetParserName() const
		{
			return L"TestParser";
		}

		TestParserT(const Toutput& output_) : ParserWithOutput(output_) { }
		bool Parse(ParseResult& result, ScriptReader& input)
		{
			wchar_t ch;
			Test temp;
			Parser p =
				FalseMsg(L"Failed parsing test...",
					Optional(Char('-', ExistsOutput(ch, temp.enabled, false)))
					+ Optional(Char('*', ExistsOutput(ch, temp.solo, true)))
					+ Identifier(temp.name)
					+ FalseMsg(L"Missing '[' for test.", Char('['))
					+ ZeroOrMoreS
					(
						Not(Char(']'))
						+
						(
							FalseMsg(L"Failed to parse break command.", BreakCommandParser(VectorOutput(temp.commands)))
							| FalseMsg(L"Failed to parse DirStructure command.", DirStructureCommandParser(VectorOutput(temp.commands)))
							| FalseMsg(L"Failed to parse RunSync command.", RunSyncCommandParser(RefOutput(temp)))
						)
					)
				+ FalseMsg(L"Missing ']' for test.", Char(']')));

			ScriptCursor oldCursor = input.GetCursorCopy();
			std::wstring oldContext = _DebugCursorToString(oldCursor, input);

			if(!p.ParseRetainingStateOnError(result, input))
				return false;

			ScriptCursor newCursor = input.GetCursorCopy();
			std::wstring newContext = _DebugCursorToString(newCursor, input);

			output.Save(input, temp);
			return true;
		}
	};

	template<typename Toutput>
	TestParserT<Toutput> TestParser(const Toutput& output_)
	{
		return TestParserT<Toutput>(output_);
	}

	template<typename Toutput>
	struct ScriptParserT : public ParserWithOutput<Toutput, ScriptParserT<Toutput> >
	{
		std::wstring GetParserName() const
		{
			return L"ScriptParser";
		}

		ScriptParserT(const Toutput& output_) : ParserWithOutput(output_) { }
		bool Parse(ParseResult& result, ScriptReader& input)
		{
			Script temp;
			Parser p =
				*Space()
				+ ZeroOrMoreS
					(
						Not(Eof()) +
						FalseMsg(L"Failed to parse a test", TestParser(VectorOutput(temp.m_tests)))
					)
				+ *Space()
				+ Eof();

			if(!p.ParseRetainingStateOnError(result, input))
				return false;
			output.Save(input, temp);
			return true;
		}
	};

	template<typename Toutput>
	ScriptParserT<Toutput> ScriptParser(const Toutput& output_)
	{
		return ScriptParserT<Toutput>(output_);
	}


Path ParsePath(LibCC::TestHarness& t, const std::wstring& p, TestParams& params)
{
	Path ret;
	std::wstring ntfs = L"ntfs\\";
	std::wstring fat = L"fat\\";
	size_t psize = p.size();
	// if p has a trailing backslash, remove it.
	if(*p.rbegin() == '\\')
		psize --;
	std::wstring path = p.substr(0, psize);

	if(p.find(ntfs) == 0)
	{
		ret = params.ntfs[path.substr(ntfs.size())];
	}
	else if(p.find(fat) == 0)
	{
		ret = params.fat[path.substr(fat.size())];
	}
	else
	{
		t.TestAssert(!"Path needs to begin with ntfs or fat");
	}
	return ret;
}

// just generate filetimes relative to May 17, 1993
FILETIME GenerateFileTime(int i)
{
	LibCC::DateTime dt(1993, LibCC::October, 23);
	FILETIME before = dt.ToFileTime();
	dt.AddHours(i);
	FILETIME ret = dt.ToFileTime();
	return ret;
}

void RunDirStructure(DirStructure& ds, LibCC::TestHarness& t, TestParams& params)
{
	for(std::vector<DirEntryLine>::iterator it = ds.lines.begin(); it != ds.lines.end(); ++ it)
	{
		DirEntryLine& entry = *it;
		if(entry.path == L"break")
		{
			// break specified in the script.
			__asm int 3;
			continue;
		}
		Path lhs = ParsePath(t, entry.path, params);
		Path rhs;
		if(entry.rhsPathExists)
		{
			rhs = ParsePath(t, entry.rhsPath, params);
		}
		if(entry.verify)
		{
			if(entry.verifyExists)
			{
				t.TestAssertWithContext(lhs.Exists(), entry.scriptContext);
			}
			if(entry.verifyNotExists)
			{
				t.TestAssertWithContext(!lhs.Exists(), entry.scriptContext);
			}
			if(entry.rhsPathExists)
			{
				t.TestAssertWithContext(lhs.EqualsExactly(rhs), entry.scriptContext);
			}
			if(entry.contentsExists)
			{
				std::wstring actualContents = lhs.ReadUTF8TextFile();
				t.TestAssertWithContext(actualContents == entry.contents, entry.scriptContext);
			}
			if(entry.secondsExists)
			{
				t.TestAssertWithContext(Equals(lhs.GetModifiedDate(), GenerateFileTime(entry.seconds)), entry.scriptContext);
			}
		}
		else
		{
			if(entry.delete_)
			{
				t.TestAssert(lhs.Delete());
			}
			else
			{
				if(entry.contentsExists)
				{
					t.TestAssertWithContext(lhs.CreateTextFile(LibCC::ToUTF8(entry.contents)), entry.scriptContext);
				}
				if(entry.secondsExists)
				{
					lhs.SetModifiedDate(GenerateFileTime(entry.seconds));
				}
				if(*entry.path.rbegin() == '\\')// it's a directory, and not a matter of verifying or deleting. create it.
				{
					t.TestAssertWithContext(lhs.CreateDirectory_(), entry.scriptContext);
				}
			}
		}
	}
}

void RunRunSync(LibCC::TestHarness& t, const std::wstring& testName, RunSync& rs, TestParams& params, bool verbose)
{
	// convert to a ConfigFile and SyncScript
	ConfigFile config;
	config.fileName = params.ntfs["GlitchenSync.xml"].GetPathW();
	config.scripts.push_back(SyncScript());
	SyncScript& ss = config.scripts.back();
	ss.comparisonType = rs.comparisonType;
	ss.fileDB = params.ntfs["GlitchenSync.db3"].GetPathW();
	ss.name = testName;
	for(std::vector<TestSubscriber>::iterator itSubscriber = rs.subscribers.begin(); itSubscriber != rs.subscribers.end(); ++ itSubscriber)
	{
		ss.subscribers.push_back(Subscriber(ParsePath(t, itSubscriber->path, params).GetPathW(), itSubscriber->isSource, itSubscriber->isDestination));
		Subscriber& s = ss.subscribers.back();
		s.id = itSubscriber->id;
		for(std::vector<std::wstring>::iterator itFilter = itSubscriber->sourceFilterIncludes.begin(); itFilter != itSubscriber->sourceFilterIncludes.end(); ++ itFilter)
		{
			s.sourceFilters.push_back(Subscriber::Filter(Subscriber::Filter::Include, *itFilter));
		}
		for(std::vector<std::wstring>::iterator itFilter = itSubscriber->sourceFilterExcludes.begin(); itFilter != itSubscriber->sourceFilterExcludes.end(); ++ itFilter)
		{
			s.sourceFilters.push_back(Subscriber::Filter(Subscriber::Filter::Ignore, *itFilter));
		}
		for(std::vector<std::wstring>::iterator itFilter = itSubscriber->destinationFilterIncludes.begin(); itFilter != itSubscriber->destinationFilterIncludes.end(); ++ itFilter)
		{
			s.destinationFilters.push_back(Subscriber::Filter(Subscriber::Filter::Include, *itFilter));
		}
		for(std::vector<std::wstring>::iterator itFilter = itSubscriber->destinationFilterExcludes.begin(); itFilter != itSubscriber->destinationFilterExcludes.end(); ++ itFilter)
		{
			s.destinationFilters.push_back(Subscriber::Filter(Subscriber::Filter::Ignore, *itFilter));
		}
	}

	// run it!
	RepositoryFileCollection fc;
	RunSyncScript(config, ss, fc, verbose);// don't assert here. some tests expect conflicts (thus, failure)

	// check analysis
	if(rs.expectedAnalysisModified != -1)
		t.TestAssertEqWithContext(fc.afc_modifiedSources, rs.expectedAnalysisModified, ScriptCursorToString(rs.expectedAnalysisModifiedContext));

	if(rs.expectedAnalysisDeleted != -1)
		t.TestAssertEqWithContext(fc.afc_deletedSources, rs.expectedAnalysisDeleted, ScriptCursorToString(rs.expectedAnalysisDeletedContext));

	if(rs.expectedAnalysisChangedToDirectory!= -1)
		t.TestAssertEqWithContext(fc.afc_changedToDirectory, rs.expectedAnalysisChangedToDirectory, ScriptCursorToString(rs.expectedAnalysisChangedToDirectoryContext));

	if(rs.expectedAnalysisChangedToFile != -1)
		t.TestAssertEqWithContext(fc.afc_changedToFile, rs.expectedAnalysisChangedToFile, ScriptCursorToString(rs.expectedAnalysisChangedToFileContext));

	// check actions
	if(rs.expectedActionsCopied != -1)
		t.TestAssertEqWithContext(fc.action_Copied, rs.expectedActionsCopied, ScriptCursorToString(rs.expectedActionsCopiedContext));

	if(rs.expectedActionsDeleted != -1)
		t.TestAssertEqWithContext(fc.action_Deleted, rs.expectedActionsDeleted, ScriptCursorToString(rs.expectedActionsDeletedContext));

}


void RunTest(LibCC::TestHarness& t, Test& currentTest, TestParams& params)
{
	for(std::vector<Command>::iterator it = currentTest.commands.begin(); it != currentTest.commands.end(); ++ it)
	{
		switch(it->type)
		{
		case Command::TBreak:
			// break specified in the script.
			__asm int 3;
			break;
		case Command::TDirStructure:
			RunDirStructure(it->dirStruct, t, params);
			break;
		case Command::TRunSync:
			RunRunSync(t, currentTest.name, it->runSync, params, params.verbose);
			break;
		}
	}
}

void RunAutomatedTests(LibCC::TestHarness& t, TestParams& params)
{
	_CrtMemState memState = {0};
	_CrtMemCheckpoint(&memState);
	_CrtSetBreakAlloc(74413);
	{
		Script s;
		std::wstring scriptText = LoadTextFileResource(g_hInstance, MAKEINTRESOURCE(IDR_TEXT3), L"TEXT");
		Parser sp = ScriptParser(RefOutput(s));
		ParseResultMem result;
		//result.SetTraceEnabled(true);
		CScriptReader input(scriptText);
		// throw in some measurements for fun.
		LibCC::Timer timer;
		timer.Tick();
		bool r = sp.Parse(result, input);
		timer.Tick();

		t.TestMessage(LibCC::FormatA("Parsed automated test script, % bytes, in % seconds. % bytes/sec")
			(scriptText.size())
			.d<2>(timer.GetLastDelta())
			.d<2>((double)scriptText.size() / timer.GetLastDelta())
			.CStr()
			);

		for(std::vector<std::wstring>::const_iterator it = result.parseMessages.begin(); it != result.parseMessages.end(); ++ it)
		{
			t.TestMessage(*it);
		}

		bool soloMode = false;
		for(std::vector<Test>::iterator it = s.m_tests.begin(); it != s.m_tests.end(); ++ it)
		{
			if(it->solo)
			{
				soloMode = true;
				break;
			}
		}

		// RUN!!!
		for(std::vector<Test>::iterator it = s.m_tests.begin(); it != s.m_tests.end(); ++ it)
		{
			if(!soloMode || it->solo)
			{
				TestParams paramsCopy;
				paramsCopy.verbose = params.verbose;;
				paramsCopy.fat = params.fat["AutomatedTests"][it->name];
				paramsCopy.ntfs = params.ntfs["AutomatedTests"][it->name];
				if(paramsCopy.fat.Exists())
					t.TestAssert(paramsCopy.fat.Delete());
				if(paramsCopy.ntfs.Exists())
					t.TestAssert(paramsCopy.ntfs.Delete());
				t.Run(RunTest, it->name, *it, paramsCopy);
			}
		}
	}
	_CrtMemDumpAllObjectsSince(&memState);
}



}

