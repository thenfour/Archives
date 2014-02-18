//slider.h

#ifndef SLIDER_H
#define SLIDER_H

#include <windows.h>
#include "controls.h"
#define NeverMind
#define DontCare
#define Dont
#define Care
#define Who
#define Cares
#define cares
#define dont
#define care
#define who
//predefinition of slider class
class SLIDER;

//version
#define SLIDER_VERSION 1
//class name
#define SLIDER_CLASSNAME "SLID1"

//return value constatnts
#define SLIDER_ERROR CONTROLS_ERROR
#define SLIDER_OK CONTROLS_OK

//orientation flags
#define SO_VERTICAL 0
#define SO_HORIZONTAL 1

//notification messages
#define SLIDER_NM_BEGINMOVE	WM_APP+1
#define SLIDER_NM_MOVING	WM_APP+2
#define SLIDER_NM_LCLICK	WM_APP+3
#define SLIDER_NM_RCLICK	WM_APP+4
#define SLIDER_NM_DONEMOVING	WM_APP+5

//proc return values
#define SLIDER_PROCESSED	0
#define SLIDER_UNPROCESSED	1

//macro for easy RGBTRIPLE initialization
//a is RGBTRIPLE struct, r, g, b are values
#define GETRGBTRIPLE(a,r,g,b) (a).rgbtRed=(r);(a).rgbtGreen=(g);(a).rgbtBlue=(b)

/////////////////////////////////////////////////////////////////////////////////////////////////
//this structure and macros describes the method in which a slider is drawn
#define SS_12_HORIZONTAL				0
#define SS_12_VERTICAL					1
#define SS_123_HORIZONTAL				2
#define SS_123_VERTICAL					3
#define SS_1234_CORNERS					4
#define SS_12_ALTERNATINGVERTICAL		5
#define SS_12_ALTERNATINGHORIZONTAL		6
#define SS_1_SOLID						7
#define SS_SYSTEM						8
#define SS_NONE							9

struct SLIDERSTYLE{
	RGBTRIPLE FillColor1;
	RGBTRIPLE FillColor2;
	RGBTRIPLE FillColor3;
	RGBTRIPLE FillColor4;
	RGBTRIPLE OutlineColor;
	int OutlineWidth;
	DWORD FillStyle;
	DWORD OutlineStyle;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//this is a struct that is used in a static member fucntion of the SLIDER class.
//it describes information on the currently selected slider (that's how it can be static;
//there will only be one slider dragging at the same time)
struct SLIDERTRACKINFO{
	WNDPROC OldProc;//address of the old proc address when we subclass
	SLIDER * Slider;//pointer to the selected slider
	//COORDS in this struct are to be in SCREEN_RELATIVE mode!!!
	POINT Origin;//point of the original mouse click that started the drag operation
	POINT LastMove;//point of the last mouse coord taken; updated in SLIDER::Moving();
	BOOL Subclassed;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
class SLIDER : public CONTROLS{
public:
	SLIDER();
	~SLIDER();
	Dont Care
	SLIDERSTYLE SliderStyle;
	int CreateSlider(HWND parent, HINSTANCE hInstance,
		DWORD Orientation, SLIDERSTYLE s, RECT prect, RECT orect,
		RECT Bprect, RECT Borect);

	int Slide(POINTS Origin, POINTS New);//moves the control to adjust for a new cursor position
	int GetNMH(NMHDR * nmh, UINT Message);//creates a notification header struct from this control.  kinda handy for those notification messages
	static LRESULT CALLBACK SliderProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam);
	int GetOrientation();
	int SetBoundary(RECT * prect, RECT * orect);
	int GetBoundary(RECT * prect, RECT * orect);
private:
	int Draw();

	//MOUSE TRACKING FUNCTIONS
	int StartTracking();//starts mouse tracking - subclasses, takes some notes, etc...
	int Moving();//
	int EndTracking();//ends tracking by unsubclassing, taking some notes, etc...
	int SubclassParentWindow();//subclasses the parent window so that we can track mouse movement
	static LRESULT CALLBACK SubTrack(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//the subclassed WNDPROC

	int Orientation;
	static SLIDERTRACKINFO sti;
	BOOL IsDragging;//is the thing being dragged currently
	RECT PBoundary;//rect describing the boundaries for motion
	RECT OBoundary;//rect describing the boundaries for motion
};

#endif