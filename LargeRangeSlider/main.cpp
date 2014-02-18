/*
  Routines:
  1) real -> display (render)
  2) display -> real (hit test)
  3) some pattern rendering
*/



#include "stdafx.h"
#include "slider.h"
#include "testvline.h"
#include "testhatchline.h"
#include "testhatch2.h"
#include "testvlinescan.h"

Slider g_slider;
TestVLine g_testVLine;
TestHatchLine g_testHatch;
TestHatchLine2 g_testHatch2;
TestVLineScan g_testVLineScan;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
  case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);
      EndPaint(hWnd, &ps);

      g_slider.Render(hWnd);
      //g_testVLine.Render(hWnd);
      //g_testHatch2.Render(hWnd);
      //g_testVLineScan.Render(hWnd);
      //g_testHatch.Render(hWnd);

      return 0;
    }
  case WM_MOUSEMOVE:
    {
      int x = LOWORD(lParam);
      int y = HIWORD(lParam);
      g_slider.SetCursor(g_slider.DisplayToReal(x));
			g_slider.SetFocus(g_slider.DisplayToReal(x));
      InvalidateRect(hWnd, 0, FALSE);
      return 0;
    }
  case WM_SIZE:
    {
      RECT rcClient;
      GetClientRect(hWnd, &rcClient);
      g_slider.OnSize(rcClient.right, rcClient.bottom);
      g_testVLine.OnSize(rcClient.right, rcClient.bottom);
      g_testHatch2.OnSize(rcClient.right, rcClient.bottom);
      g_testHatch.OnSize(rcClient.right, rcClient.bottom);
      g_testVLineScan.OnSize(rcClient.right, rcClient.bottom);
      return 0;
    }
  case WM_CLOSE:
    DestroyWindow(hWnd);
    return 0;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  case WM_CREATE:
    return 0;
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, int, PWSTR)
{
  g_slider.SetMediaLength(HoursToMS(2));
  g_slider.SetFocus(g_slider.GetMediaLength() / 2);

  WNDCLASSW wc = {0};
  wc.hInstance = hInstance;
  wc.lpfnWndProc = WindowProc;
  wc.lpszClassName = L"L";
  wc.style = CS_VREDRAW | CS_HREDRAW;
  RegisterClass(&wc);

  HWND h = CreateWindow(L"L", L"aoeu", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 800, 200, 0, 0, 0, 0);

  MSG msg;
  while(GetMessage(&msg, 0, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}


