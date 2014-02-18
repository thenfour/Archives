/*
How to use:
just add it as a child of a menu.

only support 2 states: normal, mouseover.
*/

#pragma once


#include "Control.h"
#include "PopupMenu.h"


namespace UI
{

	class MenuBarItem :
		public Control
	{
		std::wstring m_caption;
		ITheme::ButtonState m_state;
		bool m_mouseIsInside;

		bool m_activated;
		PopupMenuManager* m_menuManager;

		void ChangeState(ITheme::ButtonState st)
		{
			if(st == m_state)
				return;
			m_state = st;
			if(m_state == ITheme::ButtonState_MouseOver)
				OnStateChangeToMouseOver.Fire(this);
			SetDirty();
		}

		void MBIOnShowPopup(Control* origin)
		{
			m_popupIsActive = true;
			OnShowPopup.Fire(this);
		}

		void MBIOnHidePopup(Control* origin)
		{
			m_popupIsActive = false;
			ChangeState(m_mouseIsInside ? ITheme::ButtonState_MouseOver : ITheme::ButtonState_Normal);
			OnHidePopup.Fire(this);
		}

		// called when you add this to a menubar.
		void MBIOnSetParent(Control* origin)
		{
			m_parent->OnSetParent.Add(this, &MenuBarItem::MBIOnSetParentParent);
			HookUpOverlappedWindowEvents();
		}

		void MBIOnSetParentParent(Control* origin)
		{
			HookUpOverlappedWindowEvents();
		}

		void HookUpOverlappedWindowEvents()
		{
			// find the overlapped window parent.
			Control* parent = m_parent;
			if(parent == 0)
				return;
			while(parent)
			{
				if(parent->m_parent == 0)
					break;
				parent = parent->m_parent;
			}

			OverlappedWindow* p = (OverlappedWindow*)parent;
		}

	public:
		PopupMenu* m_popupMenu;
		bool m_popupIsActive;

		MenuBarItem() :
			m_state(ITheme::ButtonState_Normal),
			m_popupMenu(0),
			m_popupIsActive(false),
			m_mouseIsInside(false),
			m_menuManager(0)
		{
			OnLButtonDown.Add(this, &MenuBarItem::MBIOnLButtonDown);
			OnMouseLeave.Add(this, &MenuBarItem::MBIOnMouseLeave);
			OnMouseEnter.Add(this, &MenuBarItem::MBIOnMouseEnter);
			OnSetParent.Add(this, &MenuBarItem::MBIOnSetParent);
		}
		~MenuBarItem()
		{
		}

		Event0 OnShowPopup;
		Event0 OnHidePopup;
		Event0 OnStateChangeToMouseOver;

		virtual DWORD GetControlTypeId() { return 0x6193825d; }
		virtual std::wstring GetControlTypeName() { return L"Menu bar item"; };

		void SetMenuManager(PopupMenuManager* mgr)
		{
			m_menuManager = mgr;
		}

		void SetCaption(const std::wstring& s)
		{
			m_caption = s;
			// calculate this width & height
			HDC dc = GetDC(0);
			Size size;
			{
				GraphicsPtr g(new Gdiplus::Graphics(dc));
				size = m_theme->GetIdealMenuBarItemSize(ITheme::PopupMenuItemSpec(ITheme::PMIT_Popup, m_caption));//Theme::GetMenuBarItemSize(m_caption, g);
			}
			ReleaseDC(0, dc);

			SetPos(Rect(m_pos.X, m_pos.Y, size.Width, size.Height));
		}

		void SetPopupMenu(PopupMenu* p)
		{
			if(m_popupMenu)
			{
				// TODO: hide it & properly detach it.
				m_popupMenu->OnShow.Remove(this, &MenuBarItem::MBIOnShowPopup);
				m_popupMenu->OnHide.Remove(this, &MenuBarItem::MBIOnHidePopup);
			}
			m_popupMenu = p;
			m_popupMenu->OnShow.Add(this, &MenuBarItem::MBIOnShowPopup);
			m_popupMenu->OnHide.Add(this, &MenuBarItem::MBIOnHidePopup);
		}

		void OnPaint(GraphicsPtr buffer)
		{
			//virtual void DrawMenuBarItem(GraphicsPtr g, Rect rc, const std::wstring& caption) { return; }
			m_theme->DrawMenuBarItem(buffer, CalculateClientArea(), ITheme::PopupMenuItemSpec(m_state, ITheme::PMIT_Popup, true, m_caption, L""));// Theme::DrawMenuBarItemImages(m_caption, m_state, buffer);
		}

		void MBIOnLButtonDown(Control* origin, const Point& p)
		{
			ShowPopup();
		}

		void ShowPopup()
		{
			Point p2 = ClientToScreen(Point(0, m_pos.Height));
			if(m_popupMenu)	
			{
				m_popupIsActive = true;
				m_menuManager->ShowPopup(m_popupMenu, p2.X, p2.Y);
			}
		}

		void MBIOnMouseLeave(Control* origin)
		{
			m_mouseIsInside = false;
			if(!m_popupIsActive)
				ChangeState(ITheme::ButtonState_Normal);
		}

		void MBIOnMouseEnter(Control* origin)
		{
			m_mouseIsInside = true;
			ChangeState(ITheme::ButtonState_MouseOver);
		}

	};

}

