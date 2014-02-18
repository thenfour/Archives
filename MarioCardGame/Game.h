

#pragma once


#include <vector>
//#include <windows.h>


namespace MarioSim
{
  //------------------------------
  class Card
  {
  public:
    enum Face
    {
      Blank,
      Goomba,
      Mushroom,
      Flower,
      Mario,
      Bowser,
      Cloud,
      Luigi,
      Star
    };

    Card() : m_face(Blank), m_serial(0) { }
    Card(Face f, int s = 0) : m_face(f), m_serial(s) { }
    Card(const Card& rhs) : m_face(rhs.m_face), m_serial(rhs.m_serial) { }
    Card& operator =(const Card& rhs) { m_face = rhs.m_face; m_serial = rhs.m_serial; return *this; }

    Face m_face;
    int m_serial;// serial # in the deck.
  };

  //------------------------------
  typedef std::vector<Card> CardList;
  typedef std::pair<int,int> Point;

  //------------------------------
  class Game
  {
  public:
    Game(int arenaWidth, int arenaHeight);
    ~Game();

    void Reset(int nEachType);

    Card GetAt(int i) const;// from top/left
    Card GetAt(const Point& p) const;
    int GetArenaWidth() const { return m_arenaWidth; }
    int GetArenaHeight() const { return m_arenaHeight; }
    int GetCardsLeft() const;

    int XYtoIndex(const Point& p) const;
    Point IndexToXY(int index) const;

    // each move may result in these outcomes:
    enum Outcome
    {
      GameLost,
      GameWon,
      Success,//
      InvalidMove,//
      Unknown//
    };

    Outcome Move(const Point& a, const Point& b, int& pointDelta);
    Outcome Move(int a, int b, int& pointDelta);

  private:
    int m_arenaWidth;
    int m_arenaHeight;

    CardList m_deck;
    CardList m_arena;// top/left to bottom/right, like reading.

    void _DealOut();
    bool _HasMovesAvailable(int x, int y);
  };
}


