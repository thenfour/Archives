#pragma once
#include "Control.h"


namespace UI
{
	class Panel :
		public Control
	{
	public:
		Panel(int r, int g, int b) :
			m_col(r,g,b)
		{
		}

		virtual DWORD GetControlTypeId() { return 0x8db51b26; }

		Gdiplus::Color m_col;

		void OnPaint(GraphicsPtr buffer)
		{
			Gdiplus::SolidBrush* br = new Gdiplus::SolidBrush(m_col);
			buffer->FillRectangle(br, CalculateClientArea());
			delete br;
		}

	};
}

