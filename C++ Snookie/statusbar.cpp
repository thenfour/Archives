//statusbar.cpp

#include "statusbar.h"

////////////////////////////////////////////////////////////////////////////////////////////
int STATUSBAR::GetText(int nPart, char* Text){
	//puts the text into Text
	SendMessage(this->hwnd, SB_GETTEXT, (WPARAM)nPart, (LPARAM)Text);
	return STATUSBAR_OK;
}
////////////////////////////////////////////////////////////////////////////////////////////
int STATUSBAR::Create(HWND parent){

	this->hwnd=CreateStatusWindow(WS_CHILD | WS_VISIBLE, "yo", parent, this->GetId());
	if(this->hwnd==0) return STATUSBAR_ERROR;

	HFONT Font;
	Font=(HFONT)GetStockObject(ANSI_VAR_FONT);
	SendMessage(this->hwnd, WM_SETFONT, (WPARAM)Font, 0);

	return this->hwnd==NULL ? STATUSBAR_ERROR: STATUSBAR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////
int STATUSBAR::SetText(int nPart, char* Text){
	SendMessage(this->hwnd, SB_SETTEXT, (WPARAM)nPart, (LPARAM)Text);
	return STATUSBAR_OK;
}
////////////////////////////////////////////////////////////////////////////////////////////
int STATUSBAR::SetParts(int nParts, STATUSBARPART * parts){
	int i;
	//create an integer array containing the positions of the stuff.

	int *IntArray;
	int *Percent;

	IntArray=new int[nParts];
	Percent=new int[nParts];

	for(i=0;i<nParts;i++){
		Percent[i]=parts[i].WidthPercent;
		this->WidthPercentage[i]=parts[i].WidthPercent;
	}

	this->PercentToClient(nParts, IntArray, Percent);

	if(SendMessage(this->hwnd, SB_SETPARTS, (WPARAM) nParts, (LPARAM)IntArray)==FALSE){
		return STATUSBAR_ERROR;
	}

	//and now we'll go ahead and set all the text.
	for(i=0;i<nParts;i++){
		if(SendMessage(this->hwnd, SB_SETTEXT, (WPARAM)i, (LPARAM)parts[i].Text)==FALSE){
			return STATUSBAR_ERROR;
		}
	}
	return STATUSBAR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////
STATUSBAR::STATUSBAR(){
}

////////////////////////////////////////////////////////////////////////////////////////////
STATUSBAR::~STATUSBAR(){
}

////////////////////////////////////////////////////////////////////////////////////////////
int STATUSBAR::Size(){
	SendMessage(this->hwnd, WM_SIZE, 0, 0);
	int IntArray[256];
	int nParts;

	//get the number of parts
	nParts=SendMessage(this->hwnd, SB_GETPARTS, 0, 0);

	//get the screen coords now
	this->PercentToClient(nParts, IntArray, this->WidthPercentage);

	//and set them.
	if(SendMessage(this->hwnd, SB_SETPARTS, (WPARAM) nParts, (LPARAM)IntArray)==FALSE){
		return STATUSBAR_ERROR;
	}

	return STATUSBAR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////
int STATUSBAR::PercentToClient(int nParts, int* client, int*percent){
	//this function takes the buffer percent and turns it into client
	//coordinates.
	RECT rect;
	int Width;
	int i;
	int RunningTotal;

	RunningTotal=0;
	GetClientRect(this->hwnd, &rect);
	Width=(rect.right-rect.left);
	//recalculate the positions to client coords. from percentages.
	for(i=0;i<nParts;i++){
		RunningTotal+=percent[i];
		if(i==(nParts-1)){
			//if it's the last one, make it go to the end of the window
			client[i]=-1;
		}else{
			client[i]=(int)((float)Width*((float)(RunningTotal)/100));
		}
	}


	return STATUSBAR_OK;
}