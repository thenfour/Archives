

#pragma once

#include <windows.h>

extern AutoPlacement::Manager mgr;
extern AutoPlacement::Win32GUIMetrics metrics;

class VerticalSplitter
{
	HWND m_hwnd;
	int m_leftWidth;
	WNDPROC m_oldProc;
	int m_dragOriginCursor;
	int m_dragOriginLeftWidth;
	bool m_hasCapture;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		VerticalSplitter& pThis = *(VerticalSplitter*)GetProp(hwnd, L"fuck");
		switch(msg)
		{
		case WM_NCHITTEST:
			return HTCLIENT;
		case WM_SETCURSOR:
			SetCursor(LoadCursor(0, IDC_SIZEWE));
			return TRUE;
		case WM_LBUTTONDOWN:
			{
				pThis.m_dragOriginLeftWidth = pThis.m_leftWidth;
				POINT pt;
				GetCursorPos(&pt);
				pThis.m_dragOriginCursor = pt.x;
				SetCapture(hwnd);
				pThis.m_hasCapture = true;
				return TRUE;
			}
		case WM_LBUTTONUP:
			if(pThis.m_hasCapture)
			{
				ReleaseCapture();
				pThis.m_hasCapture = false;
				return TRUE;
			}
		case WM_MOUSEMOVE:
			if(pThis.m_hasCapture)
			{
				POINT pt;
				GetCursorPos(&pt);
				int change =pt.x - pThis.m_dragOriginCursor;
				pThis.m_leftWidth = pThis.m_dragOriginLeftWidth + change;

				mgr.OnSize(&metrics);
				return TRUE;
			}
		}
		return CallWindowProc(pThis.m_oldProc, hwnd, msg, wParam, lParam);
	}

public:
	VerticalSplitter()
		: m_hasCapture(false)
	{
	}

	void Hookup(HWND hStatic, int initialLeftWidth)
	{
		m_hwnd = hStatic;
		m_leftWidth = initialLeftWidth;

		SetProp(hStatic, L"fuck", (HANDLE)this);

		m_oldProc = (WNDPROC)SetWindowLongPtr(hStatic, GWLP_WNDPROC, (LONG_PTR)VerticalSplitter::WindowProc);
	}

	double GetLeftWidth(const std::wstring& identifierName)
	{
		return (double)m_leftWidth;
	}
};

