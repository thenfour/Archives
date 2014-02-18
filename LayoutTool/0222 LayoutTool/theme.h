/*

Admittedly, not all theme stuff can be put in here. Stuff like behaviors, order of things, that sort of thing. but ok.

*/

#pragma once

namespace UI
{
namespace Theme
{
	// H 220 S 17% B 90 - 80 - 70 - 60 - 50 - 40 - 30 - 20 - 10
	// fundamental COLORS
	inline Gdiplus::Color Gray90() { return Gdiplus::Color(190,203,229); }
	inline Gdiplus::Color Gray80() { return Gdiplus::Color(169,181,204); }
	inline Gdiplus::Color Gray70() { return Gdiplus::Color(148,158,179); }
	inline Gdiplus::Color Gray60() { return Gdiplus::Color(127,136,153); }
	inline Gdiplus::Color Gray50() { return Gdiplus::Color(106,113,128); }
	inline Gdiplus::Color Gray45() { return Gdiplus::Color(95,102,115); }
	inline Gdiplus::Color Gray40() { return Gdiplus::Color(85,90,102); }
	inline Gdiplus::Color Gray35() { return Gdiplus::Color(74,79,89); }
	inline Gdiplus::Color Gray30() { return Gdiplus::Color(63,68,77); }
	inline Gdiplus::Color Gray20() { return Gdiplus::Color(42,45,51); }
	inline Gdiplus::Color Gray10() { return Gdiplus::Color(21,23,26); }

	// derived colors
	inline Gdiplus::Color PanelGradientStart() { return Gray60(); }
	inline Gdiplus::Color PanelGradientEnd() { return Gray50(); }
	inline Gdiplus::Color PanelHighlight() { return Gray70(); }
	inline Gdiplus::Color PanelShadow() { return Gray40(); }

	inline Gdiplus::Color SizingFrameTopHighlight() { return Gray50(); }
	inline Gdiplus::Color SizingFrameTopShadow() { return Gray45(); }
	inline Gdiplus::Color SizingFrameBottomHighlight() { return Gray40(); }
	inline Gdiplus::Color SizingFrameBottomShadow() { return Gray35(); }

	inline Gdiplus::Color Black() { return Gray10(); }

	// metrics
	static const int SizingGripCornerSize = 22;
	static const int SizeableBorderSize = 3;
	static const int ControlSpacing = 4;

	enum ImageID
	{
		IMAGE_CLOSE,
		IMAGE_MINIMIZE,
		IMAGE_MAXIMIZE,
		IMAGE_RESTORE,
		IMAGE_SIZINGTHUMB
	};

	enum ButtonState
	{
		ButtonState_Pressed,
		ButtonState_MouseOver,
		ButtonState_Normal
	};

	//BitmapPtr LoadPNGResource(int resID)
	//{
	//	MemStream mem(g_hInstance, MAKEINTRESOURCE(resID), L"PNG");
	//	BitmapPtr bmp(new Gdiplus::Bitmap(mem.m_pStream));
	//	Rect rc(0, 0, bmp->GetWidth(), bmp->GetHeight());

	//	/*
	//		i have to make a copy of the bitmap because:
	//		- freshly loaded PNGs have incorrect resolution settings, so DrawImage will scale them.
	//		- setting SetResolution() causes a freshly loaded PNG image to cease to work.
	//		Thus, i have to create a new bitmap with correct resolution, and copy the PNG to it using the more advanced DrawImage overload.
	//	*/

	//	BitmapPtr ret(new Gdiplus::Bitmap(bmp->GetWidth(), bmp->GetHeight(), PixelFormat32bppARGB));

	//	{
	//		Gdiplus::Graphics dest(ret.get());
	//		dest.DrawImage(bmp.get(), rc, 0, 0, bmp->GetWidth(), bmp->GetHeight(), Gdiplus::UnitPixel);
	//	}

	//	HDC dc = GetDC(0);
	//	{
	//		Gdiplus::Graphics g(dc);
	//		ret->SetResolution(g.GetDpiX(), g.GetDpiY());
	//	}
	//	ReleaseDC(0, dc);

	//	return ret;
	//}

	//BitmapPtr LoadPNGSlice(int resID, Rect rc)
	//{
	//	BitmapPtr temp(LoadPNGResource(resID));
	//	BitmapPtr ret(temp->Clone(rc, PixelFormat32bppARGB));
	//	return ret;
	//}

	//BitmapPtr CreateThemeBitmap(ImageID id)
	//{
	//	int resID = IDR_PNG2;
	//	return LoadPNGResource(resID);
	//}

	//BitmapPtr CreateThemeBitmap(ImageID id, ButtonState state)
	//{
	//	int x = 0;
	//	int y = 0;
	//	switch(id)
	//	{
	//	case IMAGE_MINIMIZE:
	//		x = 0;
	//		break;
	//	case IMAGE_RESTORE:
	//		x = 16;
	//		break;
	//	case IMAGE_CLOSE:
	//		x = 32;
	//		break;
	//	case IMAGE_MAXIMIZE:
	//		x = 48;
	//		break;
	//	}

	//	switch(state)
	//	{
	//	case ButtonState_MouseOver:
	//		y = 16;
	//		break;
	//	case ButtonState_Normal:
	//		y = 0;
	//		break;
	//	case ButtonState_Pressed:
	//		y = 32;
	//		break;
	//	}

	//	return LoadPNGSlice(IDR_PNG1, Rect(x, y, 16, 16));
	//}

	inline void DrawRectangleOutline(GraphicsPtr g, Rect rc,
		Gdiplus::Pen* penLeft, Gdiplus::Pen* penTop, Gdiplus::Pen* penRight, Gdiplus::Pen* penBottom)
	{
		// because TopRight() / BottomRight() would return the coordinate BOUNDARY, back up the width of the line (1px)
		rc.Width --;
		rc.Height --;
		g->DrawLine(penLeft, BottomLeft(rc), TopLeft(rc));
		g->DrawLine(penBottom, BottomRight(rc), BottomLeft(rc));
		g->DrawLine(penRight, TopRight(rc), BottomRight(rc));
		g->DrawLine(penTop, TopLeft(rc), TopRight(rc));
	}

	inline void DrawPanel(GraphicsPtr g, Gdiplus::Rect rc)
	{
		Gdiplus::Pen black(Black());
		Gdiplus::Pen highlight(PanelHighlight());
		Gdiplus::Pen shadow(PanelShadow());
		Gdiplus::LinearGradientBrush grad(rc, PanelGradientStart(), PanelGradientEnd(), Gdiplus::LinearGradientModeVertical);

		// outer black rect
		DrawRectangleOutline(g, rc, &black, &black, &black, &black);

		// shading
		rc.Inflate(-1, -1);
		DrawRectangleOutline(g, rc, &highlight, &highlight, &shadow, &shadow);

		// frame pattern
		rc.Inflate(-1, -1);
		g->FillRectangle(&grad, rc);
	}

	inline void DrawSizingFrame(GraphicsPtr g, Gdiplus::Rect rc)
	{
		Gdiplus::SolidBrush black(Black());
		Gdiplus::Pen sizingFrameTopHighlight(SizingFrameTopHighlight());
		Gdiplus::Pen sizingFrameTopShadow(SizingFrameTopShadow());
		Gdiplus::Pen sizingFrameBottomHighlight(SizingFrameBottomHighlight());
		Gdiplus::Pen sizingFrameBottomShadow(SizingFrameBottomShadow());

		// background
		g->FillRectangle(&black, rc);

		// outer rect
		rc.Inflate(-1, -1);
		DrawRectangleOutline(g, rc, &sizingFrameTopHighlight, &sizingFrameTopHighlight, &sizingFrameBottomShadow, &sizingFrameBottomShadow);

		// inner rect
		rc.Inflate(-1, -1);
		DrawRectangleOutline(g, rc, &sizingFrameTopShadow, &sizingFrameTopShadow, &sizingFrameBottomHighlight, &sizingFrameBottomHighlight);
	}

	inline Gdiplus::Color MenuItemBackground() { return Gdiplus::Color(240,190,140); }
	inline Gdiplus::Color MenuItemBorder() { return Gdiplus::Color(120,85,70); }
	inline Gdiplus::Color MenuItemText() { return Gdiplus::Color(0, 0, 0); }

	inline Size GetMenuBarItemSize(const std::wstring& caption, GraphicsPtr g)
	{
		Gdiplus::Font f(L"Tahoma", 8);
		Gdiplus::RectF stringSize;
		Size ret;

		g->MeasureString(caption.c_str(), -1, &f, Gdiplus::PointF(0, 0), &stringSize);
		stringSize.Height = f.GetHeight(g.get());// reset height to global height so everything is always exactly the same height.

		ret.Width = (int)ceil(stringSize.Width + 11);
		ret.Height = (int)ceil(stringSize.Height+ 4);
		return ret;
	}

	inline void DrawPopupItem(const std::wstring& caption, ButtonState state, GraphicsPtr g, const Rect& rc)
	{
		Gdiplus::Font f(L"Tahoma", 8);

		Gdiplus::StringFormat stringFormat;
		//stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);

		Gdiplus::RectF rcText(5, 2, (Gdiplus::REAL)rc.Width, (Gdiplus::REAL)(rc.Height - 4));

		if(state == ButtonState_Normal)
		{
			Gdiplus::SolidBrush textColor(Gray90());
			g->DrawString(caption.c_str(), -1, &f, rcText, &stringFormat, &textColor);
		}
		else
		{
			Gdiplus::SolidBrush backgroundColor(MenuItemBackground());
			g->FillRectangle(&backgroundColor, rc);

			Gdiplus::Pen border(MenuItemBorder());
			Rect rcBorder = rc;
			rcBorder.Width --;
			rcBorder.Height --;
			g->DrawRectangle(&border, rcBorder);

			Gdiplus::SolidBrush textColor(MenuItemText());
			g->DrawString(caption.c_str(), -1, &f, rcText, &stringFormat, &textColor);
		}
	}

	inline void DrawMenuBarItemImages(const std::wstring& caption, ButtonState state, GraphicsPtr g)
	{
		Size controlSize = GetMenuBarItemSize(caption, g);
		Rect controlRect(0, 0, controlSize.Width, controlSize.Height);
		DrawPopupItem(caption, state, g, controlRect);

		//Gdiplus::Font f(L"Tahoma", 8);

		//Gdiplus::StringFormat stringFormat;
		//stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);

		//Gdiplus::RectF rcText(0, 2, (Gdiplus::REAL)controlSize.Width, (Gdiplus::REAL)(controlSize.Height - 4));

		//if(state == ButtonState_Normal)
		//{
		//	Gdiplus::SolidBrush textColor(Gray90());
		//	g->DrawString(caption.c_str(), -1, &f, rcText, &stringFormat, &textColor);
		//}
		//else
		//{
		//	Gdiplus::SolidBrush backgroundColor(MenuItemBackground());
		//	g->FillRectangle(&backgroundColor, controlRect);

		//	Gdiplus::Pen border(MenuItemBorder());
		//	Rect rcBorder = controlRect;
		//	rcBorder.Width --;
		//	rcBorder.Height --;
		//	g->DrawRectangle(&border, rcBorder);

		//	Gdiplus::SolidBrush textColor(MenuItemText());
		//	g->DrawString(caption.c_str(), -1, &f, rcText, &stringFormat, &textColor);
		//}
	}

}
}
