//imagelist.cpp
#define OEMRESOURCE
#include "imagelist.h"

#define IMAGELIST_OK CONTROLS_OK
#define IMAGELIST_ERROR CONTROLS_ERROR

///////////////////////////////////////////////////////////////////////////////////////////////
IMAGELIST::IMAGELIST(){
	this->h=0;
}
///////////////////////////////////////////////////////////////////////////////////////////////
IMAGELIST::~IMAGELIST(){
}
HIMAGELIST IMAGELIST::GetHIMAGELIST(){
	return this->h;
}

///////////////////////////////////////////////////////////////////////////////////////////////
int IMAGELIST::CreateImageList(HINSTANCE hInstance){
	this->hInstance=hInstance;
	int sizex=GetSystemMetrics(SM_CXICON);
	int sizey=GetSystemMetrics(SM_CYICON);
	sizex=16;
	sizey=16;
	this->h=ImageList_Create(sizex,sizey,FALSE,MAXIMAGES,0);
	return IMAGELIST_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int IMAGELIST::AddImageHandle(HBITMAP Bmp, int * id){
	//adds an image to the list by way of a handle to the image.
	//the id of the image is returned to id.
	*id=ImageList_Add(this->h, Bmp, NULL);
	return IMAGELIST_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int IMAGELIST::AddImageResource(int ResId, int * id){
	//************************doesn't work.
	//get a handle and call AddImageHandle
	HBITMAP temp;
	temp=(HBITMAP)LoadImage(this->hInstance, MAKEINTRESOURCE(ResId), IMAGE_BITMAP, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_DEFAULTCOLOR);
	this->AddImageHandle(temp, id);
	DeleteObject(temp);
	return IMAGELIST_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int IMAGELIST::AddImageFile(char * FileName, int * id){
	//get a handle to a bmp, and call AddImageHandle.
	HBITMAP temp;
	temp=(HBITMAP)LoadImage(NULL, FileName, IMAGE_BITMAP, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	this->AddImageHandle(temp, id);
	DeleteObject(temp);
	return IMAGELIST_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int IMAGELIST::DeleteImage(int id){
	return IMAGELIST_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int IMAGELIST::GetNumberOfImages(){
	return IMAGELIST_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////
int IMAGELIST::ClearAll(){
	return IMAGELIST_OK;
}
