#include "settings.h"
#include "stdio.h"

void settingsRedrawScreen();

#define BMP_SETTINGSH "gfx\\24settingsh.bmp"
#define BMP_SETTINGS "gfx\\24settings.bmp"

extern PASSVARS Pass;

LPDIRECTDRAWSURFACE ScreenUnhighlighted;
LPDIRECTDRAWSURFACE ScreenHighlighted;

COMMANDBUTTON cbOptions;
COMMANDBUTTON cbHighScores;
COMMANDBUTTON cbPlay;
COMMANDBUTTON cbExit;
COMMANDBUTTON cbCredits;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int SettingsMain(){
Pass.SettingsInfo=SETTINGS_NOTHING;
Pass.ProgramFlow=PF_SETTINGS;
MSG Msg;
LRESULT Result;

ScreenHighlighted=  DDLoadBitmap(Pass.DDObject,BMP_SETTINGSH, 0, 0);
ScreenUnhighlighted=DDLoadBitmap(Pass.DDObject,BMP_SETTINGS, 0, 0);

cbOptions.Initialize(&ScreenHighlighted, &ScreenUnhighlighted, &Pass.DDBack, GetRect(   340,236,557,309), cbOptionsProc);
cbHighScores.Initialize(&ScreenHighlighted, &ScreenUnhighlighted, &Pass.DDBack, GetRect(129,134,431,215), cbHighScoresProc);
cbPlay.Initialize(&ScreenHighlighted, &ScreenUnhighlighted, &Pass.DDBack, GetRect(      465,343,600,422), cbPlayProc);
cbExit.Initialize(&ScreenHighlighted, &ScreenUnhighlighted, &Pass.DDBack, GetRect(        6, 72,129,142), cbExitProc);
cbCredits.Initialize(&ScreenHighlighted, &ScreenUnhighlighted, &Pass.DDBack, GetRect(     6,341,215,422), cbCreditsProc);



Result=FadeIn(Pass.DDObject, &Pass.DDFront,  &ScreenUnhighlighted, 0);
settingsRedrawScreen();

Result=Blit(Pass.DDBack, ScreenUnhighlighted, 0,0,640,480,0,0);
//message loop now.
while(Pass.SettingsInfo==SETTINGS_NOTHING){
if(!GetMessage(&Msg, NULL, 0, 0)){
		Pass.SettingsInfo=SETTINGS_EXIT;
		Pass.ProgramFlow=PF_EXIT;
   }
   TranslateMessage(&Msg);
   DispatchMessage(&Msg);
}

//destroy the surfaces
if(ScreenHighlighted!=NULL){
	ScreenHighlighted->Release();
	ScreenHighlighted=NULL;
	ScreenUnhighlighted->Release();
	ScreenUnhighlighted=NULL;
}

return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int SettingsWindowProc(PASSPROCVARS Params){
	int r=0;
	r+=cbHighScores.ProcFunction(Params);
	r+=cbPlay.ProcFunction(Params);
	r+=cbExit.ProcFunction(Params);
	r+=cbOptions.ProcFunction(Params);
	r+=cbCredits.ProcFunction(Params);

	if(r){
//DrawString("Setting up alpha table/@", 0, 10, 8);
//DrawString("Setting up alpha table/@", 0, 20, 12);
//DrawString("Setting up alpha table/@", 0, 35, 13);
//DrawString("Setting up alpha table/@", 0, 50, 21);
		Flip(Pass.DDFront);
		Blit(Pass.DDBack, ScreenUnhighlighted, 0,0,640,480,0,0);
	}
	switch(Params.uMsg){
	case WM_PAINT:
		settingsRedrawScreen();
		break;
	case WM_CLOSE:
		Pass.SettingsInfo=SETTINGS_EXIT;
		Pass.ProgramFlow=PF_EXIT;
		break;
	}

return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
These Proc functions take in the parameters of a normal WindowProc function and they process them.
Really they don't do anything unless the mouse is over the button, and then they either just display
the highlighted portion of surface or do the action associated with it.

In the case of EXIT or PLAY, SettingsMAin should return with a value that Etetriz.cpp will understand
and then it will proceed accordingly.

*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cbOptionsProc(PASSPROCVARS Params){
	//cbsound
	FadeOut(Pass.DDObject, &Pass.DDFront, &ScreenUnhighlighted, 0);
	OptionsMain();
	if(Pass.ProgramFlow==PF_EXIT) Pass.SettingsInfo=SETTINGS_EXIT;
	else FadeIn(Pass.DDObject, &Pass.DDFront, &ScreenUnhighlighted, 0);
	Blit(Pass.DDBack, ScreenUnhighlighted, 0,0,640,480,0,0);
	settingsRedrawScreen();
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cbHighScoresProc(PASSPROCVARS Params){
	//cbKeys
	FadeOut(Pass.DDObject, &Pass.DDFront, &ScreenUnhighlighted, 0);
	DisplayHighScoresMain();
	if(Pass.ProgramFlow==PF_EXIT) Pass.SettingsInfo=SETTINGS_EXIT;
	else FadeIn(Pass.DDObject, &Pass.DDFront,&ScreenUnhighlighted, 0);
	settingsRedrawScreen();
return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cbPlayProc(PASSPROCVARS Params){
//cbPlay
FadeOut(Pass.DDObject, &Pass.DDFront, &ScreenUnhighlighted, 0);
Pass.SettingsInfo=SETTINGS_PLAY;
return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cbExitProc(PASSPROCVARS Params){
//cbExit
FadeOut(Pass.DDObject, &Pass.DDFront,&ScreenUnhighlighted, 0);
Pass.SettingsInfo=SETTINGS_EXIT;
Pass.ProgramFlow=PF_EXIT;
return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cbCreditsProc(PASSPROCVARS Params){
	//cbCredits
	FadeOut(Pass.DDObject, &Pass.DDFront, &ScreenUnhighlighted, 0);
	CreditsMain();
	if(Pass.ProgramFlow==PF_EXIT) Pass.SettingsInfo=SETTINGS_EXIT;
	else FadeIn(Pass.DDObject, &Pass.DDFront, &ScreenUnhighlighted, 0);
	settingsRedrawScreen();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RECT GetRect(int left, int top, int right, int bottom){
RECT TempRect;
TempRect.left=left;
TempRect.right=right;
TempRect.bottom=bottom;
TempRect.top=top;
return TempRect;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsInside(RECT Rectangle, POINT Point){
bool ReturnValue=false;
//this function returns true if the point is inside the rectangle
//point must be within both y and x coords
if(Point.x < Rectangle.right){
	if(Point.x > Rectangle.left){
   	if(Point.y < Rectangle.bottom){
      	if(Point.y > Rectangle.top){
         	//it's inside
            ReturnValue=true;
         }
      }
   }
}
return ReturnValue;//go figure
}

void settingsRedrawScreen(){
	ScreenHighlighted->Restore();
	ScreenUnhighlighted->Restore();
	DDReLoadBitmap(ScreenHighlighted,BMP_SETTINGSH);
	DDReLoadBitmap(ScreenUnhighlighted,BMP_SETTINGS);
	DrawString("Setting up alpha table/@", 0, 0, 8);
	Blit(Pass.DDFront, ScreenUnhighlighted, 0, 0, 640, 480, 0, 0);
	Blit(Pass.DDBack, ScreenUnhighlighted, 0, 0, 640, 480, 0, 0);
}