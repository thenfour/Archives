

#include ".\renderer.h"


namespace MarioSim
{
  const int CardWidth = 50;
  const int CardHeight = 80;
  const int CardSpacing = 10;
  const int CardOffset = 20;

  //   |--offset--[****validsize****]--spacing--[****validsize****]--....
  bool HitTest(int coord, int validsize, int spacing, int offset, int maxindex, int& out)
  {
    out = coord - offset;
    int pitch = validsize + spacing;
    if((out % pitch) > validsize) return false;// lands on a space
    out /= pitch;
    if(out > maxindex) return false;
    return true;
  }

  bool HitTest(const Game& g, int x, int y, int& cardx, int& cardy)
  {
    if(HitTest(x, CardWidth, CardSpacing, CardOffset, g.GetArenaWidth(), cardx))
    {
      if(HitTest(y, CardHeight, CardSpacing, CardOffset, g.GetArenaHeight(), cardy))
      {
        Card c = g.GetAt(Point(cardx, cardy));
        if(c.m_face != Card::Blank)
        {
          return true;
        }
      }
    }
    return false;
  }

  bool GetCardRect(int x, int y, RECT& rc)
  {
    rc.left = CardOffset + (x * (CardWidth + CardSpacing));
    rc.top = CardOffset + (y * (CardHeight + CardSpacing));
    rc.bottom = rc.top + CardHeight;
    rc.right = rc.left + CardWidth;
    return true;
  }

  void RenderGame(HDC dc, const Game& g)
  {
    for(int x = 0; x < g.GetArenaWidth(); x++)
    {
      for(int y = 0; y < g.GetArenaHeight(); y++)
      {
        RECT rc;
        GetCardRect(x,y,rc);
        RenderCard(dc, g.GetAt(Point(x,y)), rc.left, rc.top);
      }
    }
  }

  HBRUSH g_CardBrushes[10];
  bool g_CardBrushesInitialized(false);

  HBRUSH GetCardBrush(Card::Face x)
  {
    if(!g_CardBrushesInitialized)
    {
      g_CardBrushes[0] = CreateSolidBrush(RGB(40,55,55));
      g_CardBrushes[1] = CreateSolidBrush(RGB(128,128,128));
      g_CardBrushes[2] = CreateSolidBrush(RGB(0,0,255));
      g_CardBrushes[3] = CreateSolidBrush(RGB(0,128,0));
      g_CardBrushes[4] = CreateSolidBrush(RGB(0,255,255));
      g_CardBrushes[5] = CreateSolidBrush(RGB(60,0,128));
      g_CardBrushes[6] = CreateSolidBrush(RGB(255,0,128));
      g_CardBrushes[7] = CreateSolidBrush(RGB(128,128,0));
      g_CardBrushes[8] = CreateSolidBrush(RGB(128,128,255));
      g_CardBrushes[9] = CreateSolidBrush(RGB(128,255,128));
      g_CardBrushesInitialized = true;
    }
    return g_CardBrushes[x];
  }

  void RenderCard(HDC dc, const Card& c, int x, int y)
  {
    HBRUSH h = GetCardBrush(c.m_face);
    RECT rc;
    SetRect(&rc, x, y, x + CardWidth, y + CardHeight);
    FillRect(dc, &rc, h);
  }
}

