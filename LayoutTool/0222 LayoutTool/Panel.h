
#pragma once
#include "Control.h"


namespace UI
{
	class Panel :
		public Control
	{
	public:
		Panel(int a, int r, int g, int b, const std::wstring& caption) :
			m_col(a, r,g,b),
			m_caption(caption),
			m_drawDetails(true)
		{
		}

		bool m_drawDetails;

		virtual LRESULT HitTest(const Point& p)
		{
			return HTCAPTION;
		}

		virtual DWORD GetControlTypeId() { return 0xb80b0705; }

		Gdiplus::Color m_col;
		std::wstring m_caption;

		void OnPaint(GraphicsPtr buffer)
		{
			Gdiplus::SolidBrush br(m_col);
			Rect rcClient = CalculateClientArea(m_pos);
			Gdiplus::RectF layoutRectF(rcClient.X, rcClient.Y, rcClient.Width, rcClient.Height);
			layoutRectF.Width --;
			layoutRectF.Height --;
			Gdiplus::Rect layoutRect(layoutRectF.X, layoutRectF.Y, layoutRectF.Width, layoutRectF.Height);

			buffer->FillRectangle(&br, rcClient);

			if(m_drawDetails)
			{
				Gdiplus::Pen p(Gdiplus::Color(255,
					(m_col.GetR() + 128) % 255,
					(m_col.GetG() + 128) % 255,
					(m_col.GetB() + 128) % 255));
				p.SetDashStyle(Gdiplus::DashStyleCustom);
				Gdiplus::REAL dashVals[2] = { 3.0, 3.0 };
				p.SetDashPattern(dashVals, 2);

#if 0
				Gdiplus::Point vertices[6];
				vertices[0] = UI::TopLeft(layoutRect);
				vertices[1] = UI::BottomRight(layoutRect);
				vertices[2] = UI::TopRight(layoutRect);
				vertices[3] = UI::BottomLeft(layoutRect);
				vertices[4] = UI::TopLeft(layoutRect);
				vertices[5] = UI::TopRight(layoutRect);

				buffer->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

				buffer->DrawLine(&p, UI::BottomLeft(layoutRect), UI::BottomRight(layoutRect));
				buffer->DrawLines(&p, vertices, 6);
#else
				buffer->DrawRectangle(&p, layoutRect);
#endif

				Gdiplus::Font f(L"Tahoma", 10);
				Gdiplus::SolidBrush br2(Gdiplus::Color(255,
					(m_col.GetR() + 128) % 255,
					(m_col.GetG() + 128) % 255,
					(m_col.GetB() + 128) % 255));
				Gdiplus::StringFormat format;
				format.SetAlignment(Gdiplus::StringAlignmentCenter);
				format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
				buffer->DrawString(
					LibCC::Format(L"%|(%,%) w:% h:%")(m_caption)(m_pos.X)(m_pos.Y)(m_pos.Width)(m_pos.Height).CStr()
					,-1, &f, layoutRectF, &format, &br2);
			}
		}
	};
}

