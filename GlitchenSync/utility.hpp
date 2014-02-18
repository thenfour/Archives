

#pragma once


#include <windows.h>
#include "..\libcc\libcc\stringutil.hpp"
#include "..\libcc\libcc\winapi.hpp"
#include "DateTime.h"
#include <map>
#include <set>

#pragma warning(push)
#pragma warning(disable:4996)


inline std::wstring LoadTextFileResource(HINSTANCE hInstance, LPCTSTR szResName, LPCTSTR szResType)
{
    HRSRC hrsrc=FindResource(hInstance, szResName, szResType);
    if(!hrsrc) return L"";
    HGLOBAL hg1 = LoadResource(hInstance, hrsrc);
    DWORD sz = SizeofResource(hInstance, hrsrc);
    void* ptr1 = LockResource(hg1);

		// assume the encoding is ASCII.
		std::string a((const char*)ptr1, sz);
		return LibCC::ToUTF16(a);
} 

inline bool Equals(const FILETIME& lhs, const FILETIME& rhs)
{
	LibCC::DateTime dlhs, drhs;
	dlhs.FromFileTime(lhs);
	drhs.FromFileTime(rhs);
	LibCC::TimeSpan diff = drhs.Subtract(dlhs);
	diff.Abs();
	long secondsDifference = diff.TicksToSeconds(diff.GetTicks());
	return (secondsDifference < 1);
}

inline bool LessThan(const FILETIME& lhs, const FILETIME& rhs)
{
	LibCC::DateTime dlhs, drhs;
	dlhs.FromFileTime(lhs);
	drhs.FromFileTime(rhs);
	LibCC::TimeSpan diff = drhs.Subtract(dlhs);
	return diff.GetTicks() > 0;
}

extern int g_stdOutDisappearingTextLength;
inline void InitStdOutDisappearingText()
{
	g_stdOutDisappearingTextLength = 0;
}
inline void ClearStdOutDisappearingText()
{
	LibCC::StdOutPrint(std::string(g_stdOutDisappearingTextLength, 8));
	g_stdOutDisappearingTextLength = 0;
}
inline void SetStdOutDisappearingText(const std::string& text)
{
	ClearStdOutDisappearingText();
	LibCC::StdOutPrint(text);
	g_stdOutDisappearingTextLength = text.size();
}

inline UINT64 MakeUINT64(DWORD low, DWORD high)
{
	return ((UINT64)high << (sizeof(DWORD)*8)) + (UINT64)low;
}

template<typename _key, typename _val>
inline bool Contains(const std::map<_key, _val>& cont, const _key& k)
{
	return cont.end() != cont.find(k);
}

template<typename _key>
inline bool Contains(const std::set<_key>& cont, const _key& k)
{
	return cont.end() != cont.find(k);
}

#pragma warning(pop)
