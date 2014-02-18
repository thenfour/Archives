//currentcat.h
//VERSION 2 - not used in V3

#ifndef CURRENTCATV1_H
#define CURRENTCATV1_H

#define NPATHS 400

#include <windows.h>
#include "Snookiecatalogv1.h"
#include "snookiewindows.h"
#include "resource.h"
#include "catfile.h"

#include "editbox.h"
#include "list.h"
#include "imagelist.h"
#include "static.h"
#include "statusbar.h"
#include "treeview.h"

#define CURRENTCAT_OK		0
#define CURRENTCAT_ERROR	1

#define CURRENTCAT_ID "SNOOKIE  1"
#define CURRENTCAT_IDLEN 10

//PROTOTYPE

char* BytesToString(char* buf, LONGLONG Bytes);
char* AttribToString(char* buf, DWORD Attributes);
char* BytesToCommaString(char* buf, LONGLONG Bytes);
SNOOKIEFOLDER* GetNextFolder(SNOOKIEFOLDER* lpFolder);
SNOOKIEFOLDER* GetFolderFromTree(HTREEITEM ht);

/*This class contains information about the currently opened catalog.
*/
//////////////////////////////////////////////////////////////////////////////////////
class CURRENTCATV1{
public:
	CURRENTCATV1();
	~CURRENTCATV1();

	int Clear();//Clears the catalog completely - same as File->New
	int AddPath(char * Path);//Adds that path to the tree
	int RemovePath(int n);//removes that path from the tree
	int OpenFile(char * FileName);//replaces the current catalog with the specified one
	int SaveAs(char * FileName);//saves this one
	int Save();//saves this one.
	int FindFile(char * Name);//finds that file.
	int FindNext(HTREEITEM start);//finds next file starting with "start"
	int nmSelect(NMTREEVIEW *nmtv);//when an item is selected, this is what's called
	BOOL OnDisk;//on disk or not? (saved)
	int SetupIcons();
	int NumberOfPaths;

	SNOOKIEFOLDERV1 * GetSelected();//returns a pointer to the currently selected folder.

	int ReadFolder(SNOOKIECATALOGV1* Catalog, SNOOKIEFOLDERV1 * Folder, 
		HTREEITEM Parent, DWORD * mem, LONGLONG * size, BOOL IsFolder);
	int WriteFolder(SNOOKIEFOLDERV1 * Folder);
	int UpdateProperties(SNOOKIEFOLDERV1 * lpFolder);
	SNOOKIECATALOGV1* Paths[400];//there may be many paths all included in this catalog

private:
	SNOOKIEFOLDERV1 * CurrentSelection;
	char FileName[MAX_PATH];
	char FindFileName[MAX_PATH];//this is the stored name of the last file searched for by
								//the user
	DWORD MemoryUsed;
	BOOL Modified;
	CATFILE file;
	int ImageId[CATIMAGES];
};

#endif