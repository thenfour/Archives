//
#ifndef __GAME
#define __GAME

#include "passvars.h"
#include "gameover.h"
#include "animation.h"
#include "ddutil.h"
#include "playfield.h"
#include "ddrawlib.h"

int GameMain();
int GameWindowProc(PASSPROCVARS Params);
int Frame();
void GameLeave();
#endif
