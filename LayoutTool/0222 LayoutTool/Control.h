
#pragma once


#include "memory"
#include "..\..\libcc\libcc\result.hpp"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#include <list>


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

	////////////////////////////////////////////////////////////////////////////////////////////////
	/*
		Event system. Usage:
		For controls that want to have an event:
		class MyControl : public Control
		{
			...
		public:
			UI::Event1<const Point&> OnClicked;// you could just use UI::Event0 if there are no event params.
			...
			void SomeFunction()
			{
				this->OnClicked.FireEvent(this, Point(0,0));// the first param is the originator of the event.
			}
		};


		and for callers that want to subscribe to that event:
		class MyClass
		{
			void MyEventHandler(Control* origin, const Point& p)
			{
			}
			void MyFunction()
			{
				someControl.OnClicked.Add(this, &MyClass::MyEventHandler);
			}
		}

		That's all there is to it.
	*/
	////////////////////////////////////////////////////////////////////////////////////////////////
	class Control
	{
	public:
		template<typename Tparam>
		class Event1
		{
			class IDelegate
			{
			public:
				virtual void Fire(Control* origin, Tparam params) = 0;
				virtual bool IsEqual(IDelegate* rhs) = 0;
			};

			typedef std::tr1::shared_ptr<IDelegate> IDelegatePtr;

			template<typename Tobj, typename Tproc>
			class Delegate :
				public IDelegate
			{
				Tobj m_obj;
				Tproc m_proc;
			public:
				Delegate(Tobj obj, Tproc proc) :
					m_obj(obj),
					m_proc(proc)
				{
				}
				bool IsEqual(IDelegate* _rhs)
				{
					Delegate<Tobj, Tproc>* rhs = (Delegate<Tobj, Tproc>*)_rhs;
					return rhs->m_obj == m_obj && rhs->m_proc == m_proc;
				}
			protected:
				void Fire(Control* origin, Tparam params) { (m_obj->*m_proc)(origin, params); }
			};

			std::vector<IDelegatePtr> m_items;

			// no copy
			Event1(const Event1<Tparam>&) { }
			Event1& operator =(const Event1<Tparam>&) { }
		public:
			Event1() { }

			void Fire(Control* origin, Tparam param)
			{
				for(std::vector<IDelegatePtr>::const_iterator it = m_items.begin(); it != m_items.end(); ++ it)
				{
					(*it)->Fire(origin, param);
				}
			}
			template<typename Tobj, typename Tproc>
			void Add(Tobj obj, Tproc proc)
			{
				m_items.push_back(IDelegatePtr(new Delegate<Tobj, Tproc>(obj, proc)));
			}
			template<typename Tobj, typename Tproc>
			void Remove(Tobj obj, Tproc proc)
			{
				Delegate<Tobj, Tproc> temp(obj, proc);
				for(std::vector<IDelegatePtr>::const_iterator it = m_items.begin(); it != m_items.end(); ++ it)
				{
					IDelegatePtr& p = *it;
					if(p->IsEqual(&temp))
					{
						m_items.erase(it);
						break;
					}
				}
			}

		};


		// Identical event system but with 0 arguments passed in.
		class Event0
		{
			class IDelegate
			{
			public:
				virtual void Fire(Control* origin) = 0;
				virtual bool IsEqual(IDelegate* rhs) = 0;
			};
			typedef std::tr1::shared_ptr<IDelegate> IDelegatePtr;
			template<typename Tobj, typename Tproc>
			class Delegate :
				public IDelegate
			{
				Tobj m_obj;
				Tproc m_proc;
			public:
				Delegate(Tobj obj, Tproc proc) :
					m_obj(obj),
					m_proc(proc)
				{
				}
				bool IsEqual(IDelegate* _rhs)
				{
					Delegate<Tobj, Tproc>* rhs = (Delegate<Tobj, Tproc>*)_rhs;
					return rhs->m_obj == m_obj && rhs->m_proc == m_proc;
				}
			protected:
				void Fire(Control* origin) { (m_obj->*m_proc)(origin); }
			};

			std::vector<IDelegatePtr> m_items;

			// no copy
			Event0(const Event0&) { }
			Event0& operator =(const Event0&) { }
		public:
			Event0() { }

			void Fire(Control* origin)
			{
				for(std::vector<IDelegatePtr>::const_iterator it = m_items.begin(); it != m_items.end(); ++ it)
				{
					(*it)->Fire(origin);
				}
			}
			template<typename Tobj, typename Tproc>
			void Add(Tobj obj, Tproc proc)
			{
				m_items.push_back(IDelegatePtr(new Delegate<Tobj, Tproc>(obj, proc)));
			}
			template<typename Tobj, typename Tproc>
			void Remove(Tobj obj, Tproc proc)
			{
				Delegate<Tobj, Tproc> temp(obj, proc);
				for(std::vector<IDelegatePtr>::iterator it = m_items.begin(); it != m_items.end(); ++ it)
				{
					IDelegatePtr& p = *it;
					if(p->IsEqual(&temp))
					{
						m_items.erase(it);
						break;
					}
				}
			}
		};

	public:
		enum SizeInfo
		{
			SizeInfo_None = 0,
			SizeInfo_Size_X = 1,
			SizeInfo_Size_Y = 2,
			SizeInfo_Move_X = 4,
			SizeInfo_Move_Y = 8,
		};

  private:
    // don't allow ambiguous calls
		Control(const Control&) { }
		template<typename T> Control& operator =(T) { return *this; }

	protected:
		SizeInfo m_sizeInfo;
		// save cached size info here.
		int m_sizeInfoXOffset;// for moving, this is parent width - this left. for sizing, this is parent width - this width.
		int m_sizeInfoYOffset;// similar, for Y.

		Control* m_mouseCapture;// at present, this is only set by overlappedwindow
		Control* m_mouseIn;// the control the mouse is currently hovering over.
	public:
		bool m_hasCapture;

	public:
		std::list<Control*> m_children;// order = z-order
		Control* m_parent;
		bool m_visible;
		Rect m_pos;// relative to parent's client area.

		Control() :
			m_parent(0),
			m_visible(true),
			m_mouseCapture(0),
			m_hasCapture(false),
			m_mouseIn(0)
		{
		}

		virtual ~Control()
		{
			// if we still have children, remove them
			while(m_children.size())
			{
				RemoveChild(m_children.front());
			}

			if(m_parent)
				m_parent->RemoveChild(this);
		}

		void SetPos(Rect pos)// pos is relative to parent's client
		{
			if(TRUE == pos.Equals(m_pos))
				return;
			SetDirty();// invalidate old position.
			m_pos = pos;
			OnSizeInternal();
			SetDirty();// invalidate new position.
		}

		virtual void CloseOverlappedWindow()// stops at the overlapped window override of this.
		{
			if(m_parent)
				m_parent->CloseOverlappedWindow();
		}

		void SetDirty()
		{
			if(m_parent)
				m_parent->SetClientDirty(m_pos);
		}

		// calculates the window size needed to fit the given client size.
		virtual Size GetWindowSizeThatFitsClient(const Size& client)
		{
			Rect theory = CalculateClientArea(Rect());// passing in 0,0 size means the resulting width & height is the size of the nonclient borders. but of course they're negative.
			return Size(client.Width - theory.Width, client.Height - theory.Height);
		}

		virtual Point ClientToScreen(const Point& in)// overlapped window overrides this
		{
			// translate to client of parent & pass to parent.
			Point pt = ClientToWindow(in);
			pt.X += m_pos.X;
			pt.Y += m_pos.Y;
			if(m_parent)
				return m_parent->ClientToScreen(pt);
			return in;
		}

		Rect ClientToScreen(const Rect& in)
		{
			Point ul, br;
			ul.X = in.X;
			ul.Y = in.Y;
			br.X = in.GetRight();
			br.Y = in.GetBottom();
			ul = ClientToScreen(ul);
			br = ClientToScreen(br);
			return Rect(ul.X, ul.Y, br.X - ul.X, br.Y - ul.Y);
		}

		Point ClientToWindow(const Point& in)
		{
			Rect thisClientArea = CalculateClientArea(m_pos);
			Point ret = in;
			ret.X += thisClientArea.X;
			ret.Y += thisClientArea.Y;
			return ret;
		}

		Rect ClientToWindow(const Rect& in)
		{
			Point ul, br;
			ul.X = in.X;
			ul.Y = in.Y;
			br.X = in.GetRight();
			br.Y = in.GetBottom();
			ul = ClientToWindow(ul);
			br = ClientToWindow(br);
			return Rect(ul.X, ul.Y, br.X - ul.X, br.Y - ul.Y);
		}

		// p is relative to this control
		Control* ControlFromPoint(const Point& p)
		{
			if(FALSE == Rect(0,0,m_pos.Width, m_pos.Height).Contains(p))
			{
				//LibCC::g_pLog->Message(LibCC::Format(L"ControlFromPoint returning 0"));
				return 0;
			}
			Rect rcClient = CalculateClientArea(m_pos);

			// see if this point is in a child, paying attention to z-order
			std::list<Control*>::reverse_iterator it;
			for(it = m_children.rbegin(); it != m_children.rend(); ++it)
			{
				Control* ch = *it;
				if(!ch->m_visible)
					continue;

				// translate p to child coords
				Point pch = p - TopLeft(ch->m_pos) - TopLeft(rcClient);
				Control* ret = ch->ControlFromPoint(pch);
				if(ret)
				{
					//LibCC::g_pLog->Message(LibCC::Format(L"ControlFromPoint returning %").p(ret));
					return ret;
				}
			}

			//LibCC::g_pLog->Message(LibCC::Format(L"ControlFromPoint returning %").p(this));
			return this;
		}

		// takes this window coords, and makes it relative to the given control. window coordinates in and out.
		Point TranslateToDescendent(Control* c, const Point& p)
		{
			Point ret = p;
			ret = ret - TopLeft(c->m_pos);
			while(true)
			{
				c = c->m_parent;
				if(c == 0)
					break;
				Rect rcClient = c->CalculateClientArea(c->m_pos);
				ret = ret - TopLeft(c->m_pos) - TopLeft(rcClient);
				if(c == this)
					return ret;
			}
			return p;// error actually.
		}


		// do nothing. The overlapped window will override this and do windows specific things
		virtual void SetClientDirty(const Rect& rcClient)
		{
			// convert client coords to parent client coords
			if(0 == m_parent)
				return;
			Rect rc = ClientToWindow(rcClient);
			m_parent->SetClientDirty(rc);
		}

		void ResetSizingInfo(SizeInfo si)
		{
			m_sizeInfo = si;
			if(m_parent)
			{
				Rect rcParentClient = m_parent->CalculateClientArea(m_parent->m_pos);

				if(si & SizeInfo_Move_X)
					m_sizeInfoXOffset = rcParentClient.Width - m_pos.X;
				if(si & SizeInfo_Move_Y)
					m_sizeInfoYOffset = rcParentClient.Height - m_pos.Y;
				if(si & SizeInfo_Size_X)
					m_sizeInfoXOffset = rcParentClient.Width - m_pos.Width;
				if(si & SizeInfo_Size_Y)
					m_sizeInfoYOffset = rcParentClient.Height - m_pos.Height;
			}
		}

		std::list<Control*>::iterator FindChild(Control* ch)
		{
			std::list<Control*>::iterator ret;
			for(ret = m_children.begin(); ret != m_children.end(); ++ ret)
			{
				if(*ret == ch)
					break;
			}
			return ret;
		}

		LibCC::Result RemoveChild(Control* ch)
		{
			std::list<Control*>::iterator it = FindChild(ch);
			if(it == m_children.end())
				return LibCC::Result::Failure(L"Can't remove a child when it's not there.");

			if(m_mouseCapture == ch)
				StopMouseCapture();

			if(m_mouseIn == ch)
				m_mouseIn = 0;

			ch->m_parent = 0;
			m_children.erase(it);
			return LibCC::Result::Success();
		}

		LibCC::Result AddChild(Control* ch)
		{
			return AddChild(ch, ch->m_pos);
		}

		LibCC::Result AddChild(Control* ch, Rect pos, SizeInfo si = SizeInfo_None)
		{
			// TODO: make sure there is no circular reference or anything.
			if(ch->m_parent)
				ch->m_parent->RemoveChild(ch);

			ch->SetPos(pos);
			ch->m_parent = this;
			ch->ResetSizingInfo(si);
			m_children.push_back(ch);
			ch->OnSetParent.Fire(this);
			OnAddChild.Fire(this, ch);
			return LibCC::Result::Success();
		}

		void StartMouseCapture()
		{
			InternalStartMouseCapture(this);
		}
		virtual void StopMouseCapture()// overlapped window override
		{
			if(m_parent)
				m_parent->StopMouseCapture();
		}
		virtual void InternalStartMouseCapture(Control* originator)// overlapped window override
		{
			if(m_parent)
				m_parent->InternalStartMouseCapture(originator);
		}

		// helper stuff for embedding windows controls
		virtual HWND Win32_GetHWND()// win32 windows (like overlapped window) will override this.
		{
			if(m_parent)
				return m_parent->Win32_GetHWND();
			return 0;
		}
		virtual Rect Win32_TranslateRect(Rect rc)// takes a rect in CLIENT coords and translates them to win32 client coords if this is a win32 window. if this isn't a win32 window, it will propagate to parent to calculate the answer.
		{
			// translate to window coords
			Rect rcWindow = ClientToWindow(rc);
			// translate to parent CLIENT coords so we can recurse to the parent.
			if(!m_parent)
				return rcWindow;
			rcWindow.X += m_pos.X;
			rcWindow.Y += m_pos.Y;
			return m_parent->Win32_TranslateRect(rcWindow);
		}

		// EVENT HANDLERS
		Event1<const Point&> OnMouseMove;
		Event1<const Point&> OnLButtonDown;
		Event1<const Point&> OnLButtonUp;
		Event0 OnMouseLeave;
		Event0 OnMouseEnter;
		Event0 OnSetParent;
		Event1<Control*> OnAddChild;
		Event0 OnSize;

		// things that child windows can override to control things more.
		virtual void OnPaint(GraphicsPtr) { };
		virtual Rect CalculateClientArea(const Rect& pos)// return a client area, relative to THIS control (no NC area means 0,0-size.x,size.y)
		{
			return Rect(0, 0, m_pos.Width, m_pos.Height);
		}
		virtual DWORD GetControlTypeId() = 0;// return an ID of the type of control. at the moment i'm using a CRC32 of a GUID to generate these. hey it's practical.
		virtual LRESULT HitTest(const Point& p) { return HTCLIENT; }// allows controls to indicate if the point is HTCAPTION or for sizing or whatever. p is in window coords

		// internal versions of message handlers - dispatching stuff to children.

		void OnSizeInternal()
		{
			OnSize.Fire(this);
			// resize child controls according to ch->m_sizeInfo
			std::list<Control*>::iterator it;
			for(it = m_children.begin(); it != m_children.end(); ++it)
			{
				Control* ch = *it;
				if(ch->m_sizeInfo == SizeInfo_None)
					continue;

				Rect client = CalculateClientArea(m_pos);
				Rect newRect = ch->m_pos;

				if(ch->m_sizeInfo & SizeInfo_Move_X)
					newRect.X = client.Width - ch->m_sizeInfoXOffset;
				if(ch->m_sizeInfo & SizeInfo_Move_Y)
					newRect.Y = client.Height - ch->m_sizeInfoYOffset;
				if(ch->m_sizeInfo & SizeInfo_Size_X)
					newRect.Width = client.Width - ch->m_sizeInfoXOffset;
				if(ch->m_sizeInfo & SizeInfo_Size_Y)
					newRect.Height = client.Height - ch->m_sizeInfoYOffset;

				ch->SetPos(newRect);
			}
		}

		void OnPaintInternal(GraphicsPtr buffer)
		{
			// paint THIS control somehow.
			OnPaint(buffer);
			Rect clientArea = CalculateClientArea(m_pos);

			Gdiplus::Region thisClip;// save the current clip rgn & transform
			buffer->GetClip(&thisClip);

			// paint all children.
			buffer->TranslateTransform((Gdiplus::REAL)clientArea.X, (Gdiplus::REAL)clientArea.Y);
			Gdiplus::Matrix m;
			buffer->GetTransform(&m);
			std::list<Control*>::iterator it;
			for(it = m_children.begin(); it != m_children.end(); ++it)
			{
				Control* ch = *it;
				if(ch->m_visible)
				{
					buffer->SetTransform(&m);
					buffer->SetClip(Rect(0, 0, clientArea.Width, clientArea.Height));
					buffer->TranslateTransform((Gdiplus::REAL)ch->m_pos.X, (Gdiplus::REAL)ch->m_pos.Y);
					ch->OnPaintInternal(buffer);
				}
			}
		}

		void OnLButtonDownInternal(const Point& pt)
		{
			Control* c = ControlFromPoint(pt);
			if(m_mouseCapture)
				c = m_mouseCapture;

			if(c)
				c->OnLButtonDown.Fire(this, TranslateToDescendent(c, pt));
		}

		void OnLButtonUpInternal(const Point& pt)
		{
			Control* c = ControlFromPoint(pt);
			if(m_mouseCapture)
				c = m_mouseCapture;

			if(c)
				c->OnLButtonUp.Fire(this, TranslateToDescendent(c, pt));
		}

		void OnMouseMoveInternal(const Point& pt)
		{
			Control* c = ControlFromPoint(pt);
			if(m_mouseIn != c)
			{
				if(m_mouseIn)
					m_mouseIn->OnMouseLeave.Fire(this);
				m_mouseIn = c;
				if(c)
					c->OnMouseEnter.Fire(this);
			}

			if(m_mouseCapture)
				c = m_mouseCapture;

			if(c)
				c->OnMouseMove.Fire(this, TranslateToDescendent(c, pt));
		}

		void OnInternalMouseLeave()
		{
			// this is called when the mouse leaves the overlapped window. if need be, send this to a control.
			if(!m_mouseIn)
				return;
			m_mouseIn->OnMouseLeave.Fire(this);
			m_mouseIn = 0;
		}
	};

}
