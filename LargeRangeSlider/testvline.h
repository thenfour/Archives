
#pragma once

#include "animbitmap.h"
#include "hatchpattern.h"
#include "bubbleview.h"




class TestVLine
{
public:
  void OnSize(int x, int y)
  {
    m_offscreen.SetSize(x, y);
  }
  void Render(HWND hWnd)
  {
    m_offscreen.BeginDraw();

    // fill background.
    m_offscreen.Fill(0);

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    RenderLineHeightTest(rcClient);
    RenderLinePositionTest(rcClient);
    RenderSmallLineTest(rcClient);

    m_offscreen.Commit();

    HDC dc = GetDC(hWnd);
    m_offscreen.Blit(dc, 0, 0);
    ReleaseDC(hWnd, dc);
  }

private:
  void RenderMixed(int y, float mix)
  {
    m_offscreen.MixPixelSafe(m_x + m_xoffset, y, MakeRgbPixel32(255,255,255), mix);
  }
  void RenderPixel(int y)
  {
    m_offscreen.SetPixelSafe(m_x + m_xoffset, y, MakeRgbPixel32(255,255,255));
  }
  void RenderLineHeightTest(RECT& rcClient)
  {
    m_xoffset = 0;
    for(m_x = 0; m_x <= 200; m_x ++)
    {
      Hatch1::RenderVLine(10, 10.0 + ((float)m_x / 10), this, &TestVLine::RenderMixed, &TestVLine::RenderPixel);
    }
  }

  void RenderLinePositionTest(RECT& rcClient)
  {
    m_xoffset = 210;
    for(m_x = 0; m_x <= 100; m_x ++)
    {
      float yoffset = ((float)m_x / 10);
      Hatch1::RenderVLine(10.0f + yoffset, 40.0 + yoffset, this, &TestVLine::RenderMixed, &TestVLine::RenderPixel);
    }
  }

  void RenderSmallLineTest(RECT& rcClient)
  {
    m_xoffset = 310;
    for(m_x = 0; m_x <= 100; m_x ++)
    {
      float yoffset = ((float)m_x / 10);
      yoffset *= yoffset;
      Hatch1::RenderVLine(10.0f + yoffset, 10.45 + yoffset, this, &TestVLine::RenderMixed, &TestVLine::RenderPixel);
    }
  }

  int m_x;
  int m_xoffset;
  AnimBitmap<32> m_offscreen;
};
