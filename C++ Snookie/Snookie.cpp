#include "snookie.h"
int AuxStuff();
SNOOKIEVARS SnookieVars;
SNOOKIEWINDOWS SnookieWindows;
SNOOKIECATALOG * pCat;

///////////////////////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	OutputDebugString("-----------------------------------------------------------------\n");
	MSG Msg;
	//load the accelerator list
	SnookieVars.hAccelerators=LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATORS));

	//figure out which drive is a CD-Rom drive and put its path in SnookieVars.CDRomPath
	///////////////////////////////////////////////////
	//Go from drive A to Z and determine if it exists
	char i;
	char TestDrive[10];
	UINT rvalue;
	for(i='A';i<='Z';i++){
		sprintf(TestDrive, "%c:\\",i);
		rvalue=GetDriveType(TestDrive);
		if(rvalue==DRIVE_CDROM){
			//it's a cdrom
			strcpy(SnookieVars.CDRomPath, TestDrive);
			break;
		}
	}	
	///////////////////////////////////////////////////

	SnookieVars.hInstance=hInstance;
	SnookieVars.hPrevInstance=hPrevInstance;

	if(SetupWindow(&SnookieVars, nCmdShow)==SNOOKIE_ERROR){
		return 0;
	}

	SNCATCONTROLS scc;
	scc.tv=&SnookieWindows.TreeView;
	scc.StatusBar=&SnookieWindows.StatusBar;
	scc.Static0=&SnookieWindows.StaticText_Name;
	scc.Static1=&SnookieWindows.StaticText_Size;
	scc.Static2=&SnookieWindows.StaticText_OtherInfo;

	SVSTRUCT sv;
	sv.hInstance=hInstance;
	SNOOKIECATALOG Cat(scc, sv);

	pCat=&Cat;

	SetLastError(0);

	SetupDebugWindow();
	CloseDebugWindow();

	while(GetMessage(&Msg, NULL, 0, 0)){
		TranslateAccelerator(SnookieWindows.MainWindow, SnookieVars.hAccelerators, &Msg);
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	CloseDebugWindow();
	OutputDebugString("-----------------------------------------------------------------\n");
	return 0;
}



///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT SetupWindow(SNOOKIEVARS* SnookieVars, int nCmdShow){
	//setup the window and controls
	WNDCLASSEX MainWindow;
	MainWindow.cbSize=sizeof(MainWindow);
	MainWindow.hIconSm=0;
	MainWindow.cbClsExtra=0;
	MainWindow.cbWndExtra=0;
	MainWindow.hbrBackground=(HBRUSH)(COLOR_WINDOW);
	MainWindow.hCursor=LoadCursor(NULL, IDC_ARROW);
	MainWindow.hIcon=(HICON)LoadIcon(SnookieVars->hInstance, (const char *) IDI_ICON1);
	MainWindow.hInstance=SnookieVars->hInstance;
	MainWindow.lpfnWndProc=(WNDPROC)SnookieWindowProc;
	MainWindow.lpszClassName="SnookieWindow";
	MainWindow.lpszMenuName=MAKEINTRESOURCE(IDR_MENU1);
	MainWindow.style=CS_VREDRAW | CS_HREDRAW;
	
	//register the class
	if(RegisterClassEx(&MainWindow)==0){
		return SNOOKIE_ERROR;
	}

	//get the name of the window
	char WindowName[200];
	sprintf(WindowName, "Snookie V%d", VERSION);
	
	//create the window
	SnookieVars->hSnookieWindow=CreateWindowEx(WS_EX_ACCEPTFILES,"SnookieWindow", WindowName, WS_OVERLAPPEDWINDOW,
		0, 0, 500, 500, NULL, NULL, SnookieVars->hInstance, NULL);
	if(SnookieVars->hSnookieWindow==NULL){
		return SNOOKIE_ERROR;
	}

	SnookieWindows.MainWindow=SnookieVars->hSnookieWindow;
	//and finally, show it:
	ShowWindow(SnookieVars->hSnookieWindow, nCmdShow);

	AuxStuff();

	return SNOOKIE_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////
int AuxStuff(){
	RECT prect;
	RECT orect;
	int StatusBarHeight;
	//////////////////////////////
	SnookieWindows.StatusBar.Create(SnookieWindows.MainWindow);

	STATUSBARPART tx[4];
	int width;
	GetClientRect(SnookieWindows.MainWindow, &prect);
	width=prect.right-prect.left;
	tx[0].WidthPercent=40;
	tx[1].WidthPercent=20;
	tx[2].WidthPercent=20;
	tx[3].WidthPercent=20;//this one doesn't make a difference; it's always to the edge of the window
	sprintf(tx[0].Text, "Snookie Catalog V%d", VERSION);
	strcpy(tx[1].Text, "(idle)");
	strcpy(tx[2].Text, "No find text");
	strcpy(tx[3].Text, "");
	SnookieWindows.StatusBar.SetParts(4, tx);
	//now find the height of it, so we can correctly size stuff around it.
	RECT trect;
	GetWindowRect(SnookieWindows.StatusBar.GetHwnd(), &trect);
	StatusBarHeight=trect.bottom-trect.top;
	
	/////////////////////////////////////////////////////
	//TREEVIEW
	SetRect(&prect, 0, 0, 50, 80);
	SetRect(&orect, 0, 0, -6, 0);
	if(SnookieWindows.TreeView.CreateTreeView(prect, orect, SnookieWindows.MainWindow, SnookieVars.hInstance)==TREEVIEW_ERROR){
		return 0;
	}

	////////////////////////////////////////
	SetRect(&prect, 51, 0, 100, 15);
	SnookieWindows.StaticText_Name.CreateStaticText( SnookieWindows.MainWindow, SnookieVars.hInstance, 0, prect, orect);
	SnookieWindows.StaticText_Name.SetText("");
	SnookieWindows.StaticText_Name.SetFont("Ariel", 30);
	////////////////////////////////////////
	SetRect(&prect, 51, 15, 100, 30);
	SnookieWindows.StaticText_Size.CreateStaticText( SnookieWindows.MainWindow, SnookieVars.hInstance, 0, prect, orect);
	SnookieWindows.StaticText_Size.SetText("");
	SnookieWindows.StaticText_Size.SetFont("Courier New", 16);
	////////////////////////////////////////*/
	SetRect(&prect, 51, 30, 100, 50);
	SnookieWindows.StaticText_OtherInfo.CreateStaticText( SnookieWindows.MainWindow, SnookieVars.hInstance, 0, prect, orect);
	SnookieWindows.StaticText_OtherInfo.SetText("");
	SnookieWindows.StaticText_OtherInfo.SetFont("Courier New", 16);
	////////////////////////////////////////
	SetRect(&prect, 0, 80, 100, 100);
	SetRect(&orect, 0, 6, 0, -StatusBarHeight);
	SnookieWindows.EditBox.CreateEditBox(SnookieWindows.MainWindow, prect, orect, SnookieVars.hInstance);
	//////////////////////////////////////////
	//create the Vertical slider
	SetRect(&prect, 50, 0, 50, 80);
	SetRect(&orect, -6,0,0,0);
	SLIDERSTYLE s;
	GETRGBTRIPLE(s.FillColor1, 128,0,128);
	GETRGBTRIPLE(s.FillColor2, 0,128,128);
	GETRGBTRIPLE(s.FillColor3, 128,70,60);
	GETRGBTRIPLE(s.FillColor4, 70,120,208);
	s.FillStyle=SS_12_HORIZONTAL;
	s.OutlineStyle=SS_1_SOLID;
	s.OutlineWidth=1;
	GETRGBTRIPLE(s.OutlineColor, 0,0,0);
	RECT Borect, Bprect;//boundary rectangle for this window
	SetRect(&Bprect, 10, 0, 90,80);
	SetRect(&Borect, 0,0,0,0);
	SnookieWindows.VSlider.CreateSlider(SnookieWindows.MainWindow, SnookieVars.hInstance,
		SO_VERTICAL, s, prect, orect, Bprect, Borect);
	//////////////////////////////////////////
	//create the Horizontal slider
	SetRect(&prect, 0,80,100,80);
	SetRect(&orect, 0,0,0,6);
	SetRect(&Bprect, 10,10,90,100);
	SetRect(&Borect, 0, 0, 0, 0);
	SnookieWindows.HSlider.CreateSlider(SnookieWindows.MainWindow, SnookieVars.hInstance,
		SO_HORIZONTAL, s, prect, orect, Bprect, Borect);

	return 0;
}