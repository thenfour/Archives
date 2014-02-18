//currentcat.cpp

#include "currentcat.h"

extern SNOOKIEWINDOWS SnookieWindows;
extern SNOOKIEVARS SnookieVars;

///////////////////////////////////////////////////////////////////////////////////////////
CURRENTCAT::CURRENTCAT(){
	ZeroMemory(this->FileName, sizeof(this->FileName));
	this->Modified=FALSE;
	this->CurrentSelection=NULL;
	this->OnDisk=FALSE;
	this->NumberOfPaths=0;
	int i;
	for(i=0;i<NPATHS;i++){
		this->Paths[i]=new SNOOKIECATALOG;
	}
	this->MemoryUsed=0;
}
///////////////////////////////////////////////////////////////////////////////////////////
int CURRENTCAT::SetupIcons(){
	//now read in all the icons.
	HBITMAP norm, sel;
	norm=LoadBitmap(SnookieVars.hInstance, MAKEINTRESOURCE(IDB_FOLDER_NORM));
	sel=LoadBitmap(SnookieVars.hInstance, MAKEINTRESOURCE(IDB_FOLDER_SEL));
	SnookieWindows.TreeView.AddImage(&this->ImageId[CATIMAGE_FOLDER], norm, sel);
	DeleteObject(norm);
	DeleteObject(sel);

	norm=LoadBitmap(SnookieVars.hInstance, MAKEINTRESOURCE(IDB_FILE_NORM));
	sel=LoadBitmap(SnookieVars.hInstance, MAKEINTRESOURCE(IDB_FILE_NORM));
	SnookieWindows.TreeView.AddImage(&this->ImageId[CATIMAGE_FOLDER], norm, sel);
	DeleteObject(norm);
	DeleteObject(sel);
	return CURRENTCAT_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
CURRENTCAT::~CURRENTCAT(){
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
SNOOKIEFOLDER * CURRENTCAT::GetSelected(){
	HTREEITEM hti;
	hti=TreeView_GetSelection(SnookieWindows.TreeView.GetHwnd());

	return GetFolderFromTree(hti);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
int CURRENTCAT::Clear(){
	//if there is any modification to the cat, prompt...
/*	int rvalue;
	if(this->Modified==TRUE){
		rvalue=MessageBox(SnookieWindows.MainWindow, "Are you sure you want to do this?", "Snookie", MB_YESNO | MB_ICONEXCLAMATION);
		switch(rvalue){
		case IDOK:
			break;
		case IDNO:
			return CURRENTCAT_ERROR;
			break;
		default:
			break;
		}
	}
*/
	int i;
	int i2;
	i2=this->NumberOfPaths;
	for(i=0;i<i2;i++){
		this->RemovePath(0);
	}
	ZeroMemory(this->FileName, sizeof(this->FileName));
	this->FindFileName[0]=(char)0;
	this->CurrentSelection=0;
	this->FileName[0]=(char)0;
	this->OnDisk=FALSE;
	this->Modified=FALSE;

	TreeView_DeleteAllItems(SnookieWindows.TreeView.GetHwnd());
	return CURRENTCAT_OK;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
int CURRENTCAT::FindFile(char * Name){
	//sets the find file text in the object and then finds next.
	strcpy(this->FindFileName, Name);

	///////////////////////////////////
	//now select the topmost item in the treeview.
	HWND tv;//this is the treeview
	tv=SnookieWindows.TreeView.GetHwnd();
	TreeView_Select(tv, TreeView_GetRoot(tv),TVGN_CARET);

	SetFocus(tv);
	//now we have the root selected ; let's find the text.
	this->FindNext(TreeView_GetSelection(tv));
	return CURRENTCAT_OK;
}
//////////////////////////////////////////////////////////////////////////////
int CURRENTCAT::FindNext(HTREEITEM start){
	char OldStatusText[256];
	SnookieWindows.StatusBar.GetText(3, OldStatusText);

	//starts at the currently selected place and searches.  If nothing is selected,
	//start at the root.
	//but if there is no search string, then forget it
	if(strlen(this->FindFileName)==0) return CURRENTCAT_ERROR;
	//Make sure that the strings are formatted correctly.
	//make it uppercase
	strupr(this->FindFileName);
	char buf[MAX_PATH];
	HWND tv;//just another way of saying "SnookieWindows.TreeView.TreeViewVars.hTreeView"
	tv=SnookieWindows.TreeView.GetHwnd();
	HTREEITEM ht;
	SNOOKIEFOLDER *lpFolder;
	//ht=TreeView_GetSelection(tv);
	//if(ht==NULL) ht=TreeView_GetRoot(tv);//there was no selection; start at the top.
	ht=start;
	lpFolder=GetFolderFromTree(ht);
	BOOL done=FALSE;
	MSG msg;

	///////////////////////////////////////
	do{
		//first handle messages
		if(PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//now get info about the current Selection.
		strcpy(buf, lpFolder->Name);
		strupr(buf);
		SnookieWindows.StatusBar.SetText(3, buf);

		if(strstr(buf, this->FindFileName) != 0){
			//we have a match
			TreeView_Select(SnookieWindows.TreeView.GetHwnd(), lpFolder->hTreeItem, TVGN_CARET);
			done=TRUE;
		}else{
			//it's not a match; we have to move to the next node.
			lpFolder=GetNextFolder(lpFolder);
			if(lpFolder==0){//no more
				MessageBox(SnookieWindows.MainWindow, "The file does not exist in the list.", "Snookie", MB_OK | MB_ICONEXCLAMATION);
				done=TRUE;
			}
		}
	}while(done==FALSE);

	SnookieWindows.StatusBar.SetText(3, OldStatusText);
	return CURRENTCAT_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////
int CURRENTCAT::AddPath(char *Path){
	char currentpath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentpath);

	this->Paths[this->NumberOfPaths]->FillCatalog(
		this->NumberOfPaths, Path, 
		&SnookieWindows.TreeView,
		&SnookieWindows.StatusBar);

	this->MemoryUsed+=this->Paths[this->NumberOfPaths]->MemoryUsed;

	this->NumberOfPaths++;
	SetCurrentDirectory(currentpath);
	this->Modified=TRUE;
	return CURRENTCAT_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////
int CURRENTCAT::OpenFile(char *FileName){
	int i;
	if(this->Clear()==CURRENTCAT_OK){
	//this function was copied from SaveFile(), so I'll try to redo the comments as
	//needed,but there may be some remnants of the original.

	//make sure we're ready for this...

	//create a file to which we read.
	file.OpenForRead(FileName);
	strcpy(this->FileName, FileName);
	this->OnDisk=TRUE;

	char id[CURRENTCAT_IDLEN+1];//make room for null-terminator
	strcpy(id, CURRENTCAT_ID);
	file.Read(id, CURRENTCAT_IDLEN);
	//now compare the two to make sure that we have a match
	if(strcmp(id, CURRENTCAT_ID) != 0){
		//didn't work
		return CURRENTCAT_ERROR;
	}
	//now read stuff about the CURRENT CAT - actually the only thing we need
	//to read is the number of paths.
	file.Read(&this->NumberOfPaths, sizeof(this->NumberOfPaths));
	//now for that number of paths, read info about the CATALOG that goes with them.

	SNOOKIECATALOG *path;
	for(i=0;i<this->NumberOfPaths; i++){
		path=this->Paths[i];

		path->Folder=new SNOOKIEFOLDER;

		path->Index=i;
		path->sb=&SnookieWindows.StatusBar;
		path->tv=&SnookieWindows.TreeView;

		file.Read(&path->c1, sizeof(path->c1));

		//allocate memory for those variables
		path->CatalogName = new char[path->c1.CatalogNameLength];
		path->Memo = new char[path->c1.MemoLength];
		path->Path = new char[path->c1.PathLength];
		path->PathName = new char[path->c1.PathNameLength];

		//since these are dynamically allocated, we need to do this a different way
		//the DynLength member contains the sizeof() the dynamically allocated
		//variables
		file.Read(path->CatalogName, path->c1.CatalogNameDynLength);
		file.Read(path->Memo, path->c1.MemoDynLength);
		file.Read(path->Path, path->c1.PathDynLength);
		file.Read(path->PathName, path->c1.PathNameDynLength);

		//now each of these paths have a folder to fill.
		this->ReadFolder(path, path->Folder,0, &this->MemoryUsed, NULL, TRUE);
	}
	file.Close();
	SnookieWindows.StatusBar.SetText(3, "");
	}else{
		return CURRENTCAT_ERROR;
	}
	return CURRENTCAT_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
int CURRENTCAT::ReadFolder(SNOOKIECATALOG* Catalog, SNOOKIEFOLDER * Folder, 
						   HTREEITEM Parent, DWORD * mem, LONGLONG * size, BOOL IsFolder){
	int i;

	Folder->Catalog=Catalog;

	this->file.Read(&(Folder->f1), sizeof(Folder->f1));

	*mem+=Folder->f1.NameDynLength + Folder->f1.MemoDynLength + Folder->f1.PathDynLength;
	//now dynamically allocate memory for those variables.
	Folder->Name = new char[Folder->f1.NameLength+1];
	Folder->Memo = new char[Folder->f1.MemoLength+1];
	Folder->Path = new char[Folder->f1.PathLength+1];
	Folder->Label = new char[Folder->f1.LabelLength+1];
	//since these are dynamically allocated, we need to do this a different way
	//the DynLength member contains the sizeof() the dynamically allocated
	//variables
	this->file.Read(Folder->Name, Folder->f1.NameDynLength);
	Folder->Name[Folder->f1.NameLength]=0;
	this->file.Read(Folder->Memo, Folder->f1.MemoDynLength);
	Folder->Memo[Folder->f1.MemoLength]=0;
	this->file.Read(Folder->Path, Folder->f1.PathDynLength);
	Folder->Path[Folder->f1.PathLength]=0;
	this->file.Read(Folder->Label, Folder->f1.LabelDynLength);
	Folder->Label[Folder->f1.LabelLength]=0;

	//now show it on the status bar.
	SnookieWindows.StatusBar.SetText(3, Folder->Name);

	//allocate for the folders/files
	Folder->Files= new SNOOKIEFOLDER[Folder->f1.NumberOfFiles];
	Folder->Folders= new SNOOKIEFOLDER[Folder->f1.NumberOfFolders];
	*mem+=sizeof(SNOOKIEFOLDER) * (Folder->f1.NumberOfFiles+Folder->f1.NumberOfFolders);

	Folder->MemoryUsed+=*mem;
	
	//put it in the treeview
	int nChildren;
	int nFiles;
	nFiles=Folder->f1.NumberOfFiles+Folder->f1.NumberOfFolders;
	nChildren=(nFiles==0) ? 0:1;

	//now if it's got a different label, use it for the treeview.
	char Label[200];
	int Icon;

	if(Folder->f1.LabelLength == 0){
		strcpy(Label, Folder->Name);
	}else{
		strcpy(Label, Folder->Label);
	}
	if(IsFolder==TRUE){
		Icon=CATIMAGE_FOLDER;
	}else{
		Icon=CATIMAGE_FILE;
	}
	SnookieWindows.TreeView.AddNode(&Folder->hTreeItem, Parent, Label, nChildren, (LPARAM)Folder, Icon);

	for(i=0;i<Folder->f1.NumberOfFiles;i++){
		this->ReadFolder(Catalog, &Folder->Files[i], Folder->hTreeItem, &Folder->MemoryUsed, &Folder->f1.Size, FALSE);
	}
	for(i=0;i<Folder->f1.NumberOfFolders;i++){
		this->ReadFolder(Catalog, &Folder->Folders[i], Folder->hTreeItem, &Folder->MemoryUsed, &Folder->f1.Size, TRUE);
	}
	return CURRENTCAT_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
int CURRENTCAT::RemovePath(int n){
	SNOOKIECATALOG* t;
	if(this->NumberOfPaths > 0){//make sure that we have a path to delete (this should never
								//fail, considering you can't select a node if there are
								//none.
		//WE really are going to delete a catalog now.
		this->MemoryUsed-=this->Paths[n]->MemoryUsed;
		SnookieWindows.TreeView.DeleteNode(this->Paths[n]->Folder->hTreeItem);//first delete that node.
		int i;
		//now go through and copy all the paths after the deleted one down, to 
		//fill the gap
		if(NumberOfPaths !=(n+1)){//if there are no nodes after n, we don't need to copy the stuff.
			for(i=n;i<this->NumberOfPaths-1;i++){
				t=this->Paths[i];
				this->Paths[i]=this->Paths[i+1];
				this->Paths[i]->Index--;
				this->Paths[i+1]=t;
				//now adjust the lParam for the tree nodes
			}
		}
		delete this->Paths[NumberOfPaths-1];
		this->Paths[NumberOfPaths-1]= new SNOOKIECATALOG;
		NumberOfPaths--;
	}
	this->Modified=TRUE;

	//if there are no more paths, clear the properties display
	if(NumberOfPaths==0) this->UpdateProperties(0);

	return CURRENTCAT_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
int CURRENTCAT::Save(){
	int i;
	//make sure first that we have a filename to which we can save it.
	if(this->FileName[0] == 0) return CURRENTCAT_ERROR;
	
	//create a file to which we write.
	file.OpenForWrite(this->FileName);
	char id[11]=CURRENTCAT_ID;
	file.Write(id, CURRENTCAT_IDLEN);
	//now write stuff about the CURRENT CAT - actually the only thing we need
	//to save is the number of paths.
	file.Write(&this->NumberOfPaths, sizeof(this->NumberOfPaths));
	//now for that number of paths, write info about the CATALOG that goes with them.
	SNOOKIECATALOG *path;
	for(i=0;i<this->NumberOfPaths; i++){
		path=this->Paths[i];

		file.Write(&(path->c1), sizeof(path->c1));

		//since these are dynamically allocated, we need to do this a different way
		//the DynLength member contains the sizeof() the dynamically allocated
		//variables
		file.Write(path->CatalogName, path->c1.CatalogNameDynLength);
		file.Write(path->Memo, path->c1.MemoDynLength);
		file.Write(path->Path, path->c1.PathDynLength);
		file.Write(path->PathName, path->c1.PathNameDynLength);
		//now each of these paths have a folder to fill.
		this->WriteFolder(path->Folder);
	}
	file.Close();
	this->OnDisk=TRUE;
	SnookieWindows.StatusBar.SetText(3, "");
	return CURRENTCAT_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
int CURRENTCAT::WriteFolder(SNOOKIEFOLDER * Folder){
	int i;
	
	this->file.Write(&(Folder->f1), sizeof(Folder->f1));

	//since these are dynamically allocated, we need to do this a different way
	//the DynLength member contains the sizeof() the dynamically allocated
	//variables
	this->file.Write(Folder->Name, Folder->f1.NameDynLength);
	this->file.Write(Folder->Memo, Folder->f1.MemoDynLength);
	this->file.Write(Folder->Path, Folder->f1.PathDynLength);
	this->file.Write(Folder->Label, Folder->f1.LabelDynLength);

	//now show it on the status bar.
	SnookieWindows.StatusBar.SetText(3, Folder->Name);

	for(i=0;i<Folder->f1.NumberOfFiles;i++){
		this->WriteFolder(&Folder->Files[i]);
	}
	for(i=0;i<Folder->f1.NumberOfFolders;i++){
		this->WriteFolder(&Folder->Folders[i]);
	}
	this->Modified=FALSE;
	return CURRENTCAT_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
int CURRENTCAT::SaveAs(char * FileName){
	strcpy(this->FileName, FileName);
	this->Save();
	return CURRENTCAT_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
int CURRENTCAT::nmSelect(NMTREEVIEW *nmtv){
	//this is called when the user selects a new node from the treeview.
	HTREEITEM htree;
	htree=nmtv->itemNew.hItem;
	BOOL IsCatalog=FALSE;

	//now get information about it and stuff.
	//nmtv->itemNew.lParam is a pointer to either the folder or the file or catalog.
	//if it's got children, then it's either a folder or catalog, and if it doesn't
	//have a parent, then it's a catalog.
	SNOOKIEFOLDER* lpFolder;
	lpFolder=(SNOOKIEFOLDER*)nmtv->itemNew.lParam;
	if(lpFolder==NULL) return CURRENTCAT_ERROR;

	this->CurrentSelection=lpFolder;

	this->UpdateProperties(lpFolder);
	return CURRENTCAT_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
int CURRENTCAT::UpdateProperties(SNOOKIEFOLDER * lpFolder){
//updates the display to show stuff about the current selection.
	if(lpFolder==0){
		//if it's zero, then we just erase the text there.
		SnookieWindows.StaticText_OtherInfo.SetText("");
		SnookieWindows.StaticText_Name.SetText("");
		SnookieWindows.StaticText_Size.SetText("");
		return CURRENTCAT_OK;
	}
	//now we have the folder; just display the info.

	//display the memo text
	if(lpFolder->Memo != 0){
		SnookieWindows.EditBox.SetText(lpFolder->Memo);
	}
	char SizeText[400];
	char OtherInfoText[400];
	char NameText[400];
	ZeroMemory(NameText, sizeof(NameText));
	ZeroMemory(OtherInfoText, sizeof(OtherInfoText));
	ZeroMemory(SizeText, sizeof(SizeText));
	
	/////////////////////SIZE TEXT
	//This will fill SizeText with information about size and free space, if it's a drive.
	char Size1[100];
	char Size2[40];
	char Size3[40];
	char Size4[100];
	char Size5[100];
	char Size6[100];
	ZeroMemory(Size1, sizeof(Size1));
	ZeroMemory(Size2, sizeof(Size2));
	ZeroMemory(Size3, sizeof(Size3));
	ZeroMemory(Size4, sizeof(Size4));
	ZeroMemory(Size5, sizeof(Size5));
	ZeroMemory(Size6, sizeof(Size6));
	sprintf(Size1, "Size: %s (%s bytes)", BytesToString(Size2, lpFolder->f1.Size), BytesToCommaString(Size3, lpFolder->f1.Size));

	if(lpFolder->f1.IsDrive==TRUE){
		//it's a drive; display some stuff
		switch(lpFolder->f1.DriveType){
		case DRIVE_CDROM:
			sprintf(Size4, "\nCD-ROM disc.");
			break;
		case DRIVE_REMOVABLE:
			sprintf(Size4, "\nRemovable disk.\nCapacity: %s\nFree space: %s\n", BytesToString(Size5, lpFolder->f1.TotalSpace), BytesToString(Size6, lpFolder->f1.FreeSpace));
			break;
		case DRIVE_RAMDISK:
			sprintf(Size4, "\nRAM disk.\nCapacity: %s\nFree space: %s\n", BytesToString(Size5, lpFolder->f1.TotalSpace), BytesToString(Size6, lpFolder->f1.FreeSpace));
			break;
		case DRIVE_FIXED:
			sprintf(Size4, "\nHard drive.\nCapacity: %s\nFree space: %s\n", BytesToString(Size5, lpFolder->f1.TotalSpace), BytesToString(Size6, lpFolder->f1.FreeSpace));
			break;
		case DRIVE_REMOTE:
			sprintf(Size4, "\nNetwork drive.\nCapacity: %s\nFree space: %s\n", BytesToString(Size5, lpFolder->f1.TotalSpace), BytesToString(Size6, lpFolder->f1.FreeSpace));
			break;
		case DRIVE_UNKNOWN:
		default:
			sprintf(Size4, "\nUnknown drive type");
			break;
		}
	}
	strcpy(SizeText, Size1);
	strcat(SizeText, Size4);
	//////////////////////////////////////////////////
	//other info text
	char Other1[30];
	ZeroMemory(Other1, sizeof(Other1));
	if(lpFolder->Catalog->Folder->Label[0] != 0){
		sprintf(OtherInfoText, "File Attributes: %s\nFrom: %s\n", 
			AttribToString(Other1, lpFolder->f1.Attributes), lpFolder->Catalog->Folder->Label);
	}else{
		sprintf(OtherInfoText, "File Attributes: %s\nFrom: %s\n", 
			AttribToString(Other1, lpFolder->f1.Attributes), lpFolder->Catalog->Folder->Name);
	}

	char FullPath[255];
	sprintf(FullPath, "Full Path:\n%s", lpFolder->Path);
	strcat(OtherInfoText, FullPath);
	//////////////////////////////////////////////////
	//NAME TEXT
	strcpy(NameText, lpFolder->Name);

	SnookieWindows.StaticText_Name.SetText(NameText);
	SnookieWindows.StaticText_Size.SetText(SizeText);
	SnookieWindows.StaticText_OtherInfo.SetText(OtherInfoText);


	return CURRENTCAT_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////
char* BytesToString(char* buf, LONGLONG Bytes){
	//converts number of bytes into a string expression
	//it makes it precise to two decimal places.

	if(Bytes<1024){
		//bytes
		sprintf(buf, "%d bytes", Bytes);
	}else{
		if(Bytes<(1024*1024)){
			//kb
			sprintf(buf, "%.2f kb", (long double)Bytes/(1024));
		}else{
			if(Bytes<(1024*1024*1024)){
				//mb
				sprintf(buf, "%.2f mb", (long double)Bytes/(1024*1024));
			}else{
				//gb
				sprintf(buf, "%.2f gb", (long double)Bytes/(1024*1024*1024));
			}
		}
	}
	return buf;
}

/////////////////////////////////////////////////////////////////////////////////////////
char* AttribToString(char* buf, DWORD Attributes){
	if(Attributes & FILE_ATTRIBUTE_ARCHIVE){
		strcat(buf, "a");
	}else strcat(buf, "-");
	if(Attributes & FILE_ATTRIBUTE_DIRECTORY){
		strcat(buf, "d");
	}else strcat(buf, "-");
	if(Attributes & FILE_ATTRIBUTE_HIDDEN){
		strcat(buf, "h");
	}else strcat(buf, "-");
	if(Attributes & FILE_ATTRIBUTE_READONLY){
		strcat(buf, "r");
	}else strcat(buf, "-");
	if(Attributes & FILE_ATTRIBUTE_SYSTEM){
		strcat(buf, "s");
	}else strcat(buf, "-");
	return buf;
}

/////////////////////////////////////////////////////////////////////////////////////////
char* BytesToCommaString(char* buf, LONGLONG Bytes){
	//converts a number to a better looking number; for example:
	//input value: 12407243
	//output:    12,407,243
	//cool, eh?
	char temp[50];
	ZeroMemory(temp, sizeof(temp));
	sprintf(temp, "%I64d\0",Bytes);
	//now that we have it in string form, go through and put commas in.
	//we'll do it kinda funky - we'll start at the end of the temp string
	//and put a comma in every 3 places.  When we're finished; reverse the string
	//and POOF- done.
	int ptemp;
	int pbuf;
	ptemp=0;
	pbuf=0;

	do{
		buf[pbuf]=temp[strlen(temp)-ptemp-1];
		if(((float)(ptemp+1)/3) == (int)((float)(ptemp+1)/3)){
			pbuf++;
			buf[pbuf]=',';
		}
		pbuf++;
		ptemp++;
	}while(temp[ptemp] != (char)0);
	if(buf[pbuf-1]==',') buf[pbuf-1]=0;
	strrev(buf);
	return buf;
}

/////////////////////////////////////////////////////////////////////////////////////////
SNOOKIEFOLDER* GetNextFolder(SNOOKIEFOLDER* lpFolder){
	//returns a pointer to the next folder to the one given.
	HWND tv;
	tv=SnookieWindows.TreeView.GetHwnd();
	HTREEITEM sel;
	sel=lpFolder->hTreeItem;
	HTREEITEM ht;
	HTREEITEM ht2;
	BOOL done;
	done=FALSE;

	ht=TreeView_GetChild(tv, sel);//get the child of hte selection
	//now figure out if that's the next one in the list.
	if(ht==NULL){
		//there's no child; get the sibling
		ht=TreeView_GetNextSibling(tv, sel);
		if(ht==NULL){
			//there's no sibling or child; then we must proceed to the next parent.
			ht2=sel;
			do{
				ht=TreeView_GetParent(tv, ht2);
				if(ht == NULL){//no parent; we have to quit
					ht=0;
					done=TRUE;
				}
				ht2=TreeView_GetNextSibling(tv, ht);
				if(ht2 != NULL){ //there's something there
					ht=ht2;
					done=TRUE;
				}
				ht2=ht;
			}while(done == FALSE);
		}
	}
	if(ht != 0) return GetFolderFromTree(ht);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
SNOOKIEFOLDER* GetFolderFromTree(HTREEITEM ht){
	TVITEM tvitem;
	ZeroMemory(&tvitem, sizeof(tvitem));
	tvitem.mask=TVIF_PARAM | TVIF_HANDLE | TVIF_CHILDREN;
	tvitem.hItem=ht;
	TreeView_GetItem(SnookieWindows.TreeView.GetHwnd(), &tvitem);
	return (SNOOKIEFOLDER*) tvitem.lParam;
}