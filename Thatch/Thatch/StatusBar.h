

#pragma once

#include "theme.h"
#include "PushButton.h"


namespace UI
{
	// this class also contains code for moving, and the capability for exit/minimize/maximize buttons
	class StatusBar :
		public Control
	{
		BitmapPtr m_sizeThumb;
		static const int thumbMarginX = 2;
		static const int thumbMarginY = 2;
	public:

		StatusBar()
		{
			OnSetParent.Add(this, &StatusBar::SBOnSetParent);
		}

		~StatusBar()
		{
		}

		virtual DWORD GetControlTypeId() { return 0x52f5fb1; }
		virtual std::wstring GetControlTypeName() { return L"StatusBar"; };

		void OnPaint(GraphicsPtr buffer)
		{
			m_theme->DrawCommandBarBackground(buffer, Rect(0, 0, m_pos.Width, m_pos.Height), false);
			if(IsTopLevelWindowSizeable())
				buffer->DrawImage(m_sizeThumb.get(), m_pos.Width - m_sizeThumb->GetWidth() - thumbMarginX, thumbMarginY);
		}

		void SBOnSetParent(Control* origin)
		{
			Rect parentClient = m_parent->CalculateClientArea();

			m_sizeThumb = LoadBitmapResource(IDR_SIZINGTHUMB);

			m_pos.X = 0;
			m_pos.Width = parentClient.Width;
			m_pos.Height = m_sizeThumb->GetHeight() + thumbMarginY + thumbMarginY;
			m_pos.Y = parentClient.Height - m_pos.Height;

			ResetSizingInfo((Control::SizeInfo)(Control::SizeInfo_Size_X | Control::SizeInfo_Move_Y));
		}

		virtual LRESULT HitTest(const Point& p)
		{
			if(IsTopLevelWindowSizeable() && (p.X > (m_pos.Width - (int)m_sizeThumb->GetWidth() - thumbMarginX)))
				return HTBOTTOMRIGHT;
			return HTCAPTION;
		}
	};
}

