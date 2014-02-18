//debugwindow.cpp

#include "debugwindow.h"

extern SNOOKIEWINDOWS SnookieWindows;
extern SNOOKIEVARS SnookieVars;
extern SNOOKIECATALOG * pCat;

#define DEBUGWINDOWCLASS "Debugger"
////////////////////////////////////////////////////////////////////////////////////////////////
int SetupDebugWindow(){
	OutputDebugString("SetupDebugWindow : ENTRY\n");
		//setup the window and controls
	WNDCLASS MainWindow;
	MainWindow.cbClsExtra=0;
	MainWindow.cbWndExtra=0;
	MainWindow.hbrBackground=(HBRUSH)(COLOR_WINDOW);
	MainWindow.hCursor=LoadCursor(NULL, IDC_ARROW);
	MainWindow.hIcon=(HICON)LoadIcon(SnookieVars.hInstance, (const char *) IDI_ICON1);
	MainWindow.hInstance=SnookieVars.hInstance;
	MainWindow.lpfnWndProc=(WNDPROC)DebugWindowProc;
	MainWindow.lpszClassName=DEBUGWINDOWCLASS;
	MainWindow.lpszMenuName=0;
	MainWindow.style=CS_VREDRAW | CS_HREDRAW;
	
	//register the class
	if(RegisterClass(&MainWindow)==0){
		OutputDebugString("SetupDebugWindow : DONE 01\n");
		return DEBUG_ERROR;
	}
	
	//create the window
	SnookieWindows.DebugWindow=CreateWindow(DEBUGWINDOWCLASS, "Snookie Debug Window", WS_OVERLAPPEDWINDOW ^ WS_SYSMENU,
		500, 0, 400, 500, NULL, NULL, SnookieVars.hInstance, NULL);
	if(SnookieWindows.DebugWindow==NULL){
		OutputDebugString("SetupDebugWindow : DONE 02\n");
		return DEBUG_ERROR;
	}

	//and finally, show it:
	ShowWindow(SnookieWindows.DebugWindow, SW_SHOW);

	RECT prect, orect;
	SetRect(&prect, 0,0,100,50);
	SetRect(&orect, 0, 0, 0, 0);
	SnookieWindows.DebugEdit.CreateEditBox(SnookieWindows.DebugWindow, prect, orect, SnookieVars.hInstance);

	HMENU Menu;
	Menu=GetMenu(SnookieWindows.MainWindow);
	CheckMenuItem(Menu, IDM_FILE_DEBUG, MF_CHECKED);
	OutputDebugString("SetupDebugWindow : DONE 03\n");
	return DEBUG_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int DebugRefresh(){
	RECT rect;
	GetClientRect(SnookieWindows.DebugWindow, &rect);
	InvalidateRect(SnookieWindows.DebugWindow, &rect, TRUE);
	return DEBUG_OK;
}
////////////////////////////////////////////////////////////////////////////////////////////////
int PaintDebugRefresh(){
	if(pCat->Nodes != 0){//make sure we have any info.
	if(pCat->GetRoot() != -1){//make sure we have a root
	char Text[1000];
	char buf[500];
	sprintf(Text, "Number Of Nodes: %I64d\r\n", pCat->NumberOfNodes);
	sprintf(buf, "Allocated: %d\r\n", pCat->LastIndex);
	strcat(Text, buf);
	sprintf(buf, "Next ID Available: %d\r\n\r\n", pCat->NextAvailable);
	strcat(Text, buf);

	if(pCat->Sel != -1){
		SNOOKIENODE * sel;
		sel=pCat->Nodes[pCat->Sel];
		sprintf(buf, "Selected: %s\r\n", sel->Name);
		strcat(Text, buf);
		sprintf(buf, "Selected ID: %d\r\n", sel->Id);
		strcat(Text, buf);
		sprintf(buf, "Selected HTREE: %d\r\n", sel->hTreeItem);
		strcat(Text, buf);


		if(sel->Parent != 0){
			sprintf(buf, "Sel Parent: %s\r\n", sel->Parent->Name);
			strcat(Text, buf);
		}else{
			strcat(Text, "Sel Parent: [none]\r\n");
		}

		if(sel->FirstChild != 0){
			sprintf(buf, "Sel First Child: %s\r\n", sel->FirstChild->Name);
			strcat(Text, buf);
		}else{
			strcat(Text, "Sel First Child: [none]\r\n");
		}

		if(sel->PreviousSibling != 0){
			sprintf(buf, "Sel Prev Sibling: %s\r\n", sel->PreviousSibling->Name);
			strcat(Text, buf);
		}else{
			strcat(Text, "Sel Prev Sibling: [none]\r\n");
		}

		if(sel->NextSibling != 0){
			sprintf(buf, "Sel Next Sibling: %s\r\n", sel->NextSibling->Name);
			strcat(Text, buf);
		}else{
			strcat(Text, "Sel Next Sibling: [none]\r\n");
		}

		if(sel->LastChild != 0){
			sprintf(buf, "Sel Last Child: %s\r\n", sel->LastChild->Name);
			strcat(Text, buf);
		}else{
			strcat(Text, "Sel Last Child: [none]\r\n");
		}

		sprintf(buf, "Sel FileSize: %I64d\r\n", sel->Size);
		strcat(Text, buf);
		sprintf(buf, "Sel Mem Size: %d\r\n", (sizeof(char)*(sel->NameLength+sel->MemoLength))+sizeof(SNOOKIENODE));
		strcat(Text, buf);
		sprintf(buf, "Address: %d\r\n", sel);
		strcat(Text, buf);
	}

	SnookieWindows.DebugEdit.SetText(Text);

	//and this is cool - create the table so we can visually see the NAT
	if((pCat->LastIndex != -1) && (pCat->NAT != 0)){
		//first, figure out the dimentions that it will be.
		RECT rect;//rect of the box area
		int x,y;//dimentions of area to cover
		int boxw, boxh;//size of boxes
		int boxn;//number of boxes
		int boxx;//number of boxes across
		HBRUSH Selected, Available, Occupied;//the colors for the boxes
		HBRUSH NextAvailable;
		HBRUSH * Brush;//pointer to the brush we'll use
		RECT dest;//the actual rect for a particular box
		int Row, Column;//the row/column for a partiuclar box
		HDC hDC;//handle to the device context for the winodw

		GetClientRect(SnookieWindows.DebugWindow, &rect);
		x=rect.right-rect.left;
		y=rect.bottom-rect.top;
		y=y/2;
		rect.top=y;
		//now we know the part of the screen we can write to
		boxn=(int)pCat->LastIndex;
		boxw=6;
		boxh=6;
		boxx=x/boxw;
		//create the brushes
		COLORREF col;
		col=RGB(0,0,0);
		Available=CreateSolidBrush(col);
		col=RGB(255,0,0);
		Selected=CreateSolidBrush(col);
		col=RGB(80,0,255);
		Occupied=CreateSolidBrush(col);
		col=RGB(0,255,100);
		NextAvailable=CreateSolidBrush(col);

		if(Available==0) OutputDebugString("Available not available\n");
		if(Selected==0) OutputDebugString("Selected not available\n");
		if(Occupied==0) OutputDebugString("Occupied not available\n");
		if(NextAvailable==0) OutputDebugString("NextAvailable not available\n");

		hDC=GetDC(SnookieWindows.DebugWindow);
		if(hDC==NULL) OutputDebugString("NO DC!!\n");
		FillRect(hDC, &rect, (HBRUSH)(COLOR_WINDOW));
		for(int c=0;c<=boxn+1;c++){
			Row=c/(boxx);
			Column=c-(Row*boxx);
			dest.top=Row*boxh+rect.top;
			dest.bottom=dest.top+boxh-1;
			dest.left=Column*boxw;
			dest.right=dest.left+boxw-1;

			//get the appropriate brush
			if(pCat->NAT[c]==TRUE) Brush=&Available;
			if(pCat->NAT[c]==FALSE) Brush=&Occupied;
			if(pCat->Sel==c) Brush=&Selected;
			if(pCat->NextAvailable==c) Brush=&NextAvailable;
			if(FillRect(hDC, &dest, *Brush)==0) OutputDebugString("FillRect Error\n");
		}

		ReleaseDC(SnookieWindows.DebugWindow, hDC);
		DeleteObject(Available);
		DeleteObject(Selected);
		DeleteObject(Occupied);
	}
}}
	return DEBUG_ERROR;
}

////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK DebugWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg){
	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		PaintDebugRefresh();
		EndPaint(hwnd, &ps);
		return 0;
		break;
	case WM_SIZE:
		SnookieWindows.DebugEdit.Size();
		break;
//	case WM_CLOSE:
	case WM_DESTROY:
		HMENU Menu;
		Menu=GetMenu(SnookieWindows.MainWindow);
		CheckMenuItem(Menu, IDM_FILE_DEBUG, MF_UNCHECKED);
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int CloseDebugWindow(){
	DestroyWindow(SnookieWindows.DebugWindow);
	SnookieWindows.DebugWindow=0;
	UnregisterClass(DEBUGWINDOWCLASS, SnookieVars.hInstance);
	return 0;
}
