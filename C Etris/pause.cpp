
#define PAUSED_LINES_X			270
#define PAUSED_LINES_Y			162
#define PAUSED_EFFECTS_X		270
#define PAUSED_EFFECTS_Y		125
#define PAUSED_SINGLES_X		270
#define PAUSED_SINGLES_Y		183
#define PAUSED_DOUBLES_X		270
#define PAUSED_DOUBLES_Y		201
#define PAUSED_TRIPLES_X		270
#define PAUSED_TRIPLES_Y		221
#define PAUSED_QUADS_X			270
#define PAUSED_QUADS_Y			240

#define PAUSED_SCORE_X			270
#define PAUSED_SCORE_Y			106
#define PAUSED_QUINTS_X			270
#define PAUSED_QUINTS_Y			259

#define PAUSED_SINGLESIAR_X			
#define PAUSED_SINGLESIAR_Y			
#define PAUSED_DOUBLESIAR_X			
#define PAUSED_DOUBLESIAR_Y			
#define PAUSED_TRIPLESIAR_X			
#define PAUSED_TRIPLESIAR_Y			
#define PAUSED_QUADSIAR_X			
#define PAUSED_QUADSIAR_Y			

#define PAUSED_EARTH_X			270
#define PAUSED_EARTH_Y			334
#define PAUSED_FIRE_X			270
#define PAUSED_FIRE_Y			296
#define PAUSED_DESTRUCT_X 		270
#define PAUSED_DESTRUCT_Y 		315
#define PAUSED_FLAMMABLE_X		270
#define PAUSED_FLAMMABLE_Y		353
#define PAUSED_ICE_X			270
#define PAUSED_ICE_Y			373
#define PAUSED_WATER_X			270
#define PAUSED_WATER_Y			392
#define PAUSED_GRAVITY_X		270
#define PAUSED_GRAVITY_Y		411

#define PAUSED_DROPS_X  		
#define PAUSED_DROPS_Y			

#define PAUSED_BLOCK1_X			504
#define PAUSED_BLOCK1_Y			146
#define PAUSED_BLOCK2_X			504
#define PAUSED_BLOCK2_Y			394
#define PAUSED_BLOCK3_X			504
#define PAUSED_BLOCK3_Y			300
#define PAUSED_BLOCK4_X			504
#define PAUSED_BLOCK4_Y			212
#define PAUSED_BLOCK5_X			391
#define PAUSED_BLOCK5_Y			157
#define PAUSED_BLOCK6_X			391
#define PAUSED_BLOCK6_Y			297
#define PAUSED_BLOCK7_X			391
#define PAUSED_BLOCK7_Y			390

#define PAUSED_PAUSED_X			
#define PAUSED_PAUSED_Y			

#include "pause.h"
#define BMP_PAUSEH "gfx\\24pauseh.bmp"
#define BMP_PAUSE "gfx\\24pause.bmp"

extern PASSVARS Pass;

LPDIRECTDRAWSURFACE PauseHighlighted;
LPDIRECTDRAWSURFACE PauseUnhighlighted;

COMMANDBUTTON cbPauseExit;
COMMANDBUTTON cbPauseContinue;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PauseInit(){
//this is called when we start the game and only once...
//it just sets up the graphics for the pause screen and the command button.
//really, just the command button; everything else is already handled by the frame.
PauseHighlighted=DDLoadBitmap(Pass.DDObject, BMP_PAUSEH, 0, 0);
PauseUnhighlighted=DDLoadBitmap(Pass.DDObject, BMP_PAUSE,  0, 0);
cbPauseExit.Initialize(&PauseHighlighted, &PauseUnhighlighted, &Pass.DDBack, GetRect(521,121,640,202), PauseExitProc);
cbPauseContinue.Initialize(&PauseHighlighted, &PauseUnhighlighted, &Pass.DDBack, GetRect(421,34,640,113), PauseContinueProc);
return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PauseFrame(PLAYFIELD* PlayField){
//this is each frame of the pause screen.

#define PAUSE_TEXTSIZE 12
DrawNumber(PlayField->Stats.Score,     PAUSED_SCORE_X,      PAUSED_SCORE_Y       ,PAUSE_TEXTSIZE);

DrawNumber(PlayField->Stats.Lines,     PAUSED_LINES_X,      PAUSED_LINES_Y       ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Effects,   PAUSED_EFFECTS_X,    PAUSED_EFFECTS_Y     ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Singles,   PAUSED_SINGLES_X,    PAUSED_SINGLES_Y     ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Doubles,   PAUSED_DOUBLES_X,    PAUSED_DOUBLES_Y     ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Triples,   PAUSED_TRIPLES_X,    PAUSED_TRIPLES_Y     ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Quads,     PAUSED_QUADS_X,      PAUSED_QUADS_Y       ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.MoreThanQuads,     PAUSED_QUINTS_X,      PAUSED_QUINTS_Y       ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Earth,     PAUSED_EARTH_X,      PAUSED_EARTH_Y       ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Fire,      PAUSED_FIRE_X,       PAUSED_FIRE_Y        ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Destruct,  PAUSED_DESTRUCT_X,   PAUSED_DESTRUCT_Y    ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Flammable, PAUSED_FLAMMABLE_X,  PAUSED_FLAMMABLE_Y   ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Ice,       PAUSED_ICE_X,        PAUSED_ICE_Y         ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Water,     PAUSED_WATER_X,      PAUSED_WATER_Y       ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Gravity,   PAUSED_GRAVITY_X,    PAUSED_GRAVITY_Y     ,PAUSE_TEXTSIZE);

DrawNumber(PlayField->Stats.Block1,    PAUSED_BLOCK1_X,     PAUSED_BLOCK1_Y      ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Block2,    PAUSED_BLOCK2_X,     PAUSED_BLOCK2_Y      ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Block3,    PAUSED_BLOCK3_X,     PAUSED_BLOCK3_Y      ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Block4,    PAUSED_BLOCK4_X,     PAUSED_BLOCK4_Y      ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Block5,    PAUSED_BLOCK5_X,     PAUSED_BLOCK5_Y      ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Block6,    PAUSED_BLOCK6_X,     PAUSED_BLOCK6_Y      ,PAUSE_TEXTSIZE);
DrawNumber(PlayField->Stats.Block7,    PAUSED_BLOCK7_X,     PAUSED_BLOCK7_Y      ,PAUSE_TEXTSIZE);
return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PauseExitProc(PASSPROCVARS Params){
//Called when they hit the Exit button
Pass.ProgramFlow=PF_GAMEEXITING;
return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PauseContinueProc(PASSPROCVARS Params){
//called when they hit Continue
			ShowCursor(TRUE);

Pass.ProgramFlow=PF_GAME;
return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PauseProc(PASSPROCVARS Params)
{
//this is called when Pass.ProgramFlow==PF_PAUSE...it handles the command buttons
	int r=0;
	r+=cbPauseExit.ProcFunction(Params);
	r+=cbPauseContinue.ProcFunction(Params);
	if(r){
		Flip(Pass.DDFront);
		Blit(Pass.DDBack, Pass.DDFront, 0, 0, 640, 480, 0, 0);
	}
	switch(Params.uMsg){
	case WM_CLOSE:
	case WM_KILLFOCUS:
		Pass.ProgramFlow=PF_EXIT;
		break;
	}

	return;
}

void pauseRestore(){
	PauseHighlighted->Restore();
	PauseUnhighlighted->Restore();
	DDReLoadBitmap(PauseHighlighted, BMP_PAUSEH);
	DDReLoadBitmap(PauseUnhighlighted, BMP_PAUSE);
}