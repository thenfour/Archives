/*

Admittedly, not all theme stuff can be put in here. Stuff like behaviors, order of things, that sort of thing. but ok.

*/

#pragma once

extern HINSTANCE g_hInstance;


namespace UI
{
	inline BitmapPtr LoadBitmapResource(int resID, const Rect& srcArea_)
	{
		MemStream mem(g_hInstance, MAKEINTRESOURCE(resID), L"PNG");
		BitmapPtr bmp(new Gdiplus::Bitmap(mem.m_pStream));
		Rect srcRect(0, 0, bmp->GetWidth(), bmp->GetHeight());
		Rect srcArea;
		if(srcArea_.Width == 0)
			srcArea = srcRect;
		else
			srcArea = srcArea_;
		Rect destRect(0, 0, srcArea.Width, srcArea.Height);

		/*
			i have to make a copy of the bitmap because:
			- freshly loaded PNGs have incorrect resolution settings, so DrawImage will scale them.
			- setting SetResolution() causes a freshly loaded PNG image to cease to work.
			Thus, i have to create a new bitmap with correct resolution, and copy the PNG to it using the more advanced DrawImage overload.
		*/

		BitmapPtr ret(new Gdiplus::Bitmap(srcArea.Width, srcArea.Height, PixelFormat32bppARGB));
		{
			Gdiplus::Graphics dest(ret.get());
			dest.DrawImage(bmp.get(), destRect, srcArea.X, srcArea.Y, srcArea.Width, srcArea.Height, Gdiplus::UnitPixel);
		}

		HDC dc = GetDC(0);
		{
			Gdiplus::Graphics g(dc);
			ret->SetResolution(g.GetDpiX(), g.GetDpiY());
		}
		ReleaseDC(0, dc);

		return ret;
	}

	inline BitmapPtr LoadBitmapResource(int resID)
	{
		return LoadBitmapResource(resID, Rect());
	}

	// ------------------------------------------------------------------------------
	class ITheme
	{
	public:
		enum ButtonState
		{
			ButtonState_Pressed,
			ButtonState_MouseOver,
			ButtonState_Normal
		};
		struct ButtonInfo
		{
			ButtonInfo() :
				state(ButtonState_Normal),
				focusRect(false)
			{
			}
			ButtonInfo(const std::wstring& caption_, ButtonState state_, bool focusRect_) :
				caption(caption_),
				state(state_),
				focusRect(focusRect_)
			{
			}
			ButtonInfo(ButtonState state_, bool focusRect_) :
				state(state_),
				focusRect(focusRect_)
			{
			}
			std::wstring caption;
			ButtonState state;
			bool focusRect;
		};

		enum PopupMenuItemType
		{
			PMIT_Separator,
			PMIT_Command,
			PMIT_Popup
		};
		struct PopupMenuItemSpec
		{
			PopupMenuItemSpec()
			{
			}
			PopupMenuItemSpec(ButtonState state_, PopupMenuItemType type_, bool enabled_, const std::wstring& caption_, const std::wstring& keyCommand_) :
				state(state_),
				type(type_),
				enabled(enabled_),
				caption(caption_),
				keyCommand(keyCommand_)
			{
			}
			PopupMenuItemSpec(PopupMenuItemType type_, const std::wstring& caption_) :
				state(ButtonState_Normal),
				type(type_),
				enabled(true),
				caption(caption_)
			{
			}
			ButtonState state;
			PopupMenuItemType type;
			bool enabled;
			std::wstring caption;
			std::wstring keyCommand;
		};

		// low level stuff
		virtual Gdiplus::Color GetBodyBackgroundSolidColor() = 0; 
		virtual Gdiplus::Color GetBodyForegroundSolidColor() = 0; 

		//virtual void DrawMainBackground(GraphicsPtr g, Rect rc) = 0;
		//virtual void DrawMainText(GraphicsPtr g, Rect rc, const std::wstring& str) = 0;

		virtual void DrawBodyBackground(GraphicsPtr g, Rect rc) = 0;
		//virtual void DrawBodyText(GraphicsPtr g, Rect rc, const std::wstring& str) = 0;

		//virtual void DrawInnerBodyBackground(GraphicsPtr g, Rect rc) = 0;
		//virtual void DrawInnerBodyText(GraphicsPtr g, Rect rc, const std::wstring& str) = 0;

		virtual void DrawCommandBarBackground(GraphicsPtr g, Rect rc, bool activatedBorder) = 0;

		virtual void DrawPressedButtonBackground(GraphicsPtr g, Rect rc) = 0;
		virtual void DrawMouseOverButtonBackground(GraphicsPtr g, Rect rc) = 0;
		virtual void DrawButtonBackground(GraphicsPtr g, Rect rc) = 0;

		//virtual void DrawToolTipBackground() = 0;
		//virtual void DrawToolTipText() = 0;

		// --------------------------------------------------- frames
		virtual void DrawSizingFrame(GraphicsPtr g, Rect rc) = 0;
		virtual int GetSizingFrameSize() = 0;
		virtual int GetSizingCornerSize() = 0;

		// --------------------------------------------------- buttons
		virtual void DrawTextButton(GraphicsPtr g, Rect rc, const ButtonInfo& i) = 0;
		virtual void DrawImageButton(GraphicsPtr g, Rect rc, const ButtonInfo& i, BitmapPtr image) = 0;

		// --------------------------------------------------- menus
		virtual Size GetIdealMenuBarItemSize(const PopupMenuItemSpec& spec) = 0;
		virtual int GetMenuBarItemSpacing() = 0;
		virtual void DrawMenuBarItem(GraphicsPtr g, Rect rc, const PopupMenuItemSpec& spec) = 0;

		virtual Size GetIdealPopupMenuItemSize(const PopupMenuItemSpec& spec) = 0;
		virtual void DrawPopupMenuItem(GraphicsPtr g, Rect rc, const PopupMenuItemSpec& spec) = 0;
	};

	typedef std::tr1::shared_ptr<ITheme> IThemePtr;

	// ------------------------------------------------------------------------------
	class DefaultTheme :
		public ITheme
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
		inline Gdiplus::Color PanelGradientStart(ButtonState st)
		{
			if(st == ButtonState_Pressed) return Gray80();
			if(st == ButtonState_MouseOver) return Gray70();
			return Gray60();
		}
		inline Gdiplus::Color PanelGradientEnd(ButtonState st)
		{
			if(st == ButtonState_Pressed) return Gray30();
			if(st == ButtonState_MouseOver) return Gray40();
			return Gray50();
		}
		inline Gdiplus::Color PanelHighlight() { return Gray70(); }
		inline Gdiplus::Color PanelShadow() { return Gray40(); }

		inline Gdiplus::Color SizingFrameTopHighlight() { return Gray50(); }
		inline Gdiplus::Color SizingFrameTopShadow() { return Gray45(); }
		inline Gdiplus::Color SizingFrameBottomHighlight() { return Gray40(); }
		inline Gdiplus::Color SizingFrameBottomShadow() { return Gray35(); }

		inline Gdiplus::Color Black() { return Gray10(); }

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

	public:
		virtual Gdiplus::Color GetBodyForegroundSolidColor()
		{
			return Gray90();
		}
		virtual Gdiplus::Color GetBodyBackgroundSolidColor()
		{
			return Gray10();
		}

		virtual void DrawBodyBackground(GraphicsPtr g, Rect rc)
		{
			DrawButtonBackground(g, rc, ButtonState_Normal);
		}
		virtual void DrawCommandBarBackground(GraphicsPtr g, Rect rc, bool activatedBorder)
		{
			DrawButtonBackground(g, rc, activatedBorder ? ButtonState_MouseOver : ButtonState_Normal);
		}
		virtual void DrawButtonBackground(GraphicsPtr g, Rect rc, ButtonState st)
		{
			Gdiplus::Pen black(Black());
			Gdiplus::Pen highlight(PanelHighlight());
			Gdiplus::Pen shadow(PanelShadow());

			Gdiplus::LinearGradientBrush grad(rc, PanelGradientStart(st), PanelGradientEnd(st), Gdiplus::LinearGradientModeVertical);

			// outer black rect
			DrawRectangleOutline(g, rc, &black, &black, &black, &black);

			// shading
			rc.Inflate(-1, -1);
			DrawRectangleOutline(g, rc, &highlight, &highlight, &shadow, &shadow);

			// frame pattern
			rc.Inflate(-1, -1);
			g->FillRectangle(&grad, rc);
		}
		virtual void DrawPressedButtonBackground(GraphicsPtr g, Rect rc)
		{
			DrawButtonBackground(g, rc, ButtonState_Pressed);
		}
		virtual void DrawMouseOverButtonBackground(GraphicsPtr g, Rect rc)
		{
			DrawButtonBackground(g, rc, ButtonState_MouseOver);
		}
		virtual void DrawButtonBackground(GraphicsPtr g, Rect rc)
		{
			DrawButtonBackground(g, rc, ButtonState_Normal);
		}

		// --------------------------------------------------- frames
		virtual void DrawSizingFrame(GraphicsPtr g, Rect rc)
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

		virtual int GetSizingFrameSize()
		{
			return 3;
		}

		virtual int GetSizingCornerSize()
		{
			return 20;
		}

		// --------------------------------------------------- buttons
		virtual void DrawTextButton(GraphicsPtr g, Rect rc, const ButtonInfo& i)
		{
			DrawButtonBackground(g, rc, i.state);
		}

		virtual void DrawImageButton(GraphicsPtr g, Rect rc, const ButtonInfo& i, BitmapPtr image)
		{
			DrawButtonBackground(g, rc, i.state);
			g->DrawImage(image.get(), 0, 0);
		}


		// --------------------------------------------------- menus
		virtual Size GetIdealMenuBarItemSize(const PopupMenuItemSpec& spec)
		{
			Gdiplus::Font f(L"Tahoma", 8);
			Gdiplus::RectF stringSize;
			Size ret;

			ScreenGraphicsObj g;

			g->MeasureString(spec.caption.c_str(), -1, &f, Gdiplus::PointF(0, 0), &stringSize);
			stringSize.Height = f.GetHeight(g.get());// reset height to global height so everything is always exactly the same height.

			ret.Width = (int)ceil(stringSize.Width + 11);
			ret.Height = (int)ceil(stringSize.Height+ 4);
			return ret;
		}

		virtual int GetMenuBarItemSpacing()
		{
			return 5;
		}

		virtual void DrawMenuBarItem(GraphicsPtr g, Rect rc, const PopupMenuItemSpec& spec)
		{
			Size controlSize = GetIdealMenuBarItemSize(spec);
			Rect controlRect(0, 0, controlSize.Width, controlSize.Height);
			DrawPopupMenuItem(g, controlRect, spec);
		}

		virtual Size GetIdealPopupMenuItemSize(const PopupMenuItemSpec& spec)
		{
			return GetIdealMenuBarItemSize(spec);
		}

		virtual void DrawPopupMenuItem(GraphicsPtr g, Rect rc, const PopupMenuItemSpec& spec)
		{
			Gdiplus::Font f(L"Tahoma", 8);

			Gdiplus::StringFormat stringFormat;

			Gdiplus::RectF rcText(5, 2, (Gdiplus::REAL)rc.Width, (Gdiplus::REAL)(rc.Height - 4));

			if(spec.state == ButtonState_Normal)
			{
				Gdiplus::SolidBrush textColor(Gray90());
				g->DrawString(spec.caption.c_str(), -1, &f, rcText, &stringFormat, &textColor);
			}
			else
			{
				Gdiplus::SolidBrush backgroundColor(Gray60());
				g->FillRectangle(&backgroundColor, rc);

				Gdiplus::Pen border(Gray30());
				Rect rcBorder = rc;
				rcBorder.Width --;
				rcBorder.Height --;
				g->DrawRectangle(&border, rcBorder);

				Gdiplus::SolidBrush textColor(Gray80());
				g->DrawString(spec.caption.c_str(), -1, &f, rcText, &stringFormat, &textColor);
			}
		}
	};

}
