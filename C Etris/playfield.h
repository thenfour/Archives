#ifndef __PLAYFIELD

#include "passvars.h"
#include "animation.h"
#include "ddrawlib.h"
#include "dsstream.h"
#include "textlib.h"
#include "time.h"
#include "dsoundlib.h"

#ifndef __PLAYFIELD

ANIMATION * GetAnimation(int Blocktype);//function prototype


//This file is the declaration of the function prototypes in the class PlayField
//This class is created for a playfield object and handles most of the game-play.


//struct ScreenBlock is defined for all blocks in the Grid[][] member of class
//Playfield.  It contains information on what kind of block it is, and on what
//frame it is.
int GameWindowProc();

struct SCREENBLOCK{
	char Type;
   char Frame;   //I don't even think I use this one.
};

struct SHAPE{
	SCREENBLOCK BlockType;//Block type
   unsigned char Type;//Shape
   unsigned char Rotation;//duh
};

class PLAYFIELD{
	public:
		PLAYFIELD(LPDIRECTDRAWSURFACE & FieldSurface, LPDIRECTDRAWSURFACE & PauseSurface);
		~PLAYFIELD();

		int Level;//this is the current level #
		int FramesLeft;//this is how many frames until the piece will drop.
		int Frame;//this is only an int because at 60 fps, that would mean you
      				// be playing for over 19 hours and it still wouldn't wrap
                  //around.  You'd be pretty expert to last that long.
		SCREENBLOCK Grid[10][20];
		SHAPE Next;
		SHAPE Current;
		char Shapes[7][4][4][2];
		POINT Pos;
		LPDIRECTDRAWSURFACE* DDField;
		LPDIRECTDRAWSURFACE* DDPause;
		STATS Stats;
		bool GamePaused;
		int PauseFrameCount;

		void Drop();
		void Pause();
		void CalculateScore();
		void DrawShape(int GridX, int GridY, SHAPE Shape);
		void DrawShapeAnywhere(int Xcoord, int Ycoord, SHAPE Shape);
		//cheats
		void ClearField();
		void RandomizeField();
		void DoubleScore();
		void TurnToEarth();
		void GravitizeAll();
		void InstantQuad();
		void GiveAllLines();
		bool Cheat_GiveAllLines;
		void GiveAllEarth();
		bool Cheat_GiveAllEarth;
		void NextGravity();
		void NextDestruct();
		void Cheat1();
		bool Cheat_FrameRate;
		void Cheat2();
		void Cheat3();
		//

		void Reset();
		void SetupNewShape();
		int AdvanceFrame();
		bool CheckHit(SHAPE HitShape, POINT Coord);
		void DrawBlock(int Xcoord, int Ycoord, char BlockType);
		void UpdateField();
		void MoveLeft();
		void MoveRight();
		void Clockwise();
		void CounterClockwise();
		bool CheckForLines();
		void DrawNext();
	private:
		void UpdateIARs(int NumberOfLines, int HowManyInARow);
		int PausedFrame();
		int UnpausedFrame();
		void DoEffects();
		void EffectFrame();
		void CurrentStatUpdate();
   		int DoFire(int x, int y);
   		int DoEarth(int x, int y);
   		int DoFlammable(int x, int y);
   		int DoWater(int x, int y);
   		int DoIce(int x, int y);
   		int DoGravity();
   		int DoDestruct();
};

#include "pause.h"

#define __PLAYFIELD
#endif
#endif
