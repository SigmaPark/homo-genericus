#pragma once


#ifndef _SGM_LAZY_
#define _SGM_LAZY_

#include <new>
#include "..\Type_Analysis\Type_Analysis.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	template<class F>
	struct Lazy
	{
	public:
		using type = Referenceless_t< decltype(Declval<F>()()) >;

	
		Lazy(F f) : _f(f), _is_evaluated(false), _p(nullptr){}

		Lazy(Lazy const& z) 
		:	_f(z._f), _is_evaluated(z._is_evaluated), _p( z ? new(_buf) type(*z._p) : nullptr )
		{}

		Lazy(Lazy&& z) noexcept 
		:	_f( Move(z._f) ), _is_evaluated(z._is_evaluated)
		,	_p(  z ? new(_buf) type( Move(*z._p) ) : nullptr  )
		{}

		~Lazy()
		{
			if(_p != nullptr)
				_p->~type(),  _p = nullptr;
			
			_is_evaluated = true;
		}


		void clear()
		{
			this->~Lazy();  

			for(auto& c : _buf)
				c = char(0);
		}

		decltype(auto) get() const{  return _deref(*this);  }
		decltype(auto) get(){  return _deref(*this);  }

		auto operator=(Lazy const&)-> Lazy& = delete;
		auto operator=(type const& t)-> Lazy&{  return( void(get() = t),  *this );  }
		auto operator=(type&& t)-> Lazy&{  return(  void( get() = Move(t) ),  *this  );  }
	
		decltype(auto) operator*() const{  return get();  }
		decltype(auto) operator*(){  return get();  }
		operator type&(){  return get();  }
		operator type const&() const{  return get();  }

		bool is_evaluated() const{  return _is_evaluated;  }
		bool is_destructed() const{  return is_evaluated() && _p == nullptr;  }
		operator bool() const{  return is_evaluated() && _p != nullptr;  }


	private:
		char mutable _buf[sizeof(type)] = {0, };
		F _f;
		bool mutable _is_evaluated;
		type mutable *_p;
		
	
		template<class Q>
		static decltype(auto) _deref(Q&& q)
		{
			if(!q.is_evaluated())
				q._p = new(q._buf) type(q._f()),  q._is_evaluated = true;
	
			return *q._p;
		}
	};	

}

#endif