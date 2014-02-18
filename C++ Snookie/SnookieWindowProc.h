//SnookieWindowProc.h
#ifndef SNOOKIEWINDOWPROC_H
#define SNOOKIEWINDOWPROC_H

#define _WIN32_IE 0x0400
#include <windows.h>
#include <commctrl.h>
#include "debugwindow.h"
#include "snookiewindows.h"
#include "snookieCatalogv3.h"
#include "resource.h"
#include "resource2.h"
#include "findfileproc.h"
#include <shlobj.h>
#include "editbox.h"
#include "list.h"
#include "imagelist.h"
#include "static.h"
#include "statusbar.h"
#include "treeview.h"

#define SWP_OK 0
#define SWP_ERROR 1

int swpBeginLabelEdit();
int swpEditChange();
int swpSaveAs();
int swpOpen();
int swpSave();
int swpFindFile();
int swpClose();
int swpAddPath();
int swpRemovePath();
int swpFindNext();
int swpFileNew();
int swpFileExport();
int swpLabelEdit(NMTVDISPINFO* ptvdi);
int swpAdjustVSlider(int Distance);
int swpAdjustHSlider(int Distance);
int swpDropFiles(HANDLE hDrop);

#define SNOOKIE_EXPORT_INVALID 0
#define SNOOKIE_EXPORT_TXT 1

int swpExport(int ExportMethod);

SNOOKIENODE* GetNodeFromTree(HTREEITEM ht);

LRESULT CALLBACK SnookieWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif