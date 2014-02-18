//snookie.h
#ifndef SNOOKIE_H
#define SNOOKIE_H

#include <windows.h>
#include "global.h"
#include "resource.h"
#include <commctrl.h>
#include "snookiewindowproc.h"
#include "snookievars.h"
#include "snookiewindows.h"
#include "snookiecatalogv3.h"
#include "controls.h"

#include <stdio.h>

HRESULT SetupWindow(SNOOKIEVARS* SnookieVars, int nCmdShow);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);


#define SNOOKIE_OK		0
#define SNOOKIE_ERROR	1

#endif