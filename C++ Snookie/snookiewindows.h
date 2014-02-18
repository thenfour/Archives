//snookiewindows.h

#ifndef SNOOKIEWINDOWS_H
#define SNOOKIEWINDOWS_H

#include <windows.h>
#include "controls.h"
#include "snookievars.h"

#include "editbox.h"
#include "list.h"
#include "imagelist.h"
#include "static.h"
#include "statusbar.h"
#include "treeview.h"
#include "slider.h"

class SNOOKIEWINDOWS{
public:
	SNOOKIEWINDOWS();

	SLIDER VSlider;
	SLIDER HSlider;

	HWND MainWindow;
	TREEVIEW TreeView;
	STATUSBAR StatusBar;
	STATIC StaticText_Name;
	STATIC StaticText_Size;
	STATIC StaticText_OtherInfo;
	EDITBOX EditBox;
	LIST List;

	HWND DebugWindow;
	EDITBOX DebugEdit;
private:
};

#endif