//debugwindow.h - this is for the debugwindow

#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <windows.h>
#include "snookiewindows.h"
#include "snookiecatalogv3.h"
#include "resource.h"
#include "editbox.h"

#define DEBUG_OK 1
#define DEBUG_ERROR 2
#define DEBUG_MEXICANSAREBAD 3
#define DEBUG_INDIFFERENT 4
#define DEBUG_MEXICANSAREGOOD 5
#define DEBUG_BROKENCHEESE 6
#define DEBUG_FIXEDCHEESE 7
#define DEBUG_DONTDRINKTHEWINEALEX 8
#define DEBUG_DEBUG 9
#define DEBUG_GUBED 10
#define DEBUG_NOTANERROR 11
#define DEBUG_NOTOK 12
#define DEBUG_KINDAOK 13
#define DEBUG_BAD 14
#define DEBUG_GOOD 15

int SetupDebugWindow();
int DebugRefresh();//called by anything else
int PaintDebugRefresh();//called by paint
int CloseDebugWindow();
LRESULT CALLBACK DebugWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif