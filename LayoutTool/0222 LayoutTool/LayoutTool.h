/*
January 25, 2005 carlc - first version
March 3. 2006 carlc - ported to C#
Feb 22, 2008 carlc - ported back to C++ because i lost the other one

Documentation:
http://wiki.winprog.org/wiki/LibCC_AutoPlacement

*/

#pragma once

#include "..\..\libcc\libcc\log.hpp"
#include "..\..\libcc\libcc\stringutil.hpp"
#include "..\..\libcc\libcc\winapi.hpp"
#include <vector>
#include <map>


//--------------------------------------------------------------------------------
// allows nesting.
namespace AutoPlacement
{
	inline int Height(const RECT& rc)
	{
		return rc.bottom - rc.top;
	}
	inline int Width(const RECT& rc)
	{
		return rc.right - rc.left;
	}

	// helps simulate C#'s reference system. it's like ... a smart ptr but without any pointers. the main objective is to have a null option.
	template<typename T>
	class Nullable
	{
		bool m_haveValue;
		T value;
	public:
		Nullable(const Nullable<T>& rhs) :
			m_haveValue(rhs.m_haveValue),
			value(rhs.value)
		{
		}
		Nullable(const T& val) :
			m_haveValue(true),
			value(val)
		{
		}
		Nullable() :
			m_haveValue(false)
		{
		}
		void Assign(const T& v)
		{
			m_haveValue = true;
			value = v;
		}
		bool IsNull() const
		{
			return !m_haveValue;
		}
		void Delete()
		{
			m_haveValue = false;
		}

		T* operator ->()
		{
			return Get();
		}

		const T* operator ->() const
		{
			return Get();
		}

		T* Get()
		{
			if(!m_haveValue)
				return 0;
			return &value;
		}

		const T* Get() const
		{
			if(!m_haveValue)
				return 0;
			return &value;
		}
	};

	// --------------------------------------------------------------------------------
	template<typename T>
	class Stack2
	{
	public:
		typedef typename std::vector<T>::reverse_iterator iterator;
		typedef typename std::vector<T>::const_reverse_iterator const_iterator;

		void Pop()
		{
			m_container.pop_back();
		}
		void Push(const T& val)
		{
			m_container.push_back(val);
		}
		const T& Top() const
		{
			return m_container.back();
		}
		T& Top()
		{
			return m_container.back();
		}

		// walk from top of stack to the bottom.
		iterator begin()
		{
			return m_container.begin();
		}

		iterator end()
		{
			return m_container.end();
		}

		const_iterator begin() const
		{
			return m_container.rbegin();
		}

		const_iterator end() const
		{
			return m_container.rend();
		}

		size_t size() const
		{
			return m_container.size();
		}

	private:
		std::vector<T> m_container;
	};

	////////////////////////////////////////////////////////////
	class IControl
	{
	public:
		virtual ~IControl() { }
		virtual IControl* Clone() const = 0; // return a NEW object (to be delete'd)
		virtual void SetPosRelativeToParent(const RECT& rc) = 0;
		virtual RECT GetPosRelativeToParent() const = 0;
		virtual RECT GetParentsClientArea() const = 0;
	};

	////////////////////////////////////////////////////////////
	class IGUIMetrics
	{
	public:
		virtual double PointsToPixelsX(double pts) = 0;
		virtual double PointsToPixelsY(double pts) = 0;
		virtual double DLUsToPixelsX(double x) = 0;
		virtual double DLUsToPixelsY(double y) = 0;
	};

	static bool StringEqualsI(const std::wstring& a, const std::wstring& b)
	{
		return LibCC::StringToLower(a) == LibCC::StringToLower(b);
	}

	static bool StringEqualsI(const std::wstring& a, const std::string& b)
	{
		std::wstring wb;
		LibCC::StringConvert(b, wb);
		return LibCC::StringToLower(a) == LibCC::StringToLower(wb);
	}

	// parses "3.1123abcd" into 3.1123 and "abcd"
	// returns success
	static bool StringToDouble(const std::wstring& in_, double& num, std::wstring& str)
	{
		wchar_t* endptr = 0;
		num = wcstod(in_.c_str(), &endptr);
		str.assign(endptr);
		return true;
	}

	// does the string contain any chars that are NOT in chars?
	static bool StringContainsNotOf(const std::wstring& s, const std::wstring& chars)
	{
		for(std::wstring::const_iterator it = s.begin(); it != s.end(); ++ it)
		{
			if(std::wstring::npos == chars.find(*it)) return true;
		}
		return false;
	}

	static bool StartsWithNumber(const std::wstring& s)
	{
		wchar_t* endptr = 0;
		const wchar_t* startptr = s.c_str();
		wcstod(startptr, &endptr);
		return endptr != startptr;// could we run into different behavior if the string starts with whitespace?
	}

	// --------------------------------------------------------------------------------
	enum Orientation
	{
		None,
		Vertical,
		Horizontal
	};

	// --------------------------------------------------------------------------------
	class Message
	{
	public:
		enum Severity
		{
			Debug,
			Warning,
			Error
		};

		Message(const std::wstring& message_, Severity severity_, long line_, long column_)
		{
			message = message_;
			severity = severity_;
			line = line_;
			column = column_;
		}

		Message(const std::wstring& message_, Severity severity_, long line_)
		{
			message = message_;
			severity = severity_;
			line = line_;
			column = 0;
		}

		Message(const std::wstring& message_, Severity severity_)
		{
			message = message_;
			severity = severity_;
			line = 0;
			column = 0;
		}

		Message(const std::wstring& message_)
		{
			message = message_;
			severity = Error;
			line = 0;
			column = 0;
		}

		Message()
		{
			line = 0;
			column = 0;
			severity = Error;
		}

		std::wstring ToString() const
		{
			std::wstring sev = L"Unknown";
			switch(severity)
			{
			case Debug:
				sev = L"Debug";
				break;
			case Warning:
				sev = L"Warning";
				break;
			case Error:
				sev = L"Error";
				break;
			}
			return LibCC::Format(L"(%, %) %: %")(line)(column)(sev)(message).Str();
		}

		long line;
		long column;
		std::wstring message;
		Severity severity;
	};

	class IDebugLog
	{
	public:
		virtual void Warning(const std::wstring& msg) = 0;
		virtual void Error(const std::wstring& msg) = 0;
	};

	// --------------------------------------------------------------------------------
	class DimensionCallback
	{
	public:
		typedef double (__stdcall* StaticCallbackType)(const std::wstring& identifierName, void* userData);

	private:
		struct IDelegate
		{
			virtual double Call(const std::wstring& identifierName) const = 0;
			virtual IDelegate* Clone() const = 0;
		};
		template<typename Tobj, typename Tproc>
		struct Delegate :
			public IDelegate
		{
			Tobj m_obj;
			Tproc m_proc;
			Delegate(Tobj obj, Tproc proc) :
				m_obj(obj),
				m_proc(proc)
			{
			}
			double Call(const std::wstring& identifierName) const
			{
				return (m_obj->*(m_proc))(identifierName);
			}
			IDelegate* Clone() const
			{
				return new Delegate<Tobj, Tproc>(m_obj, m_proc);
			}
		};
		struct IDelegatePtr
		{
			IDelegatePtr() : m_ptr(0) { }
			~IDelegatePtr() { Reset(0); }
			const IDelegate* operator ->() const { return m_ptr; }
			void Reset(IDelegate* p)
			{
				if(m_ptr)
					delete m_ptr;
				m_ptr = p;
			}
			void CopyFrom(const IDelegatePtr& rhs)
			{
				if(!rhs.m_ptr)
				{
					Reset(0);
					return;
				}
				IDelegate* p = rhs->Clone();
				Reset(p);
			}
		private:
			// no copy / assign
			IDelegatePtr& operator =(const IDelegatePtr&) { }
			IDelegatePtr(const IDelegatePtr&) { }

			IDelegate* m_ptr;
		};

		StaticCallbackType m_staticCallbackProc;// determines whether this is a static or class callback.
		void* m_userData;
		IDelegatePtr m_classCallback;
		std::wstring m_identifierName;

		bool m_isValid;

	public:
		DimensionCallback() :
			m_userData(0),
			m_staticCallbackProc(0),
			m_isValid(false)
		{
		}

		DimensionCallback(const DimensionCallback& rhs) :
			m_userData(rhs.m_userData),
			m_staticCallbackProc(rhs.m_staticCallbackProc),
			m_isValid(rhs.m_isValid),
			m_identifierName(rhs.m_identifierName)
		{
			m_classCallback.CopyFrom(rhs.m_classCallback);
		}

		DimensionCallback& operator =(const DimensionCallback& rhs)
		{
			m_userData = rhs.m_userData;
			m_staticCallbackProc = rhs.m_staticCallbackProc;
			m_isValid = rhs.m_isValid;
			m_identifierName = rhs.m_identifierName;
			m_classCallback.CopyFrom(rhs.m_classCallback);
			return *this;
		}

		double Call() const
		{
			if(!m_isValid)
				return 0.0;
			if(m_staticCallbackProc)
				return m_staticCallbackProc(m_identifierName, m_userData);
			return m_classCallback->Call(m_identifierName);
		}

		void SetStaticCallback(const std::wstring& identifierName, StaticCallbackType callback, void* userData)
		{
			m_isValid = true;
			m_identifierName = identifierName;
			m_userData = userData;
			m_staticCallbackProc = callback;
		}

		template<typename Tobj, typename Tproc>
		void SetClassCallback(const std::wstring& name, Tobj callbackInstance, Tproc callbackClassMethod)
		{
			m_staticCallbackProc = 0;
			m_userData = 0;
			m_isValid = true;

			m_identifierName = name;
			m_classCallback.Reset(new Delegate<Tobj, Tproc>(callbackInstance, callbackClassMethod));
		}
	};

	// --------------------------------------------------------------------------------
	// A+B = [[+B]+A]
	// (A+B)*C = [[[+A]+B]*C]
	// A+(B*C) = [[[+B]*C]+A]
	class CallbackExpression
	{
	public:
		enum OperatorType
		{
			OT_Add,
			OT_Subtract,
			OT_Multiply,
			OT_Identity
		};
		enum ValueType
		{
			VT_Numeric,// use numericValue.
			VT_Callback,// call the callback to get the value.
			VT_Expression// like parenthesis. this expression's operand is the result of its children.
		};
		struct Operand
		{
			Operand() :
				expression(0)
			{
				Clear();
			}
			Operand(const CallbackExpression& rhs) :
				expression(0)
			{
				Clear();
				expression = new CallbackExpression(rhs);
				type = VT_Expression;
			}
			Operand(const Operand& rhs) :
				expression(0)
			{
				Assign(rhs);
			}
			Operand(const std::wstring& identifierName, DimensionCallback::StaticCallbackType callback, void* userData) :
				expression(0),
				type(VT_Callback)
			{
				callbackValue.SetStaticCallback(identifierName, callback, userData);
			}
			template<typename Tobj, typename Tproc>
			Operand(const std::wstring& identifierName, Tobj callbackInstance, Tproc callbackClassMethod) :
				expression(0),
				type(VT_Callback)
			{
				callbackValue.SetClassCallback(identifierName, callbackInstance, callbackClassMethod);
			}
			Operand& operator =(const Operand& rhs)
			{
				Assign(rhs);
				return *this;
			};
			void Clear()
			{
				type = VT_Numeric;
				numeric = 0.0;
				delete expression;
				expression = 0;
			}
			void Assign(const Operand& rhs)
			{
				Clear();
				type = rhs.type;
				numeric = rhs.numeric;
				callbackValue = rhs.callbackValue;
				if(rhs.expression)
					expression = new CallbackExpression(*rhs.expression);
			}
			double Evaluate() const
			{
				switch(type)
				{
				case VT_Numeric:
					return numeric;
				case VT_Callback:
					return callbackValue.Call();
				case VT_Expression:
					return expression->Evaluate();
				}
				return 0.0;
			}
			std::wstring ToString() const
			{
				switch(type)
				{
				case VT_Numeric:
					return LibCC::Format().d<1>(numeric).Str();
				case VT_Callback:
					return L"callback";
				case VT_Expression:
					return expression->ToString();
				}
				return L"(??)";
			}
			bool IsZero() const
			{
				switch(type)
				{
				case VT_Numeric:
					return (abs(numeric) < 0.0001);
				case VT_Callback:
					return false;
				case VT_Expression:
					return expression->IsZero();
				}
				return false;
			}

			ValueType type;
			double numeric;
			DimensionCallback callbackValue;
			CallbackExpression* expression;
		};

		CallbackExpression()
		{
			Clear();
		}
		CallbackExpression(const CallbackExpression& rhs)
		{
			Assign(rhs);
		}
		CallbackExpression& operator =(const CallbackExpression& rhs)
		{
			Assign(rhs);
			return *this;
		}

		void Clear()
		{
			operator_ = OT_Identity;
			lhs.Clear();
			rhs.Clear();
		}

		void Assign(const CallbackExpression& rhs_)
		{
			operator_ = rhs_.operator_;
			lhs = rhs_.lhs;
			rhs = rhs_.rhs;
		}

		double Evaluate() const
		{
			switch(operator_)
			{
			case OT_Identity:
				return lhs.Evaluate();
			case OT_Add:
				return lhs.Evaluate() + rhs.Evaluate();
			case OT_Subtract:
				return lhs.Evaluate() - rhs.Evaluate();
			case OT_Multiply:
				return lhs.Evaluate() * rhs.Evaluate();
			}
			return 0.0;
		}

		bool IsZero() const
		{
			switch(operator_)
			{
			case OT_Identity:
				return lhs.IsZero();
			case OT_Add:
			case OT_Subtract:
			case OT_Multiply:
				return lhs.IsZero() && rhs.IsZero();
			}
			return false;
		}

		void Add(const CallbackExpression& rhs_)
		{
			if(rhs_.IsZero())
				return;
			if(IsZero())
			{
				operator_ = OT_Identity;
				lhs = rhs_;
				return;
			}
			lhs = Operand(*this);
			rhs = Operand(rhs_);
			operator_ = OT_Add;
		}

		void Subtract(const CallbackExpression& rhs_)
		{
			if(rhs_.IsZero())
				return;
			if(IsZero())
			{
				operator_ = OT_Identity;
				lhs = rhs_;
				lhs.expression->Negate();
				return;
			}
			lhs = Operand(*this);
			rhs = Operand(rhs_);
			operator_ = OT_Subtract;
		}

		void Multiply(double v)
		{
			if(abs(v) < 0.0001)
			{
				Clear();
				return;
			}
			if(abs(v - 1.0) < 0.0001)
			{
				return;
			}
			// other optimizing measures...
			if(operator_ == OT_Identity && lhs.type == VT_Numeric)
			{
				lhs.numeric *= v;
				return;
			}
			if((operator_ == OT_Add || operator_ == OT_Subtract) && lhs.type == VT_Numeric && rhs.type == VT_Numeric)
			{
				lhs.numeric *= v;
				rhs.numeric *= v;
				return;
			}
			if((operator_ == OT_Multiply) && lhs.type == VT_Numeric)
			{
				lhs.numeric *= v;
				return;
			}
			if((operator_ == OT_Multiply) && rhs.type == VT_Numeric)
			{
				rhs.numeric *= v;
				return;
			}

			// ok we need to break it down.
			lhs = Operand(*this);
			rhs = Operand(rhs);
			operator_ = OT_Multiply;
		}

		void Divide(double v)
		{
			Multiply(1.0/v);
		}

		void Negate()
		{
			Multiply(-1.0);
		}

		void SetStaticCallback(const std::wstring& identifierName, DimensionCallback::StaticCallbackType callback, void* userData)
		{
			Clear();
			operator_ = OT_Identity;
			lhs = Operand(identifierName, callback, userData);
		}

		template<typename Tobj, typename Tproc>
		void SetClassCallback(const std::wstring& name, Tobj callbackInstance, Tproc callbackClassMethod)
		{
			Clear();
			operator_ = OT_Identity;
			lhs = Operand(name, callbackInstance, callbackClassMethod);
		}

		std::wstring ToString() const
		{
			switch(operator_)
			{
			case OT_Add:
				return LibCC::Format(L"(%+%)")(lhs.ToString())(rhs.ToString()).Str();
			case OT_Identity:
				return LibCC::Format(L"%")(lhs.ToString()).Str();
			case OT_Subtract:
				return LibCC::Format(L"(%-%)")(lhs.ToString())(rhs.ToString()).Str();
			case OT_Multiply:
				return LibCC::Format(L"(%*%)")(lhs.ToString())(rhs.ToString()).Str();
			}
			return L"(ERROR)";
		}

		Operand lhs;
		OperatorType operator_;
		Operand rhs;
	};

	// --------------------------------------------------------------------------------
	class Length
	{
	public:
		double _percent;
		double pixels;
		double dlus;
		double points;
		CallbackExpression callbacks;

		static Length Begin()
		{
			return Length();
		}

		static Length End()
		{
			Length n;
			n._percent = 1.0;
			return n;
		}

		Length()
		{
			Clear();
		}
		Length(const Length& rhs)
		{
			Assign(rhs);
		}

		void Clear()
		{
			callbacks.Clear();

			_percent = 0;
			pixels = 0;
			dlus = 0;
			points = 0;
		}

		void Add(const Length& x)
		{
			_percent += x._percent;
			pixels += x.pixels;
			dlus += x.dlus;
			points += x.points;
			callbacks.Add(x.callbacks);
		}

		void Negate()
		{
			_percent = -_percent;
			pixels = -pixels;
			dlus = -dlus;
			points = -points;
			callbacks.Negate();
		}

		void Subtract(const Length& x)
		{
			_percent -= x._percent;
			pixels -= x.pixels;
			dlus -= x.dlus;
			points -= x.points;
			callbacks.Subtract(x.callbacks);
		}

		void Divide(double n)
		{
			_percent /= n;
			pixels /= n;
			dlus /= n;
			points /= n;
			callbacks.Divide(n);
		}

		void Multiply(double n)
		{
			_percent *= n;
			pixels *= n;
			dlus *= n;
			points *= n;
			callbacks.Multiply(n);
		}

		void Assign(const Length& rhs)
		{
			_percent = rhs._percent;
			pixels = rhs.pixels;
			dlus = rhs.dlus;
			points = rhs.points;
			callbacks.Assign(rhs.callbacks);
		}

		// transform our values to remove dependence on a parent start/end.
		// for example, if parent is
		//                     50%+12px---------------------------------------80%+4px
		//                                                   ^ 60%+7px   <-- and this is our value.
		void Flatten(const Length& parent_start, const Length& parent_end)
		{
			Length temp(parent_end);
			temp.Subtract(parent_start);
			// now temp is the parent total distance. scale it by our percentage.
			temp.Multiply(_percent);
			temp.Add(parent_start);// now it's correct, but need to add in our non-percentage units.
			temp.dlus += dlus;
			temp.pixels += pixels;
			temp.points += points;
			temp.callbacks.Add(this->callbacks);
			Assign(temp);
		}

		std::wstring ToString() const
		{
			bool first = true;
			std::wstring result = L"(";

			if(abs(points) > 0.001)
			{
				result += LibCC::Format(L"%pt").d<1>(points).Str();
				first = false;
			}

			if(abs(_percent) > 0.001)
			{
				if(!first)
					result += L"+";
				result += LibCC::Format(L"%^%").d<0>(_percent*100).Str();
				first = false;
			}

			if(abs(pixels) > 0.001)
			{
				if(!first)
					result += L"+";
				result += LibCC::Format(L"%px").d<1>(pixels).Str();
				first = false;
			}

			if(abs(dlus) > 0.001)
			{
				if(!first)
					result += L"+";
				result += LibCC::Format(L"%dlu").d<1>(dlus).Str();
				first = false;
			}
			
			if(!callbacks.IsZero())
			{
				if(!first)
					result += L"+";
				result += callbacks.ToString();
				first = false;
			}

			if(first)
				result += L"0";
			result += L")";

			return result;
		}

		int EverythingButPercentToPixels(IGUIMetrics* m, Orientation o) const
		{
			double ret = pixels + callbacks.Evaluate();
			if(o == Vertical)
			{
				ret += m->DLUsToPixelsY(dlus);
				ret += m->PointsToPixelsY(points);
			}
			else
			{
				ret += m->DLUsToPixelsX(dlus);
				ret += m->PointsToPixelsX(points);
			}
			return (int)(ret);
		}

		int ToPixels(IGUIMetrics* m, const RECT& rc, Orientation o) const
		{
			double ret = EverythingButPercentToPixels(m, o);
			if(o == Vertical)
			{
				ret += _percent * Height(rc);
			}
			else
			{
				ret += _percent * Width(rc);
			}
			return (int)(ret);
		}
	};

	// --------------------------------------------------------------------------------
	// most of this gathered from
	// "Design Specifications and Guidelines - Visual Design"
	// http://msdn2.microsoft.com/en-us/library/ms997619.aspx
	// all items are in DLU's
	class UISettings
	{
	public:
		UISettings()
		{
		}

		void LoadDefaults()
		{
			ControlSpacingRelated = 4;
			ControlSpacingUnrelated = 7;
			LabelHeight = 8;
			ParagraphSpacing = 7;
			ClarityIndent = 10;
			LabelSpacingFromAssociatedControlTop = 3;

			// dialog
			DialogMarginLeft = 7;
			DialogMarginRight = 7;
			DialogMarginTop = 7;
			DialogMarginBottom = 7;
			DialogWideMarginLeft = 21;
			DialogWideMarginRight = 21;

			// group
			GroupMarginLeft = 9;
			GroupMarginTop = 11;
			GroupMarginRight = 9;
			GroupMarginBottom = 7;

			// controls
			ButtonWidth = 50;
			ButtonHeight = 14;
			CheckboxHeight = 10;
			ComboboxHeight = 14;
			RadioButtonHeight = 10;
			EditBoxHeight = 14;
		}

		bool GetElement(double& out_, const std::wstring& s) const
		{
			if(StringEqualsI(s, "spacing.relatedcontrol") || StringEqualsI(s, "spacing"))
			{
				out_ = ControlSpacingRelated;
			}
			else if(StringEqualsI(s, "spacing.unrelatedcontrol") || StringEqualsI(s, "spacing.unrelated"))
			{
				out_ = ControlSpacingUnrelated;
			}
			else if(StringEqualsI(s, "spacing.clarityindent") || StringEqualsI(s, "spacing.clarityindent.left"))
			{
				out_ = ClarityIndent;
			}
			else if(StringEqualsI(s, "margin.left"))
			{
				out_ = DialogMarginLeft;
			}
			else if(StringEqualsI(s, "margin.right"))
			{
				out_ = DialogMarginRight;
			}
			else if(StringEqualsI(s, "margin.top"))
			{
				out_ = DialogMarginTop;
			}
			else if(StringEqualsI(s, "margin.bottom"))
			{
				out_ = DialogMarginBottom;
			}
			else if(StringEqualsI(s, "widemargin.left"))
			{
				out_ = DialogWideMarginLeft;
			}
			else if(StringEqualsI(s, "widemargin.right"))
			{
				out_ = DialogWideMarginRight;
			}
			else if(StringEqualsI(s, "label.height"))
			{
				out_ = LabelHeight;
			}
			else if(StringEqualsI(s, "label.paragraphspacing"))
			{
				out_ = ParagraphSpacing;
			}
			else if(StringEqualsI(s, "label.offset.top") || StringEqualsI(s, "label.offset"))
			{
				out_ = LabelSpacingFromAssociatedControlTop;
			}
			else if(StringEqualsI(s, "group.margin.left"))
			{
				out_ = GroupMarginLeft;
			}
			else if(StringEqualsI(s, "group.margin.top"))
			{
				out_ = GroupMarginTop;
			}
			else if(StringEqualsI(s, "group.margin.right"))
			{
				out_ = GroupMarginRight;
			}
			else if(StringEqualsI(s, "group.margin.bottom"))
			{
				out_ = GroupMarginBottom;
			}
			else if(StringEqualsI(s, "button.width"))
			{
				out_ = ButtonWidth;
			}
			else if(StringEqualsI(s, "button.height"))
			{
				out_ = ButtonHeight;
			}
			else if(StringEqualsI(s, "checkbox.height"))
			{
				out_ = CheckboxHeight;
			}
			else if(StringEqualsI(s, "combobox.height"))
			{
				out_ = ComboboxHeight;
			}
			else if(StringEqualsI(s, "radio.height"))
			{
				out_ = RadioButtonHeight;
			}
			else if(StringEqualsI(s, "editbox.height") || StringEqualsI(s, "edit.height"))
			{
				out_ = EditBoxHeight;
			}
			else
			{
				return false;
			}
			return true;
		}

		// general layout
		double ControlSpacingRelated;// 4 dlu
		double ControlSpacingUnrelated;// 7 dlu
		double LabelHeight;// 8 dlu per line
		double ParagraphSpacing;// 7 dlu
		double ClarityIndent;// 10 dlu  -- when there is a label and some controls below, indented.
		double LabelSpacingFromAssociatedControlTop;//3 dlu -- a label with an edit box, for instance.. gets placed 3 dlu's below the top of the edit

		// dialog
		double DialogMarginLeft;// 7 dlu
		double DialogMarginRight;// 7 dlu
		double DialogMarginTop;// 7 dlu
		double DialogMarginBottom;// 7 dlu
		double DialogWideMarginLeft;// 21 dlu
		double DialogWideMarginRight;// 21 dlu

		// group
		double GroupMarginLeft;// 9 dlu
		double GroupMarginTop;// 11 dlu
		double GroupMarginRight;// 9 dlu
		double GroupMarginBottom;// 7 dlu

		// controls
		double ButtonWidth;// 50 dlu
		double ButtonHeight;// 14 dlu
		double CheckboxHeight;// 10 dlu
		double ComboboxHeight;// 14 dlu
		double RadioButtonHeight;// 10 dlu
		double EditBoxHeight;// 14 dlu
	};

	// --------------------------------------------------------------------------------
	/*
	An identifier is a symbolic or numeric value, with units.  after it's "translated" into being ALWAYS numeric,
	it's converted to a Length, in pixels
	*/
	class Identifier
	{
	public:
		Identifier()
		{
			Clear();
		}
		~Identifier()
		{
			Clear();
		}

		Identifier(const Identifier& rhs)
		{
			Assign(rhs);
		}

		Identifier& operator =(const Identifier& rhs)
		{
			Assign(rhs);
			return *this;
		}

		void Assign(const Identifier& rhs)
		{
			Clear();
			m_callbackExpression = rhs.m_callbackExpression;
			m_Type = rhs.m_Type;
			m_Parts = rhs.m_Parts;
			m_numericValue = rhs.m_numericValue;
		}

		void Clear()
		{
			m_Type = Empty;
			m_Parts.clear();
			m_callbackExpression.Clear();
		}

		enum Type
		{
			Empty,
			Flexible,
			Numeric,
			Symbolic,// uses m_Parts
			Variable,
			Callback
		};

		Type m_Type;
		std::vector<std::wstring> m_Parts;// of strings
		Length m_numericValue;

		typedef std::map<std::wstring, Identifier> VariableMap;

		CallbackExpression m_callbackExpression;

		void SetStaticCallback(const std::wstring& identifierName, DimensionCallback::StaticCallbackType callback, void* userData)
		{
			m_Type = Callback;
			m_callbackExpression.SetStaticCallback(identifierName, callback, userData);
		}

		template<typename Tobj, typename Tproc>
		void SetClassCallback(const std::wstring& name, Tobj callbackInstance, Tproc callbackClassMethod)
		{
			m_Type = Callback;
			m_callbackExpression.SetClassCallback(name, callbackInstance, callbackClassMethod);
		}

		// this function converts an identifier to a Length
		// vars maps string -> Identifier
		// output = array of message
		bool Evaluate(Length& out_, const UISettings& uiSettings, const VariableMap& vars, Orientation orientation, IDebugLog* output) const
		{
			bool r = false;
			out_.Clear();

			switch(m_Type)
			{
			case Flexible:
				output->Error(L"Unable to evaluate identifier (flexible).");
				break;
			case Numeric:
				out_ = m_numericValue;
				r = true;
				break;
			case Empty:
				output->Error(L"Unable to evaluate identifier (empty).");
				break;
			case Callback:
				out_.callbacks = m_callbackExpression;
				r = true;
				break;
			case Symbolic:
				{
					std::wstring identifier = LibCC::StringJoin(m_Parts.begin(), m_Parts.end(), L".");//string.Join(".", (string[])m_Parts.ToArray(typeof(string)));
					Identifier temp;
					double i = 0;
					if(!uiSettings.GetElement(i, identifier))
					{
						// TODO: we could try other variations here. like adding "height" or "width"
						output->Warning(LibCC::Format(L"Symbolic identifier '%' unknown")(identifier).Str());
					}
					else
					{
						temp.m_Type = Numeric;
						temp.m_numericValue.dlus = i;
						r = temp.Evaluate(out_, uiSettings, vars, orientation, output);
					}
					break;
				}
			case Variable:
				{
					if(m_Parts.size() != 1)
					{
						output->Error(L"Invalid variable spec (part count != 1).");
					}
					else
					{
						VariableMap::const_iterator it = vars.find(m_Parts[0]);
						if(it == vars.end())
						{
							output->Error(LibCC::Format(L"Undefined variable '%'.")(m_Parts[0]).Str());
						}
						else
						{
							switch(it->second.m_Type)
							{
							case Empty:
							case Flexible:
							case Symbolic:
							case Variable:
								output->Error(LibCC::Format(L"Variable '%' is not a value type.")(m_Parts[0]).Str());
								break;
							default:
								return it->second.Evaluate(out_, uiSettings, vars, orientation, output);
							}
						}
					}
					break;
				}
			default:
				output->Error(L"Unknown units.");
				break;
			}
			return r;
		}

		// *assignment,assignment2 (distance)
		std::wstring ToString() const
		{
			// construct a string that represents assignments;
			std::wstring assignments = LibCC::StringJoin(m_Parts.begin(), m_Parts.end(), L".");
			if(assignments.size() > 0)
			{
				assignments = std::wstring(L"*") + assignments;
			}

			// type->string
			std::wstring type;
			switch(m_Type)
			{
			case Empty:
				type = L"empty"; break;
			case Symbolic:
				type = L"symbolic"; break;
			case Numeric:
				type = m_numericValue.ToString(); break;
			case Variable:
				type = L"variable"; break;
			case Flexible:
				type = L"flex"; break;
			default:
				type = L"(unknown)"; break;
			}

			return LibCC::Format(L"%(%)")(assignments)(type).Str();
		}
	};

	// --------------------------------------------------------------------------------
	class OptimizedPlacement
	{
	public:
		Nullable<Length> m_HStart;
		Nullable<Length> m_HEnd;
		Nullable<Length> m_VStart;
		Nullable<Length> m_VEnd;

		// resizes p to its parent, based on the placement stuff.
		void OnSize(IControl* pc, IGUIMetrics* m) const
		{
			RECT rcParentsClientArea = pc->GetParentsClientArea();
			RECT relativeToParent = pc->GetPosRelativeToParent();

			int left = (m_HStart.IsNull()) ? relativeToParent.left : m_HStart->ToPixels(m, rcParentsClientArea, Horizontal) ;
			int right = (m_HEnd.IsNull()) ? relativeToParent.right : m_HEnd->ToPixels(m, rcParentsClientArea, Horizontal) ;
			int top = (m_VStart.IsNull()) ? relativeToParent.top : m_VStart->ToPixels(m, rcParentsClientArea, Vertical) ;
			int bottom = (m_VEnd.IsNull()) ? relativeToParent.bottom : m_VEnd->ToPixels(m, rcParentsClientArea, Vertical) ;
			RECT rc;
			SetRect(&rc, left, top, right, bottom);
			pc->SetPosRelativeToParent(rc);
		}
	};


	// --------------------------------------------------------------------------------
	class Command
	{
	public:
		Command()
		{
			Clear();
		}

		void Clear()
		{
			m_Children.clear();
			m_Distance.Clear();
			m_Assignment.clear();
			m_Orientation = None;
		}

		Identifier m_Distance;
		std::vector<Identifier> m_Assignment;// array of Identifier

		// if it's a parent node:
		Orientation m_Orientation;
		typedef std::vector<Command> ChildrenList;
		std::vector<ChildrenList> m_Children;// array of child blocks

		// used by the script runner
		Length start;
		Length end;

		std::wstring ToString(int indent) const
		{
			// construct a string of assignments
			std::vector<std::wstring> assignments;

			std::vector<Identifier>::const_iterator itIdentifier;
			for(itIdentifier = m_Assignment.begin(); itIdentifier != m_Assignment.end(); ++ itIdentifier)
			{
				assignments.push_back(itIdentifier->ToString());
			}

			std::wstring strAssignments = LibCC::StringJoin(assignments.begin(), assignments.end(), _T(", "));

			// orientation to string
			std::wstring orientation;
			switch(m_Orientation)
			{
			case Vertical:
				orientation = L"Vertical";
				break;
			case Horizontal:
				orientation = L"Horizontal";
				break;
			case None:
				orientation = L"None";
				break;
			default:
				orientation = L"unknown";
				break;
			}

			std::wstring sIndent(indent, L'\t');

			return LibCC::Format("orient[%]|%assign[%]|%distan[% start=% end=%]")
				.s(orientation)
				.s(sIndent)
				.s(strAssignments)
				.s(sIndent)
				.s(m_Distance.ToString())
				.s(start.ToString())
				.s(end.ToString())
				.Str();
		}
	};

	// --------------------------------------------------------------------------------
	class PreProcessor
	{
	public:
		PreProcessor()
		{
			start = -1;
			cursor = -1;
			line = 0;
			column = 0;
		}
		PreProcessor(const PreProcessor& rhs)
		{
			Assign(rhs);
		}
		void Assign(const PreProcessor& rhs)
		{
			start = rhs.start;
			line = rhs.line;
			column = rhs.column;
			str = rhs.str;
			cursor = rhs.cursor;
		}
		void SetString(const std::wstring& s)
		{
			str = s;
			start = 0;
			cursor = 0;
			line = 1;
			column = 1;
			AdvancePastComments();
		}
		void Clear()
		{
			str = L"";
			start = -1;
			cursor = -1;
			line = 0;
			column = 0;
		}

		const wchar_t* GetRawCursor() const { return str.c_str() + cursor; }

		bool eof() const
		{
			return (cursor == -1) || ((cursor + start) >= (int)str.length()); 
		}

		bool LessThan(const PreProcessor& rhs) const
		{
			if(rhs.eof()) return true;
			if(eof()) return false;
			if(rhs.start != start) return false;
			return cursor < rhs.cursor;
		}

		static PreProcessor LessOf(const PreProcessor& a, const PreProcessor& b)
		{
			return a.LessThan(b) ? a : b;
		}

		static PreProcessor LessOf(const PreProcessor& a, const PreProcessor& b, const PreProcessor& c)
		{
			return LessOf(LessOf(a, b), c);
		}

		wchar_t CurrentChar()
		{
			return cursor == -1 ? '?' : str[cursor];
		}

		wchar_t operator[](int i)
		{
			if(start == -1) return L'?';
			if(cursor == -1) return L'?';
			int resultingCursor = i + cursor + start;
			if(resultingCursor < 0 || resultingCursor >= (int)str.length()) return L'?';
			return str[resultingCursor];
		}

		long GetLine() { return line; }
		long GetColumn() { return column; }
		PreProcessor FindChar(wchar_t c)
		{
			PreProcessor temp(*this);
			while(!temp.eof())
			{
				if(temp[0] == c)
				{
					return temp;
				}
				temp.Advance();
			}
			return PreProcessor();// make sure this will be EOF = true
		}
		std::wstring Substring(const PreProcessor& end)
		{
			if(end.start != start) return L"";
			PreProcessor temp(*this);
			std::wstring r;

			while(!temp.eof() && (end.cursor > temp.cursor))
			{
				r.push_back(temp.CurrentChar());
				temp.Advance();
			}

			return r;
		}
		void Advance()
		{
			InternalAdvance();
			AdvancePastComments();
		}

	private:
		bool CursorStartsWith(const std::wstring& s)
		{
			int offset = start + cursor;
			return offset == str.find(s, offset);
		}
		void AdvancePastComments()
		{
			for(;;)
			{
				if(CursorStartsWith(L"//"))
				{
					// advance until a newline.
					long oldLine = line;
					while((!eof()) && (line == oldLine))
					{
						InternalAdvance();
					}
				}
				else if(CursorStartsWith(L"/*"))
				{
					while(!CursorStartsWith(L"*/"))
					{
						InternalAdvance();
					}
					// go past the terminator
					InternalAdvance();
					InternalAdvance();
				}
				else
				{
					break;
				}
			}
		}
		// adjusts line & column while advancing 1 char.
		void InternalAdvance()
		{
			// adjust new line
			if(CurrentChar() == L'\n')
			{
				line ++;
				column = 0;
			}

			if((start + cursor) < (int)str.length())
			{
				cursor ++;
				column ++;
			}

			if((start + cursor) < (int)str.length())
			{
				// skip \r characters altogether
				if(CurrentChar() == L'\r')
				{
					cursor ++;
				}
			}
		}

		int start;// position relative to str[0]
		int cursor;// position relative to start
		std::wstring str;
		long line;
		long column;
	};

	// --------------------------------------------------------------------------------
	class Manager :
		public IDebugLog
	{
	public:
		Manager()
		{
			m_settings.LoadDefaults();
		}

		// win32 windows would of course use RegisterSymbol(L"mycontrol", Win32Control(myhwnd));
		template<typename TControlInterface>
		void RegisterSymbol(const std::wstring& name, const TControlInterface& control)
		{
			if(m_symbols.find(name) != m_symbols.end())
			{
				// existing symbols will just be reused.
				m_symbols[name].SetControl(control);
			}
			else
			{
				m_symbols[LibCC::StringToLower(name)] = SymbolStorage(control);
			}
		}

		// only available for C#

		//void RegisterSymbolsAutomatically(System.Windows.Forms.Control parent)
		//{
		//  System.Reflection.MemberInfo[] members = parent.GetType().GetMembers(
		//    System.Reflection.BindingFlags.GetField |
		//    System.Reflection.BindingFlags.Instance |
		//    System.Reflection.BindingFlags.NonPublic |
		//    System.Reflection.BindingFlags.Public);

		//  foreach(System.Reflection.MemberInfo member in members)
		//  {
		//    // must be a field
		//    if(member.MemberType == System.Reflection.MemberTypes.Field)
		//    {
		//      System.Reflection.FieldInfo field = member as System.Reflection.FieldInfo;
		//      // must be a Control
		//      if(field.FieldType.IsSubclassOf(typeof(System.Windows.Forms.Control)))
		//      {
		//        // ok wire it up.
		//        Warning("Automatic symbol: " + field.Name);
		//        RegisterSymbol(field.Name, field.GetValue(parent) as System.Windows.Forms.Control);
		//      }
		//    }
		//  }
		//}

		void RegisterVariable(const std::wstring& name, double pixels)
		{
			Identifier i;
			i.m_numericValue.Clear();
			i.m_numericValue.pixels = pixels;
			i.m_Type = Identifier::Numeric;
			std::wstring nameLower = LibCC::StringToLower(name);
			m_variables[nameLower] = i;
		}

		// mgr.RegisterVariable(L"myvar", &someClassInstance, &SomeClass::SomeCallback);
		// where SomeCallback is like,   double SomeCallback(const std::wstring& identifierName); which simply returns the pixel dimension of the variable specified.
		template<typename Tobj, typename Tproc>
		void RegisterVariable(const std::wstring& name, Tobj callbackInstance, Tproc callbackClassMethod)
		{
			Identifier i;
			i.SetClassCallback(name, callbackInstance, callbackClassMethod);
			std::wstring nameLower = LibCC::StringToLower(name);
			m_variables[nameLower] = i;
		}

		// where SomeCallback is like,   double SomeCallback(const std::wstring& identifierName); which simply returns the pixel dimension of the variable specified.
		void RegisterVariable(const std::wstring& name, DimensionCallback::StaticCallbackType callback, void* userData)
		{
			Identifier i;
			i.SetStaticCallback(name, callback, userData);
			std::wstring nameLower = LibCC::StringToLower(name);
			m_variables[nameLower] = i;
		}
		void RegisterVariable(const std::wstring& name, const std::wstring& spec)
		{
			Identifier i;
			if(ParseIdentifier(spec, i))
			{
				std::wstring nameLower = LibCC::StringToLower(name);
				m_variables[nameLower] = i;
			}
		}

		bool RunResource(HINSTANCE hInstance, LPCTSTR szResName, LPCTSTR szResType, IGUIMetrics* m)
		{
			HRSRC hrsrc=FindResource(hInstance, szResName, szResType);
			if(!hrsrc) return L"";
			HGLOBAL hg1 = LoadResource(hInstance, hrsrc);
			DWORD sz = SizeofResource(hInstance, hrsrc);
			void* ptr1 = LockResource(hg1);

			// assume the encoding is ASCII.
			std::string a((const char*)ptr1, sz);
			return RunString(LibCC::ToUTF16(a), m);
		} 


		bool RunString(const std::wstring& script, IGUIMetrics* m/*, HWND parent*/)
		{
			bool r = false;
			m_root.Clear();
			m_output.clear();
			m_root.m_Children.push_back(Command::ChildrenList());// initialize a single child block.

			//m_parent = parent;

			// register all symbols automatically.
			//RegisterSymbolsAutomatically(parent);

			m_script.SetString(script);

			std::wstring tok = L"";
			while(true)
			{
				SkipWhitespaceAndNewlines();
				if(m_script.eof())
				{
					// end of file
					break;
				}
				Command This;
				ParseIdentifierPart(tok);
				if(m_script.eof())
				{
					Error(L"Expected global 'vertical' or 'horizontal' block.  EOF found while parsing the block.");
					break;
				}
				SkipWhitespaceAndNewlines();
				if(m_script.CurrentChar() != L'{')
				{
					Error(L"Expected '{', not found.");
					break;
				}

				m_script.Advance();// skip past the open brace.

				if(tok == L"vertical")
				{
					This.m_Orientation = Vertical;
					ParseChildBlock(This);
					m_root.m_Children[0].push_back(This);
				}
				else if(tok == L"horizontal")
				{
					This.m_Orientation = Horizontal;
					ParseChildBlock(This);
					m_root.m_Children[0].push_back(This);
				}
				else
				{
					Error(L"Unrecognized characters at global scope (expected 'vertical' or 'horizontal')");
				}

				// now skip past the semi-colon.  there should be NO cursor movement at this level.
				SkipWhitespaceAndNewlines();
				if(m_script.eof() || m_script.CurrentChar() != ';')
				{
					Error(LibCC::Format(L"Missing semi-colon after '%' block")(tok).Str());
				}
				if(!m_script.eof())
				{
					m_script.Advance();
				}
			}

			m_script.Clear();

			int indent = 0;

			// now interpret everything.
			r = Run(m);

			std::map<std::wstring, SymbolStorage>::const_iterator it;
			for(it = m_symbols.begin(); it != m_symbols.end(); ++it)
			{
				const SymbolStorage& ss = it->second;
				if(ss.optimizedPlacement.IsNull())
				{
					Warning(LibCC::Format(L"Symbol '%' was never assigned to. It will be ignored when resizing.")(it->first).Str());
				}
			}

			DumpNode(m_root, indent);

			//parent.Resize += new EventHandler(Internal_OnSize);

			return r;
		}

		bool RunFile(const std::wstring& fileName, IGUIMetrics* m)
		{
			bool r = false;
			m_output.clear();
			HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
			if(LibCC::IsValidHandle(hFile))
			{
				LibCC::Blob<char> contents;
				DWORD size = GetFileSize(hFile, 0);
				if(contents.Alloc(size + 1))
				{
					DWORD br;
					ReadFile(hFile, contents.GetBuffer(), size, &br, 0);
					contents.GetBuffer()[size] = 0;
					if(br == size)
					{
						std::wstring ws;
						LibCC::StringConvert(contents.GetBuffer(), ws);
						r = RunString(ws, m);
					}
				}

				CloseHandle(hFile);
			}
			return r;
		}

		bool OnSize(IGUIMetrics* m)
		{
			if(!m_symbols.size())
				return true;

			std::map<std::wstring, SymbolStorage>::const_iterator it;
			for(it = m_symbols.begin(); it != m_symbols.end(); ++it)
			{
				const SymbolStorage& ss(it->second);
				if(!ss.optimizedPlacement.IsNull() && (ss.pc != 0))
				{
					ss.optimizedPlacement->OnSize(ss.pc, m);
				}
			}
			return true;
		}

		void DumpOutput()
		{
			std::vector<Message>::const_iterator it;
			for(it = m_output.begin(); it != m_output.end(); ++ it)
			{
				OutputDebugString(LibCC::Format("%|").s(it->ToString()).CStr());
			}
		}

		static void DumpNode(Command n, int& indent)
		{
			std::vector<Command::ChildrenList>::const_iterator itBlocks;
			for(itBlocks = n.m_Children.begin(); itBlocks != n.m_Children.end(); ++ itBlocks)
			{
				OutputDebugString(LibCC::Format("%").c('\t', indent).CStr());
				OutputDebugString(n.ToString(indent).c_str());
				OutputDebugString(LibCC::Format("|%{|").c('\t', indent).CStr());
				indent ++;
				Command::ChildrenList::const_iterator it;
				for(it = itBlocks->begin(); it != itBlocks->end(); ++ it)
				{
					DumpNode(*it, indent);
				}
				indent --;
				OutputDebugString(LibCC::Format("%};|").c('\t', indent).CStr());
			}
		}

		std::vector<Message> m_output;// array of Message
		UISettings m_settings;

	private:
		void SkipWhitespaceAndNewlines()
		{
			while(!m_script.eof())
			{
				if(!IsWhitespace(m_script.CurrentChar()) && !IsNewline(m_script.CurrentChar()))
				{
					break;
				}
				m_script.Advance();
			}
		}

		bool ParseIdentifierPart(std::wstring& token)
		{
			SkipWhitespaceAndNewlines();
			token = L"";
			while(!m_script.eof())
			{
				if(!IsIdentifierChar(m_script.CurrentChar()))
				{
					break;
				}
				token += m_script.CurrentChar();
				m_script.Advance();
			}
			return token.length() != 0;
		}

		bool ParseIdentifier(const std::wstring& s_, Identifier& out_)
		{
			bool r = true;
			std::wstring s = LibCC::StringTrim(s_, L" \t\r\n");

			// determine if it's a value like '100%' or '15px', or if it's a real identifier.
			if(StartsWithNumber(s))// hmm this doesnt account for negative numbers or something like ".14". need to revise.
			{
				// parse a numeric identifier.
				std::wstring szSuffix = L"";
				double val = 0;
				if(!StringToDouble(s, val, szSuffix))
				{
					Error(LibCC::Format(L"Error parsing identifier '%'. The number format may be incorrect.")(s).Str());
				}

				out_.m_Type = Identifier::Numeric;

				std::wstring suffix = LibCC::StringToLower(LibCC::StringTrim(szSuffix, L" \t\r\n"));
				if(suffix == L"px" || suffix == L"pixel" || suffix == L"pixels" || suffix == L"")
				{
					out_.m_numericValue.pixels = val;
				}
				else if(suffix == L"%" || suffix == L"percent")
				{
					out_.m_numericValue._percent = val / 100.0;
				}
				else if(suffix == L"pt" || suffix == L"point" || suffix == L"points")
				{
					out_.m_numericValue.points = val;
				}
				else if(suffix == L"dlu" || suffix == L"dlus")
				{
					out_.m_numericValue.dlus = val;
				}
				else
				{
					// error: unknown numeric prefix.  expected something like 'px', '%', 'points'
					Error(LibCC::Format(L"Unknown units '%'").s(suffix).Str());
					r = false;
				}
			}
			else
			{
				// parse a symbolic identifier.
				out_.m_Type = Identifier::Symbolic;

				if(s == L"flex" || s == L"flexible")
				{
					out_.m_Type = Identifier::Flexible;
				}
				else if((s.length() > 0) && s[0] == '<')
				{
					// variable.
					out_.m_Type = Identifier::Variable;
					std::wstring::size_type n = s.find_first_of('>');
					if(n == std::wstring::npos)
					{
						Error(LibCC::Format(L"Malformed variable name... missing '>' on identifier '%'").s(s).Str());
					}
					else
					{
						out_.m_Parts.clear();
						// remove < > and trim
						std::wstring trimmed = LibCC::StringToLower(LibCC::StringTrim(s.substr(1, n - 1), L" \t\r\n"));

						if(StringContainsNotOf(trimmed, GetIdentifierChars()))
						{
							// error - unrecognized character in variable
							Error(LibCC::Format(L"Unrecognized character in variable '%'").s(trimmed).Str());
							r = false;
						}
						else
						{
							out_.m_Parts.push_back(trimmed);
						}
					}
				}
				else
				{
					// symbolic
					std::vector<std::wstring> Parts;
					LibCC::StringSplitByString(s, L".", std::back_inserter(Parts));

					std::vector<std::wstring>::iterator itPart;
					for(itPart = Parts.begin(); itPart != Parts.end(); ++ itPart)
					{
						// trim
						std::wstring x = LibCC::StringTrim(*itPart, L" \t\r\n");
						std::wstring trimmed = LibCC::StringToLower(x);
						trimmed = LibCC::StringReplace(trimmed, L"\r", L"");
						trimmed = LibCC::StringReplace(trimmed, L"\n", L"");

						// verify.  at this point we should have a part with nothing but identifier chars.
						if(StringContainsNotOf(trimmed, GetIdentifierChars()))
						{
							// error - unrecognized character in identifier
							Error(LibCC::Format("Unrecognized character in identifier '%'").s(trimmed).Str());
							r = false;
						}
						else
						{
							out_.m_Parts.push_back(trimmed);
						}
					}
				}
			}
			return r;
		}

		bool ParseIdentifierList(const std::wstring& sList, std::vector<Identifier>& out_)// out = Identifier list
		{
			bool r = true;
			std::vector<std::wstring> Identifiers;
			LibCC::StringSplitByString(sList, L",", std::back_inserter(Identifiers));

			std::vector<std::wstring>::iterator it;
			for(it = Identifiers.begin(); it != Identifiers.end(); ++ it)
			{
				Identifier x;
				if(ParseIdentifier(*it, x))
				{
					out_.push_back(x);
				}
				else
				{
					r = false;
				}
			}
			return r;
		}

		// --------------------------------------------------------------------------------
		// a command is in the form [* assignment list ] [{ children; }] [distance];
		bool ParseCommand(Command& This)
		{
			bool r = false;
			// part . part . part (something) ;
			bool bErrors = false;
			SkipWhitespaceAndNewlines();
			bool bHasAssignments = false;

			PreProcessor openbrace;
			PreProcessor closedbrace;
			PreProcessor semicolon;
			PreProcessor openparen;
			PreProcessor closedparen;

			if(m_script.CurrentChar() != '*' && m_script.CurrentChar() != '(' && m_script.CurrentChar() != '{')
			{
				Error(LibCC::Format(L"Unrecognized character in command.  Expected '(', '{', or '*'.  Make sure you prefix assignments with an asterisk.").s<10>(m_script.GetRawCursor()).Str());
				bErrors = true;
			}
			else
			{
				if(m_script.CurrentChar() == '*')
				{
					m_script.Advance();// ignore the asterisk; it's really not that important.
					SkipWhitespaceAndNewlines();

					// extract tokens
					std::wstring assignment;

					// it could be *assign{ or *assign( or *assign;
					openbrace = m_script.FindChar('{');
					semicolon = m_script.FindChar(';');
					openparen = m_script.FindChar('(');
					PreProcessor endOfAssignments = PreProcessor::LessOf(openbrace, openparen, semicolon);// in order of preference, 1) open brace, 2) open paren, 3) semicolon, whichever comes first & is not EOL.

					assignment = m_script.Substring(endOfAssignments);
					ParseIdentifierList(assignment, This.m_Assignment);
				}

				while(true)
				{
					openbrace = m_script.FindChar('{');
					closedbrace = openbrace.FindChar('}');
					semicolon = m_script.FindChar(';');
					openparen = m_script.FindChar('(');
					closedparen = m_script.FindChar(')');

					if((openbrace.LessThan(openparen)) && (openbrace.LessThan(semicolon)))
					{
						// we have ourselves a parent node.  parse it out before continuing.
						PreProcessor temp(m_script);
						m_script.Assign(openbrace);
						m_script.Advance();
						ParseChildBlock(This);

						semicolon = m_script.FindChar(';');
						openparen = m_script.FindChar('(');
						closedparen = m_script.FindChar(')');

						//m_script.Assign(temp);
					}
					else
					{
						break;
					}
				}

				if(!openparen.eof() && closedparen.eof())
				{
					Error(L"Syntax error while parsing a command.  Closed parenthesis not found.");
					bErrors = true;
				}
				if(semicolon.eof())
				{
					Error(L"Syntax error while parsing a command.  Semicolon not found.");
					bErrors = true;
				}

				std::wstring distance = L"";

				if(openparen.eof() || (closedparen.LessThan(openparen)) || (semicolon.LessThan(openparen)))
				{
					// no parenthesis.  this means no distance.
					distance = L"flex";
				}

				if(!bErrors)
				{
					if((0 == distance.length()) && !openparen.eof())
					{
						openparen.Advance();
						distance = openparen.Substring(closedparen);
					}
					r = ParseIdentifier(distance, This.m_Distance);

					m_script.Assign(semicolon);
					m_script.Advance();// advance past the semicolon.
				}
			}

			return r;
		}

		bool ParseChildBlock(Command& parent)
		{
			bool r = true;
			parent.m_Children.push_back(Command::ChildrenList());
			while(true)
			{
				Command This;
				This.m_Orientation = parent.m_Orientation;
				SkipWhitespaceAndNewlines();
				if(m_script.eof())
				{
					// error: no closing brace found.
					Error(L"No closing brace found on block.");
					r = false;
					break;
				}
				if(m_script.CurrentChar() == '}')
				{
					m_script.Advance();
					// clean break;
					break;
				}
				if(!ParseCommand(This))
				{
					r = false;
					break;
				}
				parent.m_Children.back().push_back(This);
			}
			return r;
		}

		bool Run(IGUIMetrics* m)
		{
			Stack2<Command*> st;// stack of Commands
			bool r = true;

			/*
				In order to deal properly with flex nodes, i need to first scan depth-first and fill in all distances and evaluate flex node distances.
			*/
			for(std::vector<Command>::iterator it = m_root.m_Children[0].begin(); it != m_root.m_Children[0].end(); ++it)
			{
				st.Push(&*it);
				it->start = Length::Begin();
				it->end = Length::End();
				ParseIdentifier(L"100%", it->m_Distance);
				r = EvaluateFlexNodes(st, m);
				st.Pop();
			}

			/*
				Now flatten the hierarchy.
			*/
			for(std::vector<Command>::iterator it = m_root.m_Children[0].begin(); it != m_root.m_Children[0].end(); ++it)
			{
				st.Push(&*it);
				r = Flatten(st);
				st.Pop();
			}

			return r;
		}

		bool EvaluateFlexNodes(Stack2<Command*>& st, IGUIMetrics* m)// stack of Commands
		{
			bool r = true;
			Command* node = st.Top();
			LibCC::LogScopeMessage l(LibCC::Format(L"Evaluating flex for ")(node->ToString(0)).Str());

			// run children first so we can be certain of their value to evaluate ourself
			for(std::vector<Command::ChildrenList>::iterator itBlock = node->m_Children.begin(); itBlock != node->m_Children.end(); ++itBlock)
			{
				for(std::vector<Command>::iterator itNode = itBlock->begin(); itNode != itBlock->end(); ++itNode)
				{
					st.Push(&*itNode);
					if(!EvaluateFlexNodes(st, m))
					{
						r = false;
					}
					st.Pop();
				}
			}

			if(!r)
				return r;

			if(node->m_Distance.m_Type == Identifier::Flexible && node->m_Children.size() == 0)
			{
				// this node is just a (flex); with no children. So there's nothing to do.
				return r;
			}

			Length maximumChildBlockDistance = Length::Begin();
			int nFlexibleNodesForAllChildren = 0;
			int nTotalNumberOfChildren = 0;

			for(std::vector<Command::ChildrenList>::iterator itBlock = node->m_Children.begin(); itBlock != node->m_Children.end(); ++itBlock)
			{
				nTotalNumberOfChildren += (int)itBlock->size();

				// count flex nodes and add up distance of children at the same time.
				int nFlexible = 0;
				Length totalDistance;
				for(std::vector<Command>::iterator it = itBlock->begin(); it != itBlock->end(); ++it)
				{
					if(it->m_Distance.m_Type == Identifier::Flexible)
					{
						nFlexible ++;
						nFlexibleNodesForAllChildren ++;
					}
					else
					{
						Length thisDistanceXX;
						if(!it->m_Distance.Evaluate(thisDistanceXX, m_settings, m_variables, it->m_Orientation, this))
						{
							Error(L"Error while advancing the internal cursor.");
							r = false;
						}
						totalDistance.Add(thisDistanceXX);
					}
				}

				Length flexDistance = Length::End();
				flexDistance.Subtract(totalDistance);
				if(nFlexible > 0)
				{
					flexDistance.Divide(nFlexible);
				}

				// walk through children again and set flex nodes correctly, and evaluate symbolic stuff.
				Length tempCursorStart = Length::Begin();
				Length tempCursorEnd;
				Length thisDistance;
				for(std::vector<Command>::iterator it = itBlock->begin(); it != itBlock->end(); ++it)
				{
					// calculate cursor start & end
					tempCursorEnd.Assign(tempCursorStart);
					if(it->m_Distance.m_Type == Identifier::Flexible)
					{
						thisDistance.Assign(flexDistance);
					}
					else
					{
						if(!it->m_Distance.Evaluate(thisDistance, m_settings, m_variables, it->m_Orientation, this))
						{
							Error(L"Error while advancing the internal cursor.");
							r = false;
						}
					}

					tempCursorEnd.Add(thisDistance);
					it->start.Assign(tempCursorStart);
					it->end.Assign(tempCursorEnd);
					tempCursorStart.Assign(tempCursorEnd);
				}


				if(maximumChildBlockDistance.EverythingButPercentToPixels(m, node->m_Orientation)
					< totalDistance.EverythingButPercentToPixels(m, node->m_Orientation))
				{
					maximumChildBlockDistance = totalDistance;
				}
			}

			if((node->m_Distance.m_Type == Identifier::Flexible) && (nFlexibleNodesForAllChildren == 0) && (nTotalNumberOfChildren > 0))
			{
				// we have no flex node children in this block. so our total distance should be the total distance of our children as already calculated.
				node->m_Distance.m_numericValue = maximumChildBlockDistance;
				node->m_Distance.m_Type = Identifier::Numeric;
			}

			return r;
		}

		bool Flatten(Stack2<Command*>& st)// stack of Commands
		{
			bool r = true;

			Command* node = st.Top();
			//LibCC::LogScopeMessage l(LibCC::Format(L"Running command %")(node->ToString(0)).Str());

			// ok now all children have start/end filled in.  do our own assignments
			if(!DoCommandAssignments(st))
			{
				Error(L"Error while assigning.");
			}
			else
			{
				// run children
				for(std::vector<Command::ChildrenList>::iterator itBlock = node->m_Children.begin(); itBlock != node->m_Children.end(); ++itBlock)
				{
					for(std::vector<Command>::iterator itNode = itBlock->begin(); itNode != itBlock->end(); ++itNode)
					{
						st.Push(&*itNode);
						if(!Flatten(st))
						{
							r = false;
						}
						st.Pop();
					}
				}
			}

			return r;
		}


		// --------------------------------------------------------------------------------
		// does assignments for the top of the stack.
		bool DoCommandAssignments(const Stack2<Command*>& st)
		{
			bool r = true;
			Command* thisCommand = st.Top();

			Length start = thisCommand->start;
			Length end = thisCommand->end;

			// flatten the dimensions by translating to parent iteratively up to the root
			Stack2<Command*>::const_iterator itParent = st.begin() + 1;
			while(itParent != st.end())// iterate from child towards parent
			{
				Command* parent = *itParent;

				Length oldStart = start;
				Length oldEnd = end;

				start.Flatten(parent->start, parent->end);
				end.Flatten(parent->start, parent->end);

				//LibCC::g_pLog->Message(LibCC::Format(
				//	L"Flatten from [%; %] with parent [%; %] results [%; %]")
				//	(oldStart.ToString()) (oldEnd.ToString())
				//	(parent->start.ToString()) (parent->end.ToString())
				//	(start.ToString()) (end.ToString())
				//	);
				++ itParent;
			}

			// build our placement spec that we will assign to everything in the assignment list.
			for(std::vector<Identifier>::const_iterator itAssignment = thisCommand->m_Assignment.begin(); itAssignment != thisCommand->m_Assignment.end(); ++ itAssignment)
			{
				const Identifier& i(*itAssignment);

				// find the identifier in the list.
				std::wstring Symbol = L"";
				std::wstring Member = L"";

				if(i.m_Type != Identifier::Symbolic)
				{
					Error(L"L-value expected. Can only assign values to symbols.");
					r = false;
					break;
				}

				switch(i.m_Parts.size())
				{
				case 1:
					Symbol = i.m_Parts[0];
					break;
				case 2:
					Symbol = i.m_Parts[0];
					Member = i.m_Parts[1];
					break;
				default:
					Error(L"Wrong number of identifier parts.");
					r = false;
					break;
				}
				if(!r) break;

				// find the symbol.
				std::map<std::wstring, SymbolStorage>::iterator itSymbol = m_symbols.find(Symbol);
				if(itSymbol == m_symbols.end())
				{
					// error: symbol '' not found.
					Warning(LibCC::Format(L"Symbol '%' not registered.").s(Symbol).Str());
					m_symbols[Symbol] = SymbolStorage();
					itSymbol = m_symbols.find(Symbol);
				}

				SymbolStorage& ss = itSymbol->second;

				if(ss.optimizedPlacement.IsNull())
				{
					ss.optimizedPlacement.Assign(OptimizedPlacement());
				}

				if(!DoCommandAssignment(thisCommand, start, end, ss.optimizedPlacement.Get(), Member))
				{
					// error propagating
					Error(LibCC::Format(L"Error while assigning to '%'.").s(Symbol).Str());
					r = false;
					break;
				}
			}

			return r;
		}

		// --------------------------------------------------------------------------------
		bool DoCommandAssignment(Command* pc, const Length& start, const Length& end, OptimizedPlacement* pPlacement, const std::wstring& member)
		{
			bool r = true;

			// figure out what parts will be set.
			if(member.length() == 0)
			{
				// figure it out based on orientation.
				if(pc->m_Orientation == Vertical)
				{
					pPlacement->m_VStart.Assign(start);
					pPlacement->m_VEnd.Assign(end);
				}
				else if(pc->m_Orientation == Horizontal)
				{
					pPlacement->m_HStart.Assign(start);
					pPlacement->m_HEnd.Assign(end);
				}
				else
				{
					Error(L"Unknown orientation.");
					r = false;
				}
			}
			else
			{
				// here, they have specified which member to set. valid are left, top, right, bottom.  it must match the current orientation.
				if(pc->m_Orientation == Vertical)
				{
					if(member == L"top")
					{
						pPlacement->m_VStart.Assign(start);
					}
					else if(member == L"bottom")
					{
						pPlacement->m_VEnd.Assign(end);
					}
					else if(member == L"left" || member == L"right")
					{
						Error(LibCC::Format("Mismatched orientation on member '%'.").s(member).Str());
						r = false;
					}
					else
					{
						Error(LibCC::Format("Unrecognized member '%'.").s(member).Str());
						r = false;
					}
				}
				else if(pc->m_Orientation == Horizontal)
				{
					if(member == L"left")
					{
						pPlacement->m_HStart.Assign(start);
					}
					else if(member == L"right")
					{
						pPlacement->m_HEnd.Assign(end);
					}
					else if(member == L"top" || member == L"bottom")
					{
						Error(LibCC::Format("Mismatched orientation on member '%'.").s(member).Str());
						r = false;
					}
					else
					{
						Error(LibCC::Format("Unrecognized member '%'.").s(member).Str());
						r = false;
					}
				}
				else
				{
					Error(L"Unknown orientation.");
					r = false;
				}
			}

			return r;
		}

		//void Internal_OnSize(object sender, System.EventArgs e)
		//{
		//  OnSize();
		//}
		void Warning(const std::wstring& msg)
		{
			Message m(msg, Message::Warning, m_script.GetLine(), m_script.GetColumn());
			m_output.push_back(m);
			OutputDebugString(LibCC::Format(L"RealTime: %|")(m.ToString()).CStr());
		}
		void Error(const std::wstring& msg)
		{
			Message m(msg, Message::Error, m_script.GetLine(), m_script.GetColumn());
			m_output.push_back(m);
			OutputDebugString(LibCC::Format(L"RealTime: %|")(m.ToString()).CStr());
		}

		static std::wstring GetIdentifierChars()
		{
			return L"abcdefghijklmnopqrstuvwxyz0123456789_";
		}
		static bool IsWhitespace(wchar_t x)
		{
			return (x == ' ') || (x == '\t');
		}
		static bool IsNewline(wchar_t x)
		{
			return (x == '\r') || (x == '\n');
		}
		static bool IsIdentifierChar(wchar_t x)
		{
			return std::wstring::npos != GetIdentifierChars().find(x);
		}

	public:
		Command m_root;
	private:
		//System.Windows.Forms.Control m_parent;

		// line numbers.
		PreProcessor m_script;

	public:
		class SymbolStorage
		{
		public:
			template<typename TControlInterface>
			SymbolStorage(const TControlInterface& ci) :
				m_pc(new TControlInterface(ci)),
				pc(m_pc)
			{
			}
			SymbolStorage() :
				m_pc(0),
				pc(m_pc)
			{
			}
			SymbolStorage(const SymbolStorage& rhs) :
				pc(m_pc),
				m_pc(0)
			{
				if(rhs.m_pc)
					m_pc = rhs.m_pc->Clone();
				optimizedPlacement = rhs.optimizedPlacement;
			}
			SymbolStorage& operator =(const SymbolStorage& rhs)
			{
				delete m_pc;
				m_pc = 0;
				if(rhs.m_pc)
					m_pc = rhs.m_pc->Clone();
				optimizedPlacement = rhs.optimizedPlacement;
				return *this;
			}
			~SymbolStorage()
			{
				delete m_pc;
			}

			template<typename TControlInterface>
			void SetControl(const TControlInterface& ci)
			{
				delete m_pc;
				m_pc = ci.Clone();
			}

			IControl* const& pc;
			Nullable<OptimizedPlacement> optimizedPlacement;

		private:
			IControl* m_pc;
		};

		const std::map<std::wstring, SymbolStorage>& GetSymbols() const { return m_symbols; }

	private:
		std::map<std::wstring, SymbolStorage> m_symbols;// maps string -> SymbolStorage
		std::map<std::wstring, Identifier> m_variables;// maps string -> Identifier
	};

	//--------------------------------------------------------------------------------
	// Win32-specifics
	class Win32Control :
		public IControl
	{
		HWND hwnd;
	public:
		Win32Control(HWND h) :
			hwnd(h)
		{
		}
		IControl* Clone() const { return new Win32Control(*this); }
		RECT GetParentsClientArea() const
		{
			RECT rc;
			GetClientRect(GetParent(hwnd), &rc);
			return rc;
		}
		void SetPosRelativeToParent(const RECT& rc)
		{
			MoveWindow(hwnd, rc.left, rc.top, Width(rc), Height(rc), TRUE);
		}
		RECT GetPosRelativeToParent() const
		{
			RECT rcParent;
			GetWindowRect(GetParent(hwnd), &rcParent);
			RECT rcThis;
			GetWindowRect(hwnd, &rcThis);
			RECT relativeToParent;
			relativeToParent.left = rcThis.left - rcParent.left;
			relativeToParent.top = rcThis.top - rcParent.top;
			relativeToParent.right = relativeToParent.left + Width(rcThis);
			relativeToParent.bottom = relativeToParent.top + Height(rcThis);
			return relativeToParent;
		}
	};

	//--------------------------------------------------------------------------------
	class Win32GUIMetrics :
		public IGUIMetrics
	{
	public:
		Win32GUIMetrics(HWND wnd = 0, HFONT font = 0)
		{
			HDC g = GetDC(wnd);

			// DPI
			DPI.cx = GetDeviceCaps(g, LOGPIXELSX);
			DPI.cy = GetDeviceCaps(g, LOGPIXELSY);

			// Dialog units
			if(font == 0)
			{
				font = static_cast<HFONT>(::GetStockObject(DEFAULT_GUI_FONT));
			}
			HGDIOBJ hOld = SelectObject(g, font);

			SIZE textSize;
			std::wstring s = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
			GetTextExtentPoint32(g, s.c_str(), (int)s.size(), &textSize);

			dialogBaseUnits.cx = (textSize.cx / 26 + 1) / 2;
			dialogBaseUnits.cy = textSize.cy;

			SelectObject(g, hOld);
			ReleaseDC(wnd, g);
		}

		SIZE dialogBaseUnits;
		SIZE DPI;

		double PointsToPixelsX(double pts)
		{
			return pts * DPI.cx / 72;
		}
		double PointsToPixelsY(double pts)
		{
			return pts * DPI.cy / 72;
		}
		double DLUsToPixelsX(double x)
		{
			double ret = x * dialogBaseUnits.cx / 4;
			return ret;
		}
		double DLUsToPixelsY(double y)
		{
			double ret = y * dialogBaseUnits.cy / 8;
			return ret;
		}
	};

}

