#include "ddrawlib.h"
#include "stdio.h"

WORD Shades[32][65536];
RGB16 rgb16;
#define DDRAWLIB_FADE_SKIP 1

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT SetupDDObject(LPDIRECTDRAW& Object, HWND hWnd){
LRESULT Result;

//Create Object, get exclusive full-screen mode, and Set the display.
Result=DirectDrawCreate(NULL, &Object, NULL);
if(Result==DD_OK){
	Result=Object->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
   if(Result==DD_OK){
		Result=Object->SetDisplayMode(640,480,16);
   }
}

return Result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Creates the primary surface for a DirectDraw object.                                                                //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT SetupPriSurface(LPDIRECTDRAW& DDobject, LPDIRECTDRAWSURFACE& DDsurface){
LRESULT Result;
DDSURFACEDESC SurfaceDesc;

SurfaceDesc.dwSize=sizeof(SurfaceDesc);
SurfaceDesc.dwFlags=DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
SurfaceDesc.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
SurfaceDesc.dwBackBufferCount = 1;

Result=DDobject->CreateSurface(&SurfaceDesc, &DDsurface, NULL);
return Result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Creates the back surface for the directDraw object                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT SetupBackSurface(LPDIRECTDRAW& DDobject, LPDIRECTDRAWSURFACE& DDpri,
	                      LPDIRECTDRAWSURFACE& DDback){
LRESULT Result;
DDSURFACEDESC SurfaceDesc;
DDSCAPS SurfaceCaps;

SurfaceDesc.dwSize=sizeof(SurfaceDesc);
SurfaceCaps.dwCaps=DDSCAPS_BACKBUFFER;
Result=DDpri->GetAttachedSurface(&SurfaceCaps, &DDback);

return Result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Creates a surface that is used simply for storing graphics, basically.                                              //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT SetupOtherSurface(LPDIRECTDRAW& DDobject, LPDIRECTDRAWSURFACE& DDsurface){
LRESULT Result;
DDSURFACEDESC SurfaceDesc;

SurfaceDesc.dwSize=sizeof(SurfaceDesc);
SurfaceDesc.dwFlags=DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
SurfaceDesc.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN;
SurfaceDesc.dwBackBufferCount = 1;

Result=DDobject->CreateSurface(&SurfaceDesc, &DDsurface,NULL);
return Result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT Flip(LPDIRECTDRAWSURFACE& DDsurface){
return DDsurface->Flip(NULL,DDFLIP_WAIT);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT Blit(LPDIRECTDRAWSURFACE& dDDsurface, LPDIRECTDRAWSURFACE& sDDsurface,
								 int sLeft, int sTop, int sRight, int sBottom,
                         int dLeft, int dTop){
RECT Rect;
LRESULT Result;

Rect.top=sTop;
Rect.bottom=sBottom;
Rect.right=sRight;
Rect.left=sLeft;

	Result=dDDsurface->BltFast(dLeft, dTop, sDDsurface, &Rect,DDBLTFAST_WAIT);
return Result;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CKBlit(LPDIRECTDRAWSURFACE& dDDsurface, LPDIRECTDRAWSURFACE& sDDsurface,
								 int sLeft, int sTop, int sRight, int sBottom,
                         int dLeft, int dTop){
RECT Rect;
LRESULT Result;

Rect.top=sTop;
Rect.bottom=sBottom;
Rect.right=sRight;
Rect.left=sLeft;

Result=dDDsurface->BltFast(dLeft, dTop, sDDsurface, &Rect,DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);

return Result;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT MakeOffScreenSurface(LPDIRECTDRAW& DDObject, LPDIRECTDRAWSURFACE& OSSurface, int x, int y){

DDSURFACEDESC ddsd;
//initialize ddsd
ZeroMemory(&ddsd, sizeof(ddsd));
ddsd.dwSize = sizeof(ddsd);
ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
ddsd.dwWidth = x;
ddsd.dwHeight = y;
DDObject->CreateSurface(&ddsd, &OSSurface, NULL);
if(OSSurface != NULL) return DD_OK;
return DDERR_GENERIC;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FadeIn8(LPDIRECTDRAW * DDObject, LPDIRECTDRAWSURFACE * Front, IDirectDrawPalette * Palette){
	IDirectDrawPalette* OldPalette;
	IDirectDrawPalette* NewPalette;

	PALETTEENTRY Oldpe[256];
	PALETTEENTRY Newpe[256];

	(*Front)->GetPalette(&OldPalette);
	OldPalette=Palette;

	//Get the palette entries
	OldPalette->GetEntries(0, 0, 255, Oldpe);

	for(unsigned int i=0;i<100;i++){
		//decrease it by one percent every time.
		for(unsigned int x=0;x<256;x++){
			Newpe[x].peFlags=0;
			Newpe[x].peGreen = (BYTE)(((float)i/100) * (float)Oldpe[x].peGreen);
			Newpe[x].peRed = (BYTE)(((float)i/100) * (float)Oldpe[x].peRed);
			Newpe[x].peBlue = (BYTE)(((float)i/100) * (float)Oldpe[x].peBlue);
		}
		//now that we have new entries, create a palette adn set it.
	(*DDObject)->CreatePalette(DDPCAPS_8BIT, Newpe, &NewPalette, 0);
		(*Front)->SetPalette(NewPalette);
		Sleep(6);
	}
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IDirectDrawPalette * FadeOut8(LPDIRECTDRAW * DDObject, LPDIRECTDRAWSURFACE * Front){
	IDirectDrawPalette* OldPalette;
	IDirectDrawPalette* NewPalette;
	PALETTEENTRY Oldpe[256];
	PALETTEENTRY Newpe[256];

	(*Front)->GetPalette(&OldPalette);

	//Get the palette entries
	OldPalette->GetEntries(0, 0, 255, Oldpe);

	for(unsigned int i=100;i>0;i--){
		//decrease it by one percent every time.
		for(unsigned int x=0;x<256;x++){
			Newpe[x].peFlags=0;
			Newpe[x].peGreen = (BYTE)(((float)i/100) * (float)Oldpe[x].peGreen);
			Newpe[x].peRed = (BYTE)(((float)i/100) * (float)Oldpe[x].peRed);
			Newpe[x].peBlue = (BYTE)(((float)i/100) * (float)Oldpe[x].peBlue);
		}
		//now that we have new entries, create a palette adn set it.
	(*DDObject)->CreatePalette(DDPCAPS_8BIT, Newpe, &NewPalette, 0);
		(*Front)->SetPalette(NewPalette);
		Sleep(6);
	}
	return OldPalette;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LPDIRECTDRAWPALETTE BlackScreen8(LPDIRECTDRAW * DDObject, LPDIRECTDRAWSURFACE * Surface){
	//this just sets the screen to a black palette;
	//returns the old palette
	LPDIRECTDRAWPALETTE OldPalette;
	LPDIRECTDRAWPALETTE Palette;
	(*Surface)->GetPalette(&OldPalette);
	PALETTEENTRY pe[256];


	for(unsigned int i=0;i<256;i++){
		pe[i].peBlue=0;
		pe[i].peRed=0;
		pe[i].peGreen=0;
		pe[i].peFlags=0;
	}
	(*DDObject)->CreatePalette(DDPCAPS_8BIT,pe,&Palette,0);
	(*Surface)->SetPalette(Palette);
	return OldPalette;
}

/////////////////////////////////////////////////////////////////////////////////////////
//This is needed before you can call the Fade routines.
void InitAlphaTable(){
	int r,g,b;
	int x;
	unsigned int Newr, Newg, Newb;
	double alpha[32];
	x=0;
	for(double i=1;i<=32;i+=1, x++){
		//fill alpha[] with values
		alpha[x]=((((-i)*(i-32))/80)*(i/32))+(i/32);
	}
	unsigned long MaxR, MaxB, MaxG;
	MaxR=rgb16.dwRBitMask >> rgb16.Positions.rgbRed;
	MaxG=rgb16.dwGBitMask >> rgb16.Positions.rgbGreen;
	MaxB=rgb16.dwBBitMask >> rgb16.Positions.rgbBlue;

	for(int color=0;color<65536;color++){
		for(int shade=0;shade<32;shade++){
			//fill in the alpha table.
			//gt the color components of this thing.
			GetRGB(color, r,g,b);
			Newr=(int)((double)r*alpha[shade]);
			Newg=(int)((double)g*alpha[shade]);
			Newb=(int)((double)b*alpha[shade]);
			if(Newr>MaxR) Newr=MaxR;
			if(Newg>MaxG) Newg=MaxG;
			if(Newb>MaxB) Newb=MaxB;
			Shades[shade][color]=GetColor(Newr, Newg, Newb);
		}		
	}


	return;
}
/////////////////////////////////////////////////////////////////////////////////////////
//a 16-bit fade in routine, double buffering

int FadeIn(LPDIRECTDRAWSURFACE * Front, LPDIRECTDRAWSURFACE *Back, LPDIRECTDRAWSURFACE *Image,
			 int Speed){
	///////////////////////
	LRESULT Result;
	int x=0, y=0;
	char percent=0;
	DDSURFACEDESC Dddsd;//destination
	DDSURFACEDESC Sddsd;//source
	DDSURFACEDESC Fddsd;//the Front surface
	//initialize the structs now.
	ZeroMemory(&Dddsd, sizeof(Dddsd));
	Dddsd.dwSize=sizeof(Dddsd);
	ZeroMemory(&Fddsd, sizeof(Fddsd));
	Fddsd.dwSize=sizeof(Fddsd);
	ZeroMemory(&Sddsd, sizeof(Sddsd));
	Sddsd.dwSize=sizeof(Sddsd);

	RECT rect;
	rect.left=0;
	rect.top=0;
	rect.right=640;
	rect.bottom=480;

	WORD *oldp;
	WORD *newp;
	WORD *fp;

	Result=(*Image)->Lock(NULL, &Sddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	if(Result!=DD_OK) return Result;
	Result=(*Front)->Lock(NULL, &Fddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL);//lock the destination surface for this pass.
	if(Result!=DD_OK){
		(*Image)->Unlock(0);
		return Result;
	}
	Result=(*Back)->Lock(&rect, &Dddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	if(Result!=DD_OK){
		(*Image)->Unlock(0);
		(*Front)->Unlock(0);
		return Result;
	}
	newp = (WORD*)Dddsd.lpSurface;
	fp=    (WORD*)Fddsd.lpSurface;
	oldp=  (WORD*)Sddsd.lpSurface;//get pointers to surface memory
	WORD * shadep;
	for(percent=1;percent<32;percent+=DDRAWLIB_FADE_SKIP){
		shadep=Shades[percent];
		for(x=0;x<(640*480);x++){
			fp[x]=shadep[oldp[x]];
		}
	}
		shadep=Shades[31];
		for(x=0;x<(640*480);x++){
			fp[x]=shadep[oldp[x]];
		}
	(*Front)->Unlock(NULL);
	(*Back)->Unlock(NULL);
	(*Image)->Unlock(NULL);

	return DD_OK;

}
/////////////////////////////////////////////////////////////////////////////////////////////////
int FadeOut(LPDIRECTDRAWSURFACE * Front, LPDIRECTDRAWSURFACE *Back, LPDIRECTDRAWSURFACE *Image,
			 int Speed){
	///////////////////////
	LRESULT Result;
	int x=0, y=0;
	char percent=0;
	DDSURFACEDESC Dddsd;//destination
	DDSURFACEDESC Sddsd;//source
	DDSURFACEDESC Fddsd;//the Front surface
	//initialize the structs now.
	ZeroMemory(&Dddsd, sizeof(Dddsd));
	Dddsd.dwSize=sizeof(Dddsd);
	ZeroMemory(&Fddsd, sizeof(Fddsd));
	Fddsd.dwSize=sizeof(Fddsd);
	ZeroMemory(&Sddsd, sizeof(Sddsd));
	Sddsd.dwSize=sizeof(Sddsd);

	RECT rect;
	rect.left=0;
	rect.top=0;
	rect.right=640;
	rect.bottom=480;

	WORD *oldp;
	WORD *newp;
	//WORD *fp;
	
	Result=DD_OK;
	//Result=(*Image)->Lock(NULL, &Sddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	if(Result!=DD_OK) return Result;
	//Result=(*Front)->Lock(NULL, &Fddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL);//lock the destination surface for this pass.
	if(Result!=DD_OK){
		(*Image)->Unlock(0);
		return Result;
	}
	//Result=(*Back)->Lock(&rect, &Dddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	if(Result!=DD_OK){
		(*Image)->Unlock(0);
		(*Front)->Unlock(0);
		return Result;
	}
	WORD * shadep;
	//////////
	for(percent=31;percent>=0;percent-=DDRAWLIB_FADE_SKIP){
		Result=(*Image)->Lock(NULL, &Sddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
		if(Result!=DD_OK){
			OutputDebugString("lock front");
			goto Leave;
		}
		//Result=(*Front)->Lock(NULL, &Fddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL);//lock the destination surface for this pass.
		//if(Result!=DD_OK){
		//	OutputDebugString("lock front");
		//	goto Leave;
		//}
		Result=(*Back)->Lock(&rect, &Dddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
		if(Result!=DD_OK){
			OutputDebugString("lock front");
			goto Leave;
		}
		oldp=  (WORD*)Sddsd.lpSurface;//get pointers to surface memory
		newp = (WORD*)Dddsd.lpSurface;
		//fp=    (WORD*)Fddsd.lpSurface;
		shadep=Shades[percent];
		for(x=0;x<(640*480);x++){
			newp[x]=shadep[oldp[x]];
		}
		Result=(*Image)->Unlock(NULL);
		if(Result!=DD_OK){
			OutputDebugString("lock front");
			goto Leave;
		}
		//Result=(*Front)->Unlock(0);
		//if(Result!=DD_OK){
		//	OutputDebugString("lock front");
		//	goto Leave;
		//}
		Result=(*Back)->Unlock(0);
		if(Result!=DD_OK){
			OutputDebugString("lock front");
			goto Leave;
		}
		while(Result != DDERR_SURFACEBUSY){
			Result=(*Front)->Flip(NULL, DDFLIP_WAIT);
			if(Result!=DD_OK){
				OutputDebugString("lock front");
				goto Leave;
			}
		}
	}
	//////////
	shadep=Shades[0];
	for(x=0;x<(640*480);x++){
	//	fp[x]=shadep[oldp[x]];
	}
	//(*Front)->Unlock(NULL);
	//(*Back)->Unlock(NULL);
	return Result;
Leave:
	//(*Front)->Unlock(NULL);
	(*Back)->Unlock(NULL);
	(*Image)->Unlock(NULL);

	return Result;

}

/*
This section is all about Drawing routines, like Pset/Get, etc...
you have to make a struct RGB16, run IniRGB16 to initialize it...that is teh info
these routines need to use the pixels on the screen.

*/////////////////////////////////////////////////////////////////////////////////////////////
//routines to handle the 16-bit pixels.
////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
int IniRGB16 (LPDIRECTDRAWSURFACE* Surface)
{
	DDSURFACEDESC ddsd;
	//fill ddsd
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_PIXELFORMAT;
	(*Surface)->GetSurfaceDesc (&ddsd);

	char r=0, g=0, b=0;
	for(int n = 1; n<65536; n<<=1) {
		if(ddsd.ddpfPixelFormat.dwRBitMask & n) ++r;
		if(ddsd.ddpfPixelFormat.dwGBitMask & n) ++g;
		if(ddsd.ddpfPixelFormat.dwBBitMask & n) ++b;
	}

	rgb16.dwRBitMask = ddsd.ddpfPixelFormat.dwRBitMask;
	rgb16.Positions.rgbRed = g + b;

	rgb16.dwGBitMask = ddsd.ddpfPixelFormat.dwGBitMask;
	rgb16.Positions.rgbGreen = b;

	rgb16.dwBBitMask = ddsd.ddpfPixelFormat.dwBBitMask;
	rgb16.Positions.rgbBlue = 0;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
// Must have the surface locked, pass the surface description 
// that gets returned from the lock call
void Pset(int x, int y, int r, int g, int b, DDSURFACEDESC *ddsd){
	WORD Pixel;
	Pixel=GetColor(r,g,b);
	WORD *pixels = (WORD*)ddsd->lpSurface;
	DWORD pitch = ddsd->dwWidth;
	pixels[y*pitch + x] = Pixel; 
}
//fills r, g, and b with values from the place on screen
///////////////////////////////////////////////////////////////////////////////////////////
void Pget(int x, int y, int& r, int& g, int& b, DDSURFACEDESC *ddsd){
	WORD Pixel;
	WORD *pixels = (WORD*)ddsd->lpSurface;
	DWORD pitch = ddsd->dwWidth;
	Pixel=pixels[y*pitch + x]; 
	GetRGB(Pixel, r, g, b);
}
///////////////////////////////////////////////////////////////////////////////////////////
void GetRGB(WORD Pixel,int &r, int &g, int&b){
	r=(Pixel & rgb16.dwRBitMask) >> rgb16.Positions.rgbRed;
	g=(Pixel & rgb16.dwGBitMask) >> rgb16.Positions.rgbGreen;
	b=(Pixel & rgb16.dwBBitMask) >> rgb16.Positions.rgbBlue;

}
///////////////////////////////////////////////////////////////////////////////////////////
WORD GetColor(int r, int g, int b){
	WORD Pixel= ((r << rgb16.Positions.rgbRed)|(g << rgb16.Positions.rgbGreen)|(b));
	return Pixel;
}

///////////////////////////////////////////////////////////////////////////////////////////
LRESULT Rectangle(RECT rect, int r, int g, int b, bool Filled){
	return DD_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
LRESULT Line(RECT rect, int r, int g, int b){
	return DD_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
LRESULT HLine(int x1, int x2, int y, int r, int g, int b){
	return DD_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
LRESULT VLine(int y1, int y2, int x, int r, int g, int b){
	return DD_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
LRESULT Circle(int x, int y, int radius, int r, int g, int b){
	return DD_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
LRESULT Triangle(POINT p1, POINT p2, POINT p3, int r, int g, int b){
	return DD_OK;
}
