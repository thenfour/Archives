//snookievars.h

#ifndef SNOOKIEVARS_H
#define SNOOKIEVARS_H


#include <windows.h>

struct SNOOKIEVARS{
public:
	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	HWND hSnookieWindow;
	char CDRomPath[MAX_PATH];
	HACCEL hAccelerators;
private:
};


#endif