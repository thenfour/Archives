//treeview.cpp
#include "treeview.h"
#include "resource.h"
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL TREEVIEW::SetSel(HTREEITEM ht){
	return TreeView_SelectItem(this->GetHwnd(),ht);
}
////////////////////////////////////////////////////////////////////////////////////////////////
HTREEITEM TREEVIEW::GetRoot(){
	return TreeView_GetRoot(this->GetHwnd());
}
////////////////////////////////////////////////////////////////////////////////////////////////
HTREEITEM TREEVIEW::GetFirstChild(HTREEITEM ht){
	return TreeView_GetChild(this->hwnd,ht);
}
////////////////////////////////////////////////////////////////////////////////////////////////
HTREEITEM TREEVIEW::GetParent(HTREEITEM ht){
	return TreeView_GetParent(this->hwnd, ht);
}
////////////////////////////////////////////////////////////////////////////////////////////////
HTREEITEM TREEVIEW::GetNextSibling(HTREEITEM ht){
	return TreeView_GetNextSibling(this->hwnd, ht);
}
////////////////////////////////////////////////////////////////////////////////////////////////
HTREEITEM TREEVIEW::GetSel(){
	return TreeView_GetSelection(this->hwnd);
}
////////////////////////////////////////////////////////////////////////////////////////////////
LPARAM TREEVIEW::GetLParam(HTREEITEM ht){
	TVITEM tvitem;
	ZeroMemory(&tvitem, sizeof(tvitem));
	tvitem.mask=TVIF_PARAM | TVIF_HANDLE | TVIF_CHILDREN;
	tvitem.hItem=ht;
	TreeView_GetItem(this->GetHwnd(), &tvitem);
	return tvitem.lParam;
}
////////////////////////////////////////////////////////////////////////////////////////////////
TREEVIEW::TREEVIEW(){
	ZeroMemory(&this->ImageId, sizeof(this->ImageId));
	this->CurrentImage=0;
}
////////////////////////////////////////////////////////////////////////////////////////////////
TREEVIEW::~TREEVIEW(){
}
////////////////////////////////////////////////////////////////////////////////////////////////
IMAGELIST TREEVIEW::GetImageList(){
	return this->ImageList;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int TREEVIEW::CreateTreeView(RECT prect, RECT orect, HWND hwnd, HINSTANCE hInstance){
	this->hInstance=hInstance;
	//now create a treeview in it.
	INITCOMMONCONTROLSEX cc;
	cc.dwSize=sizeof(cc);
	cc.dwICC=ICC_TREEVIEW_CLASSES;
	if(InitCommonControlsEx(&cc)==FALSE) return TREEVIEW_ERROR;

	//copy the HWND of the parent to local vars.
	this->parent=hwnd;

	//get the coords for this;
	//put the rect into this->rect;
	this->percentrect.top=prect.top;
	this->percentrect.bottom=prect.bottom;
	this->percentrect.left=prect.left;
	this->percentrect.right=prect.right;
	this->offsetrect.top=orect.top;
	this->offsetrect.bottom=orect.bottom;
	this->offsetrect.left=orect.left;
	this->offsetrect.right=orect.right;

	this->style=WS_CHILD | 
		WS_VISIBLE | 
		//TVS_HASLINES | 
		TVS_HASBUTTONS |
		TVS_LINESATROOT | 
		TVS_EDITLABELS;
//		WS_BORDER;
	this->hwnd=CreateWindow(WC_TREEVIEW, NULL,
		this->style,
		0, 0, 100, 100, hwnd, (HMENU)this->GetId(), hInstance, NULL);
	if(this->hwnd==0) return TREEVIEW_ERROR;

	if(this->Size()==CONTROLS_ERROR) return TREEVIEW_ERROR;

	//now create an imagelist for the imagelist.
	this->ImageList.CreateImageList(this->hInstance);
	TreeView_SetImageList(this->GetHwnd(), this->ImageList.GetHIMAGELIST(), TVSIL_NORMAL);

	return TREEVIEW_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int TREEVIEW::AddImage(int *id, HBITMAP norm, HBITMAP sel){
	//adds an image to the list.
	if(this->CurrentImage >-1){
		int ids, idn;
		this->ImageList.AddImageHandle(sel, &ids);
		this->ImageList.AddImageHandle(norm, &idn);
		this->ImageId[this->CurrentImage][0]=idn;
		this->ImageId[this->CurrentImage][1]=ids;
		*id=this->CurrentImage;
		this->CurrentImage++;
		if(this->CurrentImage > MAXIMAGE) this->CurrentImage=-1;
		return TREEVIEW_OK;
	}
	return TREEVIEW_ERROR;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int TREEVIEW::AddNode(HTREEITEM *hti, HTREEITEM htiParent, char * Text, BOOL HasChildren, LPARAM lParam, int image){

	TVINSERTSTRUCT ts;
	ts.hParent=htiParent;
	ts.hInsertAfter=TVI_SORT;
	ts.item.mask=TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE ;
	ts.item.iImage=this->ImageId[image][0];
	ts.item.iSelectedImage=this->ImageId[image][1];
	ts.item.pszText=Text;
	ts.item.cchTextMax=strlen(Text);
	ts.item.cChildren=HasChildren==TRUE ? 1:0;
	ts.item.lParam=lParam;//when the user selects this particular node, this value will be passed.
	*hti=TreeView_InsertItem(this->hwnd, &ts);
	if(*hti==NULL) return TREEVIEW_ERROR;

	
	return TREEVIEW_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int TREEVIEW::DeleteNode(HTREEITEM hitem){
	BOOL rvalue;
	rvalue=TreeView_DeleteItem(this->hwnd, hitem);
	return (rvalue==FALSE) ? TREEVIEW_ERROR : TREEVIEW_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int TREEVIEW::Clear(){
	if(!TreeView_DeleteAllItems(this->hwnd)){
		return TREEVIEW_ERROR;
	}
	return TREEVIEW_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//returns the next node in the list, whether it's a child, sibling, next parent, whatever
//return value is the newly selected node
HTREEITEM TREEVIEW::GetNext(){
	return this->GetNext(this->GetSel());
}
//Gets the next node from a certain node...
HTREEITEM TREEVIEW::GetNext(HTREEITEM sel){
	HTREEITEM ht;
	HTREEITEM ht2;
	BOOL done;
	done=FALSE;

	ht=TreeView_GetChild(this->hwnd, sel);//get the child of hte selection
	//now figure out if that's the next one in the list.
	if(ht==NULL){
		//there's no child; get the sibling
		ht=TreeView_GetNextSibling(this->hwnd, sel);
		if(ht==NULL){
			//there's no sibling or child; then we must proceed to the next parent.
			ht2=sel;
			do{
				ht=TreeView_GetParent(this->hwnd, ht2);
				if(ht == NULL){
					ht=0;
					done=TRUE;
				}
				ht2=TreeView_GetNextSibling(this->hwnd, ht);
				if(ht2 != NULL){ //there's something there
					ht=ht2;
					done=TRUE;
				}
				ht2=ht;
			}while(done == FALSE);
		}
	}
	return ht;
}

////////////////////////////////////////////////////////////////////////////////////////////////
HTREEITEM TREEVIEW::SelectNext(){
	HTREEITEM ht;
	ht=GetNext();
	if(ht != 0) TreeView_Select(this->hwnd, ht, TVGN_CARET);
	return ht;
}