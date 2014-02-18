//editbox.cpp
#include "editbox.h"
#define EDITBOX_TEXTMAX 256

//////////////////////////////////////////////////////////////////////////////////////////////
EDITBOX::EDITBOX(){
}

//////////////////////////////////////////////////////////////////////////////////////////////
EDITBOX::~EDITBOX(){
}

//////////////////////////////////////////////////////////////////////////////////////////////
int EDITBOX::SetText(char * buf){
	SetWindowText(this->GetHwnd(), buf);
	SendMessage(this->hwnd, EM_SETSEL, EDITBOX_TEXTMAX, EDITBOX_TEXTMAX);//set the limit of characters
	return EDITBOX_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////
int EDITBOX::GetText(char * buf, int max){
	GetWindowText(this->GetHwnd(), buf, max);
	return EDITBOX_OK;
}


//////////////////////////////////////////////////////////////////////////////////////////////
int EDITBOX::CreateEditBox(HWND Parent, RECT prect, RECT orect, HINSTANCE hInstance){
	this->parent=Parent;

	this->style= WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL |
		WS_BORDER | ES_NOHIDESEL;
	this->style= WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL |
		ES_NOHIDESEL;
	//copy the rect to local
	this->percentrect.top=prect.top;
	this->percentrect.left=prect.left;
	this->percentrect.bottom=prect.bottom;
	this->percentrect.right=prect.right;
	this->offsetrect.top=orect.top;
	this->offsetrect.left=orect.left;
	this->offsetrect.bottom=orect.bottom;
	this->offsetrect.right=orect.right;

	this->hwnd=CreateWindow( "EDIT", "",this->style,
		0, 0, 1, 1, Parent, (HMENU)this->GetId(), hInstance, 0);
	if(this->hwnd == 0) return EDITBOX_ERROR;
	if(this->CONTROLS::Size()==CONTROLS_ERROR) return EDITBOX_ERROR;

	HFONT Font;
	Font=(HFONT)GetStockObject(ANSI_VAR_FONT);
	Font=CreateFont(14,0,0,0,0,0,0,0,DEFAULT_CHARSET,0,0,0,0,"Courier New");
	SendMessage(this->hwnd, WM_SETFONT, (WPARAM)Font, 0);
	//EM_LINELENGTH
	WPARAM wParam = (WPARAM) 256;
	SendMessage(this->hwnd, EM_LINELENGTH, wParam, 0);//set the limit of characters
	return EDITBOX_OK;
}