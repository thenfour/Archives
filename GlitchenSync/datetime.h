

#pragma once

// ticks 
// A date and time expressed in 100-nanosecond units. 


// for COM types
#include <comutil.h>

#include <string>

namespace LibCC
{
  enum DateTimeKind
  {
	  Unspecified,
	  UTC,
	  Local
  };

  enum Month
  {
	  January = 1,
	  February = 2,
	  March = 3,
	  April = 4,
	  May = 5,
	  June = 6,
	  July = 7,
	  August = 8,
	  September = 9,
	  October = 10,
	  November = 11,
	  December = 12
  };

  enum Weekday
  {
	  Sunday = 1,
	  Monday = 2,
	  Tuesday = 3,
	  Wednesday = 4,
	  Thursday = 5,
	  Friday = 6,
	  Saturday = 7
  };


  class TimeSpan
  {
  public:
	  TimeSpan();
	  TimeSpan(__int64 ticks);
	  TimeSpan(long hours, long minutes, long seconds);
	  TimeSpan(long days, long hours, long minutes, long seconds);
	  TimeSpan(long days, long hours, long minutes, long seconds, long milliseconds);
	  TimeSpan(const TimeSpan& r);

	  void Add(const TimeSpan& r);
	  void Negate();
		void Abs();
	  void Subtract(const TimeSpan& r);
	  void Multiply(double f);
	  void Multiply(long x);
	  void Divide(double f);
	  void Divide(long x);

	  long Compare(const TimeSpan& r) const;
	  bool Equals(const TimeSpan& r) const;
	  bool LessThan(const TimeSpan& r) const;
	  bool GreaterThan(const TimeSpan& r) const;
	  bool LessThanOrEquals(const TimeSpan& r) const;
	  bool GreaterThanOrEquals(const TimeSpan& r) const;

	  __int64 GetTicks() const;
	  void ToFILETIME(FILETIME& ft) const;
	  void FromFILETIME(const FILETIME& ft);

	  static long TicksToSeconds(__int64 t);
	  static __int64 MillisecondsToTicks(long s);
	  static __int64 MillisecondsToTicks(__int64 s);
	  static __int64 SecondsToTicks(long s);
	  static __int64 SecondsToTicks(__int64 s);
	  static __int64 MinutesToTicks(long s);
	  static __int64 MinutesToTicks(__int64 s);
	  static __int64 HoursToTicks(long s);
	  static __int64 HoursToTicks(__int64 s);
	  static __int64 DaysToTicks(long s);
	  static __int64 DaysToTicks(__int64 s);
  private:
	  __int64 m_ticks;
  };


  class DateTime
  {
  public:
	  DateTime();
	  DateTime(__int64 ticks, DateTimeKind kind = Unspecified);
	  DateTime(const DateTime& r);
	  DateTime(long year, Month month, long day, DateTimeKind kind = Unspecified);
	  DateTime(long year, Month month, long day, long hour, long minute, long second, DateTimeKind kind = Unspecified);
	  DateTime(long year, Month month, long day, long hour, long minute, long second, long millisecond, DateTimeKind kind = Unspecified);

	  void Assign(__int64 ticks, DateTimeKind kind);

	  void Add(const TimeSpan& r);
	  void Subtract(const TimeSpan& r);
	  TimeSpan Subtract(const DateTime& r);
	  void AddDays(long d);
	  void AddHours(long d);
	  void AddMilliseconds(long d);
	  void AddMinutes(long d);
	  //void AddMonths(Month d);
	  void AddSeconds(long v);
	  void AddTicks(__int64 ticks);
	  void AddYears(long d);

	  long Compare(const DateTime& s);
	  bool Equals(const DateTime& s);
	  //bool IsDaylightSavingTime();
	  __int64 ToBinary();
	  //double ToOADate();
	  //DateTime ToLocalTime();// adjust for daylight savings and time zone
	  //std::string ToString();

    void FromUnixTime(time_t t);
    void ToUnixTime(time_t&) const;
    time_t ToUnixTime() const;

    void FromVariant(VARIANT& v);
    void ToVariant(_variant_t&) const;
    _variant_t ToVariant() const;

	  void FromFileTime(const FILETIME& ft);
	  void ToFileTime(FILETIME& ft) const;
	  FILETIME ToFileTime() const;

	  void FromSystemTime(const SYSTEMTIME& ft);
	  void ToSystemTime(SYSTEMTIME& ft) const;
	  SYSTEMTIME ToSystemTime() const;

	  static long DaysInMonth(long year, Month m);
	  static bool IsLeapYear(long y);

    static DateTime LocalTime();

  private:
	  __int64 m_ticks;
	  DateTimeKind m_kind;
  };
}

