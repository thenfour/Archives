#ifndef __CONTROLS
#define __CONTROLS

#include "passvars.h"
#include "ddrawlib.h"
#include "settings.h"

class COMMANDBUTTON{
	public:
		COMMANDBUTTON();
		Initialize(LPDIRECTDRAWSURFACE* highlighted, LPDIRECTDRAWSURFACE* unhighlighted, LPDIRECTDRAWSURFACE* ddSurface, 
					RECT Rect, int (&Proc) (PASSPROCVARS Params));
	   ~COMMANDBUTTON();

		int COMMANDBUTTON::ProcFunction(PASSPROCVARS Params);
   		int (*CommandButtonProc) (PASSPROCVARS Params);//pointer to the message process function for the
      											//button.  it returns an int, and takes no parameters.
   		bool Valid;
		int Top;
		int Left;
		int Bottom;
		int Right;
		bool IsHighlighted;

	private:
      LPDIRECTDRAWSURFACE* UnHighlighted;
      LPDIRECTDRAWSURFACE* Surface;
      LPDIRECTDRAWSURFACE* Highlighted;

};

#endif
