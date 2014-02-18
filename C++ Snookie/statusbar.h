//statusbar.h

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <windows.h>
#include <commctrl.h>
#include <string.h>
#include "controls.h"

#define STATUSBAR_OK		CONTROLS_OK
#define STATUSBAR_ERROR		CONTROLS_ERROR

//////////////////////////////////////////////////////////////////////////////////////////
struct STATUSBARPART{
	int WidthPercent;
	char Text[128];
};

//////////////////////////////////////////////////////////////////////////////////////////
class STATUSBAR : public CONTROLS{
public:
	STATUSBAR();
	~STATUSBAR();
	int Create(HWND parent);
	int SetParts(int nParts, STATUSBARPART* parts);
	int SetText(int nPart, char* Text);
	int GetText(int nPart, char* Text);
	int Size();

private:
	int PercentToClient(int nParts, int* client, int*percent);
	int WidthPercentage[256];
};

#endif