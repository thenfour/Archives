//this is the main file for tetriz; it contains the WinMAin function.

#ifndef __ETETRIZ
#define __ETETRIZ

#include "highscores.h"
#include "passvars.h"
#include "ddraw.h"
#include "pause.h"
#include "game.h"
#include "displayhighscores.h"
#include "settings.h"
#include "options.h"

#include "windowlibrary.h"
#include "dsoundlib.h"
#include "dinputlib.h"
#include "ddrawlib.h"
#include "dswavlib.h"
#include "textlib.h"
#include "textfilelib.h"
#include "texteditbox.h"
#include "time.h"

extern int WINAPI WinMain(HINSTANCE  hInstance, HINSTANCE  hPrevInstance, LPSTR  lpszCmdLine, int  nCmdShow);
extern LRESULT CALLBACK MainWindowProc(HWND  hwnd, UINT  uMsg, WPARAM  wParam, LPARAM  lParam);
extern void StrikeTheSet();
extern int Error(HWND hwnd);
extern int Error2(HWND hwnd, LPCTSTR string);

#endif
