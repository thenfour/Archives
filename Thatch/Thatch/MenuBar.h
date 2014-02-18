/*

menu bar needs to manage higher-level behavior of menubaritem / popupmenu.
- supporting click-and-drag behavior in addition to 2-click behavior
- switching popups as the cursor moves (or keyboard)


*/

#pragma once

#include "theme.h"
#include "OverlappedWindow.h"
#include "PushButton.h"
#include "MenuBarItem.h"
#include "PopupMenu.h"


namespace UI
{
	// this class also contains code for moving, and the capability for exit/minimize/maximize buttons
	class MenuBar :
		public Control
	{
		PushButton m_minimize;
		PushButton m_maximize;
		PushButton m_close;
		PushButton m_restore;

		PopupMenu* m_visiblePopup;

		bool m_activated;// for drawing frame.

		PopupMenuManager m_menuManager;

		void MBOnShowPopup(Control* origin)
		{
			m_visiblePopup = ((MenuBarItem*)origin)->m_popupMenu;
		}

		void MBOnHidePopup(Control* origin)
		{
			m_visiblePopup = 0;
		}

		void MBOnKeyDown(Control* origin, int vk)
		{
		}

		void OnItemChangedToMouseOver(Control* origin)
		{
			MenuBarItem* c = (MenuBarItem*)origin;
			if(m_visiblePopup)
			{
				if(c->m_popupMenu != m_visiblePopup)
				{
					// show a different popup.
					m_visiblePopup->Close();
					m_visiblePopup = c->m_popupMenu;
					c->ShowPopup();
				}
			}
		}

	public:
		virtual DWORD GetControlTypeId() { return 0xbb8d2562; }
		virtual std::wstring GetControlTypeName() { return L"MenuBar"; };

		virtual LRESULT HitTest(const Point& p)
		{
			return IsTopLevelWindowSizeable() ? HTCAPTION : HTCLIENT;
		}

		MenuBar() :
			m_visiblePopup(0),
			m_activated(false)
		{
			m_menuManager.SetOwner(this);
			OnSetParent.Add(this, &MenuBar::MBOnSetParent);
			OnAddChild.Add(this, &MenuBar::MBOnAddChild);
			OnActivate.Add(this, &MenuBar::MBOnActivate);
			OnLButtonDoubleClick.Add(this, &MenuBar::MBOnLButtonDoubleClick);
			OnDeactivate.Add(this, &MenuBar::MBOnDeactivate);
		}

		void OnPaint(GraphicsPtr g)
		{
			m_theme->DrawCommandBarBackground(g, Gdiplus::Rect(0, 0, m_pos.Width, m_pos.Height), m_activated);
		}

		void ResetPositionInfo()
		{
			int x = 22;// margin.
			for(std::list<Control*>::iterator it = m_children.begin(); it != m_children.end(); ++ it)
			{
				Control* ch = *it;
				if(ch->GetControlTypeId() == MenuBarItem().GetControlTypeId())
				{
					ch->SetPos(Rect(x, 3, ch->m_pos.Width, ch->m_pos.Height));
					x += ch->m_pos.Width;
					x += 2;
				}
			}
		}

		void MBOnAddChild(Control* origin, Control* p)
		{
			if(p->GetControlTypeId() != MenuBarItem().GetControlTypeId())
				return;
			MenuBarItem* mb = (MenuBarItem*)p;

			mb->SetMenuManager(&m_menuManager);

			mb->OnShowPopup.Add(this, &MenuBar::MBOnShowPopup);
			mb->OnSize.Add(this, &MenuBar::MBOnSize);
			mb->OnStateChangeToMouseOver.Add(this, &MenuBar::OnItemChangedToMouseOver);
			mb->OnHidePopup.Add(this, &MenuBar::MBOnHidePopup);

			// set the children's positions
			ResetPositionInfo();
		}

		void MBOnSize(Control* origin)
		{
			ResetPositionInfo();
		}

		void MBOnClose(Control* origin)
		{
			CloseTopLevelWindow();
		}

		void MBOnMinimize(Control* origin)
		{
			MinimizeTopLevelWindow();
		}

		void MBOnMaximize(Control* origin)
		{
			MaximizeTopLevelWindow();
			m_restore.SetVisible(true);
			m_maximize.SetVisible(false);
		}

		void MBOnRestore(Control* origin)
		{
			RestoreTopLevelWindow();
			m_restore.SetVisible(false);
			m_maximize.SetVisible(true);
		}

		void MBOnLButtonDoubleClick(Control* origin, const Point& p)
		{
			if(IsTopLevelWindowMaximized())
			{
				MBOnRestore(0);
			}
			else
			{
				MBOnMaximize(0);
			}
		}

		void MBOnActivate(Control* origin)
		{
			m_activated = true;
			//LibCC::g_pLog->Message(LibCC::Format(L"MBOnActivate(active=%)").i(m_activated ? 1 : 0));
			SetDirty();
		}

		void MBOnDeactivate(Control* origin)
		{
			m_activated = false;
			//LibCC::g_pLog->Message(LibCC::Format(L"MBOnDeactivate(active=%)").i(m_activated ? 1 : 0));
			SetDirty();
		}

		void MBOnSetParent(Control* origin)
		{
			Rect parentClient = m_parent->CalculateClientArea();

			m_activated = GetActiveWindow() == m_parent->Win32_GetHWND();

			// calculate child
			m_close.SetImage(
				LoadBitmapResource(IDR_SYSBUTTONS, Rect(32, 0, 16, 16)),
				LoadBitmapResource(IDR_SYSBUTTONS, Rect(32, 16, 16, 16)),
				LoadBitmapResource(IDR_SYSBUTTONS, Rect(32, 32, 16, 16))
				);
			m_minimize.SetImage(
				LoadBitmapResource(IDR_SYSBUTTONS, Rect(0, 0, 16, 16)),
				LoadBitmapResource(IDR_SYSBUTTONS, Rect(0, 16, 16, 16)),
				LoadBitmapResource(IDR_SYSBUTTONS, Rect(0, 32, 16, 16))
				);
			m_maximize.SetImage(
				LoadBitmapResource(IDR_SYSBUTTONS, Rect(48, 0, 16, 16)),
				LoadBitmapResource(IDR_SYSBUTTONS, Rect(48, 16, 16, 16)),
				LoadBitmapResource(IDR_SYSBUTTONS, Rect(48, 32, 16, 16))
				);
			m_restore.SetImage(
				LoadBitmapResource(IDR_SYSBUTTONS, Rect(16, 0, 16, 16)),
				LoadBitmapResource(IDR_SYSBUTTONS, Rect(16, 16, 16, 16)),
				LoadBitmapResource(IDR_SYSBUTTONS, Rect(16, 32, 16, 16))
				);

			m_pos.X = 0;
			m_pos.Y = 0;
			m_pos.Width = parentClient.Width;
			m_pos.Height = m_close.m_pos.Height + 8;
			ResetSizingInfo(Control::SizeInfo_Size_X);

			bool maximized = TRUE == IsZoomed(Win32_GetHWND());
			m_maximize.SetVisible(!maximized);
			m_restore.SetVisible(maximized);

			// and place all button children
			Rect ch(
				parentClient.Width - m_close.m_pos.Width - 4,
				4,
				m_close.m_pos.Width,
				m_close.m_pos.Height
				);

			AddChild(&m_close, ch, Control::SizeInfo_Move_X);
			ch.X -= m_maximize.m_pos.Width + 1;
			ch.Width = m_maximize.m_pos.Width;
			AddChild(&m_maximize, ch, Control::SizeInfo_Move_X);
			AddChild(&m_restore, ch, Control::SizeInfo_Move_X);
			ch.X -= m_minimize.m_pos.Width + 1;
			ch.Width = m_minimize.m_pos.Width;
			AddChild(&m_minimize, ch, Control::SizeInfo_Move_X);

			m_close.OnPressed.Add(this, &MenuBar::MBOnClose);
			m_minimize.OnPressed.Add(this, &MenuBar::MBOnMinimize);
			m_maximize.OnPressed.Add(this, &MenuBar::MBOnMaximize);
			m_restore.OnPressed.Add(this, &MenuBar::MBOnRestore);
		}

	};
}
