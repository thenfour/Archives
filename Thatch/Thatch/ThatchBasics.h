#include "memory"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")


namespace UI
{
	typedef Gdiplus::RectF RectF;
	typedef Gdiplus::Rect Rect;
	typedef Gdiplus::PointF PointF;
	typedef Gdiplus::Point Point;
	typedef Gdiplus::SizeF SizeF;
	typedef Gdiplus::Size Size;
	typedef std::tr1::shared_ptr<Gdiplus::Graphics> GraphicsPtr;
	typedef std::tr1::shared_ptr<Gdiplus::Bitmap> BitmapPtr;

	inline Point TopLeft(const Rect& rc)
	{
		return Point(rc.X, rc.Y);
	}

	inline Point TopRight(const Rect& rc)
	{
		return Point(rc.GetRight(), rc.Y);
	}

	inline Point BottomLeft(const Rect& rc)
	{
		return Point(rc.X, rc.GetBottom());
	}

	inline Point BottomRight(const Rect& rc)
	{
		return Point(rc.GetRight(), rc.GetBottom());
	}

	inline Point MakePoint(LPARAM p)
	{
		return Point((short)LOWORD(p), (short)HIWORD(p));
	}
	inline Point MakePoint(const POINT& p)
	{
		return Point(p.x, p.y);
	}
	inline POINT MakePOINT(LPARAM p)
	{
		POINT ret;
		ret.x = (short)LOWORD(p);
		ret.y = (short)HIWORD(p);
		return ret;
	}
	inline POINT MakePOINT(const Point& p)
	{
		POINT ret;
		ret.x = p.X;
		ret.y = p.Y;
		return ret;
	}
	inline POINT POINTSToPOINT(const POINTS& s)
	{
		POINT ret;
		ret.x = s.x;
		ret.y = s.y;
	}
	inline Point ScreenLPARAMToClientPoint(HWND h, LPARAM p)
	{
		POINT pt = MakePOINT(p);
		ScreenToClient(h, &pt);
		return MakePoint(pt);
	}

	inline Rect RECTToRect(const RECT& rc)
	{
		return Rect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	}

	inline Rect MakeRect(const Point& topLeft, const Point& bottomRight)
	{
		return Rect(topLeft.X, topLeft.Y, bottomRight.X - topLeft.X, bottomRight.Y - topLeft.Y);
	}

	inline Size GetSize(const Rect& rc)
	{
		Size ret;
		rc.GetSize(&ret);
		return ret;
	}

	class ScreenGraphicsObj
	{
	public:
		ScreenGraphicsObj()
		{
			dc = GetDC(0);
			g = new Gdiplus::Graphics(dc);
		}
		~ScreenGraphicsObj()
		{
			ReleaseDC(0, dc);
			delete g;
		}
		Gdiplus::Graphics* operator ->() { return g; }
		Gdiplus::Graphics* get() { return g; }
	private:
		HDC dc;
		Gdiplus::Graphics* g;
	};

}