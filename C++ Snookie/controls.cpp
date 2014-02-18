//controls.cpp

#include "controls.h"

unsigned int CONTROLS::NextId=500;

/////////////////////////////////////////////////////////////////////////////////////
int CONTROLS::GetPlacement(RECT * prect, RECT * orect){
	CopyRect(prect, &this->percentrect);
	CopyRect(orect, &this->offsetrect);
	return CONTROLS_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
int CONTROLS::SetPlacement(RECT * prect, RECT * orect){
	CopyRect(&this->percentrect, prect);
	CopyRect(&this->offsetrect, orect);
	return CONTROLS_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
int CONTROLS::Redraw(){
	//invalidate the window
	if(InvalidateRect(this->GetHwnd(), NULL, FALSE)==0){
		return CONTROLS_ERROR;
	}
	UpdateWindow(this->GetHwnd());
	return CONTROLS_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
CONTROLS::CONTROLS(){
	this->hwnd=0;
	this->parent=0;
	this->style=0;
	this->id=NextId;
	ZeroMemory(&this->percentrect, sizeof(RECT));
	ZeroMemory(&this->offsetrect, sizeof(RECT));
	NextId++;
}
/////////////////////////////////////////////////////////////////////////////////////
unsigned int CONTROLS::GetId(){
	return this->id;
}
/////////////////////////////////////////////////////////////////////////////////////
HWND CONTROLS::GetHwnd(){
	return this->hwnd;
}

/////////////////////////////////////////////////////////////////////////////////////
HWND CONTROLS::GetParentHWND(){
	return this->parent;
}
/////////////////////////////////////////////////////////////////////////////////////
int CONTROLS::Size(){
	//called when the parent window is sized; this will resize it.
	//get the coords for this;
	if(this->hwnd != 0){
		RECT clientrect;
		DWORD height, width;
		//since the client's top and left members are always 0
		//the bottom and right represent height and width, respectively.
		if(GetClientRect(this->parent, &clientrect)==0) return CONTROLS_ERROR;
		//put the rect into this->rect;

		RECT newrect;
		this->ControlRectToRect(&newrect, &this->percentrect, &this->offsetrect, &clientrect);

		height=newrect.bottom-newrect.top;
		width=newrect.right-newrect.left;

		if(MoveWindow(this->hwnd, newrect.left, newrect.top, width, height, TRUE)==0) return CONTROLS_ERROR;
	}
	return CONTROLS_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
CONTROLS::~CONTROLS(){
}

/////////////////////////////////////////////////////////////////////////////////////
int CONTROLS::GetRect(RECT * rect){
	if(GetWindowRect(this->hwnd,rect) != 0){
		return CONTROLS_OK;
	}
	return CONTROLS_ERROR;
}


/////////////////////////////////////////////////////////////////////////////////////
int CONTROLS::ControlRectToRect(RECT * newrect, RECT * prect, RECT * orect, RECT * clientrect){
	//convert a percent rect and an offset rect to a rect based on the clientrect
	newrect->top=(int)((((float)prect->top)/100)      *((float)(clientrect->bottom)));
	newrect->left=(int)((((float)prect->left)/100)    *((float)(clientrect->right)));
	newrect->bottom=(int)((((float)prect->bottom)/100)*((float)(clientrect->bottom)));
	newrect->right=(int)((((float)prect->right)/100)  *((float)(clientrect->right)));

	newrect->top+=orect->top;
	newrect->left+=orect->left;
	newrect->bottom+=orect->bottom;
	newrect->right+=orect->right;

	return CONTROLS_OK;
}