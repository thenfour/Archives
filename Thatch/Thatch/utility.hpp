//
// utility.hpp - miscellaneous utility functions
// Copyright (c) 2005 Roger Clark
// Copyright (c) 2003 Carl Corcoran
//

#ifndef SCREENIE_UTILITY_HPP
#define SCREENIE_UTILITY_HPP

// for std::vector
#include <vector>

// for tstd::tstring
#include "..\..\libcc\libcc/stringutil.hpp"

inline void DumpBitmap(Gdiplus::Bitmap& image, int x, int y)
{
  // draw that damn bitmap to the screen.
  HDC dc = ::GetDC(0);
  HDC dcc = CreateCompatibleDC(dc);
  HBITMAP himg;
  image.GetHBITMAP(0, &himg);
  HBITMAP hOld = (HBITMAP)SelectObject(dcc, himg);
  StretchBlt(dc, x, y, image.GetWidth(), image.GetHeight(), dcc, 0, 0, image.GetWidth(), image.GetHeight(), SRCCOPY);
  SelectObject(dcc, hOld);
  DeleteDC(dcc);
  DeleteObject(himg);
  ::ReleaseDC(0,dc);
}


inline std::wstring GetPathRelativeToApp(PCTSTR extra)
{
  wchar_t sz[MAX_PATH];
  wchar_t sz2[MAX_PATH];
  GetModuleFileName(0, sz, MAX_PATH);
  wcscpy(PathFindFileName(sz), extra);
  PathCanonicalize(sz2, sz);
  return sz2;
}

inline std::wstring GetModuleFileNameX()
{
  TCHAR sz[MAX_PATH];
  GetModuleFileName(0, sz, MAX_PATH);
  return sz;
}


inline std::wstring GetNumberFormatX(std::wstring input)
{
	LibCC::Blob<wchar_t> retBlob;
	GetNumberFormat(LOCALE_USER_DEFAULT, 0, input.c_str(), 0, retBlob.GetBuffer(100), 99);
	return retBlob.GetBuffer();
}

inline std::wstring FormatSize(DWORD dwFileSize)
{
  static const DWORD dwKB = 1024;
  static const DWORD dwMB = 1024 * dwKB;
  static const DWORD dwGB = 1024 * dwMB;

  if (dwFileSize < dwKB)
  {
		return LibCC::Format("% b")(GetNumberFormatX(LibCC::Format().f<2,2>((float)dwFileSize).Str())).Str();
  }
  if (dwFileSize < dwMB)
  {
		return LibCC::Format("% kb")(GetNumberFormatX(LibCC::Format().f<2,2>(((float)dwFileSize) / dwKB).Str())).Str();
  }
  if (dwFileSize < dwGB)
  {
		return LibCC::Format("% mb")(GetNumberFormatX(LibCC::Format().f<2,2>(((float)dwFileSize) / dwMB).Str())).Str();
  }
	return LibCC::Format("% gb")(GetNumberFormatX(LibCC::Format().f<2,2>(((float)dwFileSize) / dwGB).Str())).Str();
}

class Guid
{
public:
  GUID val;

  Guid()
  {
    memset(&val, 0, sizeof(val));
  }
  Guid(const Guid& x) { Assign(x); }
  Guid(const GUID& x) { Assign(x); }

  Guid& operator =(const Guid& x) { return Assign(x); }
  Guid& operator =(const GUID& x) { Assign(x); }
  Guid& operator =(const std::wstring& x) { Assign(x); }

  bool operator ==(const Guid& x) const  { return Equals(x); }
  bool operator !=(const Guid& x) const  { return !Equals(x); }
  Guid& Assign(const std::wstring& W)
  {
    LibCC::Blob<OLECHAR> crap;
    crap.Alloc(W.size()+1);
    wcscpy(crap.GetBuffer(), W.c_str());
    CLSIDFromString(crap.GetBuffer(), &val);
    return *this;
  }
  Guid& Assign(const Guid& x)
  {
    memcpy(&val, &x.val, sizeof(GUID));
    return *this;
  }
  Guid& Assign(const GUID& x)
  {
    memcpy(&val, &x, sizeof(GUID));
    return *this;
  }
  bool Equals(const Guid& x) const
  {
    return (memcmp(&x.val, &val, sizeof(GUID)) == 0);
  }
  void CreateNew()
  {
    CoCreateGuid(&val);
  }
  std::wstring ToString() const
  {
    LPOLESTR p;
    StringFromCLSID(val, &p);
		std::wstring ret = p;
    CoTaskMemFree(p);
    return ret;
  }
};

// RAII critsec class
class CriticalSection
{
public:
  CriticalSection() { InitializeCriticalSection(&m_cs); }
  ~CriticalSection() { DeleteCriticalSection(&m_cs); }
  bool Enter() { EnterCriticalSection(&m_cs); return true; }
  bool Leave() { LeaveCriticalSection(&m_cs); return true; }

  class ScopeLock
  {
  public:
    ScopeLock(CriticalSection& x) : m_cs(x) { m_cs.Enter(); }
    ~ScopeLock() { m_cs.Leave(); }
  private:
    CriticalSection& m_cs;
  };
private:
  CRITICAL_SECTION m_cs;
};

struct Win32Handle
{
	Win32Handle(HANDLE h) : val(h) { }
	Win32Handle(Win32Handle& copy) { operator=(copy); }

	~Win32Handle()
	{
		if(val != NULL)
      CloseHandle(val);
	}

	Win32Handle& operator=(Win32Handle& rhs)
	{
    val = rhs.val;
    rhs.val = NULL;
		return (*this);
	}

	Win32Handle& operator=(HANDLE rhs)
	{
		val = rhs;
		return (*this);
	}

	HANDLE val;
};

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


class MemStream
{
public:
	IStream* m_pStream;

	MemStream(BYTE* data, size_t size)
	{
		CreateStreamOnHGlobal(0, TRUE, &m_pStream);
		LARGE_INTEGER li;
		ULARGE_INTEGER uli;
		li.QuadPart = 0;
		m_pStream->Seek(li, STREAM_SEEK_SET, &uli);
		m_pStream->Write(data, (ULONG)size, 0);
		m_pStream->Seek(li, STREAM_SEEK_SET, &uli);
	}

	MemStream(HINSTANCE hInstance, LPCTSTR res, PCTSTR type)
	{
		CreateStreamOnHGlobal(0, TRUE, &m_pStream);
		LARGE_INTEGER li;
		ULARGE_INTEGER uli;
		li.QuadPart = 0;
		m_pStream->Seek(li, STREAM_SEEK_SET, &uli);

		HRSRC hrsrc=FindResource(hInstance, res, type);
		HGLOBAL hg1 = LoadResource(hInstance, hrsrc);
		DWORD sz = SizeofResource(hInstance, hrsrc);
		void* ptr1 = LockResource(hg1);

		m_pStream->Write(ptr1, sz, 0);
		m_pStream->Seek(li, STREAM_SEEK_SET, &uli);
	}
	~MemStream()
	{
		m_pStream->Release();
	}
};


#endif



