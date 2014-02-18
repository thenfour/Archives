#ifndef __DDRAWLIB
#define __DDRAWLIB


#include "ddraw.h"


struct RGB16{
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
    RGBQUAD Positions;
};

int		IniRGB16 (LPDIRECTDRAWSURFACE* Surface);
void	Pset(int x, int y, int  r, int  g, int  b, DDSURFACEDESC *ddsd);
void	Pget(int x, int y, int& r, int& g, int& b, DDSURFACEDESC *ddsd);
void	GetRGB(WORD Pixel,int &r, int &g, int&b);
WORD	GetColor(int r, int g, int b);
void	InitAlphaTable();
//int		FadeIn(LPDIRECTDRAWSURFACE * Front, LPDIRECTDRAWSURFACE *Back, LPDIRECTDRAWSURFACE *Image,
//			 int Speed);
//int		FadeOut(LPDIRECTDRAWSURFACE * Front, LPDIRECTDRAWSURFACE *Back, LPDIRECTDRAWSURFACE *Image,
//			 int Speed);


LPDIRECTDRAWSURFACE DDCreateSurface(LPDIRECTDRAW lpDD, int width, int height, int mem_flags);

int FadeOut(LPDIRECTDRAW lpDD, LPDIRECTDRAWSURFACE * Front, LPDIRECTDRAWSURFACE *Image,
			 int Speed);
int FadeIn(LPDIRECTDRAW lpDD, LPDIRECTDRAWSURFACE * Front, LPDIRECTDRAWSURFACE *Image,
			 int Speed);




LRESULT	SetupDDObject(LPDIRECTDRAW& Object, HWND hWnd);
LRESULT	SetupPriSurface(LPDIRECTDRAW& DDobject, LPDIRECTDRAWSURFACE& DDsurface);
LRESULT	SetupOtherSurface(LPDIRECTDRAW& DDobject, LPDIRECTDRAWSURFACE& DDsurface);
LRESULT	SetupBackSurface(LPDIRECTDRAW& DDobject, LPDIRECTDRAWSURFACE& DDpri,
	                      LPDIRECTDRAWSURFACE& DDback);
LRESULT	Flip(LPDIRECTDRAWSURFACE& DDsurface);
LRESULT	Blit(LPDIRECTDRAWSURFACE& dDDsurface, LPDIRECTDRAWSURFACE& sDDsurface,
								 int sLeft, int sTop, int sRight, int sBottom,
                         int dLeft, int dTop);
LRESULT CKBlit(LPDIRECTDRAWSURFACE& dDDsurface, LPDIRECTDRAWSURFACE& sDDsurface,
								 int sLeft, int sTop, int sRight, int sBottom,
                         int dLeft, int dTop);
LRESULT MakeOffScreenSurface(LPDIRECTDRAW& DDObject, LPDIRECTDRAWSURFACE& OSSurface,
								 int x, int y);
LPDIRECTDRAWPALETTE  BlackScreen8(LPDIRECTDRAW* DDObject, LPDIRECTDRAWSURFACE * Surface);
void                 FadeIn8(LPDIRECTDRAW * DDObject, LPDIRECTDRAWSURFACE * Front, IDirectDrawPalette * Palette);
IDirectDrawPalette * FadeOut8(LPDIRECTDRAW * DDObject, LPDIRECTDRAWSURFACE * Front);
LRESULT Rectangle(RECT rect, int r, int g, int b, bool Filled);
LRESULT Line(RECT rect, int r, int g, int b);
LRESULT HLine(int x1, int x2, int y, int r, int g, int b);
LRESULT VLine(int y1, int y2, int x, int r, int g, int b);
LRESULT Circle(int x, int y, int radius, int r, int g, int b);
LRESULT Triangle(POINT p1, POINT p2, POINT p3, int r, int g, int b);
#endif
