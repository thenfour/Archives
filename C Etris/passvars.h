//declaration of commonly used structs for tetris.

#ifndef __PASSVARS
#define __PASSVARS

#include "ddraw.h"
#include "dsound.h"
#include "dinput.h"
#include "highscores.h"
#include "dsstream.h"
#include "ddrawlib.h"


#define INITGUID//for dxguid.lib

#define KEYDOWN(name,key) (name[key] & 0x80)//used for DirectInput for reading from
															//the keyboard


#define GRIDSIZE               20

#define SETTINGS_PLAY		1
#define SETTINGS_EXIT		2
#define SETTINGS_NOTHING   0

#define TET_BLOCK_NOTHING       0
#define TET_BLOCK_FIRE          6
#define TET_BLOCK_EARTH		     2
#define TET_BLOCK_WATER		     3
#define TET_BLOCK_FLAMMABLE     4
#define TET_BLOCK_ICE		     5
#define TET_BLOCK_GRAVITY       1
#define TET_BLOCK_DESTRUCT	     7
#define TET_BLOCK_MAX           7 //This is the highest TET_BLOCK_number.

#define PLAYFIELD_NEXT_LEFT    160
#define PLAYFIELD_NEXT_TOP		140
#define PLAYFIELD_NEXT_RIGHT  100
#define PLAYFIELD_NEXT_BOTTOM 200

#define PLAYFIELD_MAIN_LEFT   258
#define PLAYFIELD_MAIN_TOP		 58
#define PLAYFIELD_MAIN_RIGHT  458
#define PLAYFIELD_MAIN_BOTTOM 458

#define STATS_EFFECTS 			  5
#define STATS_LINES            10
#define STATS_SINGLES          50
#define STATS_DOUBLES         200
#define STATS_TRIPLES         800
#define STATS_QUADS          3200
#define STATS_MORETHANQUADS 20000
#define STATS_SINGLESIAR      200
#define STATS_DOUBLESIAR     2000
#define STATS_TRIPLESIAR     8000
#define STATS_QUADSIAR      32000
#define STATS_MORETHANQUADSIAR 100000

#define STATS_EARTH             2
#define STATS_FIRE              1
#define STATS_ICE               2
#define STATS_WATER             2
#define STATS_DESTRUCT          3
#define STATS_GRAVITY           3
#define STATS_FLAMMABLE         3

#define STATS_BLOCK1            1
#define STATS_BLOCK2            1
#define STATS_BLOCK3            1
#define STATS_BLOCK4            1
#define STATS_BLOCK5            1
#define STATS_BLOCK6            1
#define STATS_BLOCK7            1

#define STATS_DROPS             1
#define STATS_CHEATS            2//not used; cheats are half your score. (except double score!)
#define STATS_PAUSED          100


//This file includes only the struct that is used to pass variables between
//modules.  I'm a beginner and am learning much about the organization of a
//C++ file...
//This is a solution to a problem of mine:  I wanted to pass my DDObjects and
//DDSurfaces and stuff to each module, making them virtually global, but controlled.
//This struct, in turn, will be passed between each module.

//THE VARIABLE: ProgramFlow.  The description:
//This variable is set to PF_??? where ??? is the current place in the game.
//It's used for the WindowProc function...in other words, when we're displaying
//the title, messages are handled differently than when we're in the actual game.
//this variable allows us to distinguish between it all.
#define PF_GAME						0
#define PF_STARTUP					1
#define PF_SETTINGS					2
#define PF_EXIT						3
#define PF_PAUSE					4
#define PF_GAMEEXITING				5
#define PF_CREDITS					6
#define PF_SOUND					7
#define PF_TEXTEDITBOX				8
#define PF_DISPLAYHIGHSCORES		9
#define PF_OPTIONS					10

struct TETRISOPTIONS{
	int Key_rotateccw;
	int Key_rotatecw;
	int Key_drop;
	int Key_left;
	int Key_right;
	int SFXVol;
	int MusicVol;
	bool SFX;
	bool Music;

};

#define NUMBER_OF_DSBUFFERS			4

struct TETRIS_SOUNDS{
	LPDIRECTSOUNDBUFFER Primary;

	DSSTREAM GameScore;
	DSSTREAM MenuScore;

	LPDIRECTSOUNDBUFFER MenuBuffer1;
	LPDIRECTSOUNDBUFFER MenuBuffer2;

	LPDIRECTSOUNDBUFFER DSBuffer[NUMBER_OF_DSBUFFERS];
};

struct PASSVARS
{
	int GeneralInt1;
	int GeneralInt2;

	RGB16 Rgb16;

	LPDIRECTDRAW DDObject;
	LPDIRECTDRAWSURFACE DDFront;
	LPDIRECTDRAWSURFACE DDBack;

	LPDIRECTDRAWSURFACE DDFont8;
	LPDIRECTDRAWSURFACE DDFont12;
	LPDIRECTDRAWSURFACE DDFont13;
	LPDIRECTDRAWSURFACE DDFont22;

	LPDIRECTSOUND DSObject;
	TETRIS_SOUNDS Sounds;

	LPDIRECTINPUT DIObject;
	LPDIRECTINPUTDEVICE DIKeyboard;

	int FrameWait;//How many Milliseconds we should waste in the game per frame to make sure it's slow enough.

	HINSTANCE hInstance;
	HWND DDWindow;
	char ProgramFlow;

	int SettingsInfo;//this is just a general number that's used by the Settings routine.
};

//This is used when we jump to different routines in the WNdPROC callback function
//- it just passes the parameters in one shot.
struct PASSPROCVARS
{
	HWND hwnd;
   UINT uMsg;
   WPARAM wParam;
   LPARAM lParam;
};

struct STATS
{
	long Level;

	long Lines;
   long Effects;
   long Doubles;
   long Triples;
   long Singles;
   long Quads;
	long MoreThanQuads;
   long DoublesIAR;
   long SinglesIAR;
   long TriplesIAR;
   long QuadsIAR;
	long MoreThanQuadsIAR;

   long Earth;
   long Fire;
   long Destruct;
   long Ice;
   long Water;
   long Flammable;
   long Gravity;

   long Drops;

   long Score;
   long Block1;
   long Block2;
   long Block3;
   long Block4;
   long Block5;
   long Block6;
   long Block7;

   long Paused;
   long Cheats;
};
#endif //__PASS
