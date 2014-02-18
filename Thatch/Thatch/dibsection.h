

#pragma once


#include <windows.h>
#include "fundamental.h"

// compare 2 floating point numbers to see if they are in a certain range.
template<typename Tl, typename Tr, typename Terr>
inline bool xequals(const Tl& l, const Tr& r, const Terr& e)
{
  Tl d = l - r;
  if(d < 0) d = -d;
  if(d > e) return false;
  return true;
}

typedef unsigned __int32 RgbPixel32;

inline BYTE R(RgbPixel32 d)
{
  return static_cast<BYTE>((d & 0x00FF0000) >> 16);
}

inline BYTE G(RgbPixel32 d)
{
  return static_cast<BYTE>((d & 0x0000FF00) >> 8);
}

inline BYTE B(RgbPixel32 d)
{
  return static_cast<BYTE>((d & 0x000000FF));
}

inline RgbPixel32 MakeRgbPixel32(BYTE r, BYTE g, BYTE b)
{
  return static_cast<RgbPixel32>((r << 16) | (g << 8) | (b));
}

template<typename T>
inline RgbPixel32 MakeRgbPixel32(const T& r, const T& g, const T& b)
{
  return MakeRgbPixel32(static_cast<BYTE>(r), static_cast<BYTE>(g), static_cast<BYTE>(b));
}

inline COLORREF RgbPixel32ToCOLORREF(RgbPixel32 x)
{
  return RGB(R(x), G(x), B(x));
}

inline RgbPixel32 COLORREFToRgbPixel32(COLORREF x)
{
  return MakeRgbPixel32(GetRValue(x), GetGValue(x), GetBValue(x));
}

inline RgbPixel32 MixColorsInt(long fa, long fmax, RgbPixel32 ca, RgbPixel32 cb)
{
  BYTE r, g, b;
  long fmaxminusfa = fmax - fa;
  r = static_cast<BYTE>(((fa * R(ca)) + (fmaxminusfa * R(cb))) / fmax);
  g = static_cast<BYTE>(((fa * G(ca)) + (fmaxminusfa * G(cb))) / fmax);
  b = static_cast<BYTE>(((fa * B(ca)) + (fmaxminusfa * B(cb))) / fmax);
  return MakeRgbPixel32(r,g,b);
}



class DibSection32
{
  private:
    // don't allow ambiguous calls
		template<typename T> DibSection32(T) { }
		template<typename T> DibSection32& operator =(T) { return *this; }
public:
  static const long bpp = 32;

  DibSection32() :
    m_y(0),
    m_x(0),
    m_bmp(0),
    m_pbuf(0)
  {
    // store our offscreen hdc
    HDC hscreen = ::GetDC(0);
    m_offscreen = CreateCompatibleDC(hscreen);
    ReleaseDC(0, hscreen);
  }

  ~DibSection32()
  {
    if(m_bmp)
    {
      SelectObject(m_offscreen, m_oldBitmap);
      DeleteObject(m_bmp);
    }
    DeleteDC(m_offscreen);
  }

  long GetWidth() const { return m_x; }
  long GetHeight() const { return m_y; }

  // MUST be called at least once.  This will allocate the bmp object.
  bool SetSize(long x, long y)
  {
    bool r = false;

    if(x & 1) x ++;
    if(y & 1) y ++;

    if((x != m_x) || (y != m_y))
    {
      // delete our current bmp.
      if(m_bmp)
      {
        SelectObject(m_offscreen, m_oldBitmap);
        DeleteObject(m_bmp);
        m_bmp = 0;
      }

      long pbiSize = sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 10;
      BITMAPINFO* pbi = reinterpret_cast<BITMAPINFO*>(HeapAlloc(GetProcessHeap(), 0, pbiSize));
      memset(pbi, 0, pbiSize);
      BITMAPINFO& bi = *pbi;

      bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
      bi.bmiHeader.biPlanes = 1;// must be 1
      bi.bmiHeader.biBitCount = 32;// for RGBQUAD
      bi.bmiHeader.biCompression = BI_RGB;
      bi.bmiHeader.biSizeImage = 0;// dont need to specify because its uncompressed
      bi.bmiHeader.biXPelsPerMeter = 0;
      bi.bmiHeader.biYPelsPerMeter = 0;
      bi.bmiHeader.biClrUsed  = 0;
      bi.bmiHeader.biClrImportant = 0;
      bi.bmiHeader.biWidth = max(x,1);
      bi.bmiHeader.biHeight = -(max(y,1));

      m_bmp = CreateDIBSection(m_offscreen, &bi, DIB_RGB_COLORS, (void**)&m_pbuf, 0, 0);
      bi.bmiHeader.biHeight = (max(y,1));
      if(m_bmp)
      {
        r = true;
        m_x = bi.bmiHeader.biWidth;
        m_y = bi.bmiHeader.biHeight;
        m_oldBitmap = (HBITMAP)SelectObject(m_offscreen, m_bmp);
      }

      HeapFree(GetProcessHeap(), 0, pbi);
    }
    return r;
  }

  RgbPixel32* GetBuffer()
  {
    return m_pbuf;
  }

  HBITMAP DetachHandle()
  {
    HBITMAP ret = m_bmp;
    if(m_bmp)
    {
      SelectObject(m_offscreen, m_oldBitmap);
      m_bmp = 0;
      DeleteDC(m_offscreen);
    }
    return ret;
  }
	HBITMAP GetHBITMAP()
	{
		return m_bmp;
	}

	bool IsInBounds(long x, long y)
	{
		if(x < 0 || y < 0) return false;
		if(x >= m_x) return false;
		if(y >= m_y) return false;
		return true;
	}

  void SetPixel(long x, long y, RgbPixel32 c)
  {
    m_pbuf[x + (y * m_x)] = c;
  }

  bool SetPixelSafe(long x, long y, RgbPixel32 c)
  {
    if(!IsInBounds(x, y)) return false;
		SetPixel(x, y, c);
		return true;
  }

  RgbPixel32 GetPixel(long x, long y)
  {
    return m_pbuf[x + (y * m_x)];
  }

  bool GetPixelSafe(RgbPixel32& out, long x, long y)
  {
    if(!IsInBounds(x, y)) return false;
		out = GetPixel(x, y);
		return true;
  }

  // xright is NOT drawn.
  void HLine(long x1, long x2, long y, RgbPixel32 c)
  {
    long xleft = min(x1, x2);
    long xright = max(x1, x2);
    RgbPixel32* pbuf = &m_pbuf[(y * m_x) + xleft];
    while(xleft != xright)
    {
      *pbuf = c;
      pbuf ++;
      xleft ++;
    }
  }

  void VLine(long x, long y1, long y2, RgbPixel32 c)
  {
    long ytop = min(y1, y2);
    long ybottom = max(y1, y2);
    RgbPixel32* pbuf = &m_pbuf[(ytop * m_x) + x];
    while(ytop != ybottom)
    {
      *pbuf = c;
      pbuf += m_x;
      ytop ++;
    }
  }

  // b and r are not drawn
  void Rect(long l, long t, long r, long b, RgbPixel32 c)
  {
    RgbPixel32* pbuf = &m_pbuf[(t * m_x) + l];
    long h = r - l;// horizontal size
    // fill downwards
    while(t != b)
    {
      // draw a horizontal line
      for(long i = 0; i < h; i ++)//  <-- i think this is a bug. h should be w.
      {
        pbuf[i] = c;
      }
      pbuf += m_x;
      t ++;
    }
  }

  // b and r are not drawn
  void SafeRect(long l, long t, long r, long b, RgbPixel32 c)
  {
    clamp(l, 0, m_x);
    clamp(r, 0, m_x);
    clamp(t, 0, m_y);
    clamp(b, 0, m_y);
    Rect(l, t, r, b, c);
  }

  void Fill(RgbPixel32 c)
  {
    RgbPixel32* pDest = m_pbuf;
    long n = m_x * m_y;
    for(long i = 0; i < n; i ++)
    {
      pDest[i] = c;
    }
  }

	void Blit(HDC dest, int xdest, int ydest, int xsrc, int ysrc, int cx, int cy)
	{
		BitBlt(dest, xdest, ydest, cx, cy, m_offscreen, xsrc, ysrc, SRCCOPY);
	}

	void Blit(HDC dest, int xdest = 0, int ydest = 0, int xsrc = 0, int ysrc = 0)
	{
		BitBlt(dest, xdest, ydest, m_x, m_y, m_offscreen, xsrc, ysrc, SRCCOPY);
	}

private:
  long m_x;
  long m_y;
  HDC m_offscreen;
  HBITMAP m_bmp;
  HBITMAP m_oldBitmap;
  RgbPixel32* m_pbuf;

  template<typename T, typename Tmin, typename Tmax>
  inline bool clamp(T& l, const Tmin& minval, const Tmax& maxval)
  {
    if(l < static_cast<T>(minval))
    {
      l = static_cast<T>(minval);
      return true;
    }
    if(l > static_cast<T>(maxval))
    {
      l = static_cast<T>(maxval);
      return true;
    }
    return false;
  }
};

