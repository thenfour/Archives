

#include "stdafx.h"
#include "datetime.h"

namespace LibCC
{

  TimeSpan::TimeSpan() :
	  m_ticks(0)
  {
  }

  TimeSpan::TimeSpan(__int64 ticks) :
	  m_ticks(ticks)
  {
  }

  __int64 TimeSpan::GetTicks() const
  {
	  return m_ticks;
  }

  TimeSpan::TimeSpan(long hours, long minutes, long seconds)
  {
	  m_ticks = HoursToTicks(hours) + MinutesToTicks(minutes) + SecondsToTicks(seconds);
  }

  TimeSpan::TimeSpan(long days, long hours, long minutes, long seconds)
  {
	  m_ticks = DaysToTicks(days) + HoursToTicks(hours) + MinutesToTicks(minutes) + SecondsToTicks(seconds);
  }

  TimeSpan::TimeSpan(long days, long hours, long minutes, long seconds, long milliseconds)
  {
	  m_ticks = DaysToTicks(days) + HoursToTicks(hours) +
		  MinutesToTicks(minutes) + SecondsToTicks(seconds) + MillisecondsToTicks(milliseconds);
  }

  TimeSpan::TimeSpan(const TimeSpan& r) :
	  m_ticks(r.m_ticks)
  {
  }

  void TimeSpan::Add(const TimeSpan& r)
  {
	  m_ticks += r.m_ticks;
  }


  void TimeSpan::Abs()
  {
		m_ticks = m_ticks < 0 ? -m_ticks : m_ticks;
  }

  void TimeSpan::Negate()
  {
	  m_ticks = -m_ticks;
  }

  void TimeSpan::Subtract(const TimeSpan& r)
  {
	  m_ticks -= r.m_ticks;
  }

  void TimeSpan::Multiply(double f)
  {
	  m_ticks = static_cast<__int64>(f * static_cast<double>(m_ticks));
  }

  long TimeSpan::Compare(const TimeSpan& r) const
  {
	  if(r.m_ticks == m_ticks) return 0;
	  if(m_ticks < r.m_ticks) return -1;
	  return 1;
  }

  bool TimeSpan::Equals(const TimeSpan& r) const
  {
	  return m_ticks == r.m_ticks;
  }

  bool TimeSpan::LessThan(const TimeSpan& r) const
  {
	  return m_ticks < r.m_ticks;
  }

  bool TimeSpan::GreaterThan(const TimeSpan& r) const
  {
	  return m_ticks > r.m_ticks;
  }

  bool TimeSpan::LessThanOrEquals(const TimeSpan& r) const
  {
	  return m_ticks <= r.m_ticks;
  }

  bool TimeSpan::GreaterThanOrEquals(const TimeSpan& r) const
  {
	  return m_ticks >= r.m_ticks;
  }

  long TimeSpan::TicksToSeconds(__int64 t)
  {
	  return static_cast<long>(t / SecondsToTicks(__int64(1)));
  }

  __int64 TimeSpan::MillisecondsToTicks(__int64 s)
  {
	  return s * 10000;
  }

  __int64 TimeSpan::MillisecondsToTicks(long s)
  {
	  return MillisecondsToTicks(__int64(s));
  }

  __int64 TimeSpan::SecondsToTicks(long s)
  {
	  return MillisecondsToTicks(s * 1000);
  }

  __int64 TimeSpan::SecondsToTicks(__int64 s)
  {
	  return MillisecondsToTicks(s * 1000);
  }

  __int64 TimeSpan::MinutesToTicks(long s)
  {
	  return SecondsToTicks(s * 60);
  }

  __int64 TimeSpan::MinutesToTicks(__int64 s)
  {
	  return SecondsToTicks(s * 60);
  }

  __int64 TimeSpan::HoursToTicks(long s)
  {
	  return MinutesToTicks(s * 60);
  }

  __int64 TimeSpan::HoursToTicks(__int64 s)
  {
	  return MinutesToTicks(s * 60);
  }

  __int64 TimeSpan::DaysToTicks(long s)
  {
	  return HoursToTicks(s * 24);
  }

  __int64 TimeSpan::DaysToTicks(__int64 s)
  {
	  return HoursToTicks(s * 24);
  }

  void TimeSpan::ToFILETIME(FILETIME& ft) const
  {
	  ft.dwHighDateTime = static_cast<DWORD>((m_ticks & 0xFFFFFFFF00000000) >> 32);
	  ft.dwLowDateTime = static_cast<DWORD>(m_ticks & 0xFFFFFFFF);
  }

  void TimeSpan::FromFILETIME(const FILETIME& ft)
  {
	  m_ticks = (static_cast<__int64>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
  }


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  DateTime::DateTime() :
	  m_ticks(0),
	  m_kind(Unspecified)
  {
  }

  DateTime::DateTime(__int64 ticks, DateTimeKind kind) :
	  m_ticks(ticks),
	  m_kind(kind)
  {
  }

  DateTime::DateTime(const DateTime& r) :
	  m_ticks(r.m_ticks),
	  m_kind(r.m_kind)
  {
  }

  DateTime::DateTime(long year, Month month, long day, DateTimeKind kind) :
	  m_kind(kind)
  {
	  SYSTEMTIME st = {0};
	  st.wYear = static_cast<WORD>(year);
	  st.wMonth = static_cast<WORD>(month);
	  st.wDay = static_cast<WORD>(day);
	  FromSystemTime(st);
  }

  DateTime::DateTime(long year, Month month, long day, long hour, long minute, long second, DateTimeKind kind) :
	  m_kind(kind)
  {
	  SYSTEMTIME st = {0};
	  st.wYear = static_cast<WORD>(year);
	  st.wMonth = static_cast<WORD>(month);
	  st.wDay = static_cast<WORD>(day);
	  st.wHour = static_cast<WORD>(hour);
	  st.wMinute = static_cast<WORD>(minute);
	  st.wSecond = static_cast<WORD>(second);
	  FromSystemTime(st);
  }

  DateTime::DateTime(long year, Month month, long day, long hour, long minute, long second, long millisecond, DateTimeKind kind) :
	  m_kind(kind)
  {
	  SYSTEMTIME st = {0};
	  st.wYear = static_cast<WORD>(year);
	  st.wMonth = static_cast<WORD>(month);
	  st.wDay = static_cast<WORD>(day);
	  st.wHour = static_cast<WORD>(hour);
	  st.wMinute = static_cast<WORD>(minute);
	  st.wSecond = static_cast<WORD>(second);
	  st.wMilliseconds = static_cast<WORD>(millisecond);
	  FromSystemTime(st);
  }

  void DateTime::Add(const TimeSpan& r)
  {
	  m_ticks += r.GetTicks();
  }

  void DateTime::Subtract(const TimeSpan& r)
  {
	  m_ticks -= r.GetTicks();
  }

  TimeSpan DateTime::Subtract(const DateTime& r)
  {
	  return TimeSpan(m_ticks - r.m_ticks);
  }

  void DateTime::AddDays(long d)
  {
		m_ticks += TimeSpan::DaysToTicks(d);
  }

  void DateTime::AddHours(long d)
  {
		m_ticks += TimeSpan::HoursToTicks(d);
  }

  void DateTime::AddMilliseconds(long d)
  {
		m_ticks += TimeSpan::MillisecondsToTicks(d);
  }

  void DateTime::AddMinutes(long d)
  {
		m_ticks += TimeSpan::MinutesToTicks(d);
  }

  //void DateTime::AddMonths(Month d)
  //{
	 // SYSTEMTIME st;
	 // ToSystemTime(st);
	 // st.wMonth ++;
		//// TODO: correct overflow / underflow of wMonth
	 // FromSystemTime(st);
  //}

  void DateTime::AddSeconds(long d)
  {
		m_ticks += TimeSpan::SecondsToTicks(d);
  }

  void DateTime::AddTicks(__int64 ticks)
  {
	  m_ticks += ticks;
  }

	// todo: what happens if you have Feb 29 on a leap year, and add a year? is it march 1st?
  void DateTime::AddYears(long d)
  {
	  SYSTEMTIME st;
	  ToSystemTime(st);
	  st.wYear ++;
	  FromSystemTime(st);
  }

  long DateTime::Compare(const DateTime& s)
  {
	  if(m_ticks < s.m_ticks)
	  {
		  return -1;
	  }
	  if(m_ticks > s.m_ticks)
	  {
		  return 1;
	  }
	  return 0;
  }

  void DateTime::FromFileTime(const FILETIME& ft)
  {
	  m_ticks = (static_cast<__int64>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
  }

  void DateTime::ToFileTime(FILETIME& ft) const
  {
	  ft.dwHighDateTime = static_cast<DWORD>((m_ticks & 0xFFFFFFFF00000000) >> 32);
	  ft.dwLowDateTime = static_cast<DWORD>(m_ticks & 0xFFFFFFFF);
  }

	FILETIME DateTime::ToFileTime() const
	{
		FILETIME ret;
		ToFileTime(ret);
		return ret;
	}


  void DateTime::FromSystemTime(const SYSTEMTIME& st)
  {
	  FILETIME ft;
	  SystemTimeToFileTime(&st, &ft);
	  FromFileTime(ft);
  }

  void DateTime::ToSystemTime(SYSTEMTIME& st) const
  {
	  FILETIME ft;
	  ToFileTime(ft);
	  FileTimeToSystemTime(&ft, &st);
  }

  bool DateTime::Equals(const DateTime& s)
  {
	  return m_ticks == s.m_ticks;
  }

  //bool DateTime::IsDaylightSavingTime()
  //{
	 // return false;
  //}

  __int64 DateTime::ToBinary()
  {
	  return m_ticks;
  }

  //double DateTime::ToOADate()
  //{
	 // return 0;
  //}

  //DateTime DateTime::ToLocalTime()
  //{
	 // return DateTime();
  //}

  //std::string DateTime::ToString()
  //{
	 // return "";
  //}

  void DateTime::FromUnixTime(time_t t)
  {
    FILETIME ft;
    LONGLONG ll;
    ll = Int32x32To64(t, 10000000) + 116444736000000000;
    ft.dwLowDateTime = (DWORD)ll;
    ft.dwHighDateTime = (DWORD)(ll >> 32);
  }

  /*
	  1  jan  31
	  2  feb  leap
	  3  mar  31
	  4  apr  30
	  5  may  31
	  6  jun  30
	  7  jul  31
	  8  aug  31
	  9  sep  30
	  10 oct  31
	  11 nov  30
	  12 dec  31
  */
  long DateTime::DaysInMonth(long year, Month m)
  {
	  switch(m)
	  {
	  case January:
	  case March:
	  case May:
	  case July:
	  case August:
	  case October:
	  case December:
		  return 31;
	  case April:
	  case June:
	  case September:
	  case November:
		  return 30;
	  case February:
		  return IsLeapYear(year) ? 29 : 28;
	  }

	  return 0;
  }

  bool DateTime::IsLeapYear(long y)
  {
	  return ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0);
  }
}