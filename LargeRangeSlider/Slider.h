
#pragma once

#include "animbitmap.h"
#include "hatchpattern.h"
#include "bubbleview.h"


class Slider
{
public:
  Slider() :
    m_renderBackground(false),
    m_renderArea(false)
  {
  }

  void OnSize(int x, int y)
  {
    m_renderBackground = false;
    m_renderArea = false;

    if(x <= 0 || y <= 0) return;

    m_renderBackground = true;

    m_offscreen.SetSize(x, y);

    static const int frame = 10;
    m_rcDisplay.left = frame;
    m_rcDisplay.right = x - frame;
    m_rcDisplay.top = frame;
    m_rcDisplay.bottom = y - frame;
    if(Width(m_rcDisplay) > 0 && Height(m_rcDisplay) > 0)
    {
      m_renderArea = true;
      m_media.SetDisplayRange(Width(m_rcDisplay));
      m_media.SetDistortionSize(m_media.DisplayToReal(m_distortionSize));

      m_view.SetDisplayRange(Width(m_rcDisplay));
      m_view.SetRealRange(Width(m_rcDisplay));
      m_view.SetUPP(.5);
      m_view.SetDistortionSize(m_distortionSize);
    }
  }
  int DisplayToReal(float x) const
  {
    return m_media.DisplayToReal(x - m_rcDisplay.left);
  }
  void SetMediaLength(int x)
  {
    m_media.SetRealRange(x);
  }
  int GetMediaLength() const
  {
    return m_media.GetRealRange();
  }
  void SetCursor(int real)
  {
    m_cursor = real;
  }
  void SetFocus(int real)
  {
    float d = m_media.RealToDisplay(real);
    if(abs(d - m_view.GetFocus()) > (m_distortionSize / 100))
    {
      m_media.SetFocus(real);
      float d = m_media.RealToDisplay(real);
      m_view.SetFocus(d);
    }
  }
  void Render(HWND hWnd)
  {
    if(m_renderBackground)
    {
      m_offscreen.BeginDraw();

      // fill background.
      m_offscreen.Fill(MakeRgbPixel32(0,0,0));

      if(m_renderArea)
      {
        // draw a rectangle.
        m_offscreen.HLine(m_rcDisplay.left - 1, m_rcDisplay.right, m_rcDisplay.bottom, MakeRgbPixel32(255,255,255));
        m_offscreen.HLine(m_rcDisplay.left - 1, m_rcDisplay.right, m_rcDisplay.top-1, MakeRgbPixel32(255,255,255));
        m_offscreen.VLine(m_rcDisplay.left - 1, m_rcDisplay.top - 1, m_rcDisplay.bottom, MakeRgbPixel32(255,255,255));
        m_offscreen.VLine(m_rcDisplay.right, m_rcDisplay.top - 1, m_rcDisplay.bottom, MakeRgbPixel32(255,255,255));

        // draw the graph.
        int hatchSize = 30;
        float lineWidth = 3.0f;
        float linearSlope = m_view.LinearDisplayToReal(1.0f);
        m_yanchor = m_rcDisplay.top + (m_rcDisplay.bottom - m_rcDisplay.top) / 2;

        for(m_x = 0; m_x < Width(m_rcDisplay); m_x ++)
        {
          float beginOffset = m_view.DisplayToReal(m_x) / linearSlope;
          float endOffset = m_view.DisplayToReal(m_x + 1) / linearSlope;

          Hatch2::RenderVScanForHatch(Height(m_rcDisplay) / 2, hatchSize,
            beginOffset, endOffset, lineWidth,
            m_view.GetZoomFactor(m_x), this, &Slider::RenderMixedMirrored, &Slider::RenderPixelMirrored);
        }

        m_offscreen.DrawText_(TimeToString(m_cursor).c_str(), 0, 0);
        m_offscreen.DrawText_(LibCC::Format("Zoom factor: %").f<5>(m_view.GetZoomFactor(m_view.RealToDisplay(m_cursor))).CStr(), 0, 32);
      }

      m_offscreen.Commit();

      HDC dc = GetDC(hWnd);
      m_offscreen.Blit(dc, 0, 0);
      ReleaseDC(hWnd, dc);
    }
  }

private:
  void RenderMixedMirrored(int y, float mix)
  {
    if(y < 0) return;
    if(y >= m_yanchor) return;
    m_offscreen.MixPixelSafe(m_rcDisplay.left + m_x, m_yanchor + y, MakeRgbPixel32(0,60,30), mix);
    m_offscreen.MixPixelSafe(m_rcDisplay.left + m_x, m_yanchor - y, MakeRgbPixel32(30,60,0), mix);
  }
  void RenderPixelMirrored(int y)
  {
    if(y < 0) return;
    if(y >= m_yanchor) return;
    m_offscreen.SetPixelSafe(m_rcDisplay.left + m_x, m_yanchor + y, MakeRgbPixel32(0,60,0));
    m_offscreen.SetPixelSafe(m_rcDisplay.left + m_x, m_yanchor - y, MakeRgbPixel32(90,30,60));
  }

  int m_x;
  int m_yanchor;

  bool m_renderBackground;
  bool m_renderArea;
  DistortedView m_media;
  DistortedView m_view;
  AnimBitmap<32> m_offscreen;
  RECT m_rcDisplay;

  int m_cursor;// in real coords

  static const int m_distortionSize = 500;// in pixels
};
