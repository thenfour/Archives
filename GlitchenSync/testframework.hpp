/*
	2008-06-11
	originally written for testing LibCC. it's evolving!

	2007-03-14
	initial add in svn.winprog.org
*/

#pragma once


#include <vector>
#include <list>
#include <iostream>
#include "..\libcc\libcc\stringutil.hpp"
#include "..\libcc\libcc\log.hpp"

namespace LibCC
{

class TestHarness
{
public:
	struct TestState
	{
		int assertCount;
		int assertPass;
		LibCC::Timer timer;
	};
	
	std::list<TestState> runningTests;

private:
	Log testLog;
	int testHarnessIndent;

	void InitRun(const std::wstring& testName)
	{
		testHarnessIndent ++;
		runningTests.push_back(TestState());
		TestState& state = runningTests.back();

		state.timer.Tick();
		state.assertCount = 0;
		state.assertPass = 0;

		TestMessage(FormatW(L"Running: %")(testName).Str());
	}
	bool EndRun()
	{
		bool r = false;
		TestState& state = runningTests.back();
		state.timer.Tick();
		if(state.assertCount == state.assertPass)
		{
			TestMessage(FormatW(L"Total: PASS 100^% (% tests; % seconds)")
				(state.assertCount)
				.d<2>(state.timer.GetLastDelta())
				.Str()
				);
			r = true;
		}
		else
		{
			TestMessage(FormatW(L"Total: FAIL (% of % passed; %^%; % seconds)")
				(state.assertPass)
				(state.assertCount)
				.i(state.assertPass * 100 / state.assertCount)
				.d<2>(state.timer.GetLastDelta())
				.Str());
		}
		runningTests.pop_back();
		-- testHarnessIndent;
		return r;
	}

public:
	TestHarness()
	{
		testLog.Create(L"Test Harness", GetModuleHandle(0), true, true, false, false, true, false);
		testHarnessIndent = 1;
	}

	template<typename Char>
	inline void TestMessage(const std::basic_string<Char>& msg)
	{
		std::wstring indent;
		for(size_t i = 0; i < runningTests.size(); ++ i)
		{
			indent.append(L"  ");
		}
		std::wstring output = FormatW("%%")(indent)(msg).Str();
		testLog.Message(output);
	}

	template<typename Char>
	inline void TestMessage(const Char* msg)
	{
		TestMessage(std::basic_string<Char>(msg));
	}

	template<typename TargRef>
	bool Run(void (*proc)(TestHarness&, TargRef&), const std::wstring& testName, TargRef& arg)
	{
		InitRun(testName);
		proc(*this, arg);
		return EndRun();
	}

	template<typename TargRef1, typename TargRef2>
	bool Run(void (*proc)(TestHarness&, TargRef1&, TargRef2&), const std::wstring& testName, TargRef1& arg1, TargRef2& arg2)
	{
		InitRun(testName);
		proc(*this, arg1, arg2);
		return EndRun();
	}

	// call this from tests to judge the results of an assertion. this is how tests pass or fail.
	// you should actually use the TestAssert() macro to make calling this easier
	inline bool __TestAssert(bool b, const char* sz, const char* file, int line)
	{
		for(std::list<TestHarness::TestState>::iterator it = runningTests.begin(); it != runningTests.end(); ++ it)
		{
			it->assertCount ++;
		}

		if(!b)
		{
			TestMessage(FormatW(L"FAILED: % (line:%, %)")(sz)(line)(file).Str());
		}
		else
		{
			for(std::list<TestHarness::TestState>::iterator it = runningTests.begin(); it != runningTests.end(); ++ it)
			{
				it->assertPass ++;
			}
			TestMessage(FormatW(L"PASS: %")(sz).Str());
		}

		return b;
	}

	template<typename T>
	inline bool __TestAssertEq(const T& a, const T& b, const char* strA, const char* strB, const char* file, int line)
	{
		for(std::list<TestHarness::TestState>::iterator it = runningTests.begin(); it != runningTests.end(); ++ it)
		{
			it->assertCount ++;
		}

		bool ret = a == b;

		if(!ret)
		{
			TestMessage(FormatW(L"FAILED: % == % (% != %) (line:%, %)")(strA)(strB)(a)(b)(line)(file).Str());
		}
		else
		{
			for(std::list<TestHarness::TestState>::iterator it = runningTests.begin(); it != runningTests.end(); ++ it)
			{
				it->assertPass ++;
			}
			TestMessage(FormatW(L"PASS: % == % (%)")(strA)(strB)(b).Str());
		}

		return ret;
	}

	// call this from tests to judge the results of an assertion. this is how tests pass or fail.
	// you should actually use the TestAssert() macro to make calling this easier
	inline bool __TestAssertWithContext(bool b, const char* sz, const std::wstring& context, const char* file, int line)
	{
		for(std::list<TestHarness::TestState>::iterator it = runningTests.begin(); it != runningTests.end(); ++ it)
		{
			it->assertCount ++;
		}

		if(!b)
		{
			TestMessage(FormatW(L"FAILED: % % (line:%, %)")(sz)(context)(line)(file).Str());
		}
		else
		{
			for(std::list<TestHarness::TestState>::iterator it = runningTests.begin(); it != runningTests.end(); ++ it)
			{
				it->assertPass ++;
			}
			TestMessage(FormatW(L"PASS: % %")(sz)(context).Str());
		}

		return b;
	}

	template<typename T>
	inline bool __TestAssertEqWithContext(const T& a, const T& b, const char* strA, const char* strB, const std::wstring& context, const char* file, int line)
	{
		for(std::list<TestHarness::TestState>::iterator it = runningTests.begin(); it != runningTests.end(); ++ it)
		{
			it->assertCount ++;
		}

		bool ret = a == b;

		if(!ret)
		{
			TestMessage(FormatW(L"FAILED: % == % (% != %) % (line:%, %)")(strA)(strB)(a)(b)(context)(line)(file).Str());
		}
		else
		{
			for(std::list<TestHarness::TestState>::iterator it = runningTests.begin(); it != runningTests.end(); ++ it)
			{
				it->assertPass ++;
			}
			TestMessage(FormatW(L"PASS: % == % (%) %")(strA)(strB)(b)(context).Str());
		}

		return ret;
	}


};

// call it like  myTestHarness.TestAssert(condition==expected);
#define TestAssert(x) __TestAssert((x), #x, __FILE__, __LINE__)
#define TestAssertEq(x,y) __TestAssertEq((x), (y), #x, #y, __FILE__, __LINE__)

#define TestAssertWithContext(x,context) __TestAssertWithContext((x), #x, (context), __FILE__, __LINE__)
#define TestAssertEqWithContext(x,y,context) __TestAssertEqWithContext((x), (y), #x, #y, (context), __FILE__, __LINE__)

}
