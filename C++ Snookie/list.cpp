//list.cpp

#include "list.h"

//////////////////////////////////////////////////////////////////////////////
LIST::LIST(){
}
LIST::~LIST(){
}
//////////////////////////////////////////////////////////////////////////////
int LIST::CreateList(HWND parent, HINSTANCE hInstance, RECT orect, RECT prect){
	CopyRect(&this->offsetrect, &orect);
	CopyRect(&this->percentrect, &prect);
	this->parent=parent;
	this->style=
		WS_VISIBLE | 
		WS_CHILD |
		LVS_EDITLABELS |
		LVS_LIST;

	this->hwnd=CreateWindowEx(0, WC_LISTVIEW, "List",this->style,0, 0, 1, 1, parent, (HMENU)this->GetId(), hInstance, 0);
	this->Size();
	return LIST_OK;
}

//////////////////////////////////////////////////////////////////////////////
int LIST::AddItem(char * Label, int * Index, LPARAM lParam){
	LVITEM lvitem;
	ZeroMemory(&lvitem, sizeof(lvitem));
	lvitem.mask=LVIF_TEXT | LVIF_PARAM ;
    lvitem.iItem=*Index;
    lvitem.iSubItem=0; 
    lvitem.state=0; 
    lvitem.stateMask=0; 
    lvitem.pszText=Label; 
    lvitem.cchTextMax=0;
    lvitem.iImage=0; 
    lvitem.lParam=lParam;

	*Index=ListView_InsertItem(this->GetHwnd(), &lvitem);
	return LIST_OK;
}