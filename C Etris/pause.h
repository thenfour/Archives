#ifndef __PAUSE
#define __PAUSE

#include "playfield.h"
#include "ddutil.h"
#include "textlib.h"
#include "commandbutton.h"

void pauseRestore();
int PauseInit();
int PauseFrame(PLAYFIELD * PlayField);
int PauseExitProc(PASSPROCVARS Params);
int PauseContinueProc(PASSPROCVARS Params);
void PauseProc(PASSPROCVARS Params);


#endif