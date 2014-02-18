/*
  Game
    Renderer
    AI
      Simulation
*/

#include <windows.h>
#include "renderer.h"
#include <tchar.h>
#include <string>

MarioSim::Game g_game(5,5);
MarioSim::Point g_tracking;
bool g_isTracking(false);
MarioSim::Point g_selected;
bool g_1selected(false);

HPEN g_penSelected;
HPEN g_penTracking;
HBRUSH g_hBackground;

std::string g_banner;

UINT IDC_RESET;
const int g_nEachCard = 8; 

void RedrawCard(HWND hWnd, int x, int y)
{
  RECT rc;
  MarioSim::GetCardRect(x, y, rc);
  InflateRect(&rc, 10, 10);
  InvalidateRect(hWnd, &rc, FALSE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
  case WM_SYSCOMMAND:
    if(wParam == IDC_RESET)
    {
      g_game.Reset(g_nEachCard);
      RECT rcClient;
      GetClientRect(hWnd, &rcClient);
      InvalidateRect(hWnd, &rcClient, FALSE);
      return 0;
    }
    break;
  case WM_LBUTTONDOWN:
    {
      int cardx, cardy;
      if(MarioSim::HitTest(g_game, LOWORD(lParam), HIWORD(lParam), cardx, cardy))
      {
        if(g_1selected)
        {
          if(cardx == g_selected.first && cardy == g_selected.second)
          {
            // deselect the current.
            g_1selected = false;
            RedrawCard(hWnd, cardx, cardy);
          }
          else
          {
            // make a move!
            MarioSim::Point b(cardx, cardy);
            int n;
            MarioSim::Game::Outcome o = g_game.Move(g_selected, b, n);
            switch(o)
            {
            case MarioSim::Game::GameLost:
              g_banner = "Lost the game";
              break;
            case MarioSim::Game::GameWon:
              g_banner = "Won the game!";
              break;
            case MarioSim::Game::Success:
              g_banner = "Move success.";
              break;
            case MarioSim::Game::InvalidMove:
              g_banner = "Invalid move";
              break;
            case MarioSim::Game::Unknown:
              g_banner = "(unknown)";
              break;
            }
            g_1selected = false;

            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            InvalidateRect(hWnd, &rcClient, FALSE);
          }
        }
        else
        {
          g_1selected = true;
          g_selected.first = cardx;
          g_selected.second = cardy;
          RedrawCard(hWnd, cardx, cardy);
        }
      }
      return 0;
    }
  case WM_MOUSEMOVE:
    {
      int cardx, cardy;
      bool isTracking(false);
      bool changed = false;
      if(MarioSim::HitTest(g_game, LOWORD(lParam), HIWORD(lParam), cardx, cardy))
      {
        isTracking = true;
        if(g_tracking.first != cardx || g_tracking.second != cardy)
        {
          changed = true;
        }
      }
      if(isTracking != g_isTracking)
      {
        changed = true;
      }
      g_isTracking = isTracking;
      if(changed)
      {
        RedrawCard(hWnd, cardx, cardy);
        RedrawCard(hWnd, g_tracking.first, g_tracking.second);
        g_tracking.first = cardx;
        g_tracking.second = cardy;
      }
      return 0;
    }
  case WM_CREATE:
    {
      // add an option to the system menu
      HMENU h = GetSystemMenu(hWnd, FALSE);

      // figure out what ID we can safely use
      UINT i = 0;
      UINT nHighestCmd = 0;
      while(1)
      {
        i = GetMenuItemID(h, i);
        if(-1 == i)
        {
          break;
        }
        if(i > nHighestCmd)
        {
          nHighestCmd = i;
        }
      }

      IDC_RESET = nHighestCmd + 1;

      MENUITEMINFO mii = {0};
      mii.cbSize = sizeof(MENUITEMINFO);
      mii.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_ID;
      mii.fType = MFT_STRING;
      mii.wID = IDC_RESET;
      mii.dwTypeData = "Reset";
      BOOL r = InsertMenuItem(h, IDC_RESET, FALSE, &mii);
      return 0;
    }
  case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);
      HDC dc = GetDC(hWnd);
      RECT rcClient;
      GetClientRect(hWnd, &rcClient);
      FillRect(ps.hdc, &rcClient, g_hBackground);
      MarioSim::RenderGame(dc, g_game);
      if(g_1selected)
      {
        // draw the selected one
        RECT rcCard;
        MarioSim::GetCardRect(g_selected.first, g_selected.second, rcCard);
        SelectObject(dc, GetStockObject(NULL_BRUSH));
        SelectObject(dc, g_penSelected);
        Rectangle(dc, rcCard.left, rcCard.top, rcCard.right, rcCard.bottom);
      }
      // draw tracking one
      if(g_isTracking)
      {
        RECT rcCard;
        MarioSim::GetCardRect(g_tracking.first, g_tracking.second, rcCard);
        SelectObject(dc, GetStockObject(NULL_BRUSH));
        SelectObject(dc, g_penTracking);
        Rectangle(dc, rcCard.left, rcCard.top, rcCard.right, rcCard.bottom);
      }
      // draw banner.
      TextOut(dc, 0, 0, g_banner.c_str(), g_banner.length());
      ReleaseDC(hWnd, dc);
      EndPaint(hWnd, &ps);
      return 0;
    }
  case WM_CLOSE:
    DestroyWindow(hWnd);
    return 0;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//#define __W(x)      L ## x
//#define _W(x)       __W(x)
//
//#define SHITA "hithere"
//#define SHITW _W(SHITA)
//#define SHIT _T(SHITA)

#define __W(x)      L ## x
#define _W(x)       __W(x)
#define SHITA "hithere"
#define SHITW _W(SHITA)


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, int)
{
  g_hBackground = CreateSolidBrush(RGB(40,45,45));
  g_penSelected = CreatePen(PS_SOLID, 4, RGB(255,0,0));
  g_penTracking = CreatePen(PS_SOLID, 2, RGB(255,255,0));

  std::wstring nthx = SHITW;
  //std::string thA = SHITA;
  //std::basic_string<TCHAR> thT = SHIT;


  WNDCLASS wc = {0};
  wc.hbrBackground = g_hBackground;
  wc.hCursor = LoadCursor(0, IDC_ARROW);
  wc.hInstance = hInstance;
  wc.lpfnWndProc = WndProc;
  wc.lpszClassName = "a";
  ATOM x = RegisterClass(&wc);

  g_banner = "begin.";

  HWND hWnd = CreateWindowEx(0, "a", "a", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 500, 500, 0, 0, hInstance, 0);
  
  //RECT rc;
  //rc.left = 0;
  //rc.right = 4;
  //rc.top = 0;
  //rc.bottom = 8;
  //MapDialogRect(hWnd, &rc);

  HDC dcWindow = GetDC(hWnd);
  HFONT windowFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
  HGDIOBJ hold = (HFONT)SelectObject(dcWindow, windowFont);
  TEXTMETRIC tm;
  GetTextMetrics(dcWindow, &tm);
  int baseUnitY = tm.tmHeight;
  SIZE s;
  GetTextExtentPoint32(dcWindow, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52, &s);
  int baseUnitX = (s.cx / 26 + 1) / 2;
  SelectObject(dcWindow, hold);
  ReleaseDC(hWnd, dcWindow);


  srand(GetTickCount());
  g_game.Reset(g_nEachCard);

  MSG msg;
  while(GetMessage(&msg, 0, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return 0;
}


