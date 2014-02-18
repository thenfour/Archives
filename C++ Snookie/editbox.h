//editbox.h

#ifndef EDITBOX_H
#define EDITBOX_H

#include <windows.h>
#include "controls.h"

#define EDITBOX_OK CONTROLS_OK
#define EDITBOX_ERROR CONTROLS_ERROR

//////////////////////////////////////////////////////////////////////////////////////////////
class EDITBOX : public CONTROLS{
public:
	EDITBOX();
	~EDITBOX();
	
	int CreateEditBox(HWND Parent, RECT prect, RECT orect, HINSTANCE hInstance);
	int GetText(char * buf, int max);
	int SetText(char * buf);
};

#endif