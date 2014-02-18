/*

	there is honestly not a very clean line between what should be here and what should be in Win32ControlHost.
	I will probably stick most of it in Win32ControlHost.

*/
#pragma once


#include "ThatchBasics.h"
#include "..\..\libcc\libcc\result.hpp"
#include <list>
#include "Events.h"
#include "Theme.h"

namespace UI
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	class Control
	{
	public:
		enum SizeInfo
		{
			SizeInfo_None = 0,
			SizeInfo_Size_X = 1,
			SizeInfo_Size_Y = 2,
			SizeInfo_Move_X = 4,
			SizeInfo_Move_Y = 8,
		};

  private:
    // don't allow ambiguous calls
		Control(const Control&) { }
		template<typename T> Control& operator =(T) { return *this; }

	protected:
		SizeInfo m_sizeInfo;
		// save cached size info here.
		int m_sizeInfoXOffset;// for moving, this is parent width - this left. for sizing, this is parent width - this width.
		int m_sizeInfoYOffset;// similar, for Y.

		Control* m_keyboardFocus;// control that has keyboard focus. only set by overlapped window.

		Control* m_mouseCapture;// at present, this is only set by overlappedwindow
		Control* m_mouseIn;// the control the mouse is currently hovering over.

		IThemePtr m_theme;

	public:
		bool m_hasCapture;

	public:
		std::list<Control*> m_children;// order = z-order
		Control* m_parent;
		bool m_visible;
		Rect m_pos;// relative to parent's client area.

		Control() :
			m_parent(0),
			m_visible(true),
			m_mouseCapture(0),
			m_hasCapture(false),
			m_mouseIn(0),
			m_keyboardFocus(0),
			m_theme(new DefaultTheme())
		{
		}

		virtual ~Control()
		{
			// if we still have children, remove them
			while(m_children.size())
			{
				RemoveChild(m_children.front());
			}

			if(m_parent)
				m_parent->RemoveChild(this);
		}

		void SetPos(Rect pos)// pos is relative to parent's client
		{
			if(TRUE == pos.Equals(m_pos))
				return;
			SetDirty();// invalidate old position.
			m_pos = pos;
			OnSizeInternal();
			SetDirty();// invalidate new position.
		}

		virtual void MoveTopLevelWindow(const Rect& rc)// stops at the Win32ControlHost override of this.
		{
			if(m_parent)
				m_parent->MoveTopLevelWindow(rc);
		}

		virtual Rect GetTopLevelWindowPosition()// stops at the Win32ControlHost override of this.
		{
			if(m_parent)
				return m_parent->GetTopLevelWindowPosition();
			return Rect();
		}

		virtual void CloseTopLevelWindow()// stops at the Win32ControlHost override of this.
		{
			if(m_parent)
				m_parent->CloseTopLevelWindow();
		}

		void SetDirty()// stops at the Win32ControlHost override of this.
		{
			if(m_parent)
				m_parent->SetClientDirty(m_pos);
		}

		// calculates the window size needed to fit the given client size.
		virtual Size GetWindowSizeThatFitsClient(const Size& client)
		{
			Rect theory = CalculateClientArea(Size());// passing in 0,0 size means the resulting width & height is the size of the nonclient borders. but of course they're negative.
			return Size(client.Width - theory.Width, client.Height - theory.Height);
		}

		// POINT MAPPING between client, window, screen ------------------------------------------------------------------------------------------
		Point ClientToWindow(const Point& in)
		{
			Rect thisClientArea = CalculateClientArea();
			Point ret = in;
			ret.X += thisClientArea.X;
			ret.Y += thisClientArea.Y;
			return ret;
		}
		Point ClientToScreen(const Point& in)
		{
			Point pt = ClientToWindow(in);
			return WindowToScreen(pt);
		}
		Point WindowToClient(const Point& in)
		{
			Rect thisClientArea = CalculateClientArea();
			Point ret = in;
			ret.X -= thisClientArea.X;
			ret.Y -= thisClientArea.Y;
			return ret;
		}
		virtual Point WindowToScreen(const Point& in)// Win32ControlHost overrides this
		{
			// translate to client of parent & pass to parent.
			Point pt = in;
			pt.X += m_pos.X;
			pt.Y += m_pos.Y;
			if(m_parent)
				return m_parent->ClientToScreen(pt);
			return in;
		}
		Point ScreenToClient(const Point& in)
		{
			Point pt = ScreenToWindow(in);
			return WindowToClient(pt);
		}
		virtual Point ScreenToWindow(const Point& in)// Win32ControlHost overrides this
		{
			if(!m_parent)
				return in;
			Point p = m_parent->ScreenToClient(in);
			// convert from parent client coords (because they are easily accessible) to this window coords.
			p.X -= m_pos.X;
			p.Y -= m_pos.Y;
			return p;
		}


		Rect ClientToWindow(const Rect& in)
		{
			return MakeRect(ClientToWindow(TopLeft(in)), ClientToWindow(BottomRight(in)));
		}
		Rect ClientToScreen(const Rect& in)
		{
			return MakeRect(ClientToScreen(TopLeft(in)), ClientToScreen(BottomRight(in)));
		}
		Rect WindowToClient(const Rect& rc)
		{
			return MakeRect(WindowToClient(TopLeft(rc)), WindowToClient(BottomRight(rc)));
		}
		Rect WindowToScreen(const Rect& rc)
		{
			return MakeRect(WindowToScreen(TopLeft(rc)), WindowToScreen(BottomRight(rc)));
		}
		Rect ScreenToClient(const Rect& rc)
		{
			return MakeRect(ScreenToClient(TopLeft(rc)), ScreenToClient(BottomRight(rc)));
		}
		Rect ScreenToWindow(const Rect& rc)
		{
			return MakeRect(ScreenToWindow(TopLeft(rc)), ScreenToWindow(BottomRight(rc)));
		}

		virtual Rect Win32_TranslateRect(Rect rc)// takes a rect in CLIENT coords and translates them to win32 client coords if this is a win32 window. if this isn't a win32 window, it will propagate to parent to calculate the answer.
		{
			// translate to window coords
			Rect rcWindow = ClientToWindow(rc);
			// translate to parent CLIENT coords so we can recurse to the parent.
			if(!m_parent)
				return rcWindow;
			rcWindow.X += m_pos.X;
			rcWindow.Y += m_pos.Y;
			return m_parent->Win32_TranslateRect(rcWindow);
		}

		// ------------------------------------------------------------------------------------------
		// p is relative to this control
		Control* ControlFromPoint(const Point& p)
		{
			if(FALSE == Rect(0,0,m_pos.Width, m_pos.Height).Contains(p))
			{
				//LibCC::g_pLog->Message(LibCC::Format(L"ControlFromPoint returning 0"));
				return 0;
			}
			Rect rcClient = CalculateClientArea();

			// see if this point is in a child, paying attention to z-order
			std::list<Control*>::reverse_iterator it;
			for(it = m_children.rbegin(); it != m_children.rend(); ++it)
			{
				Control* ch = *it;
				if(!ch->m_visible)
					continue;

				// translate p to child coords
				Point pch = p - TopLeft(ch->m_pos) - TopLeft(rcClient);
				Control* ret = ch->ControlFromPoint(pch);
				if(ret)
				{
					//LibCC::g_pLog->Message(LibCC::Format(L"ControlFromPoint returning %").p(ret));
					return ret;
				}
			}

			//LibCC::g_pLog->Message(LibCC::Format(L"ControlFromPoint returning %").p(this));
			return this;
		}

		// takes this window coords, and makes it relative to the given control. window coordinates in and out.
		Point TranslateToDescendent(Control* c, const Point& p)
		{
			Point ret = p;
			ret = ret - TopLeft(c->m_pos);
			while(true)
			{
				c = c->m_parent;
				if(c == 0)
					break;
				Rect rcClient = c->CalculateClientArea();
				ret = ret - TopLeft(c->m_pos) - TopLeft(rcClient);
				if(c == this)
					return ret;
			}
			return p;// error actually.
		}

		// do nothing. The overlapped window will override this and do windows specific things
		virtual void SetKeyboardFocus(Control* p)
		{
			if(m_parent)
				m_parent->SetKeyboardFocus(p);
		}

		// do nothing. The overlapped window will override this and do windows specific things
		virtual void SetClientDirty(const Rect& rcClient)
		{
			// convert client coords to parent client coords
			if(0 == m_parent)
				return;
			Rect rc = ClientToWindow(rcClient);
			m_parent->SetClientDirty(rc);
		}

		void ResetSizingInfo(SizeInfo si)
		{
			m_sizeInfo = si;
			if(m_parent)
			{
				Rect rcParentClient = m_parent->CalculateClientArea();

				if(si & SizeInfo_Move_X)
					m_sizeInfoXOffset = rcParentClient.Width - m_pos.X;
				if(si & SizeInfo_Move_Y)
					m_sizeInfoYOffset = rcParentClient.Height - m_pos.Y;
				if(si & SizeInfo_Size_X)
					m_sizeInfoXOffset = rcParentClient.Width - m_pos.Width;
				if(si & SizeInfo_Size_Y)
					m_sizeInfoYOffset = rcParentClient.Height - m_pos.Height;
			}
		}

		std::list<Control*>::iterator FindChild(Control* ch)
		{
			std::list<Control*>::iterator ret;
			for(ret = m_children.begin(); ret != m_children.end(); ++ ret)
			{
				if(*ret == ch)
					break;
			}
			return ret;
		}

		std::list<Control*>::iterator FindFirstChild(DWORD typeID)
		{
			std::list<Control*>::iterator ret;
			for(ret = m_children.begin(); ret != m_children.end(); ++ ret)
			{
				if((*ret)->GetControlTypeId() == typeID)
					break;
			}
			return ret;
		}

		LibCC::Result RemoveChild(Control* ch)
		{
			std::list<Control*>::iterator it = FindChild(ch);
			if(it == m_children.end())
				return LibCC::Result::Failure(L"Can't remove a child when it's not there.");

			if(m_mouseCapture == ch)
				StopMouseCapture();

			if(m_mouseIn == ch)
				m_mouseIn = 0;

			ch->m_parent = 0;
			m_children.erase(it);
			return LibCC::Result::Success();
		}

		LibCC::Result AddChild(Control* ch)
		{
			return AddChild(ch, ch->m_pos);
		}

		LibCC::Result AddChild(Control* ch, Rect pos, SizeInfo si = SizeInfo_None)
		{
			// TODO: make sure there is no circular reference or anything.
			if(ch->m_parent)
				ch->m_parent->RemoveChild(ch);

			ch->m_theme = m_theme;

			ch->SetPos(pos);
			ch->m_parent = this;
			ch->ResetSizingInfo(si);
			m_children.push_back(ch);
			ch->OnSetParent.Fire(this);
			OnAddChild.Fire(this, ch);
			return LibCC::Result::Success();
		}

		virtual void RegisterWin32Child(HWND h, Control* p)// overlapped window overrides this.
		{
			if(m_parent)
				m_parent->RegisterWin32Child(h, p);
		}
		virtual void UnregisterWin32Child(HWND h)// overlapped window overrides this.
		{
			if(m_parent)
				m_parent->UnregisterWin32Child(h);
		}

		virtual void StopMouseCapture()// overlapped window override
		{
			if(m_parent)
				m_parent->StopMouseCapture();
		}
		virtual void StartMouseCapture(Control* originator)// overlapped window override
		{
			if(m_parent)
				m_parent->StartMouseCapture(originator);
		}

		// helper stuff for embedding windows controls
		virtual HWND Win32_GetHWND()// win32 windows (like overlapped window) will override this.
		{
			if(m_parent)
				return m_parent->Win32_GetHWND();
			return 0;
		}

		void SetVisible(bool b)
		{
			m_visible = b;
			SetDirty();
		}

		// EVENT HANDLERS
		Event1<const Point&> OnMouseMove;
		Event1<const Point&> OnLButtonDown;
		Event1<const Point&> OnLButtonUp;
		Event1<const Point&> OnLButtonDoubleClick;
		Event1<std::pair<HDC, HBRUSH>& > OnCtlColorEdit;
		Event0 OnMouseLeave;
		Event0 OnMouseEnter;
		Event0 OnSetParent;
		Event1<Control*> OnAddChild;
		Event0 OnSize;
		Event1<std::pair<Point, HCURSOR>& > OnSetCursor;// set the HCURSOR to what you want.
		Event0 OnActivate;
		Event0 OnDeactivate;

		// things that child windows can override to control things more.
		virtual void OnPaint(GraphicsPtr) { };
		// return a client area, relative to THIS control's window coordinates. (no NC area means 0,0-size.x,size.y).
		// 'size' is a theoretical window size of this control.
		virtual Rect CalculateClientArea(const Size& size)
		{
			return Rect(0, 0, size.Width, size.Height);
		}
		Rect CalculateClientArea()
		{
			return CalculateClientArea(GetSize(m_pos));
		}
		virtual DWORD GetControlTypeId() = 0;// return an ID of the type of control. at the moment i'm using a CRC32 of a GUID to generate these. hey it's practical.
		virtual std::wstring GetControlTypeName() { return L"Generic Control"; };
		virtual LRESULT HitTest(const Point& p) { return HTCLIENT; }// allows controls to indicate if the point is HTCAPTION or for sizing or whatever. p is in window coords

		// p must be in this window coords.
		LRESULT DoChildHitTest(const Point& pt2, Control** pc = 0)
		{
			Control* c = ControlFromPoint(pt2);
			if(pc)
				*pc = 0;
			if(!c)
				return HTNOWHERE;
			if(pc)
				*pc = c;

			Point pt3 = TranslateToDescendent(c, pt2);
			return c->HitTest(pt3);
		}

		void OnSizeInternal()
		{
			OnSize.Fire(this);
			// resize child controls according to ch->m_sizeInfo
			std::list<Control*>::iterator it;
			for(it = m_children.begin(); it != m_children.end(); ++it)
			{
				Control* ch = *it;
				if(ch->m_sizeInfo == SizeInfo_None)
					continue;

				Rect client = CalculateClientArea();
				Rect newRect = ch->m_pos;

				if(ch->m_sizeInfo & SizeInfo_Move_X)
					newRect.X = client.Width - ch->m_sizeInfoXOffset;
				if(ch->m_sizeInfo & SizeInfo_Move_Y)
					newRect.Y = client.Height - ch->m_sizeInfoYOffset;
				if(ch->m_sizeInfo & SizeInfo_Size_X)
					newRect.Width = client.Width - ch->m_sizeInfoXOffset;
				if(ch->m_sizeInfo & SizeInfo_Size_Y)
					newRect.Height = client.Height - ch->m_sizeInfoYOffset;

				ch->SetPos(newRect);
			}
		}

		void OnPaintInternal(GraphicsPtr buffer)
		{
			// paint THIS control somehow.
			OnPaint(buffer);
			Rect clientArea = CalculateClientArea();

			Gdiplus::Region thisClip;// save the current clip rgn & transform
			buffer->GetClip(&thisClip);

			// paint all children.
			buffer->TranslateTransform((Gdiplus::REAL)clientArea.X, (Gdiplus::REAL)clientArea.Y);
			Gdiplus::Matrix m;
			buffer->GetTransform(&m);
			std::list<Control*>::iterator it;
			for(it = m_children.begin(); it != m_children.end(); ++it)
			{
				Control* ch = *it;
				if(ch->m_visible)
				{
					buffer->SetTransform(&m);
					buffer->SetClip(Rect(0, 0, clientArea.Width, clientArea.Height));
					buffer->TranslateTransform((Gdiplus::REAL)ch->m_pos.X, (Gdiplus::REAL)ch->m_pos.Y);
					ch->OnPaintInternal(buffer);
				}
			}
		}

		virtual bool IsTopLevelWindowSizeable()
		{
			if(m_parent)
				return m_parent->IsTopLevelWindowSizeable();
			return false;
		}

		virtual bool IsTopLevelWindowMaximized()
		{
			if(m_parent)
				return m_parent->IsTopLevelWindowMaximized();
			return false;
		}

		virtual void MaximizeTopLevelWindow()
		{
			if(m_parent)
				m_parent->MaximizeTopLevelWindow();
		}

		virtual void RestoreTopLevelWindow()
		{
			if(m_parent)
				m_parent->RestoreTopLevelWindow();
		}

		virtual void MinimizeTopLevelWindow()
		{
			if(m_parent)
				m_parent->MinimizeTopLevelWindow();
		}
	};

}
