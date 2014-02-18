

#pragma once

#include "control.h"
#include "theme.h"
#include "..\..\libcc\libcc/timer.hpp"

extern HINSTANCE g_hInstance;

namespace UI
{
	// the only item that actually has a real WINDOWS window
	class OverlappedWindow :
		public Control
	{
		static ATOM m_propAtom;
		static ATOM m_classAtom;
		static int m_refCount;
		#define THATCH_CLASS L"thatch window class"
		BitmapPtr m_offscreen;
		bool m_sizeable;

		bool m_subscribedToMouseLeave;
	protected:
		HWND m_hwnd;
	public:
		OverlappedWindow() :
			m_hwnd(0),
			m_sizeable(false),
			m_subscribedToMouseLeave(false)
		{
		}
		~OverlappedWindow()
		{
			if(m_hwnd)
			{
				DestroyWindow(m_hwnd);
			}
			if(m_refCount == 0)
			{
				UnregisterClass((PCWSTR)m_classAtom, g_hInstance);
				GlobalDeleteAtom(m_propAtom);
				m_classAtom = 0;
				m_propAtom = 0;
			}
		}

		virtual DWORD GetControlTypeId() { return 0x8db51b26; }

		Event0 OnClose;
		Event0 OnDestroy;
		Event0 OnActivate;
		Event0 OnDeactivate;

		virtual HWND Win32_GetHWND()
		{
			return m_hwnd;
		}

		virtual Point ClientToScreen(const Point& in)
		{
			Point ptWindow = ClientToWindow(in);
			POINT p;
			p.x = ptWindow.X;
			p.y = ptWindow.Y;
			::ClientToScreen(m_hwnd, &p);
			return Point(p.x, p.y);
		}

		void Create(Rect r, const std::wstring& title, bool sizeable)
		{
			if(m_refCount == 0)
			{
				m_propAtom = GlobalAddAtom(L"thatch prop");

				WNDCLASSEX wcx = {0};
				wcx.cbSize = sizeof(wcx);
				wcx.hCursor = LoadCursor(0, IDC_ARROW);
				wcx.hInstance = g_hInstance;
				wcx.lpfnWndProc = OverlappedWindow::WindowProc;
				wcx.lpszClassName = THATCH_CLASS;
				wcx.style = CS_HREDRAW | CS_VREDRAW;
				m_classAtom = RegisterClassEx(&wcx);
			}
			m_sizeable = sizeable;
			m_hwnd = CreateWindowEx(0, (PCWSTR)m_classAtom, title.c_str(), WS_SIZEBOX | WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN, r.X, r.Y, r.Width, r.Height, 0, 0, g_hInstance, this);
			SetWindowPos(m_hwnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);// this forces our WM_NCCALCSIZE processing to happen.
			ShowWindow(m_hwnd, SW_NORMAL);
			//LibCC::g_pLog->Message(L"Created window");
		}

		virtual Rect CalculateClientArea(const Rect& pos)
		{
			// default processing.
			//if(m_sizeable)
			//{
				return Rect(
					Theme::SizeableBorderSize,
					Theme::SizeableBorderSize,
					pos.Width - (Theme::SizeableBorderSize * 2),
					pos.Height - (Theme::SizeableBorderSize * 2)
					);
			//}
			//else
			//{
			//	return m_pos;
			//}
		}

		virtual LRESULT HitTest(const Point& p)
		{
			// default processing.
			Rect rc = CalculateClientArea(m_pos);
			if(!m_sizeable || (TRUE == rc.Contains(p)))
			{
				return HTCLIENT;
			}

			bool isLeft = p.X < Theme::SizingGripCornerSize;
			bool isTop = p.Y < Theme::SizingGripCornerSize;
			bool isRight = (rc.GetRight() - p.X) < Theme::SizingGripCornerSize;
			bool isBottom = (rc.GetBottom() - p.Y) < Theme::SizingGripCornerSize;

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

		void OnPaint(GraphicsPtr buffer)
		{
			// default processing.
			if(m_sizeable)
			{
				Theme::DrawSizingFrame(buffer, m_pos);
				return;
			}
			Theme::DrawPanel(buffer, m_pos);
		}

	private:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			OverlappedWindow* pThis = (OverlappedWindow*)GetProp(hWnd, (PCWSTR)m_propAtom);
			switch(msg)
			{
			case WM_CREATE:
				{
					m_refCount ++;
					pThis = (OverlappedWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
					SetProp(hWnd, (PCWSTR)m_propAtom, (HANDLE)pThis);
					break;
				}
			case WM_DESTROY:
				m_refCount --;
				break;
			}
			if(!pThis)
				return DefWindowProc(hWnd, msg, wParam, lParam);
			return pThis->WindowProc2(hWnd, msg, wParam, lParam);
		}

		LRESULT CALLBACK WindowProc2(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			switch(msg)
			{
			case WM_ACTIVATE:
				{
					switch(LOWORD(wParam))
					{
					case WA_CLICKACTIVE:
					case WA_ACTIVE:
						OnActivate.Fire(this);
						break;
					case WA_INACTIVE:
						OnDeactivate.Fire(this);
						break;
					}
					return 0;
				}
			case WM_NCCALCSIZE:
				//LibCC::g_pLog->Message(L"WM_NCCALCSIZE");
				return 0;
			case WM_NCHITTEST:
				{
					// in screen coords -> client
					Rect rc = CalculateClientArea(m_pos);
					POINTS pts = MAKEPOINTS(lParam);
					POINT pt;
					pt.x = pts.x;
					pt.y = pts.y;
					ScreenToClient(hWnd, &pt);
					Point pt2(pt.x, pt.y);

					Control* c = ControlFromPoint(pt2);
					if(!c)
						return HTNOWHERE;

					// TODO: translate pt2 relative to child.
					pt2 = TranslateToDescendent(c, pt2);

					return c->HitTest(pt2);
				}
			case WM_ERASEBKGND:
				return 0;

			case WM_LBUTTONDOWN:
				{
					SetForegroundWindow(hWnd);
					Point pt(LOWORD(lParam), HIWORD(lParam));
					OnLButtonDownInternal(pt);
					return 0;
				}
			case WM_MOUSEMOVE:
				{
					if(!m_subscribedToMouseLeave)
					{
						TRACKMOUSEEVENT tme = {0};
						tme.cbSize = sizeof(tme);
						tme.hwndTrack = m_hwnd;
						tme.dwFlags = TME_LEAVE;
						TrackMouseEvent(&tme);
						m_subscribedToMouseLeave = true;
					}
					Point pt(LOWORD(lParam), HIWORD(lParam));
					OnMouseMoveInternal(pt);
					return 0;
				}
			case WM_LBUTTONUP:
				{
					Point pt(LOWORD(lParam), HIWORD(lParam));
					OnLButtonUpInternal(pt);
					return 0;
				}
			case WM_MOUSELEAVE:
				m_subscribedToMouseLeave = false;
				OnInternalMouseLeave();
				return 0;
			case WM_SIZE:
				m_offscreen.reset(new Gdiplus::Bitmap(LOWORD(lParam), HIWORD(lParam), PixelFormat32bppARGB));
				m_pos.Width = LOWORD(lParam);
				m_pos.Height = HIWORD(lParam);
				OnSizeInternal();
				return 0;

			case WM_CLOSE:
				OnClose.Fire(this);
				return 0;

			case WM_DESTROY:
				OnDestroy.Fire(this);
				break;

			case WM_PAINT:
				//LibCC::g_pLog->Message(L"WM_PAINT");
				{
					LibCC::Timer t;
					t.Tick();
					{
						GraphicsPtr g(new Gdiplus::Graphics(m_offscreen.get()));
						OnPaintInternal(g);
					}

					// render to screen.
					PAINTSTRUCT ps;
					BeginPaint(hWnd, &ps);
					{
						Gdiplus::Graphics g(ps.hdc);
						g.DrawImage(m_offscreen.get(), 0, 0);
					}
					EndPaint(hWnd, &ps);
					t.Tick();
					//LibCC::g_pLog->Message(LibCC::Format(L"Painted in % sec").d<3>(t.GetLastDelta()));
					return 0;
				}
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

	protected:
		virtual void SetClientDirty(const Rect& rcClient)
		{
			Rect rc = ClientToWindow(rcClient);
			RECT rc2;
			SetRect(&rc2, rc.X, rc.Y, rc.GetRight(), rc.GetBottom());
			InvalidateRect(m_hwnd, &rc2, FALSE);
		}

		virtual void CloseOverlappedWindow()
		{
			if(m_hwnd)
				SendMessage(m_hwnd, WM_CLOSE, 0, 0);
		}

		// ** mouse capture begin
		virtual void StopMouseCapture()
		{
			if(m_mouseCapture)
				m_mouseCapture->m_hasCapture = false;
			m_mouseCapture = 0;
			ReleaseCapture();
		}
		virtual void InternalStartMouseCapture(Control* originator)
		{
			if(m_mouseCapture)
				StopMouseCapture();
			m_mouseCapture = originator;
			originator->m_hasCapture = true;
			SetCapture(m_hwnd);
		}
		// ** mouse capture end
	};
}

