//snookiecatalog.cpp
#include "snookiecatalog.h"
////////////////////////////////////////////////////////////////////////////////////////////
SNOOKIENODE::~SNOOKIENODE(){
}
////////////////////////////////////////////////////////////////////////////////////////////
SNOOKIENODE::SNOOKIENODE(){
	ZeroMemory(&this->c1, sizeof(this->c1));
	ZeroMemory(&this->f1, sizeof(this->f1));
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////
SNOOKIECATALOG::~SNOOKIECATALOG(){
	if(this->CatalogName != 0){
		delete this->CatalogName;
		this->CatalogName=0;
	}
	if(this->Memo != 0){
		delete this->Memo;
		this->Memo=0;
	}
	if(this->Path != 0){
		delete this->Path;
		this->Path=0;
	}
	if(this->PathName !=0){
		delete this->PathName;
		this->PathName=0;
	}
	if(this->Folder !=0){
		delete this->Folder;
		this->Folder=0;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////
SNOOKIECATALOG::SNOOKIECATALOG(){
	//constructor; initialize
	this->CatalogName=0;
	this->Path=0;
	this->Memo=0;
	this->PathName=0;

	this->Index=0;
	this->Folder=0;
	this->hTreeItem=0;
	this->sb=0;
	this->tv=0;
	this->Valid=FALSE;
	this->MemoryUsed=0;
}
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT SNOOKIECATALOG::FillCatalog(int Index, char * Path, TREEVIEW *TreeView, STATUSBAR *StatusBar){

	this->Index=Index;
	this->Folder=new SNOOKIEFOLDER;
	this->MemoryUsed+=sizeof(SNOOKIEFOLDER);

	int i;
	char OldStatusBarText1[256];
	StatusBar->GetText(1, OldStatusBarText1);
	char OldStatusBarText3[256];
	StatusBar->GetText(3, OldStatusBarText3);
	
	StatusBar->SetText(1, "Filling Catalog...");

	char PathName[MAX_PATH];
	int PathLength;
	char ParentName[MAX_PATH];
	int ParentLength;
	char FolderName[MAX_PATH];
	int FolderLength;
	//get the name of the path specified./////////////////////////////
	//so go back starting at the last damn letter until 
	if(SetCurrentDirectory(Path)==0){
		//there was an error
		DWORD rvalue;
		rvalue=GetLastError();
		if(rvalue==ERROR_NOT_READY){
			//notify that the path isn't ready.
			MessageBox(0, "The drive isn't ready", "Snookie", MB_OK | MB_ICONEXCLAMATION);
		}
		StatusBar->SetText(1, OldStatusBarText1);
		StatusBar->SetText(3, OldStatusBarText3);
		return CATALOG_ERROR;
	}
	GetCurrentDirectory(MAX_PATH, PathName);
	PathLength=strlen(PathName);
	//now we have a standardized path name.
	//to find the name of the folder we're in, go up one directory and use 
	//the difference to find it.  If we can't go up any more, then we're
	//at the root of the drive.  In that case, find the name of the volume
	//as well as the type of drive.
	if(SetCurrentDirectory("..")==0){
		return CATALOG_ERROR;
	}
	GetCurrentDirectory(MAX_PATH, ParentName);
	ParentLength=strlen(ParentName);
	if(ParentLength==PathLength){
		//it's a root directory because it couldn't go up anymore.
		//if it's a network drive, though, we need to make some special restrictions

		if(GetVolumeInformation(PathName, FolderName, MAX_PATH, 0, 0, 0, 0, 0)==0){
			//didn't work; may be a network drive
			if(PathName[0]=='\\' && PathName[1]=='\\'){
				//network drive
				strcpy(FolderName, PathName);
				this->Folder->f1.DriveType=DRIVE_REMOTE;
			}
			DWORD rvalue;
			rvalue=GetLastError();
		}else{
			if(strlen(FolderName)==0){
				strcpy(FolderName, PathName);
			}
			//and since we know it's a root directory, get some more info, such as
			//free space, 
			this->Folder->f1.DriveType=GetDriveType(PathName);
			//and get free space
			ULARGE_INTEGER totalspace;
			ULARGE_INTEGER freespace;
			GetDiskFreeSpaceEx(PathName, 0, &totalspace, &freespace);
			this->Folder->f1.FreeSpace=freespace.QuadPart;
			this->Folder->f1.TotalSpace=totalspace.QuadPart;
			this->Folder->f1.IsDrive=TRUE;
		}
			////////////////////////////////////////////////////////////////
	}else{
		//it's not a root directory.
		//so we'll go from the end of ParentName to the end of PathName.

		//let's make sure that we're not going to make a mistake here...
		//consider this.  Consider these two parent dirs to the current dir:
		//"c:\" and "c:\system".  One ends in a "\" and the other one doesn't.
		//we need to test that last character to make sure that it's not a "\"
		if(ParentName[ParentLength-1] =='\\'){
			for(i=0;i<(PathLength-ParentLength); i++){
				FolderName[i]=PathName[ParentLength+i];
			}
		}else{
			for(i=0;i<((PathLength-ParentLength)-1); i++){
				FolderName[i]=PathName[ParentLength+i+1];
			}
		}

		FolderName[i]=0;
	}
	FolderLength=strlen(FolderName);
	///////////////////////////////////////////////////////////////////

	this->tv=TreeView;
	this->sb=StatusBar;

	//we need to get the name of the path, or if it doesn't have a name,
	//just use the name of the path.
	TreeView->AddNode(&(this->Folder->hTreeItem), 0, FolderName, 1, (LPARAM)this->Folder, CATIMAGE_FOLDER);

	int tPathLength;
	int tNameLength;

	tPathLength=strlen(Path);
	tNameLength=strlen(FolderName);

	this->Folder->Name=new char[tNameLength];
	this->Folder->f1.NameDynLength=sizeof(char)*(tNameLength);
	this->Folder->f1.NameLength=tNameLength;
	this->MemoryUsed+=sizeof(char)*(tNameLength);
	strcpy(this->Folder->Name,FolderName);

	this->Folder->Path=new char[tPathLength];
	this->Folder->f1.PathLength=tPathLength;
	this->Folder->f1.PathDynLength=sizeof(char)*(tPathLength);
	this->MemoryUsed+=this->f1.PathDynLength;
	strcpy(this->Folder->Path,Path);

	this->Folder->Catalog=this;
	this->Folder->FillFolder(Path, FolderName, TreeView, StatusBar, this, &this->MemoryUsed);
	this->MemoryUsed+=sizeof(this);

	StatusBar->SetText(1, OldStatusBarText1);
	StatusBar->SetText(3, OldStatusBarText3);
	return CATALOG_OK;
}
////////////////////////////////////////////////////////////////////////////////////////////
SNOOKIEFOLDER::~SNOOKIEFOLDER(){
/*	if(this->Name != 0){
		delete this->Name;
		this->Name=0;
	}
	if(this->Path != 0){
		delete this->Path;
		this->Path=0;
	}
*/	if(this->Label != 0){
		delete this->Label;
		this->Label=0;
	}
	if(this->Memo != 0){
		delete  this->Memo;
		this->Memo=0;
	}
	if(this->Files != 0){
		delete [] this->Files;
		this->Files=0;
	}
	if(this->Folders != 0){
		delete [] this->Folders;
		this->Folders=0;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////
SNOOKIEFOLDER::SNOOKIEFOLDER(){
	this->Label=0;

	this->Files=0;
	this->Folders=0;
	this->Memo=0;
	this->Name=0;
	this->Path=0;

	this->Catalog=0;

	this->hTreeItem=0;
	this->Valid=FALSE;
	this->MemoryUsed=0;
}
////////////////////////////////////////////////////////////////////////////////////////////
LONGLONG SNOOKIEFOLDER::FillFolder(char * Path, char * Name, TREEVIEW * TreeView, STATUSBAR* StatusBar, SNOOKIECATALOG* Catalog, DWORD *Memory){
	//fills the folder with the directory
	//and returns the size of the folder (and all files in it)
	char				tPath[MAX_PATH];
	char				cd[MAX_PATH];
    int					tNameLength;
    int					tPathLength;
	int					nFolders, nFiles;
	int					cFolder, cFile;
	int					Done=FALSE;
	int					i;
	MSG msg;
	HANDLE				hFindFile;
	WIN32_FIND_DATA		FileData;
	
	nFolders=0;
	nFiles=0;
	cFolder=0;
	cFile=0;
	SetCurrentDirectory(Path);
	//////////////////////////////////////////////////////////////////////////////////////////////
	GetFolderStatistics(&nFolders, &nFiles);
	this->f1.NumberOfFiles=nFiles;
	this->f1.NumberOfFolders=nFolders;

	//now that we know how many folders/files, we can allocate memory accordingly.
	this->Files=new SNOOKIEFOLDER[nFiles+1];
	this->Folders=new SNOOKIEFOLDER[nFolders+1];
	this->MemoryUsed+=sizeof(SNOOKIEFOLDER)*(nFiles+2+nFolders);

	//fill information about the folders/files now
	hFindFile=FindFirstFile("*.*", &FileData);

	GetCurrentDirectory(MAX_PATH, tPath);
	i=0;

	do{

		//and also check for messages from windows
		if(PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)==TRUE){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//we have a fileinfo struct filled; now transfer it to this
		if(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			//it's a folder
			if(strcmp(FileData.cFileName,".")!=0 && strcmp(FileData.cFileName,"..")!=0){

				//set the name stuff
				tNameLength=strlen(FileData.cFileName);
				this->Folders[cFolder].Name=new char[tNameLength+1];
				this->Folders[cFolder].f1.NameDynLength=sizeof(char)*(tNameLength+1);
				this->MemoryUsed+=this->Folders[cFolder].f1.NameDynLength;
				strcpy(this->Folders[cFolder].Name,FileData.cFileName);
				this->Folders[cFolder].f1.NameLength=tNameLength;

				//set the Path stuff
				GetCurrentDirectory(MAX_PATH, cd);
				SetCurrentDirectory(FileData.cFileName);
				GetCurrentDirectory(MAX_PATH, tPath);
				SetCurrentDirectory(cd);
				tPathLength=strlen(tPath);
				this->Folders[cFolder].Path=new char[tPathLength+1];
				this->Folders[cFolder].f1.PathDynLength=sizeof(char)*(tPathLength+1);
				this->MemoryUsed+=this->Folders[cFolder].f1.PathDynLength;
				strcpy(this->Folders[cFolder].Path, tPath);
				this->Folders[cFolder].f1.PathLength=tPathLength;

				//and get the attributes.
				this->Folders[cFolder].f1.Attributes=GetFileAttributes(FileData.cFileName);
				this->Folders[cFolder].Catalog=Catalog;
				//and add it to the treeview
				StatusBar->SetText(3, this->Folders[cFolder].Name);
				TreeView->AddNode(&(this->Folders[cFolder].hTreeItem),this->hTreeItem,FileData.cFileName,TRUE, (LPARAM)&this->Folders[cFolder], CATIMAGE_FOLDER);

				cFolder++;
			}
		}else{

			//it's a file
			//set the name stuff
			tNameLength=strlen(FileData.cFileName);
			this->Files[cFile].Name=new char[tNameLength+1];
			this->Files[cFile].f1.NameDynLength=sizeof(char)*(tNameLength+1);
			this->MemoryUsed+=this->Files[cFile].f1.NameDynLength;
			strcpy(this->Files[cFile].Name,FileData.cFileName);
			this->Files[cFile].f1.NameLength=tNameLength;

			//set the Path stuff
			tPathLength=strlen(tPath);
			this->Files[cFile].Path=new char[tPathLength+1];
			this->Files[cFile].f1.PathDynLength=sizeof(char)*(tPathLength+1);
			this->MemoryUsed+=this->Files[cFile].f1.PathDynLength;
			strcpy(this->Files[cFile].Path, tPath);
			this->Files[cFile].f1.PathLength=tPathLength;

			//now fill in the other info like size
			this->Files[cFile].f1.Size=FileData.nFileSizeLow+(MAXDWORD*FileData.nFileSizeHigh);
			this->f1.Size+=this->Files[cFile].f1.Size;//add it to the current dir.

			//and get the attributes.
			this->Files[cFile].f1.Attributes=GetFileAttributes(FileData.cFileName);
			this->Files[cFile].Catalog=Catalog;
			//and add it to the treeview
			TreeView->AddNode(&(this->Files[cFile].hTreeItem),this->hTreeItem,FileData.cFileName,FALSE, (LPARAM)&this->Files[cFile], CATIMAGE_FILE);
			StatusBar->SetText(3, this->Files[cFile].Name);

			cFile++;
		}

		i++;
	}while(FindNextFile( hFindFile, &FileData)!=0);

	FindClose(hFindFile);
	DWORD Mem=0;
	for(i=0; i<nFolders; i++){
		//go through and do all the folders.
		this->f1.Size+=this->Folders[i].FillFolder(this->Folders[i].Path, this->Folders[i].Name, TreeView, StatusBar, Catalog, &Mem);
		this->MemoryUsed+=Mem;
	}

	//return values
	*Memory=this->MemoryUsed;
	return this->f1.Size;
}
////////////////////////////////////////////////////////////////////////////////////////////
//this function allocates enough space at s to copy text to it, and then copies text to s.
//basically it's strcpy, but with dynamic memory allocation
int StringAllocateCopy(char * s, const char * text){
	int Length;
	Length=strlen(text);
	s=new char[Length];
	strcpy(s, text);

	return CATALOG_OK;
}