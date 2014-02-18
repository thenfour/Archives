// 0222 LayoutTool.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "LayoutTool.h"
#include "..\..\libcc\libcc\log.hpp"
#include "Panel.h"
#include "OverlappedWindow.h"
#include <list>

HINSTANCE g_hInstance;

LibCC::Log* LibCC::g_pLog = 0;

namespace UI
{
	ATOM OverlappedWindow::m_propAtom = 0;
	ATOM OverlappedWindow::m_classAtom = 0;
	int OverlappedWindow::m_refCount = 0;
}


inline int Width(const RECT& rc)
{
	return rc.right - rc.left;
}

inline int Height(const RECT& rc)
{
	return rc.bottom - rc.top;
}

	class ThatchControl :
		public AutoPlacement::IControl
	{
		UI::Control* m_p;
	public:
		ThatchControl(UI::Control* p) :
			m_p(p)
		{
		}
		IControl* Clone() const { return new ThatchControl(*this); }
		RECT GetParentsClientArea() const
		{
			RECT rc;
			UI::Control* parent = m_p;
			if(m_p->m_parent)
				parent = m_p->m_parent;

			UI::Rect rc1 = m_p->m_parent->CalculateClientArea(parent->m_pos);
			SetRect(&rc, 0, 0, rc1.Width, rc1.Height);
			return rc;
		}
		void SetPosRelativeToParent(const RECT& rc)
		{
			m_p->SetPos(UI::Rect(rc.left, rc.top, Width(rc), Height(rc)));
		}
		RECT GetPosRelativeToParent() const
		{
			RECT rc;
			SetRect(&rc, m_p->m_pos.X, m_p->m_pos.Y, m_p->m_pos.GetRight(), m_p->m_pos.GetBottom());
			return rc;
		}
	};

AutoPlacement::Manager mgr;


class TestingWindow :
	public UI::OverlappedWindow
{
public:
	TestingWindow()
	{
		OnClose.Add(this, &TestingWindow::AWOnClose);
		OnDestroy.Add(this, &TestingWindow::AWOnDestroy);
	}
	void AWOnClose(UI::Control* origin)
	{
		DestroyWindow(m_hwnd);
	}
	void AWOnDestroy(UI::Control* origin)
	{
		PostQuitMessage(0);
	}
	void AWOnSize(UI::Control* origin)
	{
		mgr.OnSize(&AutoPlacement::Win32GUIMetrics(m_hwnd));
	}
};

int Rand(int max)
{
	return (int)(((double)rand() / RAND_MAX) * max);
}


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE,  LPTSTR    lpCmdLine, int       nCmdShow)
{
	g_hInstance = hInstance;
	LibCC::g_pLog = new LibCC::Log(L"LayoutTool.log", g_hInstance, true, false, false);
	CoInitialize(0);

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::Status status = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	if (status == Gdiplus::Ok)
	{
		{
			TestingWindow wnd;
			UI::Panel mainPanel(255,230,255,230,L"");
			mainPanel.m_drawDetails = false;
			mainPanel.OnSize.Add(&wnd, &TestingWindow::AWOnSize);

			std::list<UI::Panel*> panels;

			mgr.RunFile(L"test.txt", &AutoPlacement::Win32GUIMetrics(0));

			panels.clear();
			for(std::map<std::wstring, AutoPlacement::Manager::SymbolStorage>::const_iterator it = mgr.GetSymbols().begin(); it != mgr.GetSymbols().end(); ++ it)
			{
				panels.push_back(new UI::Panel(120, Rand(128)+127, Rand(128)+127, Rand(128)+127, it->first));
				panels.back()->SetPos(UI::Rect(0,0,100,100));
				mainPanel.AddChild(panels.back());
				mgr.RegisterSymbol(it->first, ThatchControl(panels.back()));
			}

			wnd.Create(UI::Rect(300, 300, 400, 400), L"test", true);
			UI::Rect rc = wnd.CalculateClientArea(wnd.m_pos);
			rc.X = 0;
			rc.Y = 0;
			rc.Inflate(-10, -10);
			wnd.AddChild(&mainPanel, rc, (UI::Control::SizeInfo)(UI::Control::SizeInfo_Size_X | UI::Control::SizeInfo_Size_Y));

			MSG msg;
			while(GetMessage(&msg, 0, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			for(std::list<UI::Panel*>::iterator it = panels.begin(); it != panels.end(); ++ it)
			{
				delete *it;
			}
			panels.clear();
		}
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}

	CoUninitialize();
	delete LibCC::g_pLog;
	return 0;


	return 0;
}


