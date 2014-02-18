/*
overlappedWindow pretty much just provides basic painting & sizing frame like you'd expect from a top level control parent.
*/

#pragma once

#include "Win32ControlHost.h"

namespace UI
{

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class OverlappedWindow :
		public Win32ControlHost
	{
		bool m_sizeable;
		bool m_wasSizeable;
		bool m_wasMaximizedThenMinimized;// see RestoreTopLevelWindow

	public:
		OverlappedWindow() :
			m_sizeable(false),
			m_wasSizeable(false),
			m_wasMaximizedThenMinimized(false)
		{
		}

		virtual bool IsTopLevelWindowSizeable()
		{
			return m_sizeable;
		}

		// how to maximize windows with no border.
		// http://blogs.msdn.com/llobo/archive/2006/08/01/Maximizing-window-_2800_with-WindowStyle_3D00_None_2900_-considering-Taskbar.aspx
		static const DWORD NonsizeableStyle = WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		static const DWORD RestoredStyle = WS_SIZEBOX | WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPSIBLINGS;

		virtual DWORD GetControlTypeId() { return 0x40ce79d5; }
		virtual std::wstring GetControlTypeName() { return L"OverlappedWindow"; };

		void Create(Rect r, const std::wstring& title, bool sizeable)
		{
			m_sizeable = sizeable;
			m_wasSizeable = sizeable;

			Win32ControlHost::Create(r, title, WS_EX_APPWINDOW, sizeable ? RestoredStyle : NonsizeableStyle);

			ShowWindow(m_hwnd, SW_NORMAL);
		}

		void OnPaint(GraphicsPtr buffer)
		{
			if(m_sizeable)
			{
				m_theme->DrawSizingFrame(buffer, m_pos);
				return;
			}
			m_theme->DrawBodyBackground(buffer, m_pos);
		}

		virtual Rect CalculateClientArea(const Size& pos)
		{
			if(m_sizeable)
			{
				int bs = m_theme->GetSizingFrameSize();
				return Rect(
					bs,
					bs,
					pos.Width - (bs * 2),
					pos.Height - (bs * 2)
					);
			}
			return Rect(0, 0, pos.Width, pos.Height);
		}

		LRESULT HitTest(const Point& p)
		{
			Rect rc = Control::CalculateClientArea();
			if(!m_sizeable || (TRUE == rc.Contains(p)))
			{
				return HTCLIENT;
			}

			int cornerSize = m_theme->GetSizingCornerSize();
			bool isLeft = p.X < cornerSize;
			bool isTop = p.Y < cornerSize;
			bool isRight = (rc.GetRight() - p.X) < cornerSize;
			bool isBottom = (rc.GetBottom() - p.Y) < cornerSize;

			if(isTop)
			{
				if(isRight)
					return HTTOPRIGHT;
				if(isLeft)
					return HTTOPLEFT;
				return HTTOP;
			}

			if(isBottom)
			{
				if(isRight)
					return HTBOTTOMRIGHT;
				if(isLeft)
					return HTBOTTOMLEFT;
				return HTBOTTOM;
			}
			if(isRight)
				return HTRIGHT;
			return HTLEFT;
		}

		virtual bool IsTopLevelWindowMaximized()
		{
			return TRUE == IsZoomed(m_hwnd);
		}

		virtual void MaximizeTopLevelWindow()
		{
			m_sizeable = false;
			m_wasMaximizedThenMinimized = false;
			ShowWindow(Win32_GetHWND(), SW_MAXIMIZE);
		}

		virtual void RestoreTopLevelWindow()
		{
			if(m_wasMaximizedThenMinimized)
			{
				// if you maximize, then minimize, then restore, it should actually go back to maximized. so i need to remember whether it was maximized.
				// when left to DefWindowProc this is automatic. but it's a headache here for me to design this to call DefWindowProc.
				MaximizeTopLevelWindow();
			}
			else
			{
				m_sizeable = m_wasSizeable;
				ShowWindow(Win32_GetHWND(), SW_RESTORE);
			}
		}

		virtual void MinimizeTopLevelWindow()
		{
			m_wasMaximizedThenMinimized = IsTopLevelWindowMaximized();
			ShowWindow(m_hwnd, SW_MINIMIZE);
		}
	};
}

