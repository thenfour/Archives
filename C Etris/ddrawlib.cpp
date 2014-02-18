#include "ddrawlib.h"
#include "stdio.h"


WORD Shades[32][65536];
RGB16 rgb16;
#define DDRAWLIB_FADE_SKIP 2

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
		//alpha[x]=(double)i/32;
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

int FadeIn(LPDIRECTDRAW lpDD, LPDIRECTDRAWSURFACE * Front, LPDIRECTDRAWSURFACE *Image,
			 int Speed){
	///////////////////////
	//we'll create two surface in SYSTEM MEORY (faster).  One copy of the source image
	//and one that we'll use as our destination surface.  That destination surface will
	//be blitted to the front surface every "frame" of the fade.

	LRESULT Result=DD_OK;
	unsigned short * shadep;
	int x=0, y=0;
	char percent=0;
	DDSURFACEDESC ddsdDest;//destination
	DDSURFACEDESC ddsdSource;//source
	DDSURFACEDESC ddsdPrimary;//the Front surface

	LPDIRECTDRAWSURFACE DestSurface=DDCreateSurface(lpDD, 640, 480, DDSCAPS_SYSTEMMEMORY);
	LPDIRECTDRAWSURFACE SourceSurface=DDCreateSurface(lpDD, 640, 480, DDSCAPS_SYSTEMMEMORY);;
	//initialize the structs now.
	ZeroMemory(&ddsdDest, sizeof(ddsdDest));
	ddsdDest.dwSize=sizeof(ddsdDest);
	ZeroMemory(&ddsdSource, sizeof(ddsdSource));
	ddsdSource.dwSize=sizeof(ddsdSource);
	ZeroMemory(&ddsdPrimary, sizeof(ddsdPrimary));
	ddsdPrimary.dwSize=sizeof(ddsdPrimary);

	//blit to the temp surface
	RECT rect;
	SetRect(&rect, 0, 0, 640, 480);
	SourceSurface->BltFast(0,0,*Image, &rect, DDBLTFAST_WAIT);

	//lock all the surfaces now
	DestSurface->Lock(NULL, &ddsdDest, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, 0);
	SourceSurface->Lock(NULL, &ddsdSource, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, 0);
	(*Front)->Lock(NULL, &ddsdPrimary, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, 0);

	//get pointers to memory
	WORD * lpDest;
	WORD * lpSource;
	WORD * lpPrimary;

	lpDest = (WORD*)ddsdDest.lpSurface;
	lpSource = (WORD*)ddsdSource.lpSurface;
	lpPrimary = (WORD*)ddsdPrimary.lpSurface;

	for(percent=0;percent<=31;percent+=DDRAWLIB_FADE_SKIP){
		shadep=Shades[percent];
		for(x=0;x<(640*480);x++){
			lpDest[x]=shadep[lpSource[x]];
		}
		CopyMemory(lpPrimary, lpDest, 640*480*2);
	}
	DestSurface->Unlock(0);
	SourceSurface->Unlock(0);
	(*Front)->Unlock(0);
	DestSurface->Release();
	DestSurface=NULL;
	SourceSurface->Release();
	SourceSurface=NULL;

	Blit(*Front, *Image, 0, 0, 640, 480, 0, 0);
	return DD_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
int FadeOut(LPDIRECTDRAW lpDD, LPDIRECTDRAWSURFACE * Front, LPDIRECTDRAWSURFACE *Image,
			 int Speed){
	///////////////////////
	//we'll create two surface in SYSTEM MEORY (faster).  One copy of the source image
	//and one that we'll use as our destination surface.  That destination surface will
	//be blitted to the front surface every "frame" of the fade.

	LRESULT Result=DD_OK;
	unsigned short * shadep;
	int x=0, y=0;
	char percent=0;
	DDSURFACEDESC ddsdDest;//destination
	DDSURFACEDESC ddsdSource;//source
	DDSURFACEDESC ddsdPrimary;//the Front surface

	LPDIRECTDRAWSURFACE DestSurface=DDCreateSurface(lpDD, 640, 480, DDSCAPS_SYSTEMMEMORY);
	LPDIRECTDRAWSURFACE SourceSurface=DDCreateSurface(lpDD, 640, 480, DDSCAPS_SYSTEMMEMORY);;
	//initialize the structs now.
	ZeroMemory(&ddsdDest, sizeof(ddsdDest));
	ddsdDest.dwSize=sizeof(ddsdDest);
	ZeroMemory(&ddsdSource, sizeof(ddsdSource));
	ddsdSource.dwSize=sizeof(ddsdSource);
	ZeroMemory(&ddsdPrimary, sizeof(ddsdPrimary));
	ddsdPrimary.dwSize=sizeof(ddsdPrimary);

	//blit to the temp surface
	RECT rect;
	SetRect(&rect, 0, 0, 640, 480);
	SourceSurface->BltFast(0,0,*Image, &rect, DDBLTFAST_WAIT);

	//lock all the surfaces now
	DestSurface->Lock(NULL, &ddsdDest, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, 0);
	SourceSurface->Lock(NULL, &ddsdSource, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, 0);
	(*Front)->Lock(NULL, &ddsdPrimary, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, 0);

	//get pointers to memory
	WORD * lpDest;
	WORD * lpSource;
	WORD * lpPrimary;

	lpDest = (WORD*)ddsdDest.lpSurface;
	lpSource = (WORD*)ddsdSource.lpSurface;
	lpPrimary = (WORD*)ddsdPrimary.lpSurface;

	for(percent=31;percent>=0;percent-=DDRAWLIB_FADE_SKIP){
		shadep=Shades[percent];
		for(x=0;x<(640*480);x++){
			lpDest[x]=shadep[lpSource[x]];
		}
		CopyMemory(lpPrimary, lpDest, 640*480*2);
	}
	DestSurface->Unlock(0);
	SourceSurface->Unlock(0);
	(*Front)->Unlock(0);
	DestSurface->Release();
	DestSurface=NULL;
	SourceSurface->Release();
	SourceSurface=NULL;
	return DD_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////
LPDIRECTDRAWSURFACE DDCreateSurface(LPDIRECTDRAW lpDD, int width, int height, int mem_flags)
{
   DDSURFACEDESC ddsd;         // working description
   LPDIRECTDRAWSURFACE lpdds;  // temporary surface
    
   // set to access caps, width, and height
   memset(&ddsd,0,sizeof(ddsd));
   ddsd.dwSize  = sizeof(ddsd);
   ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

   // set dimensions of the new bitmap surface
   ddsd.dwWidth  =  width;
   ddsd.dwHeight =  height;

   // set surface to offscreen plain
   ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;

   // create the surface
   if(lpDD->CreateSurface(&ddsd,&lpdds,NULL)!=DD_OK)
      return(NULL);

   // return surface
   return(lpdds);
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
