#ifndef __DISPLAYHIGHSCORES
#define __DISPLAYHIGHSCORES

#include "passvars.h"
#include "ddrawlib.h"
#include "ddutil.h"
#include "textlib.h"


void DisplayHighScoresMain();
int DisplayHighScoresProc(PASSPROCVARS Params);
void dhsUpdateScreen();

int cbSortScoreProc(PASSPROCVARS Params);
int cbSortNameProc(PASSPROCVARS Params);
int cbSortDateProc(PASSPROCVARS Params);
int cbSortEffectsProc(PASSPROCVARS Params);
int cbSortLinesProc(PASSPROCVARS Params);
int cbSortCheatsProc(PASSPROCVARS Params);

int dhscbOKProc(PASSPROCVARS Params);

#ifndef __DISPLAYHIGHSCORES_VARS1
	//this is for the parameters for this screen about the displaying information
	struct DHSPARAMS{
		int a;
	};
#define __DISPLAYHIGHSCORES_VARS1
#endif

#define DHS_NOTHING		0
#define DHS_DONE		1



#endif