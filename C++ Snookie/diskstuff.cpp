//diskstuff.cpp
#include "diskstuff.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//returns the number of folders and files in a given path.
int GetFolderStatistics(int * Folders, int * Files){
	*Folders=0;
	*Files=0;

	WIN32_FIND_DATA FileData;
	HANDLE hFind;
	BOOL Done;
	Done=FALSE;
	int x=0;

	hFind=FindFirstFile("*.*",  &FileData);
	if(hFind==INVALID_HANDLE_VALUE) return DISKSTUFF_ERROR;

	do{
		if(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			//it's a folder
			//make sure not to include the current directory or parent directory.
			if(strcmp(FileData.cFileName,".")!=0 && strcmp(FileData.cFileName,"..")!=0){
				*Folders=(*Folders)+1;
			}
		}else{
			//it's not a folder
			*Files=(*Files)+1;
		}
		//get the next file
		if(FindNextFile(hFind, &FileData)==0){
			Done=TRUE;
		}

	}while(Done==FALSE);

	FindClose(hFind);
	return x;
}

