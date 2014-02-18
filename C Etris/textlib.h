#ifndef __TEXTLIB
#define __TEXTLIB

#include <windows.h>
#include "passvars.h"
#include "ddrawlib.h"

#define BMP_FONT8 "gfx\\24font8.bmp"
#define BMP_FONT22 "gfx\\24font22.bmp"
#define BMP_FONT12 "gfx\\24font12.bmp"
#define BMP_FONT13 "gfx\\24font13.bmp"

void RestoreFonts();
void DrawLetter(char Character, int X, int Y, int Size);
void DrawString(LPCSTR String, int X, int Y, int Size);
void DrawNumber(long Num,int X, int Y, int Size);

#endif