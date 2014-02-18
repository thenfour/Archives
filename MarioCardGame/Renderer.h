

#pragma once


#include "game.h"
#include <windows.h>


namespace MarioSim
{
  void RenderGame(HDC dc, const Game& g);
  void RenderCard(HDC dc, const Card& c, int x, int y);
  bool HitTest(const Game& g, int x, int y, int& cardx, int& cardy);// will not return blank card hits
  bool GetCardRect(int x, int y, RECT& rc);
}

