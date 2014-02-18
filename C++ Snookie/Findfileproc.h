//FindFileProc.h
#ifndef FINDFILEPROC_H
#define FINDFILEPROC_H

#include <windows.h>
#include "resource.h"
#include "resource2.h"

BOOL CALLBACK FindFileProc(
  HWND hwndDlg,  // handle to dialog box
  UINT uMsg,     // message
  WPARAM wParam, // first message parameter
  LPARAM lParam  // second message parameter
);

#endif