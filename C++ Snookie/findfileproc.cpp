//FindFileProc.cpp
#include "findfileproc.h"

BOOL CALLBACK FindFileProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam){
	static char * Text;
	int wID;

	switch(uMsg){
	case WM_INITDIALOG:
		//lParam is the passed value.
		Text=(char*)lParam;
		//and set focus to the edit box.
		HWND hEdit;
		hEdit=GetDlgItem(hwndDlg, IDC_TEXTBOX);
		SetFocus(hEdit);
		return 0;
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		return 1;
		break;
	case WM_PAINT:
		HDC hDC;
		PAINTSTRUCT ps;
		hDC=BeginPaint(hwndDlg, &ps);
		EndPaint(hwndDlg, &ps);
		return 1;
		break;

	case WM_COMMAND:
		wID = LOWORD(wParam);
		switch(wID){
		case ID_OK:
			//copy the text to the buffer
			GetDlgItemText(hwndDlg, IDC_TEXTBOX, Text, MAX_PATH);
			EndDialog(hwndDlg, 0);
			return 1;
			break;
		case IDCANCEL:
		case ID_CANCEL:
			EndDialog(hwndDlg, 0);
			return 1;
			break;
		}
		break;
	}
	return 0;
}