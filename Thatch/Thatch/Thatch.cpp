/*
TODO:
- deal with keyboard focus & other keyboard events
o Menu Bar
o Push Button
o Status bar
o Menu Bar Item
o Popup Menu
o Popup Menu Item
- child window flickering on size
- optimize painting
- Tooltip
- Toggle Button
- Scroll bars
- Label
- Tree view
- List view
- List item
- Tab bar
- Tab item
x Edit box
x Get mousein/out messages happening in win32 children (i think we need a thread hook for that)
x Main window


TODO:
- When a control is shown/hidden, MouseEnter/MouseLeave should be evaluated.
- keyboard navigation / tab navigation.

*/

#include "stdafx.h"
#include "Thatch.h"
#include "..\..\libcc\libcc\result.hpp"
#include "utility.hpp"

#include "OverlappedWindow.h"
#include "Panel.h"
#include "MenuBar.h"
#include "StatusBar.h"
#include "MenuBarItem.h"
#include "Textbox.h"
#include "PopupMenu.h"

#include "RenoiseTheme.h"

#include "Serialization.h"
#pragma comment(lib, "gdiplus.lib")


HINSTANCE g_hInstance = 0;
LibCC::Log* LibCC::g_pLog = 0;


namespace UI
{
	ATOM Win32ControlHost::m_propAtom = 0;
	ATOM Win32ControlHost::m_childControlAtom = 0;
	DWORD Win32ControlHost::m_tlsIndex = 0;
	ATOM Win32ControlHost::m_classAtom = 0;
	ATOM Win32ControlHost::m_classAtomWithShadow = 0;
	int Win32ControlHost::m_refCount = 0;
}


class ThatchAppWindow :
	public UI::OverlappedWindow
{
public:
	ThatchAppWindow()
	{
		OnClose.Add(this, &ThatchAppWindow::AWOnClose);
		OnDestroy.Add(this, &ThatchAppWindow::AWOnDestroy);
	}
	void AWOnClose(Control* origin)
	{
		DestroyWindow(m_hwnd);
	}
	void AWOnDestroy(Control* origin)
	{
		PostQuitMessage(0);
	}
};


class Thatch
{
	ThatchAppWindow m_mainWindow;
public:
	void Run()
	{
#if 0
		ThatchAppWindow m_mainWindow;
		m_mainWindow.Create(Rect(-300,200,200,200), L"Thatch demo", true);
		/*
			P11:   P12:
			y--   y---
			-x-   -xx-
			-x-   ---z
			--z
		*/
		UI::Panel p1(255, 0, 0);
		UI::Panel p11(128, 128, 128);
		UI::Panel p12(128, 0, 128);

		UI::Panel p111(255,255,255);
		UI::Panel p112(0,0,255);
		UI::Panel p113(0,255,0);

		UI::Panel p121(0,255,255);
		UI::Panel p122(255,255,0);
		UI::Panel p123(255,0,255);

		m_mainWindow.AddChild(&p1, Rect(1, 1, 100, 100));
		p1.AddChild(&p11, Rect(0, 0, 3, 4));
		p1.AddChild(&p12, Rect(3, 4, 4, 3));

		p11.AddChild(&p111, Rect(-1,-1,2,2));
		p11.AddChild(&p112, Rect(1,1,1,2));
		p11.AddChild(&p113, Rect(2,3,2,2));

		p12.AddChild(&p121, Rect(0,0,1,1));
		p12.AddChild(&p122, Rect(1,1,2,1));
		p12.AddChild(&p123, Rect(3,2,2,2));
#else
		m_mainWindow.Create(UI::Rect(400,200,300,300), L"Thatch demo", true);

		UI::MenuBar p1;
		m_mainWindow.AddChild(&p1);

		UI::StatusBar s1;
		m_mainWindow.AddChild(&s1);

		UI::MenuBarItem menuItem1;
		p1.AddChild(&menuItem1);
		menuItem1.SetCaption(L"File");

		UI::MenuBarItem menuItem2;
		p1.AddChild(&menuItem2);
		menuItem2.SetCaption(L"Edit");

		UI::MenuBarItem menuItem3;
		p1.AddChild(&menuItem3);
		menuItem3.SetCaption(L"View");

		UI::MenuBarItem menuItem4;
		p1.AddChild(&menuItem4);
		menuItem4.SetCaption(L"Help");

		UI::PopupMenu pu;

		UI::PopupMenuItem pi1;
		UI::PopupMenuItem pi2;
		pi1.SetCaption(L"omg");
		pi2.SetCaption(L"Exit");
		pu.AddChild(&pi1);
		pu.AddChild(&pi2);
		pi2.OnClicked.Add(this, &Thatch::OnExit);
		pi1.OnClicked.Add(this, &Thatch::OnMenuItem);

		menuItem1.SetPopupMenu(&pu);
		UI::PopupMenu pu3;
		UI::PopupMenuItem pi31;
		pi31.SetCaption(L"ëmbàller le fetus");
		pu3.AddChild(&pi31);
		menuItem2.SetPopupMenu(&pu3);
		UI::PopupMenu pu4;
		UI::PopupMenuItem pi41;
		pu4.AddChild(&pi41);
		pi41.SetCaption(LibCC::Format().c(0xfec8).c(0xfecb).c(0xfb2c).c(0xe52).Str());
		menuItem3.SetPopupMenu(&pu4);

		UI::PopupMenu pu2;
		UI::PopupMenuItem pi3;
		pi3.SetCaption(L"Another menu item - this one is a BIIIIIIT longer. YOU FAGGOT.");
		pu2.AddChild(&pi3);
		menuItem4.SetPopupMenu(&pu2);

		//UI::PushButton pb1;
		//m_mainWindow.AddChild(&pb1, UI::Rect(101, 124, 0, 0), (UI::Control::SizeInfo)(UI::Control::SizeInfo_Move_X | UI::Control::SizeInfo_Move_Y));
		//pb1.SetImage(
		//	UI::Theme::CreateThemeBitmap(UI::Theme::IMAGE_CLOSE, UI::Theme::ButtonState_Normal),
		//	UI::Theme::CreateThemeBitmap(UI::Theme::IMAGE_CLOSE, UI::Theme::ButtonState_MouseOver),
		//	UI::Theme::CreateThemeBitmap(UI::Theme::IMAGE_CLOSE, UI::Theme::ButtonState_Pressed)
		//	);
		//pb1.OnPressed.Add(this, &Thatch::OnPB1Clicked);

		UI::Textbox tb;
		m_mainWindow.AddChild(&tb, UI::Rect(1,24,292,244), (UI::Control::SizeInfo)(UI::Control::SizeInfo_Size_X | UI::Control::SizeInfo_Size_Y));

#endif

		UI::Win32ControlHost::GetMessageLoop().Run();
	}
private:
		UI::MenuBarItem menuItem1;

	void OnMenuItem(UI::Control* origin)
	{
	}

	void OnExit(UI::Control* origin)
	{
		m_mainWindow.CloseTopLevelWindow();
	}

	//void OnPB1Clicked(UI::Control* origin)
	//{
	//	menuItem1.SetCaption(L"Menu.caption.changed");
	//}
};


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, int, void*)
{
	g_hInstance = hInstance;
	LibCC::g_pLog = new LibCC::Log(L"Thatch.log", g_hInstance);
	CoInitialize(0);

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::Status status = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	if (status == Gdiplus::Ok)
	{
		{
			Thatch t;
			t.Run();
		}
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}

	CoUninitialize();
	delete LibCC::g_pLog;
	return 0;
}

