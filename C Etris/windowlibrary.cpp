#include "windowlibrary.h"

HWND MakeAWindow(WNDPROC WindowProc, HINSTANCE hInstance, LPCTSTR ClassName)
{
HWND hWnd;

WNDCLASS wclass;
wclass.style = CS_HREDRAW | CS_VREDRAW;
wclass.lpfnWndProc = WindowProc;
wclass.cbClsExtra = 0;
wclass.cbWndExtra = 0;
wclass.hInstance = hInstance;
wclass.hIcon = LoadIcon( hInstance, IDI_APPLICATION );
wclass.hCursor = LoadCursor( NULL, IDC_ARROW );
wclass.hbrBackground =0;
wclass.lpszMenuName = "x";
wclass.lpszClassName = ClassName;

RegisterClass(&wclass);

hWnd=CreateWindow(ClassName, "Yup, A Window.", WS_POPUP, 0, 0, 320,200, NULL, NULL, hInstance, NULL);
ShowWindow(hWnd, SW_SHOWNORMAL);

return hWnd;
}