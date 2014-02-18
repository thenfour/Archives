
#pragma once

#include "Control.h"
#include "Win32ControlHost.h"
#include "Theme.h"
#include <list>

namespace UI
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class PopupMenuItem :
		public Control
	{
		std::wstring m_caption;
		ITheme::ButtonState m_state;

	public:

		void ChangeState(ITheme::ButtonState st)
		{
			if(st == m_state)
				return;
			m_state = st;
			if(m_state == ITheme::ButtonState_MouseOver)
				OnStateChangeToMouseOver.Fire(this);
			SetDirty();
		}

		virtual DWORD GetControlTypeId() { return 0x4153ace8; }
		virtual std::wstring GetControlTypeName() { return L"Popup Menu item"; };

		Event0 OnClicked;
		Event0 OnStateChangeToMouseOver;

		PopupMenuItem() :
			m_state(ITheme::ButtonState_Normal)
		{
			OnLButtonUp.Add(this, &PopupMenuItem::PMIOnLButtonUp);
			OnMouseLeave.Add(this, &PopupMenuItem::PMIOnMouseLeave);
			OnMouseEnter.Add(this, &PopupMenuItem::PMIOnMouseEnter);
		}

		Size GetIdealSize()
		{
			HDC dc = GetDC(0);
			Size size;
			{
				GraphicsPtr g(new Gdiplus::Graphics(dc));
				size = m_theme->GetIdealPopupMenuItemSize(ITheme::PopupMenuItemSpec(m_state, ITheme::PMIT_Command, true, m_caption, L""));
			}
			ReleaseDC(0, dc);
			return size;
		}

		void SetCaption(const std::wstring& s)
		{
			m_caption = s;
			Size size = GetIdealSize();
			SetPos(Rect(m_pos.X, m_pos.Y, size.Width, size.Height));
		}

		void PMIOnLButtonUp(Control* origin, const Point& p)
		{
			OnClicked.Fire(this);
		}

		void PMIOnMouseLeave(Control* origin)
		{
			LibCC::g_pLog->Message(L"PMIOnMouseLeave");
			ChangeState(ITheme::ButtonState_Normal);
		}

		void PMIOnMouseEnter(Control* origin)
		{
			LibCC::g_pLog->Message(L"PMIOnMouseEnter");
			ChangeState(ITheme::ButtonState_MouseOver);
		}

		void OnPaint(GraphicsPtr buffer)
		{
			m_theme->DrawPopupMenuItem(buffer, Rect(0, 0, m_pos.Width, m_pos.Height), ITheme::PopupMenuItemSpec(m_state, ITheme::PMIT_Command, true, m_caption, L""));
		}
	};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class PopupMenu :
		public Win32ControlHost
	{
		void OnItemClicked(Control* origin)
		{
			CloseTopLevelWindow();
		}
		void OnItemChangedToMouseOver(Control* origin)
		{
		}
		void PMOnClose(Control* origin)
		{
			DestroyWindow(m_hwnd);
			OnHide.Fire(this);
		}
	public:
		PopupMenu()
		{
			OnAddChild.Add(this, &PopupMenu::PMOnAddChild);
			OnClose.Add(this, &PopupMenu::PMOnClose);
			OnDestroy.Add(this, &PopupMenu::PMOnDestroy);
			OnMouseActivate.Add(this, &PopupMenu::PMOnMouseActivate);
		}

		virtual DWORD GetControlTypeId() { return 0x418514c3; }
		virtual std::wstring GetControlTypeName() { return L"Popup Menu window"; };

		Event0 OnShow;
		Event0 OnHide;

		// only the MenuManager should call this.
		void __Show(int screenx, int screeny, Control* owner)
		{
			CloseTopLevelWindow();

			// make sure items are not selected.
			for(std::list<Control*>::iterator it = m_children.begin(); it != m_children.end(); ++ it)
			{
				Control* ch = *it;
				if(ch->GetControlTypeId() == PopupMenuItem().GetControlTypeId())
				{
					PopupMenuItem* pumi = (PopupMenuItem*)ch;
					pumi->ChangeState(ITheme::ButtonState_Normal);
				}
			}

			Create(Rect(screenx, screeny, m_pos.Width, m_pos.Height), L"",
				WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
				WS_POPUP, 0, true);

			ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);

			OnShow.Fire(this);
		}

		virtual Rect CalculateClientArea(const Size& pos)
		{
			return Rect(
				m_theme->GetSizingFrameSize(),
				m_theme->GetSizingFrameSize(),
				pos.Width - (m_theme->GetSizingFrameSize() * 2),
				pos.Height - (m_theme->GetSizingFrameSize() * 2)
				);
		}

		void PMOnMouseActivate(Control* origin, LRESULT& ret)
		{
			ret = MA_NOACTIVATE;
		}

		void Close()
		{
			CloseTopLevelWindow();
		}

		void OnPaint(GraphicsPtr buffer)
		{
			m_theme->DrawBodyBackground(buffer, m_pos);
			//Theme::DrawPanel(buffer, m_pos);
		}

		void PMOnChildSized(Control* origin)
		{
			ResetPositionInfo();
		}

		void PMOnDestroy(Control* origin)
		{
		}

		void ResetPositionInfo()
		{
			int height = 0;
			int width = 100;// set a minimum width here.

			// first figure out the width we will use.
			for(std::list<Control*>::iterator it = m_children.begin(); it != m_children.end(); ++ it)
			{
				Control* ch = *it;
				if(ch->GetControlTypeId() == PopupMenuItem().GetControlTypeId())
				{
					PopupMenuItem* pumi = (PopupMenuItem*)ch;
					width = max(width, pumi->GetIdealSize().Width);
				}
			}

			// and heights
			for(std::list<Control*>::iterator it = m_children.begin(); it != m_children.end(); ++ it)
			{
				Control* ch = *it;
				if(ch->GetControlTypeId() == PopupMenuItem().GetControlTypeId())
				{
					ch->SetPos(Rect(0, height, width, ch->m_pos.Height));
					height += ch->m_pos.Height;
				}
			}

			// make sure this control is big enough.
			Size requiredSize = GetWindowSizeThatFitsClient(Size(width, height));
			LibCC::g_pLog->Message(LibCC::Format(L"Setting popup menu size (%,%)")(requiredSize.Width)(requiredSize.Height));
			SetPos(Rect(0, 0, requiredSize.Width, requiredSize.Height));
		}

		void PMOnAddChild(Control* origin, Control* _p)
		{
			if(_p->GetControlTypeId() != PopupMenuItem().GetControlTypeId())
				return;
			PopupMenuItem* p = (PopupMenuItem*)_p;

			ResetPositionInfo();

			// sign up for notifications.
			p->OnSize.Add(this, &PopupMenu::PMOnChildSized);
			p->OnClicked.Add(this, &PopupMenu::OnItemClicked);
			p->OnStateChangeToMouseOver.Add(this, &PopupMenu::OnItemChangedToMouseOver);
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// this is required to correctly hook up events / set capture / dispatch to the right popup menu
	class PopupMenuManager
	{
		Control* m_owner;
		std::list<PopupMenu*> m_activePopups;
		PopupMenu* m_rootPopup;
	public:
		void SetOwner(Control* owner)
		{
			m_owner = owner;
		}

		void ShowPopup(PopupMenu* pu, int screenx, int screeny)
		{
			m_activePopups.clear();
			m_rootPopup = pu;

			pu->OnShow.Add(this, &PopupMenuManager::PMMOnShowPopup);
			pu->OnHide.Add(this, &PopupMenuManager::PMMOnHidePopup);
			pu->OnDestroy.Add(this, &PopupMenuManager::PMMOnDestroyRootPopup);

			Win32ControlHost::GetMessageLoop().OnPreTranslateMessage.Add(this, &PopupMenuManager::PMMOnPreTranslateMessage);
			Win32ControlHost::GetMessageLoop().OnAfterDispatchMessage.Add(this, &PopupMenuManager::PMMOnAfterDispatchMessage);

			pu->__Show(screenx, screeny, m_owner);
		}

		void CheckForActivationSwitch()
		{
      HWND hwndActive = GetActiveWindow();
			HWND hwndOwner = m_owner->Win32_GetHWND();
      if (hwndActive != hwndOwner && !IsChild(hwndActive, hwndOwner))
			{
				m_rootPopup->Close();
				return;
			}
		}

		void PMMOnDestroyRootPopup(Control* origin)
		{
			Win32ControlHost::GetMessageLoop().OnPreTranslateMessage.Remove(this, &PopupMenuManager::PMMOnPreTranslateMessage);
			Win32ControlHost::GetMessageLoop().OnAfterDispatchMessage.Remove(this, &PopupMenuManager::PMMOnAfterDispatchMessage);
		}

		void PMMOnPreTranslateMessage(Control* origin, MSG& msg)
		{
			// if the user clicked somewhere outside of a popup menu that we know about, close it.
			switch(msg.message)
			{
			case WM_LBUTTONDOWN:
				{
					HWND h = WindowFromPoint(msg.pt);
					bool found = false;
					for(std::list<PopupMenu*>::iterator it = m_activePopups.begin(); it != m_activePopups.end(); ++ it)
					{
						if((*it)->Win32_GetHWND() == h)
						{
							found = true;
							break;
						}
					}
					if(!found)
					{
						m_rootPopup->Close();
						return;
					}
					break;
				}
			}
			CheckForActivationSwitch();
		}

		void PMMOnAfterDispatchMessage(Control*)
		{
			CheckForActivationSwitch();
		}

		void PMMOnShowPopup(Control* origin)
		{
			m_activePopups.push_back((PopupMenu*)origin);
		}

		void PMMOnHidePopup(Control* origin)
		{
			for(std::list<PopupMenu*>::iterator it = m_activePopups.begin(); it != m_activePopups.end(); ++ it)
			{
				if(*it == (PopupMenu*)origin)
				{
					m_activePopups.erase(it);
					return;
				}
			}
		}
	};
}

