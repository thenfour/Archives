#define BMP_ONOFFON "gfx\\24onoffon.bmp"
#define BMP_ONOFFOFF "gfx\\24onoffoff.bmp"


#include "onoff.h"
#include "stdio.h"
/////////////////////////////////////////////////////////////////////////////////////////////
ONOFF::ONOFF(){
	this->Valid=false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
ONOFF::~ONOFF(){
}

/////////////////////////////////////////////////////////////////////////////////////////////
//We'll need placement on screen, and that's it, I guess.
int ONOFF::Initialize(RECT rect, IDirectDrawSurface ** ddS, LPDIRECTDRAW * lpDD){
	this->Value=false;//initialize the vlaue
	this->Bounds=rect;//set the screeen boundaries;
	this->dds=ddS;//set the display surface.

	//create the surfaces/load teh images
	this->ddsOff=DDLoadBitmap(*lpDD, BMP_ONOFFOFF, 0,0);
	this->ddsOn =DDLoadBitmap(*lpDD, BMP_ONOFFON , 0,0);
	DDSetColorKey(this->ddsOn, CLR_INVALID);
	DDSetColorKey(this->ddsOff, CLR_INVALID);

	//now fill in values for the srect member;
	DDSURFACEDESC ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_ALL;
	this->ddsOff->GetSurfaceDesc(&ddsd);
	this->srect.top=0;
	this->srect.left=0;
	this->srect.right=ddsd.dwWidth;
	this->srect.bottom=ddsd.dwHeight;

	this->Valid=true;
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
int ONOFF::Update(){
	if(this->Valid){
		switch(this->Value){
		case false:
			switch((*this->dds)->Blt(&(this->Bounds), this->ddsOn, &(this->srect), DDBLT_WAIT | DDBLT_KEYSRC, 0)){
			case DDERR_UNSUPPORTED:
				OutputDebugString("Didn't work");
				break;
			case DDERR_INVALIDPARAMS:
				OutputDebugString("Blue turds shoudl be fed to infants");
				break;
			case DD_OK:
				//OutputDebugString("DD_OK");
				break;
			case DDERR_INVALIDOBJECT:
				OutputDebugString("DD_obkect");
				break;
			case DDERR_WASSTILLDRAWING:
				OutputDebugString("DD_obkect234234234");
				break;
			case DDERR_SURFACELOST:
				OutputDebugString("why do I always hav to eat shit");
				break;
			case DDERR_GENERIC:
				OutputDebugString("It's a wonderful day in teh toilet");
				break;
			case DDERR_NOSTRETCHHW:
				OutputDebugString("Tetris is a gummy game");
				break;
			case DDERR_NOBLTHW:
				OutputDebugString("Foundry");
				break;
			case DDERR_INVALIDRECT:
				OutputDebugString("Teletubbies ate my balls");
				break;
			default:
				OutputDebugString("Abort your fetus");
				break;
			};
			
			break;
		case true:
			(*this->dds)->Blt(&this->Bounds, this->ddsOff, &this->srect, DDBLT_WAIT | DDBLT_KEYSRC, 0);
			break;
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
int ONOFF::OnOffProc(PASSPROCVARS Params){
	int Rvalue=0;
	if(this->Valid==true){
		POINT point;
		switch(Params.uMsg){
		case WM_LBUTTONDOWN:
			point.x = LOWORD(Params.lParam);  // horizontal position of cursor 
			point.y = HIWORD(Params.lParam);  // vertical position of cursor 
			if(IsInside(this->Bounds, point)){
				//we clicked in it.
				//toggle the value
				if(this->Value==true){
					this->Value=false;
				}
				else{
					this->Value=true;
				}
				this->Update();
				Rvalue=1;
			}
			break;
		case WM_PAINT:
			break;
		}
		}
	return Rvalue;
}

