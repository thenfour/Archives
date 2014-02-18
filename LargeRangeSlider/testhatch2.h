
#pragma once

#include "animbitmap.h"
#include "hatchpattern.h"
#include "bubbleview.h"




class TestHatchLine2
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
    if(y < m_top) return;
    if(y >= m_bottom) return;
    m_offscreen.MixPixelSafe(m_x + m_xoffset, y, MakeRgbPixel32(255,255,255), mix);
  }
  void RenderPixel(int y)
  {
    if(y < m_top) return;
    if(y >= m_bottom) return;
    m_offscreen.SetPixelSafe(m_x + m_xoffset, y, MakeRgbPixel32(255,255,255));
  }

  void RenderMixedMirrored(int y, float mix)
  {
    if(y < 0) return;
    if(y >= m_size) return;
    m_offscreen.MixPixelSafe(m_x + m_xoffset, m_yanchor + y, MakeRgbPixel32(255,255,255), mix);
    m_offscreen.MixPixelSafe(m_x + m_xoffset, m_yanchor - y, MakeRgbPixel32(255,255,255), mix);
  }
  void RenderPixelMirrored(int y)
  {
    if(y < 0) return;
    if(y >= m_size) return;
    m_offscreen.SetPixelSafe(m_x + m_xoffset, m_yanchor + y, MakeRgbPixel32(255,255,255));
    m_offscreen.SetPixelSafe(m_x + m_xoffset, m_yanchor - y, MakeRgbPixel32(255,255,255));
  }

  void RenderSmoothZoomTest(RECT& rcClient)
  {
    m_size = 100;
    m_xoffset = 0;
    int hatchSize = 20;
    m_yanchor = 110;
    for(m_x = 0; m_x <= 200; m_x ++)
    {
      float hatchBegin = m_x;
      float hatchEnd = m_x + 1;
      float zoom = 0.2f + (float)(m_x+0.5) / 50;
      Hatch2::RenderVScanForHatch(m_size, hatchSize,
        hatchBegin, hatchEnd, 1, zoom, this, &TestHatchLine2::RenderMixedMirrored, &TestHatchLine2::RenderPixelMirrored);
    }
  }

  void RenderGapFillTest(RECT& rcClient)
  {
    m_xoffset = 210;
    m_size = 100;
    m_yanchor = 110;
    int hatchSize = 50;
    for(m_x = 0; m_x <= 200; m_x ++)
    {
      float hatchBegin = m_x * 4;
      float hatchEnd = (m_x + 1) * 4;
      Hatch2::RenderVScanForHatch(m_size, hatchSize,
        hatchBegin, hatchEnd, 2.0f, 2.0f, this, &TestHatchLine2::RenderMixedMirrored, &TestHatchLine2::RenderPixelMirrored);
    }
  }

  void RenderLineWidthTest(RECT& rcClient)
  {
    m_xoffset = 410;
    m_size = 100;
    m_yanchor = 110;
    int hatchSize = 50;
    for(m_x = 0; m_x <= 400; m_x ++)
    {
      Hatch2::RenderVScanForHatch(m_size, hatchSize,
        m_x + 0.5, m_x + 1.5, (1.0 + sin((float)m_x / 60)) * 3, 1.0f, this, &TestHatchLine2::RenderMixedMirrored, &TestHatchLine2::RenderPixelMirrored);
    }
  }

  int m_top;
  int m_bottom;
  int m_size;
  int m_yanchor;
  int m_x;
  int m_xoffset;
  AnimBitmap<32> m_offscreen;
};


