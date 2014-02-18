

#pragma once

#include <vector>

namespace UI
{
	class Event0
	{
		class IDelegate
		{
		public:
			virtual void Fire(class Control* origin) = 0;
			virtual bool IsEqual(IDelegate* rhs) = 0;
		};
		typedef std::tr1::shared_ptr<IDelegate> IDelegatePtr;
		typedef std::pair<bool, IDelegatePtr> Subscriber;// the bool is for "enabled". if a subscriber removes themself in the middle of an iteration, this gets set to false instead of erasing the item.

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
			void Fire(class Control* origin) { (m_obj->*m_proc)(origin); }
		};

		std::vector<Subscriber> m_items;
		mutable int m_iteratingRefCount;

		// no copy
		Event0(const Event0&) { }
		Event0& operator =(const Event0&) { }
	public:
		Event0() :
			m_iteratingRefCount(0)
		{ }

		void Fire(class Control* origin)
		{
			m_iteratingRefCount ++;
			for(std::vector<Subscriber>::const_iterator it = m_items.begin(); it != m_items.end(); ++ it)
			{
				const Subscriber& s = *it;
				if(s.first)
				{
					s.second->Fire(origin);
				}
			}
			m_iteratingRefCount --;
			Cleanup();
		}
		template<typename Tobj, typename Tproc>
		void Add(Tobj obj, Tproc proc)
		{
			std::vector<Subscriber>::iterator it = Find(obj, proc);
			if(it == m_items.end())
				m_items.push_back(Subscriber(true, IDelegatePtr(new Delegate<Tobj, Tproc>(obj, proc))));
			else
				it->first = true;
		}
		template<typename Tobj, typename Tproc>
		void Remove(Tobj obj, Tproc proc)
		{
			Delegate<Tobj, Tproc> temp(obj, proc);
			for(std::vector<Subscriber>::iterator it = m_items.begin(); it != m_items.end(); ++ it)
			{
				Subscriber& s = *it;
				if(s.second->IsEqual(&temp))
				{
					if(m_iteratingRefCount)
						s.first = false;
					else
						m_items.erase(it);
					break;
				}
			}
		}
		template<typename Tobj, typename Tproc>
		typename std::vector<Subscriber>::iterator Find(Tobj obj, Tproc proc)
		{
			m_iteratingRefCount ++;
			Delegate<Tobj, Tproc> temp(obj, proc);
			for(std::vector<Subscriber>::iterator it = m_items.begin(); it != m_items.end(); ++ it)
			{
				const Subscriber& s = *it;
				if(s.second->IsEqual(&temp))
				{
					m_iteratingRefCount --;
					return it;
				}
			}
			m_iteratingRefCount --;
			return m_items.end();
		}

		void Cleanup()
		{
			if(m_iteratingRefCount)
				return;
			for(std::vector<Subscriber>::iterator it = m_items.begin(); it != m_items.end(); )
			{
				const Subscriber& s = *it;
				if(!s.first)
				{
					it = m_items.erase(it);
				}
				else
				{
					++ it;
				}
			}
		}
	};




	template<typename Tparam>
	class Event1
	{
		class IDelegate
		{
		public:
			virtual void Fire(class Control* origin, Tparam param) = 0;
			virtual bool IsEqual(IDelegate* rhs) = 0;
		};
		typedef std::tr1::shared_ptr<IDelegate> IDelegatePtr;
		typedef std::pair<bool, IDelegatePtr> Subscriber;// the bool is for "enabled". if a subscriber removes themself in the middle of an iteration, this gets set to false instead of erasing the item.

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
			void Fire(class Control* origin, Tparam param) { (m_obj->*m_proc)(origin, param); }
		};

		std::vector<Subscriber> m_items;
		mutable int m_iteratingRefCount;

		// no copy
		Event1(const Event1<Tparam>&) { }
		Event1& operator =(const Event1<Tparam>&) { }
	public:
		Event1() :
			m_iteratingRefCount(0)
		{ }

		void Fire(class Control* origin, Tparam param)
		{
			m_iteratingRefCount ++;
			for(std::vector<Subscriber>::const_iterator it = m_items.begin(); it != m_items.end(); ++ it)
			{
				const Subscriber& s = *it;
				if(s.first)
				{
					s.second->Fire(origin, param);
				}
			}
			m_iteratingRefCount --;
			Cleanup();
		}
		template<typename Tobj, typename Tproc>
		void Add(Tobj obj, Tproc proc)
		{
			std::vector<Subscriber>::iterator it = Find(obj, proc);
			if(it == m_items.end())
				m_items.push_back(Subscriber(true, IDelegatePtr(new Delegate<Tobj, Tproc>(obj, proc))));
			else
				it->first = true;
		}
		template<typename Tobj, typename Tproc>
		void Remove(Tobj obj, Tproc proc)
		{
			Delegate<Tobj, Tproc> temp(obj, proc);
			for(std::vector<Subscriber>::iterator it = m_items.begin(); it != m_items.end(); ++ it)
			{
				Subscriber& s = *it;
				if(s.second->IsEqual(&temp))
				{
					if(m_iteratingRefCount)
						s.first = false;
					else
						m_items.erase(it);
					break;
				}
			}
		}
		template<typename Tobj, typename Tproc>
		typename std::vector<Subscriber>::iterator Find(Tobj obj, Tproc proc)
		{
			m_iteratingRefCount ++;
			Delegate<Tobj, Tproc> temp(obj, proc);
			for(std::vector<Subscriber>::iterator it = m_items.begin(); it != m_items.end(); ++ it)
			{
				const Subscriber& s = *it;
				if(s.second->IsEqual(&temp))
				{
					m_iteratingRefCount --;
					return it;
				}
			}
			m_iteratingRefCount --;
			return m_items.end();
		}

		void Cleanup()
		{
			if(m_iteratingRefCount)
				return;
			for(std::vector<Subscriber>::iterator it = m_items.begin(); it != m_items.end(); )
			{
				const Subscriber& s = *it;
				if(!s.first)
				{
					it = m_items.erase(it);
				}
				else
				{
					++ it;
				}
			}
		}
	};

}