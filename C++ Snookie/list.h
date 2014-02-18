//list.h

#ifndef LIST_H
#define LIST_H

#include <windows.h>
#include "controls.h"

#define LIST_ERROR CONTROLS_ERROR
#define LIST_OK CONTROLS_OK

class LIST : public CONTROLS{
public:
	LIST();
	~LIST();

	int CreateList(HWND parent, HINSTANCE hInstance, RECT orect, RECT prect);
	int AddItem(char * Label, int * Index, LPARAM lParam);

private:
};

#endif

