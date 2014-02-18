//SnookieWindowProc.cpp

#include "snookiewindowproc.h"

extern SNOOKIEWINDOWS SnookieWindows;
extern SNOOKIEVARS SnookieVars;
extern SNOOKIECATALOG * pCat;

///////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK SnookieWindowProc(HWND hwnd, UINT uMsg,WPARAM wParam, LPARAM lParam){
	unsigned int wID;
	int wNotifyCode;
	static BOOL IsEditing=FALSE;//is the user editing a label?
	int i;
	char Buf[200];
	Buf[0]=0;

	switch(uMsg){
	case WM_COMMAND:
		wID = LOWORD(wParam);
		wNotifyCode = HIWORD(wParam);
		//clicked on a button
			if(wID==SnookieWindows.EditBox.GetId()){
				//it was the edit box; see if they edited it.
				if(wNotifyCode==EN_CHANGE){
					swpEditChange();
				}
			}

			switch(wID){
			case IDM_FILE_DEBUG:
			case IDA_DEBUG:
				//if it's checked, close the debug window
				HMENU Menu;
				Menu=GetMenu(SnookieWindows.MainWindow);

				if(GetMenuState(Menu, IDM_FILE_DEBUG, MF_BYCOMMAND) && MF_CHECKED){
					CloseDebugWindow();
				}else{
					SetupDebugWindow();
				}
				break;
			case ID_CATALOG_CLOSEALL:
			case IDA_CLOSEALL:
				//close all the trees
				for(i=0;i<pCat->NumberOfNodes;i++){
					SendMessage(SnookieWindows.TreeView.GetHwnd(), TVM_EXPAND, (WPARAM)TVE_COLLAPSE, (LPARAM)pCat->Nodes[i]->hTreeItem);
				}
				pCat->Sel=-1;
				pCat->Refresh();
				break;
			case IDM_CATALOG_CLOSEALLKEEPSEL:
			case IDA_CLOSEALLKEEPSEL:
				//close all trees, but keep the selection there.
				HTREEITEM Sel;
				//get the current selection
				Sel=SnookieWindows.TreeView.GetSel();
				//collapse the list...
				for(i=0;i<pCat->NumberOfNodes;i++){
					SendMessage(SnookieWindows.TreeView.GetHwnd(), TVM_EXPAND, (WPARAM)TVE_COLLAPSE, (LPARAM)pCat->Nodes[i]->hTreeItem);
				}
				//and select that node.
				SendMessage(SnookieWindows.TreeView.GetHwnd(), TVM_SELECTITEM, (WPARAM)TVGN_FIRSTVISIBLE , (LPARAM)Sel);
				SendMessage(SnookieWindows.TreeView.GetHwnd(), TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)Sel);
				break;
			case IDA_LABELEDIT:
			case ID_LABELEDIT:
				swpBeginLabelEdit();
				pCat->Refresh();
				break;
			case ID_FILEEXPORT:
				swpFileExport();
				break;
			case ID_FILE_NEW:
				swpFileNew();
				pCat->Refresh();
				break;
			case ID_FILE_EXIT:
				pCat->Clear();
				PostQuitMessage(0);
				//exit code
				break;
			case IDA_OPEN:
			case ID_FILE_OPEN:
				swpOpen();
				//open
				pCat->Refresh();
				break;
			case IDA_EXPORT:
			case IDM_FILE_EXPORT:
				swpExport(SNOOKIE_EXPORT_TXT);
				break;
			case IDA_SAVE:
			case ID_FILE_SAVE:
				swpSave();
				pCat->Refresh();
				break;
			case IDA_SAVEAS:
			case ID_FILE_SAVEAS:
				swpSaveAs();
				pCat->Refresh();
				break;
			case ID_FILE_CLOSE:
				swpClose();
				pCat->Refresh();
				break;
			case IDA_FIND:
			case ID_SEARCH_FINDFILE:
				swpFindFile();
				break;
			case IDA_FINDNEXT:
			case ID_SEARCH_FINDNEXT:
				swpFindNext();
				break;
			case IDA_ADDPATH:
				if(SnookieWindows.EditBox.GetHwnd() == GetFocus()){
					break;
				}
			case ID_CATALOG_ADDPATH:
				swpAddPath();
				pCat->Refresh();
				break;
			case IDA_REMOVEPATH:
				//this is the accelerator; if they pressed insert or delete
				//while being in teh text box, then we need to make sure that
				//we don't do the action; they are manipulating text
				if(SnookieWindows.EditBox.GetHwnd() == GetFocus()){
					break;
				}
				if(IsEditing==TRUE) break;
			case ID_CATALOG_REMOVE:
				swpRemovePath();
				pCat->Refresh();
				break;
			}
		break;
	case WM_PAINT:
		HDC hDC;
		PAINTSTRUCT ps;
		hDC=BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;
	case WM_CREATE:
		break;
	case WM_DROPFILES:
		//handle a dropped file
		swpDropFiles((HANDLE) wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		//adjust the positions and sizes of windows.
		SnookieWindows.StatusBar.Size();
		SnookieWindows.TreeView.Size();
		SnookieWindows.StaticText_Name.Size();
		SnookieWindows.StaticText_Size.Size();
		SnookieWindows.StaticText_OtherInfo.Size();
		SnookieWindows.EditBox.Size();
		SnookieWindows.VSlider.Size();
		SnookieWindows.HSlider.Size();
		return 0;
		break;
	case WM_NOTIFY:
		//control notification messages go here.
		wID=(unsigned int)wParam;
		LPNMHDR pnmh;
		pnmh=(LPNMHDR) lParam;
		if(pnmh->idFrom==SnookieWindows.VSlider.GetId()){
			switch(pnmh->code){
			case SLIDER_NM_BEGINMOVE:
				return 0;
			case SLIDER_NM_MOVING:
				return 0;
			case SLIDER_NM_DONEMOVING:
				swpAdjustVSlider(wParam);
				return 0;
			}
		}
		if(pnmh->idFrom==SnookieWindows.HSlider.GetId()){
			switch(pnmh->code){
			case SLIDER_NM_BEGINMOVE:
				return 0;
			case SLIDER_NM_MOVING:
				return 0;
			case SLIDER_NM_DONEMOVING:
				swpAdjustHSlider(wParam);
				return 0;
			}
		}
		if(wID == SnookieWindows.TreeView.GetId()){
			//dispatch the code to the control from which the message came
			if(pnmh->hwndFrom==SnookieWindows.TreeView.GetHwnd()){
				//we're ok; now figure out what what's going on.
				switch(pnmh->code){
				case TVN_SELCHANGED:
					//the selection changed; display information accordingly
					char Buf[100];
					NMTREEVIEWA * nm;
					nm=(LPNMTREEVIEW)lParam;
					sprintf(Buf, "**NM: Sel Changed; HTREE: %d\n",nm->itemNew.hItem);
					OutputDebugString(Buf);

					pCat->nmSelect(nm);
					pCat->Refresh();
					break;
				case TVN_BEGINLABELEDIT:
					IsEditing=TRUE;
					return FALSE;
					break;
				case TVN_ENDLABELEDIT:
					NMTVDISPINFO* ptvdi;
					ptvdi = (NMTVDISPINFO*) lParam;
					IsEditing=FALSE;
					return swpLabelEdit(ptvdi)==SWP_OK ? TRUE : FALSE;
					DebugRefresh();
					break;
				}
			}
		}
		break;
	default:
		return DefWindowProc (hwnd, uMsg, wParam, lParam); 
		break;
	}



	return 0;

}
///////////////////////////////////////////////////////////////////////////////////////////////
int swpSaveAs(){
	char Filter[] = "Snookie files (*.cat)\0*.cat\0All Files (*.*)\0*.*\0\0";

	char File[MAX_PATH];
	ZeroMemory(File, sizeof(File));
	OPENFILENAME OpenFileName;
	ZeroMemory(&OpenFileName, sizeof(OPENFILENAME));

	OpenFileName.lStructSize		=sizeof(OPENFILENAME);
	OpenFileName.hwndOwner			=SnookieWindows.MainWindow;
	OpenFileName.hInstance			=0;
	OpenFileName.lpstrFilter		=Filter;
	OpenFileName.lpstrCustomFilter	=0;
	OpenFileName.nMaxCustFilter		=0;
	OpenFileName.nFilterIndex		=0;
	OpenFileName.lpstrFile			=File;
	OpenFileName.nMaxFile			=MAX_PATH;
	OpenFileName.lpstrFileTitle		=0;
	OpenFileName.nMaxFileTitle		=0;
	OpenFileName.lpstrInitialDir	=0;
	OpenFileName.lpstrTitle			="Save catalog";
	OpenFileName.Flags				=OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	OpenFileName.nFileOffset		=0;
	OpenFileName.nFileExtension		=0;
	OpenFileName.lpstrDefExt		="cat";
	OpenFileName.lCustData			=0;
	OpenFileName.lpfnHook			=0;
	OpenFileName.lpTemplateName		=0;

	if(GetSaveFileName(&OpenFileName) != 0){
		//the user pressed the OK button, let's go...
		pCat->SaveAs(File);
	}

	return SWP_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int swpOpen(){
	char OldStatusText[256];
	SnookieWindows.StatusBar.GetText(1, OldStatusText);
	SnookieWindows.StatusBar.SetText(1, "Opening...");

	char Filter[] = "Snookie files (*.cat)\0*.cat\0All Files (*.*)\0*.*\0\0";

	char File[MAX_PATH];
	ZeroMemory(File, sizeof(File));
	OPENFILENAME OpenFileName;
	ZeroMemory(&OpenFileName, sizeof(OPENFILENAME));

	OpenFileName.lStructSize		=sizeof(OPENFILENAME);
	OpenFileName.hwndOwner			=SnookieWindows.MainWindow;
	OpenFileName.hInstance			=0;
	OpenFileName.lpstrFilter		=Filter;
	OpenFileName.lpstrCustomFilter	=0;
	OpenFileName.nMaxCustFilter		=0;
	OpenFileName.nFilterIndex		=0;
	OpenFileName.lpstrFile			=File;
	OpenFileName.nMaxFile			=MAX_PATH;
	OpenFileName.lpstrFileTitle		=0;
	OpenFileName.nMaxFileTitle		=0;
	OpenFileName.lpstrInitialDir	=0;
	OpenFileName.lpstrTitle			="Load catalog";
	OpenFileName.Flags				=OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	OpenFileName.nFileOffset		=0;
	OpenFileName.nFileExtension		=0;
	OpenFileName.lpstrDefExt		="cat";
	OpenFileName.lCustData			=0;
	OpenFileName.lpfnHook			=0;
	OpenFileName.lpTemplateName		=0;

	if(GetOpenFileName(&OpenFileName) != 0){
		//the user pressed the OK button, let's go...
		pCat->Open(File, pCat->GetRoot());
	}

	TreeView_Select(SnookieWindows.TreeView.GetHwnd(), TreeView_GetRoot(SnookieWindows.TreeView.GetHwnd()), TVGN_CARET);
	SnookieWindows.StatusBar.SetText(1, OldStatusText);
	return SWP_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int swpSave(){
	if(pCat->OnDisk==TRUE){
		char OldStatusText[256];
		SnookieWindows.StatusBar.GetText(1, OldStatusText);
		SnookieWindows.StatusBar.SetText(1, "Saving...");
		pCat->Save();
		SnookieWindows.StatusBar.SetText(1, OldStatusText);
	}else{
		swpSaveAs();
	}
	return SWP_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int swpFindFile(){
	//displays the find file box, selects the first node, and searches fromthere
	char OldStatusText[256];
	SnookieWindows.StatusBar.GetText(1, OldStatusText);
	SnookieWindows.StatusBar.SetText(1, "Finding...");

	//display the IDD_FINDFILE dialog box and get a name
	char FindName[MAX_PATH];
	ZeroMemory(FindName, sizeof(FindName));

	DialogBoxParam(SnookieVars.hInstance, MAKEINTRESOURCE(IDD_FIND),
		SnookieWindows.MainWindow, FindFileProc, (LPARAM)FindName);
	if(strlen(FindName)!=0){
		//we have a new find file name, and we can search now.
		HWND tv;
		tv=SnookieWindows.TreeView.GetHwnd();
		TreeView_Select(tv, TreeView_GetRoot(tv),TVGN_CARET);
		SnookieWindows.StatusBar.SetText(2, FindName);
		pCat->SetSearchString(FindName, strlen(FindName)+1);
		pCat->SearchTree(TreeView_GetSelection(tv));
	}

	SetFocus(SnookieWindows.MainWindow);
	SnookieWindows.StatusBar.SetText(1, OldStatusText);
	return SWP_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int swpFindNext(){
	//selects teh next node and searches from there
	char OldStatusText[256];
	SnookieWindows.StatusBar.GetText(1, OldStatusText);
	SnookieWindows.StatusBar.SetText(1, "Finding Next...");

	//get the next node in the tree and start looking.
	//SnookieWindows.TreeView.SelectNext();
	HTREEITEM ht;
	ht=SnookieWindows.TreeView.GetNext();
	if(ht != 0) pCat->SearchTree(ht);
	SnookieWindows.StatusBar.SetText(1, OldStatusText);
	return SWP_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int swpClose(){
	//prompt for close, and then do File->New
	pCat->Clear();
	pCat->SetupRoot();
	return SWP_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int swpAddPath(){
	//get the name of a folder
	char DisplayName[MAX_PATH];
	char FolderName[MAX_PATH];
	char Title[50];
	strcpy(Title, "Select the path to add to the catalog and then click \"OK.\"");

	BROWSEINFO  bi;
	bi.hwndOwner=		SnookieWindows.MainWindow;
	bi.pidlRoot=		NULL;
	bi.pszDisplayName=	DisplayName;
	bi.lpszTitle=		Title;
	bi.ulFlags=			0;
	bi.lpfn=			NULL;
	bi.lParam=			NULL;
	bi.iImage=			0;

	ITEMIDLIST *IDList;
	IDList=SHBrowseForFolder(&bi);
	if(IDList != NULL){
		//they clicked on OK, so let's add the path
		int NewOne;
		SHGetPathFromIDList(IDList, FolderName);
		SN_FFD ffd;
		ZeroMemory(&ffd, sizeof(SN_FFD));
		NewOne=pCat->FillFromDisk(FolderName, pCat->GetRoot(), &ffd);
		pCat->Nodes[pCat->GetRoot()]->Size+=ffd.Size;

		TreeView_EnsureVisible(SnookieWindows.TreeView.GetHwnd(), pCat->Nodes[NewOne]->hTreeItem);
		SnookieWindows.TreeView.SetSel(pCat->Nodes[NewOne]->hTreeItem);
	}

	
	return SWP_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int swpRemovePath(){
	//figure out which one is selected, and delete it.
	HTREEITEM hti;
	HWND tv;
	tv=SnookieWindows.TreeView.GetHwnd();
	hti=TreeView_GetSelection(tv);
	//if there's nothing selected, then don't delete anything
	if(hti != 0){
		//prompt for confirmation
//		if(MessageBox(SnookieWindows.MainWindow, "Are you sure?", "Delete path", MB_YESNO | MB_ICONEXCLAMATION)==IDYES){
			//remove the path
			//get the node
			pCat->Remove(GetNodeFromTree(hti)->Id);
//		}
	}
	return SWP_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////
SNOOKIENODE* GetNodeFromTree(HTREEITEM ht){
	return (SNOOKIENODE*) SnookieWindows.TreeView.GetLParam(ht);
}
///////////////////////////////////////////////////////////////////////////////////////////////
int swpFileNew(){
	pCat->Clear();
	pCat->SetupRoot();
	return SWP_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int swpFileExport(){
	return SWP_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int swpEditChange(){
	return SWP_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
int swpLabelEdit(NMTVDISPINFO* ptvdi){
	//if the label equals the name, don't change it
	//if it's different, then use it.
	SNOOKIENODE* pNode;
	pNode=pCat->Nodes[pCat->Sel];

	//figure out if we need to change the label.
	//hItem, lParam, and pszText members are valid.
	if(ptvdi->item.pszText==0) return SWP_ERROR;//they cancelled it, or there's no label.

	//deallocate and delete previous label/allocate for new label
	if(pNode->Name !=0){
		delete pNode->Name;
		pNode->Name=0;
	}

	//copy the label and length to the folder.
	int len;
	len=strlen(ptvdi->item.pszText);
	pNode->Name=new char[len+1];
	strcpy(pNode->Name, ptvdi->item.pszText);
	pNode->NameLength=len;
	//and update it in the properties display.
	pCat->Refresh();
	return SWP_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
int swpBeginLabelEdit(){
	TreeView_EditLabel(SnookieWindows.TreeView.GetHwnd(),
		TreeView_GetSelection(SnookieWindows.TreeView.GetHwnd()));
	return SWP_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
int swpAdjustVSlider(int Distance){
	//this is for the vertical slider on the screen
	RECT orect, prect;//prect/orects for all CONTROLS::controls
	RECT Window;//boundaries for the window
	int WindowWidth;//width of the parent window
	float Percent;//percent of window width that distance is

	//get the client size of the main window
	GetClientRect(SnookieWindows.MainWindow, &Window);
	WindowWidth=Window.right-Window.left;
	Percent=100*((float)Distance/(float)WindowWidth);
	
	//first, do the treeview
	SnookieWindows.TreeView.GetPlacement(&prect, &orect);
	prect.right+=(long)Percent;
	SnookieWindows.TreeView.SetPlacement(&prect, &orect);

	//then the slider
	SnookieWindows.VSlider.GetPlacement(&prect, &orect);
	prect.right+=(long)Percent;
	prect.left+=(long)Percent;
	SnookieWindows.VSlider.SetPlacement(&prect, &orect);
	//then all the static texts
	SnookieWindows.StaticText_Name.GetPlacement(&prect, &orect);
	prect.left+=(long)Percent;
	SnookieWindows.StaticText_Name.SetPlacement(&prect, &orect);

	SnookieWindows.StaticText_OtherInfo.GetPlacement(&prect, &orect);
	prect.left+=(long)Percent;
	SnookieWindows.StaticText_OtherInfo.SetPlacement(&prect, &orect);

	SnookieWindows.StaticText_Size.GetPlacement(&prect, &orect);
	prect.left+=(long)Percent;
	SnookieWindows.StaticText_Size.SetPlacement(&prect, &orect);
	
	//and update the damn window
	InvalidateRect(SnookieWindows.MainWindow, NULL, TRUE);
	UpdateWindow(SnookieWindows.MainWindow);
		SnookieWindows.StatusBar.Size();
		SnookieWindows.TreeView.Size();
		SnookieWindows.StaticText_Name.Size();
		SnookieWindows.StaticText_Size.Size();
		SnookieWindows.StaticText_OtherInfo.Size();
		SnookieWindows.EditBox.Size();
		SnookieWindows.VSlider.Size();
		SnookieWindows.HSlider.Size();

	return SWP_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int swpAdjustHSlider(int Distance){
	//this is for the horizontal slider on the screen
	RECT orect, prect;//prect/orects for all CONTROLS::controls
	RECT Window;//boundaries for the window
	int WindowHeight;//width of the parent window
	float Percent;//percent of window width that distance is

	//get the client size of the main window
	GetClientRect(SnookieWindows.MainWindow, &Window);
	WindowHeight=Window.bottom-Window.top;
	Percent=100*((float)Distance/(float)WindowHeight);

	//first, do the treeview
	SnookieWindows.TreeView.GetPlacement(&prect, &orect);
	prect.bottom+=(long)Percent;
	SnookieWindows.TreeView.SetPlacement(&prect, &orect);

	SnookieWindows.EditBox.GetPlacement(&prect, &orect);
	prect.top+=(long)Percent;
	SnookieWindows.EditBox.SetPlacement(&prect, &orect);

	SnookieWindows.HSlider.GetPlacement(&prect, &orect);
	prect.top+=(long)Percent;
	prect.bottom+=(long)Percent;
	SnookieWindows.HSlider.SetPlacement(&prect, &orect);

	SnookieWindows.StatusBar.GetPlacement(&prect, &orect);
	prect.top+=(long)Percent;
	SnookieWindows.StatusBar.SetPlacement(&prect, &orect);
	
	SnookieWindows.VSlider.GetPlacement(&prect, &orect);
	prect.bottom+=(long)Percent;
	SnookieWindows.VSlider.SetPlacement(&prect, &orect);

	//adjust the boundaries for the vslider
	SnookieWindows.VSlider.GetBoundary(&prect, &orect);
	prect.bottom+=(long)Percent;
	SnookieWindows.VSlider.SetBoundary(&prect, &orect);

	//and update the damn window
	InvalidateRect(SnookieWindows.MainWindow, NULL, TRUE);
	UpdateWindow(SnookieWindows.MainWindow);
	SnookieWindows.StatusBar.Size();
	SnookieWindows.TreeView.Size();
	SnookieWindows.StaticText_Name.Size();
	SnookieWindows.StaticText_Size.Size();
	SnookieWindows.StaticText_OtherInfo.Size();
	SnookieWindows.EditBox.Size();
	SnookieWindows.VSlider.Size();
	SnookieWindows.HSlider.Size();

	return SWP_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////
int swpDropFiles(HANDLE hDrop){
	char FileName[MAX_PATH];

	char OldStatusText[256];
	SnookieWindows.StatusBar.GetText(1, OldStatusText);
	SnookieWindows.StatusBar.SetText(1, "Opening...");

	DragQueryFile((HDROP)hDrop, 0, FileName, MAX_PATH);//get the first filename
	OutputDebugString(FileName);
	if(pCat->NumberOfNodes > 1){
		//there's a catalog already there
	}
	pCat->Open(FileName, pCat->GetRoot());
	SnookieWindows.TreeView.SetSel(pCat->Nodes[pCat->GetRoot()]->hTreeItem);

	SnookieWindows.StatusBar.SetText(1, OldStatusText);
	return SWP_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////
int swpExport(int ExportMethod){
	//get a file name, and then export it.
	char Filter[200];
	char FileName[MAX_PATH];
	OPENFILENAME OpenFileName;
	ZeroMemory(FileName, sizeof(FileName));
	ZeroMemory(&OpenFileName, sizeof(OPENFILENAME));

	//get the filter for the dialogue box
	switch(ExportMethod){
	case SNOOKIE_EXPORT_TXT:
		sprintf(Filter, "Text files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0");
		break;
	}

	OpenFileName.lStructSize		=sizeof(OPENFILENAME);
	OpenFileName.hwndOwner			=SnookieWindows.MainWindow;
	OpenFileName.hInstance			=0;
	OpenFileName.lpstrFilter		=Filter;
	OpenFileName.lpstrCustomFilter	=0;
	OpenFileName.nMaxCustFilter		=0;
	OpenFileName.nFilterIndex		=0;
	OpenFileName.lpstrFile			=FileName;
	OpenFileName.nMaxFile			=MAX_PATH;
	OpenFileName.lpstrFileTitle		=0;
	OpenFileName.nMaxFileTitle		=0;
	OpenFileName.lpstrInitialDir	=0;
	OpenFileName.lpstrTitle			="Export";
	OpenFileName.Flags				=OFN_HIDEREADONLY;
	OpenFileName.nFileOffset		=0;
	OpenFileName.nFileExtension		=0;
	OpenFileName.lpstrDefExt		="txt";
	OpenFileName.lCustData			=0;
	OpenFileName.lpfnHook			=0;
	OpenFileName.lpTemplateName		=0;

	if(GetOpenFileName(&OpenFileName) == 0){
		//the user decided to chicken out...
		return SWP_OK;
	}

	//so the filename is FileName
	CATFILE File;
	File.OpenForWrite(FileName);
	char Buf[400];
	char Buf2[400];
	Buf2[0]=0;
	int Level=0;//# of levels up a directory (for indents)
	SNOOKIENODE * Node;
	Node=pCat->Nodes[pCat->GetRoot()];
	int x;
	int Iterations, Relationship;

	for(int i=0;i<pCat->NumberOfNodes;i++){
		Buf[0]=0;
		for(x=1;x<=Level;x++){
			strcat(Buf, "\t");
		}
		strcat(Buf, Node->Name);
		strcat(Buf, "\r\n");
		File.Write(Buf, strlen(Buf));

		//now get the next node
		Node=GetNextNode(Node, &Relationship, &Iterations);
		//and adjust accordingly the tabs.
		if(Relationship==GNN_PARENT){
			Level-=Iterations;
		}
		if(Relationship==GNN_FIRSTCHILD){
			Level++;
		}
	}

	File.Close();

	return SWP_OK;
}