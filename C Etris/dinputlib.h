//
#ifndef __DINPUTLIB
#define __DINPUTLIB


#include "dinput.h"


LRESULT SetupDIObject(LPDIRECTINPUT & Object, HINSTANCE hInstance);
LRESULT SetupDIDevice(LPDIRECTINPUT& Object, LPDIRECTINPUTDEVICE& Device, HWND hWnd);
LRESULT Acquire(LPDIRECTINPUTDEVICE & Device);
LRESULT ReadKeyboard(char far * Buffer, LPDIRECTINPUTDEVICE & Device);

#endif
