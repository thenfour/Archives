/*

TODO: make this actually a thread pool and not a bunch of instantly fired threads.

*/

#pragma once

#include <Windows.h>
#include "..\libcc\libcc/Result.hpp"
#include <vector>
#include <list>
#include <process.h>

// sets a value somewhere on destruction.
template<typename T>
class PushValue
{
public:
	PushValue(T& valRef, const T& newTemporaryValue) :
		m_valueRef(valRef)
	{
		m_oldValue = m_valueRef;
		m_valueRef = newTemporaryValue;
	}

	~PushValue()
	{
		m_valueRef = m_oldValue;
	}

	T& m_valueRef;
	T m_oldValue;
};


class CritSecLock
{
public:
	CritSecLock(CRITICAL_SECTION& cs) :
		m_cs(cs)
	{
		EnterCriticalSection(&m_cs);
	}

	~CritSecLock()
	{
		LeaveCriticalSection(&m_cs);
	}

	CRITICAL_SECTION& m_cs;
};

#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

inline void SetThreadName(const char* threadName)
{
   Sleep(10);
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = threadName;
	 info.dwThreadID = GetCurrentThreadId();
   info.dwFlags = 0;

   __try
   {
      RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
   }
   __except(EXCEPTION_EXECUTE_HANDLER)
   {
   }
}

// NOTE: THIS CLASS IS NOT THREAD-SAFE!
// this "thread pool" executes things all in different threads right away. extremely simple.
//class ThreadPool
//{
//public:
//	typedef unsigned (__stdcall *Proc)(void*);
//
//private:
//	std::vector<HANDLE> m_threads;
//
//	struct ThreadParams
//	{
//		Proc proc;
//		void* param;
//	};
//
//	static unsigned __stdcall ThreadProcInternal(void* param)
//	{
//		ThreadParams* x = (ThreadParams*)param;
//		unsigned ret = x->proc(x->param);
//		delete x;
//		return ret;
//	}
//
//public:
//
//	LibCC::Result AddTask(Proc proc, void* args, const std::wstring& name)
//	{
//		ThreadParams* x = new ThreadParams;
//		x->param = args;
//		x->proc = proc;
//
//		SetThreadName(LibCC::ToUTF8(name).c_str());
//		HANDLE hThread = (HANDLE)_beginthreadex(0, 0, ThreadProcInternal, x, 0, 0);
//		if(hThread == 0)
//			return LibCC::Result::Failure(LibCC::Format(L"_beginthreadex returned 0. gle=%").gle().Str());
//		m_threads.insert(m_threads.begin(), hThread);
//
//		return LibCC::Result::Success();
//	}
//
//	void WaitUntilCompletion()
//	{
//		if(!m_threads.empty())
//		{
//			WaitForMultipleObjects((DWORD)m_threads.size(), &m_threads[0], TRUE, INFINITE);
//			m_threads.clear();
//		}
//	}
//};




//// this "thread pool" executes things sequentially - easier for debugging & logging.
//class ThreadPool
//{
//public:
//	typedef unsigned (__stdcall *Proc)(void*);
//
//private:
//	struct TaskData
//	{
//		Proc proc;
//		void* args;
//		std::wstring name;
//	};
//
//	HANDLE m_thread;
//	std::list<TaskData> m_queue;
//	CRITICAL_SECTION m_cs;
//
//	static unsigned __stdcall ThreadProcInternal(void* param)
//	{
//		((ThreadPool*)param)->ThreadProcInternal();
//		return 0;
//	}
//
//	void ThreadProcInternal()
//	{
//		TaskData td;
//		while(true)
//		{
//			{
//				CritSecLock l(m_cs);
//				if(m_queue.empty())
//					return;
//				td = m_queue.front();
//				m_queue.pop_front();
//			}
//
//			// deal with this task.
//			SetThreadName(LibCC::ToUTF8(td.name).c_str());
//			td.proc(td.args);
//		}
//	}
//
//public:
//	ThreadPool() :
//		m_thread(0)
//	{
//		InitializeCriticalSection(&m_cs);
//	}
//
//	~ThreadPool()
//	{
//		DeleteCriticalSection(&m_cs);
//	}
//
//	LibCC::Result AddTask(Proc proc, void* args, const std::wstring& name)
//	{
//		EnterCriticalSection(&m_cs);
//		m_queue.push_back(TaskData());
//		m_queue.back().proc = proc;
//		m_queue.back().args = args;
//		m_queue.back().name = name;
//		LeaveCriticalSection(&m_cs);
//
//		if(m_thread != 0)
//			return LibCC::Result::Success();
//
//		m_thread = (HANDLE)_beginthreadex(0, 0, ThreadProcInternal, this, 0, 0);
//		if(m_thread == 0)
//			return LibCC::Result::Failure(LibCC::Format(L"_beginthreadex returned 0. gle=%").gle().Str());
//
//		return LibCC::Result::Success();
//	}
//
//	void WaitUntilCompletion()
//	{
//		if(m_thread == 0)
//			return;
//		WaitForSingleObject(m_thread, INFINITE);
//		m_thread = 0;
//	}
//};



// this one doesn't even create threads; it just executes the task immediately when "queued"
class ThreadPool
{
public:
	typedef unsigned (__stdcall *Proc)(void*);

	std::vector<int> m_results;

	LibCC::Result AddTask(Proc proc, void* args, const std::wstring& name)
	{
		m_results.push_back(proc(args));
		return LibCC::Result::Success();
	}

	void WaitUntilCompletion() { }
};


