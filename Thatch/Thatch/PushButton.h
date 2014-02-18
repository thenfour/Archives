

#pragma once


#include "control.h"
#include "theme.h"


namespace UI
{

	class PushButton :
		public Control
	{
		BitmapPtr m_normal;
		BitmapPtr m_mouseover;
		BitmapPtr m_pressed;
		ITheme::ButtonState m_state;

		void ChangeState(ITheme::ButtonState st)
		{
			if(st == m_state)
				return;
			//LibCC::g_pLog->Message(LibCC::Format(L"ChangeState(%)").i(st));
			m_state = st;
			SetDirty();
		}
	public:
		PushButton() :
			m_state(ITheme::ButtonState_Normal)
		{
			OnLButtonDown.Add(this, &PushButton::PBOnLButtonDown);
			OnLButtonUp.Add(this, &PushButton::PBOnLButtonUp);
			OnMouseLeave.Add(this, &PushButton::PBOnMouseLeave);
			OnMouseEnter.Add(this, &PushButton::PBOnMouseEnter);
		}

		virtual DWORD GetControlTypeId() { return 0xd2170ae2; }
		virtual std::wstring GetControlTypeName() { return L"PushButton"; };

		Event0 OnPressed;

		void SetImage(BitmapPtr normal, BitmapPtr mouseover, BitmapPtr pressed)
		{
			m_normal = normal;
			m_mouseover = mouseover;
			m_pressed = pressed;
			SetPos(Rect(m_pos.X, m_pos.Y, m_normal->GetWidth(), m_normal->GetHeight()));
			ResetSizingInfo(m_sizeInfo);
		}

		void OnPaint(GraphicsPtr buffer)
		{
			switch(m_state)
			{
			case ITheme::ButtonState_Normal:
				m_theme->DrawImageButton(buffer, CalculateClientArea(), ITheme::ButtonInfo(m_state, false), m_normal);
				break;
			case ITheme::ButtonState_MouseOver:
				m_theme->DrawImageButton(buffer, CalculateClientArea(), ITheme::ButtonInfo(m_state, false), m_mouseover);
				break;
			case ITheme::ButtonState_Pressed:
				m_theme->DrawImageButton(buffer, CalculateClientArea(), ITheme::ButtonInfo(m_state, false), m_pressed);
				break;
			}
		}

		void PBOnLButtonDown(Control* origin, const Point& p)
		{
			if(!m_hasCapture)
			{
				ChangeState(ITheme::ButtonState_Pressed);
				StartMouseCapture(this);
			}
		}

		void PBOnLButtonUp(Control* origin, const Point& p)
		{
			if(m_hasCapture)// required so that if you capture with R button, L button doesn't stop it.
			{
				StopMouseCapture();
				if(m_state == ITheme::ButtonState_Pressed)
				{
					OnPressed.Fire(this);
				}
				ChangeState(ITheme::ButtonState_Normal);
			}
		}

		void PBOnMouseLeave(Control* origin)
		{
			ChangeState(ITheme::ButtonState_Normal);
		}

		void PBOnMouseEnter(Control* origin)
		{
			//LibCC::g_pLog->Message(LibCC::Format(L"PBOnMouseEnter"));
			ChangeState(m_hasCapture ? ITheme::ButtonState_Pressed : ITheme::ButtonState_MouseOver);
		}
	};

}

