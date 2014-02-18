//static.cpp
#include "static.h"

/////////////////////////////////////////////////////////////////////////////////////
STATIC::STATIC(){
}


/////////////////////////////////////////////////////////////////////////////////////
STATIC::~STATIC(){
}

int STATIC::SetFont(char* FontName, int Size){
	HFONT Font;
//	Font=(HFONT)GetStockObject(ANSI_VAR_FONT);
//	Font=CreateFont(14,0,0,0,0,0,0,0,DEFAULT_CHARSET,0,0,0,0,"Courier New");
	Font=CreateFont(Size,0,0,0,0,0,0,0,DEFAULT_CHARSET,0,0,0,0,FontName);
	SendMessage(this->hwnd, WM_SETFONT, (WPARAM)Font, 0);
	return STATIC_OK;
}

/////////////////////////////////////////////////////////////////////////////////////
int STATIC::CreateStaticText(HWND Parent, HINSTANCE hInstance, int id, RECT prect, RECT orect){
	//creates the window and all

	this->parent=Parent;

	this->style= WS_VISIBLE | WS_CHILD | SS_LEFT
		//|WS_BORDER
		;
	//copy the rect to local
	this->percentrect.top=prect.top;
	this->percentrect.left=prect.left;
	this->percentrect.bottom=prect.bottom;
	this->percentrect.right=prect.right;

	this->offsetrect.top=orect.top;
	this->offsetrect.left=orect.left;
	this->offsetrect.bottom=orect.bottom;
	this->offsetrect.right=orect.right;


	this->hwnd=CreateWindow( "STATIC", "",this->style,
		0, 0, 1, 1, Parent, (HMENU)this->GetId(), hInstance, 0);
	if(this->hwnd == 0) return STATIC_ERROR;

	if(this->Size()==CONTROLS_ERROR) return STATIC_ERROR;
	

	this->SetFont("Courier New", 14);
	if(SetWindowText(this->hwnd, "(text)")==0) return STATIC_ERROR;
	return STATIC_OK;
}

/////////////////////////////////////////////////////////////////////////////////////
int STATIC::SetText(char * Text){
	if(SetWindowText(this->hwnd, Text)==0) return STATIC_ERROR;
	return STATIC_OK;
}
