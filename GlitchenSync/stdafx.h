// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define _SECURE_SCL 0

#include <stdio.h>
#include <tchar.h>


#define LIBCC_LOG_WINDOW_WIDTH 500
#define LIBCC_LOG_WINDOW_HEIGHT 500
// TODO: reference additional headers your program requires here
#include <windows.h>
#include <string>
#include <vector>

extern HINSTANCE g_hInstance;