// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define UNICODE
#define _UNICODE

#include <windows.h>
#include <math.h>
#include "..\libcc\libcc\stringutil.hpp"



inline int SecondsToMS(int s)
{
  return s * 1000;
}
inline int MinutesToMS(int m)
{
  return SecondsToMS(m * 60);
}
inline int HoursToMS(int h)
{
  return MinutesToMS(h * 60);
}

inline int Width(RECT& rc)
{
  return (rc.right - rc.left);
}

inline int Height(RECT& rc)
{
  return (rc.bottom - rc.top);
}

inline std::wstring TimeToString(int ms)
{
  int h = ms / HoursToMS(1);
  ms -= HoursToMS(h);
  int m = ms / MinutesToMS(1);
  ms -= MinutesToMS(m);
  int s = ms / SecondsToMS(1);
  return LibCC::Format("%:%:%").i<10,2>(h).i<10,2>(m).i<10,2>(s).Str();
}
