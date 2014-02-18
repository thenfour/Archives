//snookiecatalog.h
//definitions of structs/classes used for the storage of catalogs
/////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SNOOKIECATALOGV1_H
#define SNOOKIECATALOGV1_H

#include <windows.h>
#include <commctrl.h>
#include "diskstuff.h"
#include <stdio.h>
#include "controls.h"

#include "editbox.h"
#include "list.h"
#include "imagelist.h"
#include "static.h"
#include "statusbar.h"
#include "treeview.h"

#define CATALOG_OK		0
#define	CATALOG_ERROR	1

#define CATIMAGE_FOLDER 0
#define CATIMAGE_FILE 1
#define CATIMAGE_EXE 2
#define CATIMAGE_ZIP 3
#define CATIMAGE_DRIVE 4
#define CATIMAGE_CDROM 5
#define CATIMAGE_FIXEDDISK 6
#define CATIMAGE_NETWORKDRIVE 7
#define CATIMAGE_RAMDISK 8
#define CATIMAGE_REMOVABLE 9
#define CATIMAGE_UNKNOWN 10
#define CATIMAGE_RAR 11
#define CATIMAGE_ARJ 12
#define CATIMAGE_ACE 13
#define CATIMAGE_BMP 14
#define CATIMAGE_WAV 15
#define CATIMAGE_DOC 16
#define CATIMAGE_TXT 17
#define CATIMAGES 17

//this function will allocate memory for s, and then copy the text to it.
//the return value is the number of bytes copied, if it fails, it's -1.
int StringAllocateCopy(char * s, const char * text);

class SNOOKIECATALOGV1;
class SNOOKIEFOLDERV1;
struct ccFOLDER1V1;
struct ccCATALOG1V1;
class CURRENTCATV1;

///////////////////////////////////////////////////////////////////////////////////////////
//these structs are for continuity in saving/opening.
//ccfolder1 is what's written/read first in the Write/ReadFolder() function.

//dynamically allocated variables need to be written by the function
struct ccFOLDER1V1{
	int NameLength;
	int MemoLength;
	int PathLength;
	int LabelLength;

	int NameDynLength;
	int MemoDynLength;
	int PathDynLength;
	int LabelDynLength;

	ULONG Attributes;
	UINT DriveType;
	LONGLONG FreeSpace;
	BOOL IsDrive;
	int NumberOfFiles;
	int NumberOfFolders;
	LONGLONG Size;
	LONGLONG TotalSpace;
};
///////////////////////////////////////////////////////////////////////////////////////////
struct ccCATALOG1V1{
	int CatalogNameLength;
	int MemoLength;
	int PathLength;
	int PathNameLength;
	int CatalogNameDynLength;
	int MemoDynLength;
	int PathDynLength;
	int PathNameDynLength;
};
///////////////////////////////////////////////////////////////////////////////////////////
class SNOOKIENODEV1{
public:
	~SNOOKIENODEV1();
	SNOOKIENODEV1();

	ccCATALOG1V1 c1;
	ccFOLDER1V1 f1;
};
/////////////////////////////////////////////////////////////////////////////////////////////////
class SNOOKIEFOLDERV1 : public SNOOKIENODEV1{
public:
	SNOOKIEFOLDERV1();
	~SNOOKIEFOLDERV1();

	LONGLONG FillFolder(char * Path, char * Name, TREEVIEW * TreeView, STATUSBAR* StatusBar, SNOOKIECATALOGV1* Catalog, DWORD *Memory);

	char * Memo;						//Some notes about the file
	char * Label;						//if the label changed, this isn't null.
	char * Name;

	SNOOKIECATALOGV1* Catalog;			//a pointer to the base catalog to which this folder belongs

	DWORD MemoryUsed;					//Memory used by this folder (in bytes)

	char * Path;

	SNOOKIEFOLDERV1 * Folders;


	SNOOKIEFOLDERV1 * Files;

	HTREEITEM hTreeItem;				//Handle to the item on the tree

private:
	BOOL Valid;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
class SNOOKIECATALOGV1 : public SNOOKIENODEV1{
public:
	SNOOKIECATALOGV1();
	~SNOOKIECATALOGV1();

	HRESULT FillCatalog(int Index, char * Path, TREEVIEW *TreeView, STATUSBAR *StatusBar);

	int Index;						//since a catalog exists only in a collection of
									//catalogs in a CURRENTCAT object, we should keep
									//track of what index it is.

	char *Memo;						//Some notes about the file
	char *Path;
    char *PathName;
    char *CatalogName;

	DWORD MemoryUsed;

	SNOOKIEFOLDERV1 * Folder;				//the root directory of the catalog

	TREEVIEW * tv;
	STATUSBAR * sb;
	HTREEITEM hTreeItem;				//Handle to the item on the tree
private:
	BOOL Valid;
};

#endif
