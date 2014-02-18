//
#ifndef __ANIMATION
#define __ANIMATION

#include "ddraw.h"
#include "passvars.h"

//This file includes the prototype of the class ANIMATION
//ANIMATION holds information about the animation's speed,
//the DirectDraw surfae on which its frames are, the number of frames,
//etc...


//class ANIMATION represents a surface and the number of frames/speed of a particular animation
class ANIMATION
{
   public:
		ANIMATION(LPDIRECTDRAWSURFACE& surface, LPCTSTR bitmapname, int frames, int speed);
      ~ANIMATION();

   	int FrameCount;
      int Speed;
      LPDIRECTDRAWSURFACE* DDSurface;
};

struct PASSANIMS
{
	LPDIRECTDRAWSURFACE* sFire;
	LPDIRECTDRAWSURFACE* sEarth;
	LPDIRECTDRAWSURFACE* sWater;
	LPDIRECTDRAWSURFACE* sIce;
	LPDIRECTDRAWSURFACE* sGravity;
	LPDIRECTDRAWSURFACE* sDestruct;
	LPDIRECTDRAWSURFACE* sFlammable;
	ANIMATION* aFire;
	ANIMATION* aEarth;
	ANIMATION* aWater;
	ANIMATION* aIce;
	ANIMATION* aGravity;
	ANIMATION* aDestruct;
	ANIMATION* aFlammable;
};

#endif

