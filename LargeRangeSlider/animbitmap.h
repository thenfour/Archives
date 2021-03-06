/*
  (c) 2004-2005 Carl Corcoran

  Jun 30, 2004 carlc
  This is a class for "animated bitmaps".  Basically the idea is that you get easy low level
  access do a DIB and its meant to be drawn in frames.
  This is only meant for SCREEN purposes, and meant to be FAST!

  Jun 05, 2005 carlc - added more rgbpixel helper crap
*/


#pragma once


#include <windows.h>


// compare 2 floating point numbers to see if they are in a certain range.
template<typename Tl, typename Tr, typename Terr>
inline bool xequals(const Tl& l, const Tr& r, const Terr& e)
{
  Tl d = l - r;
  if(d < 0) d = -d;
  if(d > e) return false;
  return true;
}

template<long bpp>
struct BitmapTypes
{
  typedef unsigned __int8 RgbPixel;
};

template<>
struct BitmapTypes<16>
{
  typedef unsigned __int16 RgbPixel;
};

template<>
struct BitmapTypes<32>
{
  typedef unsigned __int32 RgbPixel;
};

typedef BitmapTypes<32>::RgbPixel RgbPixel32;


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

inline RgbPixel32 MakeRgbPixel32(COLORREF c)
{
  return COLORREFToRgbPixel32(c);
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

inline RgbPixel32 MixColors(float fa, RgbPixel32 ca, RgbPixel32 cb)
{
  BYTE r, g, b;
  float inv = 1.0f - fa;
  r = static_cast<BYTE>((fa * R(ca)) + (inv * R(cb)));
  g = static_cast<BYTE>((fa * G(ca)) + (inv * G(cb)));
  b = static_cast<BYTE>((fa * B(ca)) + (inv * B(cb)));
  return MakeRgbPixel32(r,g,b);
}



template<long Tbpp>
class AnimBitmap
{
public:
  static const long bpp = Tbpp;
  typedef typename BitmapTypes<bpp>::RgbPixel RgbPixel;

  AnimBitmap() :
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

  ~AnimBitmap()
  {
    FreeBitmap();
    DeleteDC(m_offscreen);
  }

  //CSize GetSize() const
  //{
  //  return CSize(m_x, m_y);
  //}

  long GetWidth() const
  {
    return m_x;
  }

  long GetHeight() const
  {
    return m_y;
  }

  // because we are locked into a particular format (bpp),
  // we have to load the thing & copy it.
  bool LoadFromResource(HINSTANCE instance, LPCTSTR name)
  {
    return Attach(LoadBitmap(instance, name));
  }

  bool Attach(HBITMAP src) {
    BITMAP bmp;
    GetObject(src, sizeof(bmp), &bmp);
    if(SetSize(bmp.bmWidth, bmp.bmHeight))
    {
      HDC dcScreen = ::GetDC(0);
      HDC dcMem = CreateCompatibleDC(dcScreen);
      HBITMAP hOld = (HBITMAP)SelectObject(dcMem, src);

      // copy it
      BitBlt(m_offscreen, 0, 0, m_x, m_y, dcMem, 0, 0, SRCCOPY);

      SelectObject(dcMem, hOld);
      DeleteDC(dcMem);
      ReleaseDC(0, dcScreen);
    }
    DeleteObject(src);
    return true;
  }

  // MUST be called at least once.  This will allocate the bmp object.
  bool SetSize(long x, long y)
  {
    bool r = false;

    if((x != m_x) || (y != m_y))
    {
      FreeBitmap();

      long pbiSize = sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 10;
      BITMAPINFO* pbi = reinterpret_cast<BITMAPINFO*>(HeapAlloc(GetProcessHeap(), 0, pbiSize));
      memset(pbi, 0, pbiSize);
      BITMAPINFO& bi = *pbi;

      bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
      bi.bmiHeader.biPlanes = 1;// must be 1

      if(bpp == 32)
      {
        bi.bmiHeader.biBitCount = 32;// for RGBQUAD
        bi.bmiHeader.biCompression = BI_RGB;
      }
      else if(bpp == 16)
      {
        bi.bmiHeader.biBitCount = 16;
        bi.bmiHeader.biCompression = BI_BITFIELDS;
        *(unsigned int*)(&bi.bmiColors[0]) = 0xF800;
        *(unsigned int*)(&bi.bmiColors[1]) = 0x07E0;
        *(unsigned int*)(&bi.bmiColors[2]) = 0x001F;
      }

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

  bool SetSizeIfNecessary(long x, long y)
  {
    if((m_x < x) || (m_y < y))
    {
      return SetSize(x,y);
    }
    return true;
  }

  // commits the changes
  bool Commit()
  {
    return true;
  }

  bool BeginDraw()
  {
    GdiFlush();
    return true;
  }

  HDC GetDC()
  {
    return m_offscreen;
  }

  // no boundschecking for speed.
  void SetPixel(long x, long y, RgbPixel c)
  {
    ATLASSERT(x >= 0);
    ATLASSERT(y >= 0);
    ATLASSERT(y < m_y);
    ATLASSERT(x < m_x);
    //y = m_y - y;
    m_pbuf[x + (y * m_x)] = c;
  }

  // xright is NOT drawn.
  void SafeHLine(long x1, long x2, long y, RgbPixel c)
  {
    long xleft = min(x1, x2);
    long xright = max(x1, x2);
    if(xleft > 0)
    {
      if(xright <= m_x)
      {
        if(y > 0)
        {
          if(y < m_y)
          {
            HLine(xleft, xright, y, c);
          }
        }
      }
    }
  }

  // xright is NOT drawn.
  void HLine(long x1, long x2, long y, RgbPixel c)
  {
    long xleft = min(x1, x2);
    long xright = max(x1, x2);
    RgbPixel* pbuf = &m_pbuf[(y * m_x) + xleft];
    while(xleft != xright)
    {
      *pbuf = c;
      pbuf ++;
      xleft ++;
    }
  }

  void VLine(long x, long y1, long y2, RgbPixel c)
  {
    long ytop = min(y1, y2);
    long ybottom = max(y1, y2);
    RgbPixel* pbuf = &m_pbuf[(ytop * m_x) + x];
    while(ytop != ybottom)
    {
      *pbuf = c;
      pbuf += m_x;
      ytop ++;
    }
  }

  // b and r are not drawn
  void Rect(long l, long t, long r, long b, RgbPixel c)
  {
    RgbPixel* pbuf = &m_pbuf[(t * m_x) + l];
    long h = r - l;// horizontal size
    // fill downwards
    while(t != b)
    {
      // draw a horizontal line
      for(long i = 0; i < h; i ++)
      {
        pbuf[i] = c;
      }
      pbuf += m_x;
      t ++;
    }
  }

  // b and r are not drawn
  void SafeRect(long l, long t, long r, long b, RgbPixel c)
  {
    clamp(l, 0, m_x);
    clamp(r, 0, m_x);
    clamp(t, 0, m_y);
    clamp(b, 0, m_y);
    Rect(l, t, r, b, c);
  }

  bool SetPixelSafe(long x, long y, RgbPixel c)
  {
    bool r = false;
    if(x > 0 && y > 0 && x < m_x && y < m_y)
    {
      m_pbuf[x + (y * m_x)] = c;
      r = true;
    }
    return r;
  }

  // mixes with the background.
  bool MixPixelSafe(long x, long y, RgbPixel c, float amount)
  {
    bool r = false;
    if(x > 0 && y > 0 && x < m_x && y < m_y)
    {
      RgbPixel* p = &m_pbuf[x + (y * m_x)];
      *p = MixColors(amount, c, *p);
      r = true;
    }
    return r;
  }

  RgbPixel GetPixel(long x, long y)
  {
    return m_pbuf[x + (y * m_x)];
  }

  bool GetPixelSafe(RgbPixel& out, long x, long y)
  {
    bool r = false;
    if(x > 0 && y > 0 && x < m_x && y < m_y)
    {
      out = m_pbuf[x + (y * m_x)];
      r = true;
    }
    return r;
  }

  void Fill(RgbPixel c)
  {
    RgbPixel* pDest = m_pbuf;
    long n = m_x * m_y;
    for(long i = 0; i < n; i ++)
    {
      pDest[i] = c;
    }
  }

  inline static BYTE InvertColorantForSelection(const BYTE& x, int offset)
  {
    int ret = /*(255 - x) +*/x + offset;
    if(ret < 0) ret = 256 - ret;
    if(ret > 255) ret -= 256;
    return static_cast<BYTE>(ret);
  }

  inline static RgbPixel32 InvertColorForSelection(const RgbPixel32& x, int offset)
  {
    return MakeRgbPixel32(
      InvertColorantForSelection(R(x), offset),
      InvertColorantForSelection(G(x), offset),
      InvertColorantForSelection(B(x), offset)
      );
  }

  // "skip" lets us make this for either vertical or horizontal lines.  skip=1 = horizontal.  skip=m_x = vertical.
  template<int patternFreq, int colorOffset>// templated for efficiency.
  void DrawSelectionWithOffset(int patternOffset, RgbPixel32* startPixel, RgbPixel32* endPixel, int skip)
  {
    const int tickSize = patternFreq / 2;
    RgbPixel32* start = startPixel + (patternOffset*skip);
    RgbPixel32* p = start;
    int totalPixels = (endPixel - startPixel) / skip;
    int count = 0;
    int i;
    for(;;)
    {
      for(i = 0; i < tickSize; i ++, count ++, p += skip)
      {
        if(count >= totalPixels) return;
        if(p >= endPixel) p = startPixel;
        *p = InvertColorForSelection(*p, colorOffset);
      }
      for(i = 0; i < tickSize; i ++, count ++, p += skip)
      {
        if(count >= totalPixels) return;
        if(p >= endPixel) p = startPixel;
        *p = InvertColorForSelection(*p, -colorOffset);
      }
    }
  }

  template<int patternFreq, int colorOffset>// templated for efficiency.
  void DrawSelectionHLineSafe(int patternOffset, int y, int xleft, int xright)
  {
    if(y < 0) return;
    if(y >= m_y) return;
    if(xleft < 0) xleft = 0;
    if(xright >= m_x) xright = m_x - 1;
    RgbPixel32* leftMostPixel = &m_pbuf[(y * m_x) + xleft];
    RgbPixel32* rightMostPixel = leftMostPixel + xright - xleft;
    DrawSelectionWithOffset<patternFreq, colorOffset>(patternOffset, leftMostPixel, rightMostPixel, 1);
  }

  template<int patternFreq, int colorOffset>// templated for efficiency.
  void DrawSelectionVLineSafe(int patternOffset, int x, int ytop, int ybottom)
  {
    if(x < 0) return;
    if(x >= m_x) return;
    if(ytop < 0) ytop = 0;
    if(ybottom >= m_y) ybottom = m_y - 1;
    RgbPixel32* topMostPixel = &m_pbuf[(ytop * m_x) + x];
    RgbPixel32* bottomMostPixel = topMostPixel + ((ybottom - ytop) * m_x);
    DrawSelectionWithOffset<patternFreq, colorOffset>(patternOffset, topMostPixel, bottomMostPixel, m_x);
  }

  template<int patternFreq, int colorOffset>
  void DrawSelectionRectSafe(int patternOffset, RECT& rc)
  {
    // the formula for color is inverted, plus or minus 20 (with wrap) on each colorant.
    DrawSelectionHLineSafe<patternFreq, colorOffset>(patternOffset, rc.top, rc.left, rc.right);
    DrawSelectionHLineSafe<patternFreq, colorOffset>(patternFreq - patternOffset, rc.bottom, rc.left, rc.right);
    DrawSelectionVLineSafe<patternFreq, colorOffset>(patternFreq - patternOffset, rc.left, rc.top, rc.bottom);
    DrawSelectionVLineSafe<patternFreq, colorOffset>(patternOffset, rc.right, rc.top, rc.bottom);
  }

  bool StretchBlit(AnimBitmap& dest, long destx, long desty, long destw, long desth, long srcx, long srcy, long srcw, long srch, int mode = HALFTONE)
  {
    SetStretchBltMode(dest.m_offscreen, mode);
    int r = StretchBlt(
      dest.m_offscreen, destx, desty, destw, desth,
      m_offscreen, srcx, srcy, srcw, srch, SRCCOPY);
    return r != 0;
  }

  bool StretchBlit(AnimBitmap& dest, long x, long y, long w, long h, int mode = HALFTONE)
  {
    SetStretchBltMode(dest.m_offscreen, mode);
    int r = StretchBlt(dest.m_offscreen, x, y, w, h, m_offscreen, 0, 0, m_x, m_y, SRCCOPY);
    return r != 0;
  }

  bool StretchBlit(HDC hDest, long x, long y, long w, long h)
  {
    int r = StretchBlt(hDest, x, y, w, h, m_offscreen, 0, 0, m_x, m_y, SRCCOPY);
    return r != 0;
  }

  bool Blit(HDC hDest, long x, long y)
  {
    int r = BitBlt(hDest, x, y, x + m_x, y + m_y, m_offscreen, 0, 0, SRCCOPY);
    return r != 0;
  }

  bool Blit(HDC hDest, long x, long y, long width, long height)
  {
    int r = BitBlt(hDest, x, y, width, height, m_offscreen, 0, 0, SRCCOPY);
    return r != 0;
  }

  template<long _Tbpp>
  bool Blit(AnimBitmap<_Tbpp>& dest, long x, long y)
  {
    int r = BitBlt(dest.m_offscreen, x, y, x + m_x, y + m_y, m_offscreen, 0, 0, SRCCOPY);
    return r != 0;
  }

  template<long _Tbpp>
  bool Blit(AnimBitmap<_Tbpp>& dest, long x, long y, long width, long height)
  {
    int r = BitBlt(dest.m_offscreen, 0, 0, width, height, m_offscreen, x, y, SRCCOPY);
    return r != 0;
  }

  bool BlitFrom(HDC src, long x, long y, long w, long h, long DestX = 0, long DestY = 0)
  {
    int r = BitBlt(m_offscreen, DestX, DestY, w, h, src, x, y, SRCCOPY);
    return r != 0;
  }

  bool StretchBlitFrom(int destx, int desty, int destw, int desth, HDC src, int srcx, int srcy, int srcw, int srch, int mode = HALFTONE)
  {
    SetStretchBltMode(m_offscreen, mode);
    int r = StretchBlt(m_offscreen, destx, desty, destw, desth, src, srcx, srcy, srcw, srch, SRCCOPY);
    return r != 0;
  }

  bool DrawText_(PCTSTR s, long x, long y)
  {
    RECT rc;
    rc.left = x;
    rc.top = y;
    rc.right = m_x;
    rc.bottom = m_y;
    DrawText(m_offscreen, s, static_cast<int>(_tcslen(s)), &rc, 0);
    return true;
  }

  RgbPixel* GetBuffer()
  {
    return m_pbuf;
  }

  HBITMAP GetHBITMAP() const
  {
    return m_bmp;
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

private:
  long m_x;
  long m_y;
  HDC m_offscreen;
  HBITMAP m_bmp;
  HBITMAP m_oldBitmap;
  RgbPixel* m_pbuf;

  void FreeBitmap()
  {
    if(m_bmp)
    {
      SelectObject(m_offscreen, m_oldBitmap);
      DeleteObject(m_bmp);
      m_bmp = 0;
      m_oldBitmap = 0;
    }
  }

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

