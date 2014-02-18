#include "slider.h"
#include "stdio.h"
#define BMP_SLIDERSLIDER "gfx\\24Sliderslider.bmp"
#define BMP_SLIDERBUTTON "gfx\\24sliderbutton.bmp"

//////////////////////////////////////////////////////////////////////////////////////////////
int SLIDER::Initialize(LPDIRECTDRAW * lpDD, LPDIRECTDRAWSURFACE * ddSurface, 
					   int left, int top, int right, int bottom){
	this->Initialized=true;
	this->DDObject=lpDD;
	//now setup the images
	//here's how this thing is drawn.  The Slider image is a certain width and is drawn
	//as many times as necessary, and then finally drawn incomplete to fill in the width of this.
	//it's centered in it, too.
	//then to draw the marker (the button that slides), that is drawn with a color key centered
	//on the appropriate place.
	
	//create a surface and load in the bitmap into it.
	this->Slider=DDLoadBitmap(*lpDD, BMP_SLIDERSLIDER, 0, 0);
	this->Button=DDLoadBitmap(*lpDD, BMP_SLIDERBUTTON, 0, 0);

	if(this->Slider==NULL){
		OutputDebugString("Yo, didn't work.");
	}
	//now set the color keys
	DDSetColorKey((this->Slider),CLR_INVALID);
	DDSetColorKey((this->Button),CLR_INVALID);

	//setup the boundaries for the control
	this->Bounds.bottom=bottom;
	this->Bounds.top=top;
	this->Bounds.left=left;
	this->Bounds.right=right;

	this->Surface=ddSurface;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
SLIDER::SLIDER(){
	this->MaxValue=100;
	this->MinValue=0;
	this->Value=0;
	this->Visible=true;
	this->Initialized=false;

}

//////////////////////////////////////////////////////////////////////////////////////////////
SLIDER::~SLIDER(){
	//destroy the surfaces
}

//////////////////////////////////////////////////////////////////////////////////////////////
int SLIDER::SliderProc(PASSPROCVARS Params){
	int Rvalue=0;
	if(this->Initialized==true){
		//	RECT rect;
		POINT point;

		switch(Params.uMsg){
		case WM_MOUSEMOVE:
			if(Params.wParam!=MK_LBUTTON){//must be holding the left mouse button
				break;
			}
		case WM_LBUTTONDOWN:
			point.x= LOWORD(Params.lParam);
			point.y= HIWORD(Params.lParam);
			if(IsInside(this->Bounds, point)){
				//now we can move it.
				//calculate the value that corresponds to the position of the mouse.
				int RelativeMousePos=0;
				int vRange=0;//the range of this control (maxvalue-minvalue)
				int sRange=0;//screen range (right-left);
				vRange=(this->MaxValue)-(this->MinValue);
				sRange=(this->Bounds.right)-(this->Bounds.left);
				RelativeMousePos=(point.x)-(this->Bounds.left);//shift the nubmers to 0
				//mouse position:screen range :: VALUE:value range;
				float Percent;
				Percent=((float)RelativeMousePos/(float)sRange);
				this->Value=(int)(Percent*(float)vRange)+this->MinValue;

				//it changed, so update it.
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

//////////////////////////////////////////////////////////////////////////////////////////////
int SLIDER::Update(){
	if(this->Visible && this->Initialized){
		//if it's visible, show it
		//to show it, first figure out the coords of where to place it on screen
		//and the coords of what to blit.  Not hard
		
//		char str[100];
		
		RECT drect;//the place where we will be putting it.
		RECT srect;
		DDSURFACEDESC Sliderddsd;
		DDSURFACEDESC Buttonddsd;
		Sliderddsd.dwSize=sizeof(DDSURFACEDESC);
		this->Slider->GetSurfaceDesc(&Sliderddsd);
		Buttonddsd.dwSize=sizeof(DDSURFACEDESC);
		this->Button->GetSurfaceDesc(&Buttonddsd);
		SetRect(&srect, 0, 0, Sliderddsd.dwWidth, Sliderddsd.dwHeight);

		//now blit the background of the slider
		(*Surface)->Blt(&(this->Bounds), this->Slider,&srect , DDBLT_WAIT | DDBLT_KEYSRC, 0);

		//now draw the sliding button
		float percent;
		int x;
		int OffsetX;
		//we have to blit it so that it ends up centered on the corresponding place on screen

		//so first find the offset X

		OffsetX=0;
		percent=(float)(this->Value)/(float)(((this->MaxValue)-(this->MinValue)));//the value divided by the range of values is the percent
		//offsetX will be that percent times the range on the screen
		OffsetX=(int)((float)(((this->Bounds.right)-(this->Bounds.left)-(Buttonddsd.dwWidth)))*percent);
		//now make the offset the real pos on screen.
		x=OffsetX+(this->Bounds.left)+(Buttonddsd.dwWidth/2);//now X is the center, but we need to offset that by the
										//size of the button;
		//so we need to figure out how much to shrink the button to make it fit in the slider (
		//(if it's too big)
		this->Button->GetSurfaceDesc(&Buttonddsd);		
		x-=(int)((float)Buttonddsd.dwWidth/2);

		//now blit itj (the button)
		SetRect(&drect, x, this->Bounds.top+3, x+Buttonddsd.dwWidth, this->Bounds.bottom-3);

		(*Surface)->Blt(&drect, this->Button, NULL, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	}
	return 0;
}