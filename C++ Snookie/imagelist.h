//imagelist.h
#ifndef IMAGELIST_H
#define IMAGELIST_H

#include "controls.h"

#define IMAGELIST_OK CONTROLS_OK
#define IMAGELIST_ERROR CONTROLS_ERROR

#define MAXIMAGES 32 //this number is the maximum number of images we can store here.

/////////////////////////////////////////////////////////////////////////////////////////////
class IMAGELIST : public CONTROLS{
public:
	IMAGELIST();
	~IMAGELIST();

	int AddImageResource(int ResId, int * id);
	int CreateImageList(HINSTANCE hInstance);
	int AddImageHandle(HBITMAP Bmp, int * id);
	int AddImageFile(char * FileName, int * id);
	int DeleteImage(int id);
	int GetNumberOfImages();
	int ClearAll();
	HIMAGELIST GetHIMAGELIST();
	
private:
	int NumberOfImages;
	HIMAGELIST h;
	HINSTANCE hInstance;
};

#endif