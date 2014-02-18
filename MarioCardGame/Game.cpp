

#include ".\game.h"
#include <windows.h>
#include <algorithm>


int MarioRand(int n)
{
  return rand() % n;
}


namespace MarioSim
{

  Game::Game(int arenaWidth, int arenaHeight) :
    m_arenaWidth(arenaWidth),
    m_arenaHeight(arenaHeight)
  {
    srand(GetTickCount());
  }

  Game::~Game()
  {
  }

  Card Game::GetAt(int i) const
  {
    if(i < 0) return Card();
    if(i >= (int)m_arena.size()) return Card();
    return m_arena[i];
  }

  Card Game::GetAt(const Point& p) const
  {
    return GetAt(XYtoIndex(p));
  }

  int Game::XYtoIndex(const Point& p) const
  {
    if(p.first < 0) return -1;
    if(p.first >= m_arenaWidth) return -1;
    if(p.second < 0) return -1;
    if(p.second >= m_arenaHeight) return -1;
    return (p.second * m_arenaHeight) + p.first;
  }

  Point Game::IndexToXY(int index) const
  {
    return Point(index % m_arenaWidth, index / m_arenaHeight);
  }

  Game::Outcome Game::Move(const Point& a, const Point& b, int& pointDelta)
  {
    return Move(XYtoIndex(a), XYtoIndex(b), pointDelta);
  }

  void Game::Reset(int nEachType)
  {
    m_deck.clear();
    m_arena.clear();
    int count;
    int serial = 100;

    std::vector<Card::Face> faces;
    faces.push_back(Card::Goomba);
    faces.push_back(Card::Mushroom);
    faces.push_back(Card::Flower);
    faces.push_back(Card::Mario);
    faces.push_back(Card::Bowser);
    faces.push_back(Card::Cloud);
    faces.push_back(Card::Luigi);
    faces.push_back(Card::Star);

    std::vector<Card::Face>::const_iterator faceIt;

    for(count = 0; count < nEachType; count ++, serial ++)
    {
      for(faceIt = faces.begin(); faceIt != faces.end(); ++ faceIt)
      {
        m_deck.push_back(Card(*faceIt, serial));
      }
    }

    std::random_shuffle(m_deck.begin(), m_deck.end(), MarioRand);

    _DealOut();
  }

  Game::Outcome Game::Move(int a, int b, int& pointDelta)
  {
    pointDelta = 0;

    // sort a & b so that a < b (makes stuff easier)
    if(b < a)
    {
      std::swap(a, b);
    }

    Card cardA = GetAt(a);
    Card cardB = GetAt(b);
    Point ptA = IndexToXY(a);
    Point ptB = IndexToXY(b);

    // must be similar cards
    if(cardA.m_face != cardB.m_face)
    {
      return Game::InvalidMove;
    }
    // must be valid cards
    if(cardA.m_face == Card::Blank)
    {
      return Game::InvalidMove;
    }
    // must be near eachother.
    if((abs(ptA.first - ptB.first) > 1) || (abs(ptA.second - ptB.second) > 1))
    {
      return Game::InvalidMove;
    }
    // must not be the same card.
    if(a == b)
    {
      return Game::InvalidMove;
    }

    // OK; appears to be a legal move.  Go ahead.
    m_arena.erase(m_arena.begin() + b);
    m_arena.erase(m_arena.begin() + a);

    // deal out cards.
    _DealOut();

    // Check for winnar!
    if(0 == m_arena.size())
    {
      pointDelta = 10;
      return GameWon;
    }

    // check for loser!
    int nMovesLeft = 0;
    // go through each card and see if it has a move. if so, break.
    int x, y;
    for(y = 0; y < m_arenaHeight; y ++)
    {
      for(x = 0; x < m_arenaWidth; x ++)
      {
        if(_HasMovesAvailable(x, y))
        {
          nMovesLeft = 1;
          break;
        }
      }
      if(nMovesLeft)
      {
        break;
      }
    }
    if(!nMovesLeft)
    {
      pointDelta = (int)m_arena.size();
      pointDelta = -pointDelta;
      return GameLost;
    }

    return Success;
  }

  void Game::_DealOut()
  {
    // remove trailing blank cards (just in case)
    while(m_arena.size() && (m_arena.back().m_face == Card::Blank))
    {
      m_arena.pop_back();
    }

    // now deal in cards until the arena is filled
    while(m_deck.size() && ((int)m_arena.size() < (m_arenaHeight * m_arenaWidth)))
    {
      m_arena.push_back(m_deck.back());
      m_deck.pop_back();
    }

    return;
  }

  bool Game::_HasMovesAvailable(int x, int y)
  {
    Card card = GetAt(Point(x, y));
    if(card.m_face == Card::Blank) return false;

    // check to the left
    if(GetAt(Point(x - 1, y)).m_face == card.m_face)
    {
      return true;
    }
    // check to the right
    if(GetAt(Point(x + 1, y)).m_face == card.m_face)
    {
      return true;
    }
    // check lower-left
    if(GetAt(Point(x - 1, y + 1)).m_face == card.m_face)
    {
      return true;
    }
    // check lower-center
    if(GetAt(Point(x, y + 1)).m_face == card.m_face)
    {
      return true;
    }
    // check lower-right
    if(GetAt(Point(x + 1, y + 1)).m_face == card.m_face)
    {
      return true;
    }

    return false;
  }
}

