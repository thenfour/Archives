#include "animation.h"
#include "ddutil.h"

extern PASSVARS Pass;

ANIMATION::~ANIMATION()
{
}

ANIMATION::ANIMATION(LPDIRECTDRAWSURFACE& surface,
							LPCTSTR bitmapname, int frames, int speed)
{
	DDSurface=&surface;//Specify the surface
   *DDSurface=DDLoadBitmap(Pass.DDObject, bitmapname, 0, 0);//load in the frames image
	FrameCount=frames;//Number of frames in the animation until it loops
   Speed=speed;//The speed could be described technically as:
   				//GameFrames until AnimationFrame advance.  So a 1 here means that for
               //every 1 game frame, the animation frame will also advance.
               //a 2 here would mean that for every 2 game frames...(making it twice
               //as slow), etc....

//Now set the color-key for the surface to create a transparency.
//this sets the color key to whatever the first pixel is in teh image.
    DDSetColorKey(*DDSurface, CLR_INVALID);

}

