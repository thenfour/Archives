//treeview.h
#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <windows.h>
#include <commctrl.h>
#include <string.h>
#include "controls.h"
#include "imagelist.h"

#define TREEVIEW_OK			CONTROLS_OK
#define TREEVIEW_ERROR		CONTROLS_ERROR

#define MAXIMAGE 50

/////////////////////////////////////////////////////////////////////////////////////////////////
class TREEVIEW : public CONTROLS{
public:
	TREEVIEW();
	~TREEVIEW();
	int CreateTreeView(RECT prect, RECT orect, HWND hwnd, HINSTANCE hInstance);
	int Clear();
	int DeleteNode(HTREEITEM hitem);
	int AddImage(int *id, HBITMAP norm, HBITMAP sel);
	int AddNode(HTREEITEM *hti, HTREEITEM htiParent, char * Text, BOOL HasChildren, LPARAM lParam, int image);
	IMAGELIST GetImageList();
	BOOL SetSel(HTREEITEM ht);

	HTREEITEM SelectNext();
	HTREEITEM GetNext();
	HTREEITEM GetNext(HTREEITEM ht);
	HTREEITEM GetRoot();
	HTREEITEM GetFirstChild(HTREEITEM ht);
	HTREEITEM GetParent(HTREEITEM ht);
	HTREEITEM GetNextSibling(HTREEITEM ht);
	HTREEITEM GetSel();
	LPARAM GetLParam(HTREEITEM ht);

private:
	HINSTANCE hInstance;
	IMAGELIST ImageList;
	int CurrentImage;
	int ImageId[MAXIMAGE][2];//0 for normal, 1 for selected.
};

#endif