//static.h

#ifndef STATIC_H
#define STATIC_H

#include <windows.h>
#include "controls.h"

#define STATIC_OK CONTROLS_OK
#define STATIC_ERROR CONTROLS_ERROR

/////////////////////////////////////////////////////////////////////////////////////
class STATIC : public CONTROLS{
public:
	STATIC();
	~STATIC();

	int CreateStaticText(HWND Parent, HINSTANCE hInstance, int id, RECT prect, RECT orect);
	int SetText(char * Text);
	int SetFont(char* Font, int Size);
};

#endif