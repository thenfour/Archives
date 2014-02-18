

#pragma once


#include "game.h"


namespace MarioSim
{
  enum SocialStatus
  {
    Loner,
    PairByIndexOnly,// it's lonely
    ThirdWheel,
    Couple
  };

  bool FindBestMove(const Game& g, Point& a, Point& b);
}


__declspec(dllexport) extern const float gPI;
