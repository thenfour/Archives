//slider.cpp

#include "slider.h"
#include "stdio.h"

SLIDERTRACKINFO SLIDER::sti;

////////////////////////////////////////////////////////////////////////////////////////////////
int SLIDER::GetBoundary(RECT * prect, RECT* orect){
	CopyRect(prect, &this->PBoundary);
	CopyRect(orect, &this->OBoundary);
	return SLIDER_OK;
}
////////////////////////////////////////////////////////////////////////////////////////////////
int SLIDER::SetBoundary(RECT * prect, RECT* orect){
	CopyRect(&this->PBoundary, prect);
	CopyRect(&this->OBoundary, orect);
	return SLIDER_OK;
}
////////////////////////////////////////////////////////////////////////////////////////////////
SLIDER::SLIDER(){
	this->IsDragging=FALSE;
	ZeroMemory(&this->sti, sizeof(this->sti));
}

////////////////////////////////////////////////////////////////////////////////////////////////
SLIDER::~SLIDER(){
}

////////////////////////////////////////////////////////////////////////////////////////////////
int SLIDER::CreateSlider(HWND parent, HINSTANCE hInstance,
						 DWORD Orientation, SLIDERSTYLE s, RECT prect, RECT orect,
						 RECT Bprect, RECT Borect){
	OutputDebugString("SLIDER::CreateSlider() Entry\n");
	char Buf[200];
	this->parent=parent;
	this->Orientation=Orientation;
	CopyMemory(&this->SliderStyle, &s, sizeof(s));
	//creates a slider control in the parent window.
	//orientation is SO_VERTICAL for an up/down slider control,
	//orect/prect are the position of it.

	//we need to register a custom class
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize=sizeof(wc);
	wc.cbClsExtra=0;
	wc.cbWndExtra=8;//8 extra bytes allocated for storage of a THIS pointer...see readme for details
	wc.hbrBackground=0;
	wc.hCursor=(Orientation==SO_VERTICAL) ?
		LoadCursor(hInstance, IDC_SIZEWE)
		:
		LoadCursor(hInstance, IDC_SIZENS);
	wc.hIcon=0;
	wc.hIconSm=0;
	wc.hInstance=hInstance;
	wc.lpfnWndProc=(WNDPROC)this->SliderProc;
	wc.lpszClassName=SLIDER_CLASSNAME;
	wc.lpszMenuName=NULL;
	wc.style=CS_HREDRAW | CS_VREDRAW;
	//if this fails, it's probably because there's another slider on the screen already;
	//don't worry about it.
	if(RegisterClassEx(&wc) == 0){
		//failed
		sprintf(Buf, "RegisterClassEx Failed, Error #:%d\n", GetLastError());
		OutputDebugString(Buf);
	}

	this->hwnd=CreateWindowEx(WS_EX_WINDOWEDGE, SLIDER_CLASSNAME, "Mongolians Feet",
		WS_CHILD | WS_VISIBLE, 0, 0, 100, 100,	parent,
		(HMENU)this->GetId(), hInstance, this);//pass a this pointer
		//as window creation data - that will be stored in 8 extra allocated window bytes
	if(this->hwnd==NULL){
		sprintf(Buf, "SLIDER::CreateSlider() Done 01 -- Error #:%d\n", GetLastError());
		OutputDebugString(Buf);
		return SLIDER_ERROR;
	}

	//now that it's created,
	CopyMemory(&this->offsetrect, &orect, sizeof(orect));
	CopyMemory(&this->percentrect, &prect, sizeof(prect));
	CopyRect(&this->PBoundary, &Bprect);
	CopyRect(&this->OBoundary, &Borect);
	this->Size();

	OutputDebugString("SLIDER::CreateSlider() Done 02\n");
	return SLIDER_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT SLIDER::SliderProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam){
	char Buf[100];
	Buf[0]=0;
	/*
	Because this is a static member function, we cant' use a this pointer; we'll have to
	create our own
	*/

	RECT NewRect;//used for creating new coords for the window.
	ZeroMemory(&NewRect, sizeof(NewRect));
	NMHDR nmh;
	ZeroMemory(&nmh, sizeof(nmh));
	int Distance=0;//distance to be calculated for notification messages


	SLIDER * slider;
	if(uMsg==WM_CREATE){
		//if we're creating the window, we need to set the extra allocated window memory to
		//the correct pointer, which should be passed in the lparam struct
		CREATESTRUCT * cs;
		cs=(CREATESTRUCT*)lparam;
		slider=(SLIDER*)cs->lpCreateParams;

		SetWindowLong(hwnd, 0, (LPARAM)slider);
		return 0;
	}

	//well, if it's not being created, get the slider pointer
	slider=(SLIDER*)GetWindowLong(hwnd, 0);

	switch(uMsg){
	case WM_MOUSEMOVE:
		//show the correct cursor
		SetCursor(
			(slider->Orientation==SO_HORIZONTAL) ?
			LoadCursor(NULL, IDC_SIZENS) :
			LoadCursor(NULL, IDC_SIZEWE)
			);
		break;
	case WM_LBUTTONDOWN:
		//we're clear for drag...let's DragDetect()
		slider->StartTracking();
		break;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
		SLIDER::sti.Slider->EndTracking();
		return 0;
		break;
	case WM_PAINT:
		slider->Draw();
		return 0;
		break;
	}

	return DefWindowProc(hwnd, uMsg, wparam, lparam);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int SLIDER::Draw(){
	//this function should only be called from the windows proc function...this draws
	//the slider on teh screen.
	PAINTSTRUCT ps;
	RECT rect;

	//get the windows size
	if(GetClientRect(this->GetHwnd(), &rect)==0){
		OutputDebugString("SLIDER::Draw() - GetClientRect FAILED 01\n");
		return SLIDER_ERROR;
	}

	//let's get the update rect..
	RECT UpdateRect;
	if(GetUpdateRect(this->GetHwnd(), &UpdateRect, FALSE)==0){
		//there's no update region
		return SLIDER_ERROR;
	}

	HDC hdc;
	hdc=BeginPaint(this->GetHwnd(), &ps);
	if(hdc==0){
		//didn't work
		OutputDebugString("SLIDER::Draw() Failed: No hDC!\n");
		return SLIDER_ERROR;
	}

	CopyRect(&rect, &ps.rcPaint);


	//first, do the outline...//////////////////
	switch(this->SliderStyle.OutlineStyle){
	case SS_NONE:
		break;
	default:
		if(this->IsDragging==FALSE){
			DrawEdge(hdc, &rect, EDGE_RAISED, BF_RECT);
		}else{
			DrawEdge(hdc, &rect, EDGE_RAISED, BF_RECT);
		}
		break;
	}

	//then the fill...//////////////////////////
	switch(this->SliderStyle.FillStyle){
	case SS_12_HORIZONTAL:
	case SS_12_VERTICAL:
	case SS_SYSTEM:
	case SS_1234_CORNERS:
	case SS_123_HORIZONTAL:
	case SS_123_VERTICAL:
	case SS_12_ALTERNATINGVERTICAL:
	case SS_12_ALTERNATINGHORIZONTAL:
	case SS_1_SOLID:
	case SS_NONE:
		break;
	}

	////////////////////////////////////////////
	EndPaint(this->GetHwnd(), &ps);
	return SLIDER_OK;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//slides the control from origin to new (those are 2d coords of a mouse cursor)
int SLIDER::Slide(POINTS Origin, POINTS New){
	char Buf[200];
	Buf[0]=0;

	//get the distance, and shift this window accordingly
	int Distance;
	Distance=(this->Orientation==SO_VERTICAL) ?
		New.x-Origin.x :
		New.y-Origin.y ;
	//get the old coordinates of the window and adjust for the new coords
	//we want them to be relative to the upper-left corner of the parent window's client area
	RECT rect;
	WINDOWPLACEMENT wp;
	wp.length=sizeof(wp);
	GetWindowPlacement(hwnd, &wp);
	CopyRect(&rect, &wp.rcNormalPosition);
	if(this->Orientation==SO_VERTICAL){
		rect.left+=Distance;
		rect.right+=Distance;
	}else{
		rect.top+=Distance;
		rect.bottom+=Distance;
	}
	//we now have the new position
	MoveWindow(this->GetHwnd(), rect.left, rect.top,
		rect.right-rect.left,
		rect.bottom-rect.top, TRUE);

	//NOW HERE IS WHERE WE SEND the notification message SLIDER_NM_MOVING::::
	//setup the nmh structure
	NMHDR nmh;
	this->GetNMH(&nmh, SLIDER_NM_MOVING);
	//and send the message
	SendMessage(this->GetParentHWND(), WM_NOTIFY, Distance, (LPARAM)&nmh);

	return SLIDER_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int SLIDER::GetNMH(NMHDR * nmh, UINT Message){
	nmh->code=Message;
	nmh->hwndFrom=this->GetHwnd();
	nmh->idFrom=this->GetId();
	return SLIDER_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT SLIDER::SubTrack(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	char Buf[200];
	Buf[0]=0;

	switch(uMsg){
	case WM_SETCURSOR:
		UINT MouseMsg=HIWORD(lParam);
		if(MouseMsg==WM_MOUSEMOVE){//we're moving the cursor; adjust accordingly.
			SLIDER::sti.Slider->Moving();
			return 0;
		}
		if(MouseMsg==WM_RBUTTONDOWN || MouseMsg==WM_LBUTTONUP){
			SLIDER::sti.Slider->EndTracking();
			return 0;
		}
		break;
	}
	return CallWindowProcA(SLIDER::sti.OldProc, hwnd, uMsg, wParam, lParam);
}
////////////////////////////////////////////////////////////////////////////////////////////////
int SLIDER::StartTracking(){
	if(SLIDER::sti.Subclassed==FALSE){
		POINT a;
		GetCursorPos(&a);
		ScreenToClient(this->GetParentHWND(), &a);

		SLIDER::sti.Origin.x=(short)a.x;
		SLIDER::sti.Origin.y=(short)a.y;
		SLIDER::sti.LastMove.x=(short)a.x;
		SLIDER::sti.LastMove.y=(short)a.y;
		SLIDER::sti.Slider=this;

		this->IsDragging=TRUE;

		///////////////////////////
		BringWindowToTop(this->GetHwnd());

		///Send the Notification Message SLIDER_NM_BEGINMOVE
		NMHDR nmh;
		this->GetNMH(&nmh, SLIDER_NM_BEGINMOVE);
		//and send the message
		SendMessage(this->GetParentHWND(), WM_NOTIFY, this->GetId(), (LPARAM)&nmh);

		//Subclass the parent window
		SLIDER::sti.OldProc=(WNDPROC)GetWindowLong(this->GetParentHWND(), GWL_WNDPROC);
		SetWindowLong(this->GetParentHWND(), GWL_WNDPROC, (LONG)SLIDER::SubTrack);
		SLIDER::sti.Subclassed=TRUE;
	}
	return SLIDER_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int SLIDER::Moving(){
	char Buf[200];
	Buf[0]=0;
	int Distance=0;
	POINTS Origin;
	POINTS New;

	Origin.x=(short)SLIDER::sti.LastMove.x;
	Origin.y=(short)SLIDER::sti.LastMove.y;

	POINT a;
	GetCursorPos(&a);

	ScreenToClient(this->GetParentHWND(), &a);

	New.x=(short)a.x;
	New.y=(short)a.y;

	//make sure the cursor is in the RECT
	BOOL IsWithinBoundary=FALSE;
	RECT NewRect, ClientRect;
	//adjust the p/orects of the boundary according to the parent window's rect
	GetClientRect(this->GetParentHWND(), &ClientRect);
	this->ControlRectToRect(&NewRect, &this->PBoundary, &this->OBoundary, &ClientRect);


	//and test New with the NewRect
	if(New.x < NewRect.right){
		if(New.x > NewRect.left){
			if(New.y < NewRect.bottom){
				if(New.y > NewRect.top){
					IsWithinBoundary=TRUE;
				}
			}
		}
	}

	if(IsWithinBoundary==TRUE){
		this->Slide(Origin, New);

		SLIDER::sti.LastMove.x=a.x;
		SLIDER::sti.LastMove.y=a.y;
	}else{
		this->EndTracking();
	}
	return SLIDER_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int SLIDER::EndTracking(){
	char Buf[100];
	Buf[0]=0;
	if((this->IsDragging==TRUE) && (SLIDER::sti.Subclassed==TRUE)){
		//Stop subclassing
		SetWindowLong(this->GetParentHWND(), GWL_WNDPROC, (LONG)SLIDER::sti.OldProc);
		SLIDER::sti.Subclassed=FALSE;
		this->IsDragging=FALSE;

		///Send the Notification Message SLIDER_NM_DONEMOVING
		int Distance;
		NMHDR nmh;
		if(this->Orientation==SO_VERTICAL){
			Distance=SLIDER::sti.LastMove.x-SLIDER::sti.Origin.x;
		}else{
			Distance=SLIDER::sti.LastMove.y-SLIDER::sti.Origin.y;
		}
		this->GetNMH(&nmh, SLIDER_NM_DONEMOVING);
		//and send the message
		if(SendMessage(this->GetParentHWND(), WM_NOTIFY, Distance, (LPARAM)&nmh)==0){
		}

	}
	return SLIDER_OK;
}