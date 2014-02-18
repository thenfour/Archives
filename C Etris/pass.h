#ifndef __PASS
	#define __PASS

#define INITGUID//for dxguid.lib

#define KEYDOWN(name,key) (name[key] & 0x80)//used for DirectInput for reading from
															//the keyboard


#define GRIDSIZE               20

#define SETTINGS_PLAY		1
#define SETTINGS_EXIT		2
#define SETTINGS_NOTHING   0

#define PAUSE_FRAMES				  1
#define LEFT_FRAMES				  4
#define RIGHT_FRAMES				  4
#define DROP_FRAMES				  4
#define ROTATE_FRAMES			  8

#define TET_BLOCK_NOTHING       0
#define TET_BLOCK_FIRE          6
#define TET_BLOCK_EARTH		     2
#define TET_BLOCK_WATER		     3
#define TET_BLOCK_FLAMMABLE     4
#define TET_BLOCK_ICE		     5
#define TET_BLOCK_GRAVITY       1
#define TET_BLOCK_DESTRUCT	     7
#define TET_BLOCK_MAX           7 //This is the highest TET_BLOCK_number.

#define PLAYFIELD_NEXT_LEFT    87
#define PLAYFIELD_NEXT_TOP		151
#define PLAYFIELD_NEXT_RIGHT  100
#define PLAYFIELD_NEXT_BOTTOM 200

#define PLAYFIELD_MAIN_LEFT   258
#define PLAYFIELD_MAIN_TOP		 52
#define PLAYFIELD_MAIN_RIGHT  458
#define PLAYFIELD_MAIN_BOTTOM 452

#define PAUSED_LINES_X		   110
#define PAUSED_LINES_Y		   127
#define PAUSED_EFFECTS_X	   110
#define PAUSED_EFFECTS_Y	   142
#define PAUSED_SINGLES_X	   110
#define PAUSED_SINGLES_Y	   157
#define PAUSED_DOUBLES_X	   110
#define PAUSED_DOUBLES_Y	   172
#define PAUSED_TRIPLES_X	   110
#define PAUSED_TRIPLES_Y	   187
#define PAUSED_QUADS_X		   110
#define PAUSED_QUADS_Y		   202
#define PAUSED_SINGLESIAR_X   110
#define PAUSED_SINGLESIAR_Y	217
#define PAUSED_DOUBLESIAR_X	110
#define PAUSED_DOUBLESIAR_Y	232
#define PAUSED_TRIPLESIAR_X	110
#define PAUSED_TRIPLESIAR_Y	247
#define PAUSED_QUADSIAR_X		110
#define PAUSED_QUADSIAR_Y		262

#define PAUSED_EARTH_X	      110
#define PAUSED_EARTH_Y	      292
#define PAUSED_FIRE_X	      110
#define PAUSED_FIRE_Y         307
#define PAUSED_DESTRUCT_X 	   110
#define PAUSED_DESTRUCT_Y 	   322
#define PAUSED_FLAMMABLE_X	   110
#define PAUSED_FLAMMABLE_Y	   337
#define PAUSED_ICE_X	         110
#define PAUSED_ICE_Y	         352
#define PAUSED_WATER_X	      110
#define PAUSED_WATER_Y	      367
#define PAUSED_GRAVITY_X	   110
#define PAUSED_GRAVITY_Y	   382

#define PAUSED_DROPS_X  	   110
#define PAUSED_DROPS_Y	      412

#define PAUSED_BLOCK1_X		   140
#define PAUSED_BLOCK1_Y		   127
#define PAUSED_BLOCK2_X	      140
#define PAUSED_BLOCK2_Y	      142
#define PAUSED_BLOCK3_X	      140
#define PAUSED_BLOCK3_Y	      157
#define PAUSED_BLOCK4_X	      140
#define PAUSED_BLOCK4_Y	      172
#define PAUSED_BLOCK5_X	      140
#define PAUSED_BLOCK5_Y	      187
#define PAUSED_BLOCK6_X		   140
#define PAUSED_BLOCK6_Y		   202
#define PAUSED_BLOCK7_X       140
#define PAUSED_BLOCK7_Y	      217

#define PAUSED_PAUSED_X			160
#define PAUSED_PAUSED_Y			127

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
#define STATS_CHEATS            1
#define STATS_PAUSED          100


//This file includes only the struct that is used to pass variables between
//modules.  I'm a beginner and am learning much about the organization of a
//C++ file...
//This is a solution to a problem of mine:  I wanted to pass my DDObjects and
//DDSurfaces and stuff to each module, making them virtually global, but controlled.
//This struct, in turn, will be passed between each module.
#include <windows.h>
#include "ddraw.h"
#include "dinput.h"
#include "dsound.h"
#include "ddutil.h"
#include <stdlib.h>//random
#include <time.h>//randomize();
#include <dos.h>
#include <string.h>

#include "tetris.rh"

//THE VARIABLE: ProgramFlow.  The description:
//This variable is set to PF_??? where ??? is the current place in the game.
//It's used for the WindowProc function...in other words, when we're displaying
//the title, messages are handled differently than when we're in the actual game.
//this variable allows us to distinguish between it all.
#define PF_GAME                 0
#define PF_STARTUP              1
#define PF_SETTINGS             2
#define PF_EXIT					  3
#define PF_PAUSE					  4
#define PF_GAMEEXITING			  5
#define PF_CREDITS				  6
#define PF_SOUND					  7
#define PF_TEXTEDITBOX			  8
#define PF_DISPLAYHIGHSCORES    9

#include "dsstream.h"
struct PASSVARS
{
   int GeneralInt1;
   int GeneralInt2;

	LPDIRECTDRAW DDObject;
   LPDIRECTDRAWSURFACE DDFront;
   LPDIRECTDRAWSURFACE DDBack;

   LPDIRECTDRAWSURFACE DDFont8;
   LPDIRECTDRAWSURFACE DDFont22;

   LPDIRECTSOUND DSObject;
   LPDIRECTSOUNDBUFFER DSPrimary;
   LPDIRECTSOUNDBUFFER DSScore;
   LPDIRECTSOUNDBUFFER DSSound1;
   DSSTREAM Stream1;

   LPDIRECTINPUT DIObject;
   LPDIRECTINPUTDEVICE DIKeyboard;

	HINSTANCE hInstance;
   HWND DDWindow;
   char ProgramFlow;

   LPDIRECTDRAWPALETTE MainPalette;
   LPDIRECTDRAWPALETTE HSPalette;

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
#include "animation.h"
#include "playfield.h"

#include "dinputlib.h"
#include "ddrawlib.h"
#include "dswavlib.h"
#include "dsstream.h"
#include "dstrwave.h"
#include "dsoundlib.h"

#include "textlib.h"
#include "textfilelib.h"
#include "windowlibrary.h"

#include "texteditbox.h"
#include "commandbutton.h"

#include "highscores.h"

#include "settings.h"
#include "displayhighscores.h"
#include "gameover.h"
#include "etetriz.h"
#include "game.h"
#include "pause.h"


#endif //__PASS
