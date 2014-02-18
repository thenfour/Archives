

#include ".\ai.h"

const float gPI=3.14f;


// favor cards at the top because they are most likely to be stuck
// favor lonely vertical pairs because they are more rare (where each card is not next to a match)
// "paired only by index" cards also get less precedence

namespace MarioSim
{
  bool SocialStatusGreaterThan(SocialStatus lhs, SocialStatus rhs)
  {
    return false;
  }

  SocialStatus GetSocialStatus(const Game& g, Point& a, Point& b)
  {
    SocialStatus ret;
    return ret;
  }

  bool FindBestMove(const Game& g, Point& a, Point& b)
  {
    // first figure out each card's "social status", meaning their relationship to other cards
    return 0;
  }
}


