//OnOff.h

#ifndef __ONOFF
#define __ONOFF

#include "ddrawlib.h"
#include "Settings.h"
#include "passvars.h"

class ONOFF{
public:
	ONOFF();
	~ONOFF();
	int Initialize(RECT rect, IDirectDrawSurface ** ddS, LPDIRECTDRAW * lpDD);
	int OnOffProc(PASSPROCVARS Params);
	int Update();

	//properties
	bool Value;
	RECT Bounds;//coords on screen
	RECT srect;//coords of the surface from which to blit. (size of the on/off images)
	bool Valid;//True is this is initialized.

	LPDIRECTDRAWSURFACE* dds;//the surface on which to write.
	LPDIRECTDRAWSURFACE ddsOn;
	LPDIRECTDRAWSURFACE ddsOff;

private:


};



#endif