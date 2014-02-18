/*
	here are some considerations when using Windows controls in this ui lib:
	- coordinates need to be converted from these coords to those absolute to the nearest win32 window
	- mouse messages may get deceptive. for example, moving the mouse onto a win32 window will not send a OnMouseLeave() to the old control.
		i think the true solution is to use a windows hook for overlapped window instead of the window proc. for now, i won't worry about it.
*/

#pragma once


#include "control.h"


namespace UI
{
	class Textbox : 
		public Control
	{
		HWND m_hwnd;
		HFONT m_font;
		HBRUSH m_background;

	public:
		Textbox() :
			m_hwnd(0),
			m_font(0)
		{
			OnSize.Add(this, &Textbox::TBOnSize);
			OnCtlColorEdit.Add(this, &Textbox::TBOnCtlColorEdit);
			OnMouseEnter.Add(this, &Textbox::TBOnMouseEnter);
			OnMouseLeave.Add(this, &Textbox::TBOnMouseLeave);
			OnMouseMove.Add(this, &Textbox::TBOnMouseMove);
		}

		~Textbox()
		{
			EnsureDestroyed();
		}

		virtual DWORD GetControlTypeId() { return 0x5c7383ca; }
		virtual std::wstring GetControlTypeName() { return L"Textbox"; };

		void SetCaption(const std::wstring& str)
		{
			SetWindowText(m_hwnd, str.c_str());
		}

		void EnsureDestroyed()
		{
			if(m_hwnd)
			{
				DestroyWindow(m_hwnd);
				m_hwnd = 0;

				DeleteObject(m_font);
				m_font = 0;

				DeleteObject(m_background);
				m_background = 0;

				UnregisterWin32Child(m_hwnd);
			}
		}

		void EnsureCreated()
		{
			if(!m_hwnd && m_parent)
			{
				m_background = CreateSolidBrush(m_theme->GetBodyBackgroundSolidColor().ToCOLORREF());

				Rect rc = m_parent->Win32_TranslateRect(m_pos);
				m_hwnd = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | ES_MULTILINE, rc.X, rc.Y, rc.Width, rc.Height, Win32_GetHWND(), 0, 0, 0);
				RegisterWin32Child(m_hwnd, this);
				ResetSizingInfo(m_sizeInfo);

				HDC dc = GetDC(0);
				m_font = CreateFont(-MulDiv(8, GetDeviceCaps(dc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, L"Tahoma");
				ReleaseDC(0, dc);
				SendMessage(m_hwnd, WM_SETFONT, (WPARAM)m_font, 0);

				ShowWindow(m_hwnd, SW_NORMAL);
			}
		}

		void TBOnCtlColorEdit(Control* origin, std::pair<HDC, HBRUSH>& x)
		{
			SetTextColor(x.first, m_theme->GetBodyForegroundSolidColor().ToCOLORREF());
			SetBkColor(x.first, m_theme->GetBodyBackgroundSolidColor().ToCOLORREF());
			x.second = m_background;
		}

		void TBOnMouseLeave(Control* origin)
		{
			//LibCC::g_pLog->Message(L"TBOnMouseLeave");
		}

		void TBOnMouseEnter(Control* origin)
		{
			//LibCC::g_pLog->Message(L"TBOnMouseEnter");
		}

		void TBOnMouseMove(Control* origin, Point x)
		{
			//LibCC::g_pLog->Message(L"TBOnMouseMove");
		}

		void TBOnSize(Control* origin)
		{
			EnsureCreated();
			if(m_parent)
			{
				Rect rc = m_parent->Win32_TranslateRect(m_pos);
				MoveWindow(m_hwnd, rc.X, rc.Y, rc.Width, rc.Height, TRUE);
			}
		}

		Event0 OnTextChanged;

		void OnPaint(GraphicsPtr)
		{
			EnsureCreated();
		}
	};
}

