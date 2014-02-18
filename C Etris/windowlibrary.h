/*This file is a library file that has routines to ease the handling of windows.
(it basically takes repeated window tasks and simplify them (duh, what else does a library do?)

The functions are as follows:

MakeAWindow()
---------------------------------------------------------------------------------------------------------------------------
Fills the WNDCLASS class data (wc.style, wc.clsextra...),
Registers the class (RegisterClass()),
Creates the window (CreateWindow),
Shows the window (ShowWindow).


---------------------------------------------------------------------------------------------------------------------------
*/

#ifndef __WINDOWLIBRARY
#define __WINDOWLIBRARY

#include <windows.h>

extern HWND MakeAWindow(WNDPROC WindowProc, HINSTANCE hInstance, LPCTSTR ClassName);
#endif
