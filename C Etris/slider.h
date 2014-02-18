//slider.h

#ifndef __SLIDER
#define __SLIDER

#include "settings.h"//IsInside();
#include "ddrawlib.h"
#include "passvars.h"

class SLIDER{
public:
	SLIDER();
	~SLIDER();

	int Initialize(LPDIRECTDRAW * lpDD, LPDIRECTDRAWSURFACE* ddSurface, 
					int left, int top, int right, int bottom);
	int SliderProc(PASSPROCVARS Params);
	int Update();

	//properties:
	RECT Bounds;
	int MinValue;
	int MaxValue;
	int Value;
	bool Visible;
	bool Initialized;


private:
	LPDIRECTDRAW * DDObject;
	LPDIRECTDRAWSURFACE * Surface;
	LPDIRECTDRAWSURFACE Button;//the image of the button that moves along the slider( | )
	LPDIRECTDRAWSURFACE Slider;//the image of the actual slider. (-------------)
};



#endif