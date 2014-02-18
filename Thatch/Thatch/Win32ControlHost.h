/*
	Win32ControlHost is the bare minimum you need to host Thatch controls on a win32 window. it must be the top-level window for any thatch scenario.
	It is the absolute bare minimum, handling dispatching messages and stuff to children. it is also responsible for
	starting up & shutting down the more overarching windows hook and shit.

	This is NOT thread-safe. You can't create an Win32ControlHost object in thread A and then start using it in thread B.
	a more serious potential issue here is that a lot of stuff happens in the constructor. So, this stuff might happen before you
	really wish it to, if you have a global Win32ControlHost or something like that.

	Just to combat that i might add some guard against things happening too early.


	One seemingly annoying thing is that Win32ControlHost doesn't know how to use its child controls. For example if you #include "MenuBar.h"
	you'll be screwed with a circular dependency (Win32ControlHost->menubar->popupmenu->Win32ControlHost).

	Some of the idea behind stuff like menubar and status bar being child controls is that the overlapped window does not know how to use them
	directly, but fires off all the appropriate messages and events to be able to hook them up seemlessly. So, when an Win32ControlHost
	gets WM_ACTIVATE, it doesn't say m_myMenuBar->Activate. Instead the menu bar, knowing it cares about that message, subscribes to that event.

	**
	NOTE: non-client areas will NOT work. WM_NCHITTEST must always return HTCLIENT. Reason: it's too difficult to be able
	to send OnMouseEnter / OnMouseLeave when there is a mixture of client & nonclient. TrackMouseEvent will send both
	WM_MOUSELEAVE and WM_NCMOUSELEAVE, but it won't tell me when it leaves the WINDOW. So when children like menubar support
	returning HTCAPTION, the system sends WM_MOUSELEAVE when we wouldn't expect it. There is one way around it: use GetMessagePos(). But it's still
	more complicated than just not supporting nonclient areas at all.

	So, how does that factor into child win32 controls that have non-client areas? Well, it wouldn't work. period. but it's far less likely
	to be a problem because it only affects thatch behavior and this would be a win32 window.
*/

#pragma once

#include "control.h"
#include "theme.h"
#include "..\..\libcc\libcc/timer.hpp"

extern HINSTANCE g_hInstance;

namespace UI
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// do not create this yourself. it should only be created by ThreadContext.
	class MessageLoop
	{
	public:
		Event1<MSG&> OnPreTranslateMessage;
		Event0 OnAfterDispatchMessage;

		void Run()
		{
			MSG msg;
			while(GetMessage(&msg, 0, 0, 0))
			{
				OnPreTranslateMessage.Fire(0, msg);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				OnAfterDispatchMessage.Fire(0);
			}
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class Win32ControlHost :
		public Control
	{
	public:
		static ATOM m_propAtom;
		static ATOM m_classAtom;
		static ATOM m_childControlAtom;
		static ATOM m_classAtomWithShadow;
	private:
		static int m_refCount;
		#define THATCH_CLASS L"Thatch Win32ControlHost class"
		#define THATCH_CLASS_WITH_SHADOW L"Thatch Win32ControlHost class with shadow"
		BitmapPtr m_offscreen;
		bool m_subscribedToMouseLeave;
		static DWORD m_tlsIndex;

		struct TLSData
		{
			int refCount;
			HHOOK hhook;
			MessageLoop msgLoop;
		};

	protected:
		HWND m_hwnd;
	public:
		Win32ControlHost() :
			m_hwnd(0),
			m_subscribedToMouseLeave(false),
			m_currentFakeNonClientOperation(HTNOWHERE)
		{
			// check global ref count stuff.
			if(m_refCount == 0)
			{
				m_propAtom = GlobalAddAtom(L"thatch Win32ControlHost class info prop");
				m_childControlAtom = GlobalAddAtom(L"thatch Win32ControlHost child control info prop");

				WNDCLASSEX wcx = {0};
				wcx.cbSize = sizeof(wcx);
				wcx.hCursor = LoadCursor(0, IDC_ARROW);
				wcx.hInstance = g_hInstance;
				wcx.lpfnWndProc = Win32ControlHost::WindowProc;
				wcx.lpszClassName = THATCH_CLASS;
				wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
				m_classAtom = RegisterClassEx(&wcx);

				wcx.lpszClassName = THATCH_CLASS_WITH_SHADOW;
				wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_DROPSHADOW;
				m_classAtomWithShadow = RegisterClassEx(&wcx);
			}

			{
				void* tlsData = TlsGetValue(m_tlsIndex);
				if(0 == tlsData)
				{
					TLSData* pn = new TLSData;
					pn->refCount = 0;
					pn->hhook = SetWindowsHookEx(WH_GETMESSAGE, Win32ControlHost::ChildHookProc, g_hInstance, GetCurrentThreadId());

					TlsSetValue(m_tlsIndex, new TLSData);
				}

				TLSData* pdata = (TLSData*)TlsGetValue(m_tlsIndex);
				pdata->refCount ++;
			}

			m_refCount ++;
		}

		~Win32ControlHost()
		{
			if(m_hwnd)
			{
				DestroyWindow(m_hwnd);
			}

			{
				TLSData* pdata = (TLSData*)TlsGetValue(m_tlsIndex);
				if(!pdata)
					return;

				pdata->refCount --;

				if(pdata->refCount == 0)
				{
					TlsSetValue(m_tlsIndex, 0);
					UnhookWindowsHookEx(pdata->hhook);
					delete pdata;
				}
			}

			// deal with global ref count
			if(m_refCount == 0)
			{
				UnregisterClass((PCWSTR)m_classAtom, g_hInstance);
				UnregisterClass((PCWSTR)m_classAtomWithShadow, g_hInstance);
				GlobalDeleteAtom(m_propAtom);
				GlobalDeleteAtom(m_childControlAtom);
				m_classAtomWithShadow = 0;
				m_classAtom = 0;
				m_propAtom = 0;
				m_childControlAtom = 0;
			}
		}

		static MessageLoop& GetMessageLoop()
		{
			TLSData* pdata = (TLSData*)TlsGetValue(m_tlsIndex);
			return pdata->msgLoop;
		}

		static void RunMessageLoop()
		{
			GetMessageLoop().Run();
		}

		virtual DWORD GetControlTypeId() { return 0x8db51b26; }
		virtual std::wstring GetControlTypeName() { return L"Win32ControlHost"; };

		Event0 OnClose;
		Event0 OnDestroy;
		Event1<LRESULT&> OnMouseActivate;

		virtual HWND Win32_GetHWND()
		{
			return m_hwnd;
		}

		virtual void MoveTopLevelWindow(const Rect& rc)
		{
			MoveWindow(m_hwnd, rc.X, rc.Y, rc.Width, rc.Height, TRUE);
		}

		virtual Rect GetTopLevelWindowPosition()
		{
			RECT rc;
			GetWindowRect(m_hwnd, &rc);
			return RECTToRect(rc);
		}

		virtual Point WindowToScreen(const Point& in)
		{
			POINT p = MakePOINT(in);
			::ClientToScreen(m_hwnd, &p);// because i do not support any nonclient area, ClientToScreen goes from Thatch window coords to screen coords.
			return MakePoint(p);
		}
		virtual Point ScreenToWindow(const Point& in)
		{
			POINT p = MakePOINT(in);
			::ScreenToClient(m_hwnd, &p);// because i do not support any nonclient area, ScreenToClient goes from Thatch window coords to screen coords.
			return MakePoint(p);
		}

		virtual void SetKeyboardFocus(Control* p)
		{
		}

		virtual void RegisterWin32Child(HWND h, Control* p)
		{
			SetProp(h, (PCWSTR)m_childControlAtom, (HANDLE)p);
		}

		virtual void UnregisterWin32Child(HWND h)
		{
			SetProp(h, (PCWSTR)m_childControlAtom, 0);
		}

		void Create(Rect r, const std::wstring& title, DWORD exStyle, DWORD style, HWND parent = 0, bool shadow = false)
		{
			m_hasCapture = false;
			m_mouseIn = 0;
			m_currentFakeNonClientOperation = HTNOWHERE;
			m_hwnd = 0;
			m_keyboardFocus = 0;

			m_hwnd = CreateWindowEx(exStyle, (PCWSTR)(shadow ? m_classAtomWithShadow : m_classAtom), title.c_str(), style, r.X, r.Y, r.Width, r.Height, parent, 0, g_hInstance, this);
			SetWindowPos(m_hwnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOACTIVATE);// this forces our WM_NCCALCSIZE processing to happen.
		}

	private:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			Win32ControlHost* pThis = (Win32ControlHost*)GetProp(hWnd, (PCWSTR)m_propAtom);
			switch(msg)
			{
			case WM_CREATE:
				{
					pThis = (Win32ControlHost*)((CREATESTRUCT*)lParam)->lpCreateParams;
					SetProp(hWnd, (PCWSTR)m_propAtom, (HANDLE)pThis);
					break;
				}
			}
			if(!pThis)
				return DefWindowProc(hWnd, msg, wParam, lParam);
			return pThis->WindowProc2(hWnd, msg, wParam, lParam);
		}

		LRESULT CALLBACK WindowProc2(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			switch(msg)
			{
			case WM_SYSCOMMAND:
				{
					switch(0xFFF0 & wParam)
					{
					case SC_MINIMIZE:
						MinimizeTopLevelWindow();
						return 0;
					case SC_MAXIMIZE:
						MaximizeTopLevelWindow();
						return 0;
					case SC_RESTORE:
						RestoreTopLevelWindow();
						return 0;
					}
					break;
				}
			//case WM_SYSCOMMAND:// for the most part, we don't care about this message; things are handled fairly automatically by the OS.
			//	{
			//		switch(wParam)
			//		{
			//		case SC_KEYMENU:
			//		case SC_MOUSEMENU:
			//			// see if there is a menu bar.
			//			itMenu = FindFirstChild(MenuBar().GetControlTypeId());
			//			if(m_children.end() != itMenu)
			//			{
			//				((MenuBar*)*itMenu);
			//			}
			//			return 0;
			//		}
			//		break;
			//	}
			case WM_ACTIVATE:
				{
					switch(LOWORD(wParam))
					{
					case WA_CLICKACTIVE:
					case WA_ACTIVE:
						OnActivateInternal();
						break;
					case WA_INACTIVE:
						OnDeactivateInternal();
						break;
					}
					return 0;
				}
			case WM_NCCALCSIZE:
				return 0;
			case WM_NCHITTEST:
				{
					return HTCLIENT;
				}
			case WM_ERASEBKGND:
				return 0;

			case WM_CTLCOLOREDIT:
				{
					Control* pControl = (Control*)GetProp((HWND)lParam, (PCWSTR)m_childControlAtom);
					if(pControl)
					{
						std::pair<HDC, HBRUSH> p((HDC)wParam, 0);
						pControl->OnCtlColorEdit.Fire(pControl, p);
						if(p.second != 0)
							return (LRESULT)p.second;
					}
					break;
				}
			case WM_LBUTTONDOWN:
				{
					OnWM_LBUTTONDOWN(MakePoint(lParam));
					return 0;
				}

			case WM_LBUTTONDBLCLK:
				OnInternalLButtonDoubleClick(MakePoint(lParam));
				return 0;

			case WM_MOUSEMOVE:
				{
					OnWM_MOUSEMOVE(MakePoint(lParam), m_hwnd);
					return 0;
				}
			case WM_LBUTTONUP:
				{
					OnWM_LBUTTONUP(MakePoint(lParam));
					return 0;
				}
			case WM_MOUSEACTIVATE:
				{
					LRESULT ret = 0;
					OnMouseActivate.Fire(this, ret);
					if(ret)
						return ret;
					break;
				}
			case WM_GETMINMAXINFO:
				{
					MINMAXINFO& mmi = *(MINMAXINFO*)lParam;
					// Adjust the maximized size and position to fit the work area of the correct monitor
					HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
					if (monitor == NULL)
						return 0;

					MONITORINFO monitorInfo = {0};
					monitorInfo.cbSize = sizeof(monitorInfo);
					GetMonitorInfo(monitor, &monitorInfo);

					RECT rcWorkArea = monitorInfo.rcWork;
					RECT rcMonitorArea = monitorInfo.rcMonitor;

					mmi.ptMaxPosition.x = abs(rcWorkArea.left - rcMonitorArea.left);
					mmi.ptMaxPosition.y = abs(rcWorkArea.top - rcMonitorArea.top);
					mmi.ptMaxSize.x = abs(rcWorkArea.right - rcWorkArea.left);
					mmi.ptMaxSize.y = abs(rcWorkArea.bottom - rcWorkArea.top);
					return 0;
				}
			case WM_MOUSELEAVE:
				m_subscribedToMouseLeave = false;
				OnInternalMouseLeave();
				return 0;
			case WM_SETCURSOR:
				{
					POINT pt;
					GetCursorPos(&pt);
					::ScreenToClient(hWnd, &pt);
					Point pt2 = MakePoint(pt);
					Control* c = 0;
					LRESULT hitTest = DoChildHitTest(pt2, &c);

					HCURSOR h = 0;
					switch(hitTest)
					{
					case HTCLIENT:
						{
							std::pair<Point, HCURSOR> p(pt2, 0);
							c->OnSetCursor.Fire(this, p);
							h = p.second;
							break;
						}
					case HTBOTTOMLEFT:
					case HTTOPRIGHT:
						h = LoadCursor(0, IDC_SIZENESW);
						break;
					case HTTOPLEFT:
					case HTBOTTOMRIGHT:
						h = LoadCursor(0, IDC_SIZENWSE);
						break;
					case HTTOP:
					case HTBOTTOM:
						h = LoadCursor(0, IDC_SIZENS);
						break;
					case HTRIGHT:
					case HTLEFT:
						h = LoadCursor(0, IDC_SIZEWE);
						break;
					}

					if(h)
					{
						SetCursor(h);
						return TRUE;
					}
					break;
				}
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
				// reset things so when the window is shown again its state is clean.
				if(m_hasCapture)
					ReleaseCapture();
				m_hasCapture = false;
				break;

			case WM_PAINT:
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
					LibCC::g_pLog->Message(LibCC::Format(L"Painted in % sec").d<3>(t.GetLastDelta()));
					return 0;
				}

			case WM_SETFOCUS:
				break;
			case WM_KILLFOCUS:
				break;
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

	protected:
		virtual void SetClientDirty(const Rect& rcClient)
		{
			// for things like overlappedwindow or popupmenu, you could set properties and things before the win32 window is created.
			// so, handle this gracefully.
			if(FALSE == IsWindow(m_hwnd))
				return;

			Rect rc = ClientToWindow(rcClient);
			RECT rc2;
			SetRect(&rc2, rc.X, rc.Y, rc.GetRight(), rc.GetBottom());

			InvalidateRect(m_hwnd, &rc2, FALSE);
		}

		LRESULT m_currentFakeNonClientOperation;// HTCLIENT, HTLEFT, etc... indicates what the user is currently doing. HTNOWHERE means "nothing".
		Rect m_originalTopLevelWindowPosition;
		POINT m_mouseCursorOrigin;

public:

		void OnWM_LBUTTONDOWN(const Point& pt)
		{
			if(!m_mouseCapture && (m_currentFakeNonClientOperation == HTNOWHERE))
			{
				LRESULT hitTest = DoChildHitTest(pt);
				switch(hitTest)
				{
				case HTCAPTION:
				case HTTOPLEFT:
				case HTTOP:
				case HTTOPRIGHT:
				case HTRIGHT:
				case HTBOTTOMRIGHT:
				case HTBOTTOM:
				case HTBOTTOMLEFT:
				case HTLEFT:
					{
						m_originalTopLevelWindowPosition = GetTopLevelWindowPosition();
						GetCursorPos(&m_mouseCursorOrigin);
						StartMouseCapture(this);
						m_currentFakeNonClientOperation = hitTest;
						return;
					}
				}
			}

			OnLButtonDownInternal(pt);
		}

		void OnWM_LBUTTONUP(const Point& pt)
		{
			if(m_currentFakeNonClientOperation != HTNOWHERE)
			{
				StopMouseCapture();
				m_currentFakeNonClientOperation = HTNOWHERE;
				return;
			}

			OnLButtonUpInternal(pt);
		}

		void OnWM_MOUSEMOVE(const Point& pt, HWND hwndTrack)
		{
			if(m_currentFakeNonClientOperation != HTNOWHERE)
			{
				Rect newRect = m_originalTopLevelWindowPosition;
				POINT pt;
				GetCursorPos(&pt);
				switch(m_currentFakeNonClientOperation)
				{
				case HTCAPTION:
					newRect.X += (pt.x - m_mouseCursorOrigin.x);
					newRect.Y += (pt.y - m_mouseCursorOrigin.y);
					break;
				}
				// X
				switch(m_currentFakeNonClientOperation)
				{
				case HTBOTTOMRIGHT:
				case HTTOPRIGHT:
				case HTRIGHT:
					newRect.Width += (pt.x - m_mouseCursorOrigin.x);
					break;
				case HTBOTTOMLEFT:
				case HTTOPLEFT:
				case HTLEFT:
					newRect.Width += -(pt.x - m_mouseCursorOrigin.x);
					newRect.X += (pt.x - m_mouseCursorOrigin.x);
					break;
				}
				// Y
				switch(m_currentFakeNonClientOperation)
				{
				case HTTOPLEFT:
				case HTTOP:
				case HTTOPRIGHT:
					newRect.Height += -(pt.y - m_mouseCursorOrigin.y);
					newRect.Y += (pt.y - m_mouseCursorOrigin.y);
					break;
				case HTBOTTOMRIGHT:
				case HTBOTTOM:
				case HTBOTTOMLEFT:
					newRect.Height += (pt.y - m_mouseCursorOrigin.y);
					break;
				}
				MoveTopLevelWindow(newRect);
			}

			//LibCC::g_pLog->Message(LibCC::Format(L"WM_MOUSEMOVE (% %) % - %").i<10,3>(pt.X).i<10,3>(pt.Y).p(this)(rand()));
			if(!m_subscribedToMouseLeave)
			{
				TRACKMOUSEEVENT tme = {0};
				tme.cbSize = sizeof(tme);
				//LibCC::g_pLog->Message(LibCC::Format(L"TrackMouseEvent ").s(GetControlTypeName()));
				HWND hCapture = GetCapture();
				tme.hwndTrack = hwndTrack;
				tme.dwFlags = TME_LEAVE;
				TrackMouseEvent(&tme);
				m_subscribedToMouseLeave = true;
			}
			OnMouseMoveInternal(pt);
		}

		virtual void CloseTopLevelWindow()
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
		virtual void StartMouseCapture(Control* originator)
		{
			if(m_mouseCapture)
				StopMouseCapture();
			m_mouseCapture = originator;
			originator->m_hasCapture = true;
			SetCapture(m_hwnd);
		}
		// ** mouse capture end

		// activate gets sent to all children, period.
		void OnActivateInternal()
		{
			OnActivate.Fire(this);
			for(std::list<Control*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
			{
				Control* ch = *it;
				if(ch->m_visible)
				{
					ch->OnActivate.Fire(this);
				}
			}
		}

		void OnDeactivateInternal()
		{
			OnDeactivate.Fire(this);
			for(std::list<Control*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
			{
				Control* ch = *it;
				if(ch->m_visible)
				{
					ch->OnDeactivate.Fire(this);
				}
			}
		}
		
		void OnInternalLButtonDoubleClick(const Point& pt)
		{
			Control* c = 0;
			if(m_mouseCapture)
				c = m_mouseCapture;
			else
				c = ControlFromPoint(pt);

			if(c)
				c->OnLButtonDoubleClick.Fire(this, TranslateToDescendent(c, pt));
		}

		void OnLButtonDownInternal(const Point& pt)
		{
			Control* c = 0;
			if(m_mouseCapture)
				c = m_mouseCapture;
			else
				c = ControlFromPoint(pt);

			if(c)
				c->OnLButtonDown.Fire(this, TranslateToDescendent(c, pt));
		}
		
		void OnLButtonUpInternal(const Point& pt)
		{
			Control* c = 0;
			if(m_mouseCapture)
				c = m_mouseCapture;
			else
				c = ControlFromPoint(pt);

			if(c)
				c->OnLButtonUp.Fire(this, TranslateToDescendent(c, pt));
		}

		void OnMouseMoveInternal(const Point& pt)
		{
			Control* c = ControlFromPoint(pt);
			// we still want to send OnMouseEnter / OnMouseLeave messages even if there is mouse capture.
			if(m_mouseIn != c)
			{
				if(m_mouseIn)
				{
					m_mouseIn->OnMouseLeave.Fire(this);
				}
				//LibCC::g_pLog->Message(LibCC::Format(L"OnMouseMoveInternal %, m_mouseIn changing from % to %")
				//	.s(this->GetControlTypeName())
				//	.s(m_mouseIn ? m_mouseIn->GetControlTypeName() : L"[NULL]")
				//	.s(c ? c->GetControlTypeName() : L"[NULL]")
				//	);

				m_mouseIn = c;
				if(c)
				{
					c->OnMouseEnter.Fire(this);
				}
			}

			if(m_mouseCapture)
			{
				c = m_mouseCapture;
			}

			if(c)
			{
				Point p = TranslateToDescendent(c, pt);
				c->OnMouseMove.Fire(this, TranslateToDescendent(c, pt));
			}
		}

		void OnInternalMouseLeave()
		{
			// this is called when the mouse leaves the overlapped window. if need be, send this to a control.
				//LibCC::g_pLog->Message(LibCC::Format(L"OnInternalMouseLeave %, m_mouseIn changing from % to [NULL]")
				//	.s(this->GetControlTypeName())
				//	.s(m_mouseIn ? m_mouseIn->GetControlTypeName() : L"[NULL]")
				//	);
			if(!m_mouseIn)
				return;
			m_mouseIn->OnMouseLeave.Fire(this);
			m_mouseIn = 0;
		}

		void HandleChildMessage(HWND h, UINT msg, WPARAM wParam, LPARAM lParam, Control* pControl)
		{
			Control* pChildControl = (Control*)GetProp(h, (PCWSTR)m_childControlAtom);

			// for child controls we need to replicate a lot of processing as it is handled for overlapped windows
			switch(msg)
			{
			case WM_MOUSEMOVE:
				{
					POINT pt = MakePOINT(lParam);
					MapWindowPoints(h, m_hwnd, &pt, 1);
					OnWM_MOUSEMOVE(MakePoint(pt), h);
					return;
				}
			case WM_LBUTTONDOWN:
				{
					POINT pt = MakePOINT(lParam);
					MapWindowPoints(h, m_hwnd, &pt, 1);
					OnWM_LBUTTONDOWN(MakePoint(pt));
					return;
				}
			case WM_LBUTTONUP:
				{
					POINT pt = MakePOINT(lParam);
					MapWindowPoints(h, m_hwnd, &pt, 1);
					OnWM_LBUTTONUP(MakePoint(pt));
					return;
				}
			case WM_LBUTTONDBLCLK:
				{
					POINT pt = MakePOINT(lParam);
					MapWindowPoints(h, m_hwnd, &pt, 1);
					OnInternalLButtonDoubleClick(MakePoint(pt));
					return;
				}
			case WM_MOUSELEAVE:
				m_subscribedToMouseLeave = false;
				OnInternalMouseLeave();
				return;
			}
			// todo: add all messages that Win32ControlHost wants
		}

		static LRESULT CALLBACK ChildHookProc(int code, WPARAM wParam, LPARAM lParam)
		{
			TLSData* pdata = (TLSData*)TlsGetValue(m_tlsIndex);
			MSG* p = (MSG*)lParam;
			//LibCC::g_pLog->Message(LibCC::Format(L"message from hook %").ui(p->message));

			Control* pControl = (Control*)GetProp(p->hwnd, (PCWSTR)m_childControlAtom);
			if(pControl)
			{
				HWND parent = GetParent(p->hwnd);
				LONG_PTR lp = GetClassLongPtr(parent, GCW_ATOM);
				if(Win32ControlHost::m_classAtom == lp || Win32ControlHost::m_classAtomWithShadow == lp)
				{
					// ok this is the right scenario.
					Win32ControlHost* pThis = (Win32ControlHost*)GetProp(parent, (PCWSTR)m_propAtom);
					pThis->HandleChildMessage(p->hwnd, p->message, p->wParam, p->lParam, pControl);
				}
			}
			return CallNextHookEx(pdata->hhook, code, wParam, lParam);
		}

	};

}

