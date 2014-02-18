

#include <windows.h>
#include "..\0222 LayoutTool\layouttool.h"
#include "resource.h"
#include "splitter.h"

HBRUSH g_leftBrush = 0;
HBRUSH g_splitterBrush = 0;
HBRUSH g_rightBrush = 0;

LibCC::Log* LibCC::g_pLog = 0;

VerticalSplitter g_splitter;

AutoPlacement::Manager mgr;
AutoPlacement::Win32GUIMetrics metrics;

BOOL CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_SIZE:
		{
			mgr.OnSize(&metrics);
			return TRUE;
		}
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;

	case WM_CTLCOLORSTATIC:
		{
			if((HWND)lParam == GetDlgItem(hwnd, IDC_LEFT))
			{
				SetBkMode((HDC)wParam, TRANSPARENT);
				return (BOOL)g_leftBrush;
			}
			if((HWND)lParam == GetDlgItem(hwnd, IDC_SPLITTER))
			{
				SetBkMode((HDC)wParam, TRANSPARENT);
				return (BOOL)g_splitterBrush;
			}
			if((HWND)lParam == GetDlgItem(hwnd, IDC_RIGHT))
			{
				SetBkMode((HDC)wParam, TRANSPARENT);
				return (BOOL)g_rightBrush;
			}
			return TRUE;
		}
	case WM_INITDIALOG:
		{
			g_splitter.Hookup(GetDlgItem(hwnd, IDC_SPLITTER), 100);

			mgr.RegisterSymbol(L"left", AutoPlacement::Win32Control(GetDlgItem(hwnd, IDC_LEFT)));
			mgr.RegisterSymbol(L"splitter", AutoPlacement::Win32Control(GetDlgItem(hwnd, IDC_SPLITTER)));
			mgr.RegisterSymbol(L"right", AutoPlacement::Win32Control(GetDlgItem(hwnd, IDC_RIGHT)));

			mgr.RegisterVariable(L"splitterWidth", 6);
			mgr.RegisterVariable(L"leftSize", &g_splitter, &VerticalSplitter::GetLeftWidth);

			mgr.RunFile(L"layout.txt", &metrics);
			mgr.OnSize(&metrics);
			return TRUE;
		}
	}
	return FALSE;
}

int WINAPI wWinMain(HINSTANCE hInstance, int, int, int)
{
	LibCC::g_pLog = new LibCC::Log(L"LayoutTool.log", hInstance, true, false, false);

	g_leftBrush = CreateSolidBrush(RGB(192,128,64));
	g_splitterBrush = CreateSolidBrush(RGB(192,192,32));
	g_rightBrush = CreateSolidBrush(RGB(64,128,192));

	::DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc);

	DeleteObject(g_rightBrush);
	DeleteObject(g_leftBrush);
	DeleteObject(g_splitterBrush);

	delete LibCC::g_pLog;
	return 0;
}

