#include "displayhighscores.h"
#include "commandbutton.h"
#include <stdio.h>
#include <time.h>

extern PASSVARS Pass;
extern HIGHSCORES HighScores;

#define BMP_HIGHSCORES "gfx\\24highscores.bmp"
#define BMP_HIGHSCORESH "gfx\\24highscoresh.bmp"

int dhsInfo;
LPDIRECTDRAWSURFACE sHighScores;
LPDIRECTDRAWSURFACE sHighScoresh;
COMMANDBUTTON cbSortScore;
COMMANDBUTTON cbSortDate;
COMMANDBUTTON cbSortName;
COMMANDBUTTON cbSortLines;
COMMANDBUTTON cbSortEffects;
COMMANDBUTTON cbSortCheats;

COMMANDBUTTON dhscbOK;

////////////////////////////////////////////////////////////////////////////////////////////////
void DisplayHighScoresMain(){
	dhsInfo=DHS_NOTHING;
//	LRESULT Result;
	MSG Msg;
	//save the current program flow so we can set it back later.
	int OldPF=Pass.ProgramFlow;
	Pass.ProgramFlow=PF_DISPLAYHIGHSCORES;//and set it now to make sure windows' messages are handled int DisplayHighScoresProc
	//

	sHighScores=DDLoadBitmap(Pass.DDObject, BMP_HIGHSCORES,0,0);
	sHighScoresh=DDLoadBitmap(Pass.DDObject, BMP_HIGHSCORESH,0,0);

	//setup the commandbuttons
	RECT rect;
	SetRect(&rect, 173, 11, 248, 64);//scores
	cbSortScore.Initialize(&sHighScoresh, &sHighScores, &Pass.DDBack, rect, cbSortScoreProc);
	SetRect(&rect, 5, 11, 80, 64);//name
	cbSortName.Initialize(&sHighScoresh, &sHighScores, &Pass.DDBack, rect, cbSortNameProc);
	SetRect(&rect, 266, 11, 341, 64);//date
	cbSortDate.Initialize(&sHighScoresh, &sHighScores, &Pass.DDBack, rect, cbSortDateProc);
	SetRect(&rect, 0, 399, 127, 480);//OK
	dhscbOK.Initialize(&sHighScoresh, &sHighScores, &Pass.DDBack, rect, dhscbOKProc);
	SetRect(&rect, 571, 11, 640, 64);//lines
	cbSortLines.Initialize(&sHighScoresh, &sHighScores, &Pass.DDBack, rect, cbSortLinesProc);
	SetRect(&rect, 395, 11, 491, 64);//effects
	cbSortEffects.Initialize(&sHighScoresh, &sHighScores, &Pass.DDBack, rect, cbSortEffectsProc);
	SetRect(&rect, 490, 11, 571, 64);//cheats
	cbSortCheats.Initialize(&sHighScoresh, &sHighScores, &Pass.DDBack, rect, cbSortCheatsProc);

	//display the screen and start the message loop;
	SortHighScores(SHS_NAME);
	FadeIn(Pass.DDObject, &Pass.DDFront, &sHighScores,0);
	Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	dhsUpdateScreen();
	Flip(Pass.DDFront);
	dhsUpdateScreen();
	while(dhsInfo!=DHS_DONE){
		if(!GetMessage(&Msg, NULL, 0, 0)){
			//trying to quit
   			Pass.ProgramFlow=PF_EXIT;
	   		dhsInfo=DHS_DONE;
		}
		else{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	//reset the screen back to normal and the program flow back to waht it was.
	FadeOut(Pass.DDObject, &Pass.DDFront, &sHighScores,0);

	SortHighScores(SHS_SCORE);
	if(Pass.ProgramFlow!=PF_EXIT) Pass.ProgramFlow=OldPF;//if the user tried to quit, ProgramFlow was changed to exit...
	//destroy the surfaces
	sHighScores->Release();
	sHighScores=NULL;
	sHighScoresh->Release();
	sHighScoresh=NULL;
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int DisplayHighScoresProc(PASSPROCVARS Params){
	int r=0;
	r+=cbSortScore.ProcFunction(Params);
	r+=cbSortEffects.ProcFunction(Params);
	r+=cbSortName.ProcFunction(Params);
	r+=cbSortCheats.ProcFunction(Params);
	r+=cbSortLines.ProcFunction(Params);
	r+=cbSortDate.ProcFunction(Params);
	r+=dhscbOK.ProcFunction(Params);
	if(r){
		//flip the screen.
		dhsUpdateScreen();
		Flip(Pass.DDFront);
		Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	}
switch(Params.uMsg){
	case WM_CLOSE:
		Pass.ProgramFlow=PF_EXIT;
		dhsInfo=DHS_DONE;
		break;

	case WM_PAINT:
		sHighScores->Restore();
		sHighScoresh->Restore();
		DDReLoadBitmap(sHighScores, BMP_HIGHSCORES);
		DDReLoadBitmap(sHighScoresh, BMP_HIGHSCORESH);
		dhsUpdateScreen();

		break;

	case WM_KEYDOWN:
   	switch(Params.wParam){
      	case VK_ESCAPE:
         	dhsInfo=DHS_DONE;
         break;
      }
	break;
}

return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void dhsUpdateScreen(){
//
#define DHS_SKIPX_NAME			20//pixels allocated X for each high score.
#define DHS_SKIPX_SCORE			230//
#define DHS_SKIPX_DATE			266//
#define DHS_SKIPX_EFFECTS		490//
#define DHS_SKIPX_CHEATS		551//
#define DHS_SKIPX_LINES			610//
#define DHS_SKIPY			10//same thing, only instead of for columns - rows.
#define DHS_MAXDISPLAY		HIGHSCORESCOUNT//maximum you can put on the screen
#define DHS_GRIDTOP			70//the place allocated on teh screen for displaying
#define DHS_GRIDLEFT		0//the same thing - only X, not Y.
char strdate[100];

for(int i=0;i<DHS_MAXDISPLAY;i++){
	if(HighScores.HighScore[i].Score!=0){
		//display highscore[i]
		DrawString(HighScores.HighScore[i].Name, DHS_GRIDLEFT, DHS_GRIDTOP+(DHS_SKIPY*i), 8);
		DrawNumber(HighScores.HighScore[i].Score, DHS_GRIDLEFT+DHS_SKIPX_SCORE,DHS_GRIDTOP+(DHS_SKIPY*i), 8);
		DrawNumber(HighScores.HighScore[i].Lines, DHS_GRIDLEFT+(DHS_SKIPX_LINES),DHS_GRIDTOP+(DHS_SKIPY*i), 8);
		sprintf(strdate, "%2d/%2d/%2d, %2d:%2d:%2d", 
			HighScores.HighScore[i].Year,
			HighScores.HighScore[i].Month,
			HighScores.HighScore[i].Day,

			HighScores.HighScore[i].Hour,
			HighScores.HighScore[i].Minute,
			HighScores.HighScore[i].Second
			);
		DrawString(strdate, DHS_GRIDLEFT+(DHS_SKIPX_DATE),DHS_GRIDTOP+(DHS_SKIPY*i), 8);
		DrawNumber(HighScores.HighScore[i].Effects, DHS_GRIDLEFT+(DHS_SKIPX_EFFECTS),DHS_GRIDTOP+(DHS_SKIPY*i), 8);
		DrawNumber(HighScores.HighScore[i].Cheats, DHS_GRIDLEFT+(DHS_SKIPX_CHEATS),DHS_GRIDTOP+(DHS_SKIPY*i), 8);
	}
}


}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cbSortScoreProc(PASSPROCVARS Params){
	SortHighScores(SHS_SCORE);
	Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	dhsUpdateScreen();
	Flip(Pass.DDFront);
	Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cbSortNameProc(PASSPROCVARS Params){
	SortHighScores(SHS_NAME);
	Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	dhsUpdateScreen();
	Flip(Pass.DDFront);
	Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cbSortLinesProc(PASSPROCVARS Params){
	SortHighScores(SHS_LINES);
	Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	dhsUpdateScreen();
	Flip(Pass.DDFront);
	Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cbSortEffectsProc(PASSPROCVARS Params){
	SortHighScores(SHS_EFFECTS);
	Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	dhsUpdateScreen();
	Flip(Pass.DDFront);
	Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cbSortDateProc(PASSPROCVARS Params){
	SortHighScores(SHS_DATE);
	Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	dhsUpdateScreen();
	Flip(Pass.DDFront);
	Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cbSortCheatsProc(PASSPROCVARS Params){
	SortHighScores(SHS_CHEATS);
	Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	dhsUpdateScreen();
	Flip(Pass.DDFront);
	Blit(Pass.DDBack, sHighScores, 0, 0, 640, 480, 0, 0);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int dhscbOKProc(PASSPROCVARS Params){
	dhsInfo=DHS_DONE;
	return 0;
}