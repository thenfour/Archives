//snookiecatalogv3.h

#define SNOOKIEDISKID "SNOOKIE V3"
#define SNOOKIEDISKIDLENGTH 11

#ifndef SNOOKIECATALOGV3_H
#define SNOOKIECATALOGV3_H

#include <windows.h>
#include "treeview.h"
#include "statusbar.h"
#include "static.h"
#include <stdio.h>
#include "resource.h"
#include "catfile.h"
#include "debugwindow.h"

typedef __int32 SNOOKIEITEM;


#define CATIMAGE_CATALOG 3
#define CATIMAGE_FOLDER 0
#define CATIMAGE_FILE 1
#define CATIMAGE_EXE 2
#define CATIMAGE_ZIP 18
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

#define SN_INSUFFICIENTMEMORY -1
#define SN_ERROR 1
#define SN_OK 0


class SNOOKIENODE;
class SNOOKIEFOLDER;
class SNOOKIEFILE;
class SNOOKIECATALOG;


////////////////////////////////////////////////////////////////////////////////////
int SetupIcon(SNOOKIECATALOG* sc, int IdSel, int IdNorm, int CatId);
char* BytesToString(char* buf, LONGLONG Bytes);
char* BytesToCommaString(char* buf, LONGLONG Bytes);

SNOOKIENODE * GetNextNode(SNOOKIENODE * Node, int * Relationship, int * Iterations);
#define GNN_FIRSTCHILD 0
#define GNN_PARENT 1
#define GNN_NEXTSIBLING 2
////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////
//this represents any item on the catalog tree
class SNOOKIENODE{
public:
	SNOOKIENODE * Parent;
	SNOOKIENODE * NextSibling;
	SNOOKIENODE * PreviousSibling;
	SNOOKIENODE * FirstChild;
	SNOOKIENODE * LastChild;

	HTREEITEM hTreeItem;

	SNOOKIECATALOG * Catalog;
	char * Name;
	int NameLength;

	char * Memo;
	int MemoLength;

	SNOOKIEITEM Id;

	__int64 Size;
//	__int32 NumberOfFolders;
//	__int32 NumberOfFiles;
//	__int32 NumberOfImmediateFiles;
//	__int32 NumberOfImmediateFolders;
};

////////////////////////////////////////////////////////////////////////////////////
//this is used on each call to FillFromDisk() or whatever
struct SN_FFD{
	__int64 Size;
//	__int32 NumberOfFolders;
//	__int32 NumberOfFiles;
};

////////////////////////////////////////////////////////////////////////////////////
//this is a structure used for registering a new node to the catalog.
struct RNSTRUCT{
	char Name[300];
	SNOOKIEITEM Parent;
	__int64 Size;
	BOOL HasChildren;
	int Image;
};
////////////////////////////////////////////////////////////////////////////////////
//this is a struct that has pointers to the interfaces for some controls, for 
//the catalog to use in displaying stuff.
struct SNCATCONTROLS{
	TREEVIEW * tv;
	STATUSBAR * StatusBar;
	STATIC * Static0;
	STATIC * Static1;
	STATIC * Static2;
};

////////////////////////////////////////////////////////////////////////////////////
//Some stuff we need to know...
struct SVSTRUCT{
	HINSTANCE hInstance;
};
////////////////////////////////////////////////////////////////////////////////////
struct SNOOKIEOPTIONS{
	//this holds the options for the catalog
	BOOL ExcludeFiles;
	BOOL OnlyMP3;
	BOOL DisplayExtention;
};

////////////////////////////////////////////////////////////////////////////////////
//This is a structure used to write/load catalogs to/from disk
struct SNOOKIEHEADER{
	//immediatly preceding this structure should be an ID like, "SNOOKIE V3" or something
	__int64 NumberOfNodes;
	int RootNameLength;//include the 0 term. (total length of string)
	//immediately following this struct should be the Root name length, including the 0 term.
};
struct SNOOKIEDISKNODE{
	int NameLength;//include the 0 term - this is the total length of the string.
	//int MemoLength;//include 0 term

	BOOL HasChildren;
	int Image;

	__int64 Size;

	int Action;//action to be taken following this entry.
	//immediatly following this entry should be the name of the node, including the 0 term.
	//NOT NOW---and then the memo, including 0 term
	//THEN, an int IF AND ONLY IF the action == SN_ACTION_PARENT, which describes how many parents to go through.
};

//these macros describe what to do next when reading the file from disk.
//for instance, when a node is read and it's .Action member is SN_ACTION_PARENT
//then the next node is the next parent of that node.  If the .Action member is
//SN_ACTION_CHILD, then the next node is the first child of that node...etc...
#define SN_ACTION_PARENT 1
#define SN_ACTION_SIBLING 2
#define SN_ACTION_CHILD 3
#define SN_ACTION_NONE 4

////////////////////////////////////////////////////////////////////////////////////
//this serves as a "container" or "collection" or interface for the nodes/files/folders
class SNOOKIECATALOG{
public:
	SNOOKIECATALOG(SNCATCONTROLS scc, SVSTRUCT sv);
	~SNOOKIECATALOG();
	int Open(char * FileName, SNOOKIEITEM si);//opens a catalog FileName, and under SNOOKIEITEM si
	int Save();
	int SaveAs(char * FileName);
	int Clear();//removes all nodes
	int Remove(SNOOKIEITEM si);//removes si and all it's children
	int SetupIcons();
	SNOOKIEITEM RegisterNewNode(RNSTRUCT* rn);
	SNOOKIEITEM GetRoot();
	HINSTANCE GetInstance();
	int ImageId[100];
	TREEVIEW* GetTree();//returns the treeview
	SNOOKIEITEM FillFromDisk(char * FileName, SNOOKIEITEM si, SN_FFD * ffd);//fills a node with contents of a directory structure
	int nmSelect(LPNMTREEVIEW lParam);//call when you select a new node - this sets the sel member and refreshes screen
	int Refresh();//updates the display for the properties, and debug window
	int SetSearchString(char * String, int len);//sets the search string.
	int SearchTree(HTREEITEM ht);//searches the tree down the list for the search string.
									//we use the tree here because our order of nodes might not
									//(and probably aren't) in what order the visual tree has them.
	int SetupRoot();
	SNOOKIEITEM Sel;//the index of the selected item.
	SNCATCONTROLS scc;
	SVSTRUCT sv;
	BOOL OnDisk;

	SNOOKIENODE ** Nodes;//a dynamic array that has all the nodes in it
	BOOL * NAT;//dynamic array that is tRUE if that index is valid, FALSE if it's unused.
	__int64 LastIndex;//the very last index - this is how many is allocated.
	__int64 NumberOfNodes;//how many nodes there are.  NOT the next node to use for allocating.
	__int64 NextAvailable;//index of the next free node, if it's == NumberOfNodes, it will need to be allocated

	SNOOKIEITEM Root;
	char FileName[MAX_PATH];
	char SearchString[MAX_PATH];
	SNOOKIEOPTIONS opt;
private:
};

#endif
