#ifndef __SETTINGS
#define __SETTINGS

#include "passvars.h"
#include "commandbutton.h"
#include "ddutil.h"
#include "displayhighscores.h"
#include "options.h"
#include "credits.h"

int SettingsMain();
int SettingsWindowProc(PASSPROCVARS Params);

int cbOptionsProc(PASSPROCVARS Params);
int cbHighScoresProc(PASSPROCVARS Params);
int cbPlayProc(PASSPROCVARS Params);
int cbExitProc(PASSPROCVARS Params);
int cbCreditsProc(PASSPROCVARS Params);


bool IsInside(RECT Rectangle, POINT Point);
RECT GetRect(int left, int top, int right, int bottom);

#endif