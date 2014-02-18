// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
//extern HINSTANCE g_hInstance;
#include "..\..\libcc\libcc\stringutil.hpp"
#include "..\..\libcc\libcc\winapi.hpp"
#include "..\..\libcc\libcc\log.hpp"
#include <gdiplus.h>
#include "memory"
/*
typedef std::tr1::shared_ptr<Gdiplus::Graphics> GraphicsPtr;
typedef std::tr1::shared_ptr<Gdiplus::Image> ImagePtr;
typedef std::tr1::shared_ptr<Gdiplus::Bitmap> BitmapPtr;
typedef std::tr1::shared_ptr<Gdiplus::Matrix> MatrixPtr;

inline GraphicsPtr NewGraphicsPtr(HDC dc){ return GraphicsPtr(new Gdiplus::Graphics(dc)); }
inline GraphicsPtr NewGraphicsPtr(ImagePtr i){ return GraphicsPtr(new Gdiplus::Graphics(i.get())); }

typedef Gdiplus::Rect Rect;
typedef Gdiplus::Point Point;
typedef Gdiplus::Size Size;

*/