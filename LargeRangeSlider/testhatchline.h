
#pragma once

#include "animbitmap.h"
#include "hatchpattern.h"
#include "bubbleview.h"




class TestHatchLine
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

    RenderSmoothZoomTest(rcClient);
    RenderGapFillTest(rcClient);
    RenderLineWidthTest(rcClient);

    m_offscreen.Commit();

    HDC dc = GetDC(hWnd);
    m_offscreen.Blit(dc, 0, 0);
    ReleaseDC(hWnd, dc);
  }

private:
  void RenderMixed(int y, float mix)
  {
    if(y < m_top) y = m_top;
    if(y >= m_bottom) y = m_bottom;
    m_offscreen.MixPixelSafe(m_x + m_xoffset, y, MakeRgbPixel32(255,255,255), mix);
  }
  void RenderPixel(int y)
  {
    if(y < m_top) y = m_top;
    if(y >= m_bottom) y = m_bottom;
    m_offscreen.SetPixelSafe(m_x + m_xoffset, y, MakeRgbPixel32(255,255,255));
  }

  void RenderSmoothZoomTest(RECT& rcClient)
  {
    m_xoffset = 0;
    m_top = 10;
    m_bottom = 210;
    int hatchSize = 20;
    for(m_x = 0; m_x <= 200; m_x ++)
    {
      Hatch1::RenderHatchVLine(m_top, m_bottom - m_top, hatchSize, m_x % hatchSize, 1.5f, 0.2f + (float)m_x / 50, this, &TestHatchLine::RenderMixed, &TestHatchLine::RenderPixel);
    }
  }

  void RenderGapFillTest(RECT& rcClient)
  {
    m_xoffset = 210;
    m_top = 10;
    m_bottom = 210;
    int hatchSize = 50;
    for(m_x = 0; m_x <= 200; m_x ++)
    {
      Hatch1::RenderHatchVLine(m_top, m_bottom - m_top, hatchSize, (m_x * 4) % hatchSize, 2.0f, 2.0f, this, &TestHatchLine::RenderMixed, &TestHatchLine::RenderPixel);
    }
  }

  void RenderLineWidthTest(RECT& rcClient)
  {
    m_xoffset = 420;
    m_top = 10;
    m_bottom = 210;
    int hatchSize = 50;
    for(m_x = 0; m_x <= 400; m_x ++)
    {
      Hatch1::RenderHatchVLine(m_top, m_bottom - m_top, hatchSize, m_x % hatchSize, (1.0 + sin((float)m_x / 60)) * 3, 1.0f, this, &TestHatchLine::RenderMixed, &TestHatchLine::RenderPixel);
    }
  }

  int m_top;
  int m_bottom;

  int m_x;
  int m_xoffset;
  AnimBitmap<32> m_offscreen;
};


