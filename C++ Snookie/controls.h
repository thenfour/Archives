//controls.h -
//this is a base class for controls
#ifndef CONTROLS_H
#define CONTROLS_H

#include <windows.h>
#include <commctrl.h>

#define CONTROLS_OK 0
#define CONTROLS_ERROR 1

////////////////////////////////////////////////////////////////////////////////////////////////
class CONTROLS{
public:
	CONTROLS();
	~CONTROLS();

	virtual int Size();
	unsigned int GetId();
	HWND GetHwnd();
	HWND GetParentHWND();
	int GetRect(RECT * rect);
	int SetPlacement(RECT * orect, RECT * prect);
	int GetPlacement(RECT * prect, RECT * orect);
	static unsigned int NextId;
	int Redraw();

private:
protected:
	UINT id;

	RECT offsetrect;
	RECT percentrect;
	HWND hwnd;
	HWND parent;
	DWORD style;
	int ControlRectToRect(RECT * newrect, RECT * prect, RECT * orect, RECT * clientrect);
};

#endif

