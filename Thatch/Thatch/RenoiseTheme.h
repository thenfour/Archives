/*

Since the haphazard "goal" of this was to recreate a Renoise-like-ui, i might as well steal
Renoise's graphics and even their theme structure instead of creating my own.

music-specific stuff i'll just ignore.

the theme only controls filling in things. no metrics are specified in the theme file at all, although i might do it here anyway.

it would be impossible to get this pixel-accurate to renoise without disassembling the app. there are filters applied to colors 
and stuff before rendering, so even the same source color + mask value will result in different colors on different
controls. renoise uses ddraw. renoise also uses HSV for the global color filter, so that may be a hint that they are doing
internal transformations using HSV. makes sense.

the basic process for rendering using a theme is this:

[base color]
   +
[theme skin] * [shading factor]
   +
[global filters]

*/

#pragma once

#include "serialization.h"
#include "Control.h"


namespace UI
{
namespace Theme
{
	class RenoiseTheme
	{
	public:
		BitmapPtr BodyTexture;// colors are discarded. tiled. used for inactive window titles
		BitmapPtr ControlBarBlending;// this is affected by bodyshading. it is tiled horizontally and scaled vertically. used for active caption bar, status bar, menu bar, tab bars, other command bars.
		BitmapPtr InnerBodyTexture;// tiled. not shaded. used for "inner body" / panels.

		BitmapPtr HSliderBarBlending;// affected by knobshade. scaled vertically, tiled horiz.
		BitmapPtr VSliderBarBlending;// affected by knobshade. scaled horizontally, tiled vertically.
		BitmapPtr PanelBlending;// affected by bodyshade. tiled horiz. scaled vertically. 30%

		BitmapPtr ButtonBlending;// this is stretched. - all buttons, including combo down arrows, scrollbar arrows, buttons, tabs. affected by Knobshade.
		BitmapPtr PressedButtonBlending;// affected by knobshade. scaled x/y. for pressed buttons. including those rounded ones at the top.

		BitmapPtr ValueBackBlending;// used for combobox areas, value entry boxes. scaled vertically, tiled horiz. affected by knobshade.

		Gdiplus::Color Main_Back;// background of window elements like txt boxes, a thin border around all controls, etc.
		Gdiplus::Color Main_Font;
		Gdiplus::Color Body_Back;
		Gdiplus::Color Body_Font;
		Gdiplus::Color Strong_Body_Font;

		Gdiplus::Color Button_Back;
		Gdiplus::Color Button_Font;
		Gdiplus::Color Selected_Button_Back;
		Gdiplus::Color Selected_Button_Font;

		Gdiplus::Color Selection_Back;
		Gdiplus::Color Selection_Font;
		Gdiplus::Color StandBy_Selection_Back;
		Gdiplus::Color StandBy_Selection_Font;

		Gdiplus::Color ToolTip_Back;
		Gdiplus::Color ToolTip_Font;
		Gdiplus::Color SliderBar_Back;// scrollbars

		double ButtonBevalAmount;
		double BodyBevalAmount;
		std::wstring TextureSet;

		RenoiseTheme(const std::wstring& skinsPath)
		{
		}

		static Gdiplus::Color DeserializeColor(Xml::Element parent, const std::wstring& elementName)
		{
			std::wstring text;
			if(!Xml::DeserializeFromElement(parent, elementName, text, false))
				return 0;
			std::vector<std::wstring> textParts;
			LibCC::StringSplitByString(text, L",", std::back_inserter(textParts));
			if(textParts.size() != 3)
				return 0;
			long r = wcstol(textParts[0].c_str(), 0, 10);
			long g = wcstol(textParts[1].c_str(), 0, 10);
			long b = wcstol(textParts[2].c_str(), 0, 10);
			return Gdiplus::Color((BYTE)r, (BYTE)g, (BYTE)b);
		}

		static double DeserializeDouble(Xml::Element parent, const std::wstring& elementName)
		{
			std::wstring text;
			if(!Xml::DeserializeFromElement(parent, elementName, text, false))
				return 0.0;
			return wcstod(text.c_str(), 0);
		}

		std::wstring GetTexturePath(const std::wstring& fileName)
		{
		}

		void Deserialize(Xml::Element n)
		{
			ButtonBevalAmount = DeserializeDouble(n, L"ButtonBevalAmount");
			BodyBevalAmount = DeserializeDouble(n, L"BodyBevalAmount");
			Xml::DeserializeFromElement(n, L"TextureSet", TextureSet, false);
			Main_Back = DeserializeColor(n, L"Main_Back");
			Main_Font = DeserializeColor(n, L"Main_Font");
			Body_Back = DeserializeColor(n, L"Body_Back");
			Body_Font = DeserializeColor(n, L"Body_Font");
			Strong_Body_Font = DeserializeColor(n, L"Strong_Body_Font");
			Button_Back = DeserializeColor(n, L"Button_Back");
			Button_Font = DeserializeColor(n, L"Button_Font");
			Selected_Button_Back = DeserializeColor(n, L"Selected_Button_Back");
			Selected_Button_Font = DeserializeColor(n, L"Selected_Button_Font");
			Selection_Back = DeserializeColor(n, L"Selection_Back");
			Selection_Font = DeserializeColor(n, L"Selection_Font");
			StandBy_Selection_Back = DeserializeColor(n, L"StandBy_Selection_Back");
			StandBy_Selection_Font = DeserializeColor(n, L"StandBy_Selection_Font");
			ToolTip_Back = DeserializeColor(n, L"ToolTip_Back");
			ToolTip_Font = DeserializeColor(n, L"ToolTip_Font");
			SliderBar_Back = DeserializeColor(n, L"SliderBar_Back");

			// load textures.
			BodyTexture;
			ControlBarBlending;
			InnerBodyTexture;

			HSliderBarBlending;
			VSliderBarBlending;
			PanelBlending;

			ButtonBlending;
			PressedButtonBlending;

			ValueBackBlending;
		}
	};
}
}

