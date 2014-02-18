/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       ddutil.cpp
 *  Content:    Routines for loading bitmap and palettes from resources
 *
 ***************************************************************************/

#ifndef __DDUTIL
#define __DDUTIL

#include "ddraw.h"
#include <windows.h>
#include <tchar.h>

extern IDirectDrawPalette * DDLoadPalette(IDirectDraw *pdd, LPCTSTR szBitmap);
extern IDirectDrawSurface * DDLoadBitmap(IDirectDraw *pdd, LPCTSTR szBitmap, int dx, int dy);
extern HRESULT              DDReLoadBitmap(IDirectDrawSurface *pdds, LPCTSTR szBitmap);
extern HRESULT              DDCopyBitmap(IDirectDrawSurface *pdds, HBITMAP hbm, int x, int y, int dx, int dy);
extern DWORD                DDColorMatch(IDirectDrawSurface *pdds, COLORREF rgb);
extern HRESULT              DDSetColorKey(IDirectDrawSurface *pdds, COLORREF rgb);
#endif
