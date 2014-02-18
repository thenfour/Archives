//while(ShowCursor(FALSE) >=0){}
//while(ShowCursor(TRUE) <0){}
//#DEFINE WAV_CHEAT "sfx\\cheat.wav"

/*This is the module that is the actual game.
it sets up the game screen first, and then handles the rest.
*/

#include "stdio.h"
#include "game.h"

//to use cheats, I create a buffer for keyboard input
//it's not put in with the DirectInput code, but
//it uses windows messages to fill it.
//(it's faster/easier this way).
//each keypress, it's scanned for cheat codes and action is taken accordingly.
#define CHEAT_BUFFER_SIZE 20
TCHAR CheatBuffer[CHEAT_BUFFER_SIZE+1];

#define PAUSE_FRAMES				2
#define LEFT_FRAMES					2
#define RIGHT_FRAMES				2
#define DROP_FRAMES					2
#define ROTATE_FRAMES				5

#define BMP_FIRE "gfx\\24fire.bmp"
#define BMP_FLAMMABLE "gfx\\24flammable.bmp"
#define BMP_DESTRUCT "gfx\\24destruct.bmp"
#define BMP_GRAVITY "gfx\\24gravity.bmp"
#define BMP_WATER "gfx\\24water.bmp"
#define BMP_ICE "gfx\\24ice.bmp"
#define BMP_EARTH "gfx\\24earth.bmp"

#define BMP_PLAYFIELD "gfx\\24playfield.bmp"
#define BMP_PAUSE "gfx\\24pause.bmp"

//Globals
extern PASSVARS Pass;
extern TETRISOPTIONS toOptions;
PASSANIMS Anims;
LPDIRECTDRAWSURFACE PlayField;
LPDIRECTDRAWSURFACE PauseScreen;

//the animations' surfaces
LPDIRECTDRAWSURFACE sFire;
LPDIRECTDRAWSURFACE sEarth;
LPDIRECTDRAWSURFACE sWater;
LPDIRECTDRAWSURFACE sIce;
LPDIRECTDRAWSURFACE sGravity;
LPDIRECTDRAWSURFACE sDestruct;
LPDIRECTDRAWSURFACE sFlammable;

//Now create our playfield.
PLAYFIELD pfPlayField(PlayField, PauseScreen);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameLeave(){
while(ShowCursor(TRUE) <0){}
//	ShowCursor(TRUE);
	Pass.Sounds.GameScore.Stop();
	//destroy the surfaces
	if(PlayField != NULL){
		PlayField->Release();
		PlayField=NULL;
		PauseScreen->Release();
		PauseScreen=NULL;
	}
	//destroy the animations
	if(sFire != NULL){
		sFire->Release();
		sFire=NULL;
		sEarth->Release();
		sEarth=NULL;
		sWater->Release();
		sWater=NULL;
		sIce->Release();
		sIce=NULL;
		sGravity->Release();
		sGravity=NULL;
		sDestruct->Release();
		sDestruct=NULL;
		sFlammable->Release();
		sFlammable=NULL;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int GameMain()
{

Pass.ProgramFlow=PF_GAME;//state that we are to use the GameWindowProc() function for window-message handling
//LRESULT Result;
MSG Msg;

FillMemory(CheatBuffer, sizeof(CheatBuffer), 0x20);//clear the cheat buffer
CheatBuffer[CHEAT_BUFFER_SIZE]=0;

PlayField=DDLoadBitmap(Pass.DDObject,BMP_PLAYFIELD, 0, 0);//Load a copy of the playfield into an off-screen surface.
if(!PlayField) return DD_OK;
PauseScreen=DDLoadBitmap(Pass.DDObject,BMP_PAUSE,0,0);
if(!PauseScreen) return DD_OK;

//Setup all the animations.
ANIMATION aFire(sFire, 			  BMP_FIRE, 32, 1);
ANIMATION aEarth(sEarth, 		  BMP_EARTH, 1, 1);
ANIMATION aWater(sWater,        BMP_WATER, 32, 1);
ANIMATION aIce(sIce,            BMP_ICE, 32, 1);
ANIMATION aGravity(sGravity,    BMP_GRAVITY, 1, 1);
ANIMATION aDestruct(sDestruct,  BMP_DESTRUCT, 1, 1);
ANIMATION aFlammable(sFlammable,BMP_FLAMMABLE, 1, 1);
//Now we can use them all.
//But first we have to enter them all into the Anims struct so we can use them
//globally.

//First the Surfaces.
Anims.sFire         =&sFire;
Anims.sEarth        =&sEarth;
Anims.sWater        =&sWater;
Anims.sIce          =&sIce;
Anims.sGravity      =&sGravity;
Anims.sDestruct     =&sDestruct;
Anims.sFlammable    =&sFlammable;
//Now the ANIMATIONs
Anims.aFire         =&aFire;
Anims.aEarth        =&aEarth;
Anims.aWater        =&aWater;
Anims.aIce          =&aIce;
Anims.aGravity      =&aGravity;
Anims.aDestruct     =&aDestruct;
Anims.aFlammable    =&aFlammable;

//and now clear everything in the playfield
pfPlayField.Reset();

//hide the cursor
while(ShowCursor(FALSE) >=0){}
//while(ShowCursor(TRUE) <0){}


//fade out, then
//fade in the playfield screen.
FadeIn(Pass.DDObject, &Pass.DDFront, &PlayField, 0);

//start the score
Pass.Sounds.GameScore.Play();

//message loop.
do{
	if(!Frame()){//frame will return a 0 if the game is over
		GameLeave();
		return 0;
	}
	if(PeekMessage(&Msg, NULL, 0,0,PM_NOREMOVE)){
		//we have a message
		GetMessage(&Msg, NULL, 0, 0 );
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}while(Pass.ProgramFlow!=PF_EXIT);

GameLeave();
return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int GameWindowProc(PASSPROCVARS Params){
	TCHAR Char;
	switch(Params.uMsg){
	case WM_CHAR:
		Char=(TCHAR) Params.wParam;
		MoveMemory(CheatBuffer, CheatBuffer+1, CHEAT_BUFFER_SIZE-1);//move everything over 1
		CheatBuffer[CHEAT_BUFFER_SIZE-1]=Char;//set the last one to the last pressed key
		//char buf2[50];
		//sprintf(buf2, "%s\n", CheatBuffer);
		//OutputDebugString(buf2);
		//ok, now we can search them for cheats
		if(lstrcmp((CheatBuffer+20)-5, "alpha")==0){
			pfPlayField.DoubleScore();
		}
		if(lstrcmp((CheatBuffer+20)-5, "bravo")==0){
			pfPlayField.RandomizeField();
		}
		if(lstrcmp((CheatBuffer+20)-7, "charlie")==0){
			pfPlayField.TurnToEarth();
		}
		if(lstrcmp((CheatBuffer+20)-5, "delta")==0){
			pfPlayField.GravitizeAll();
		}
		if(lstrcmp((CheatBuffer+20)-4, "echo")==0){
			pfPlayField.InstantQuad();
		}
		if(lstrcmp((CheatBuffer+20)-7, "foxtrot")==0){
			pfPlayField.GiveAllLines();
		}
		if(lstrcmp((CheatBuffer+20)-4, "golf")==0){
			pfPlayField.GiveAllEarth();
		}
		if(lstrcmp((CheatBuffer+20)-5, "hotel")==0){
			pfPlayField.NextGravity();
		}
		if(lstrcmp((CheatBuffer+20)-5, "india")==0){
			pfPlayField.NextDestruct();
		}
		if(lstrcmp((CheatBuffer+20)-6, "juliet")==0){
			PlayWaveFile("sfx\\cheat.wav");
			pfPlayField.Stats.Cheats++;
			pfPlayField.ClearField();
		}

		break;
	case WM_KILLFOCUS:
	case WM_CLOSE:
		Pass.ProgramFlow=PF_EXIT;
		break;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Frame()
{
LRESULT Result;
//char Key;
char DIBuffer[256];
//read from the keyboard

Result=Pass.DIKeyboard->GetDeviceState(sizeof(DIBuffer),(LPVOID)DIBuffer);

if(pfPlayField.GamePaused==false){
	if((pfPlayField.Frame%RIGHT_FRAMES==0) && (KEYDOWN(DIBuffer, toOptions.Key_right))){
		pfPlayField.MoveRight();
  	}
	if((pfPlayField.Frame%LEFT_FRAMES==0) && (KEYDOWN(DIBuffer, toOptions.Key_left))){
		pfPlayField.MoveLeft();
	}
	if((pfPlayField.Frame%ROTATE_FRAMES==0) && (KEYDOWN(DIBuffer, toOptions.Key_rotatecw))){
		pfPlayField.Clockwise();
	}
	if((pfPlayField.Frame%ROTATE_FRAMES==0) && (KEYDOWN(DIBuffer, toOptions.Key_rotateccw))){
		pfPlayField.CounterClockwise();
	}
	if((pfPlayField.Frame%DROP_FRAMES==0) && (KEYDOWN(DIBuffer, toOptions.Key_drop))){
		pfPlayField.Drop();
	}
}
if((pfPlayField.Frame%PAUSE_FRAMES==0) && (KEYDOWN(DIBuffer, DIK_ESCAPE))){
	pfPlayField.Pause();
}

if(Pass.ProgramFlow==PF_EXIT) return 0;
if(pfPlayField.AdvanceFrame()){
	GameOverMain();
	Blit(PlayField, Pass.DDFront, 0, 0, 640, 480, 0, 0);
	FadeOut(Pass.DDObject, &Pass.DDFront, &PlayField, 0);
	return 0;
}

Flip(Pass.DDFront);
return 1;
}
