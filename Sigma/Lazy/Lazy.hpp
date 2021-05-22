#pragma once


#ifndef _SGM_LAZY_
#define _SGM_LAZY_

#include "..\Type_Analysis\Type_Analysis.hpp"
#include <new>
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	template<class F>
	struct Lazy;	

	SGM_USER_DEFINED_TYPE_CHECK(_, Lazy, class F, F);


	template<class F>
	static auto Make_Lazy(F&& f)-> Lazy< Referenceless_t<F> >;

	template
	<	class LAZY, class _lazy_t = Referenceless_t<LAZY>
	,	class = Guaranteed_t< is_Lazy<_lazy_t>::value >
	>
	using Lazy_value_t = typename _lazy_t::type;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class F>
struct sgm::Lazy
{
public:
	using type = Referenceless_t< decltype(Declval<F>()()) >;


	Lazy(F f) : _f(f), _yet(true), _p(nullptr){}

	Lazy(Lazy const& z) 
	:	_f(z._f), _yet(z.has_yet())
	,	_p( z.is_evaluated() ? new(_buf) type(*z._p) : nullptr )
	{}

	Lazy(Lazy&& z) noexcept
	:	_f( Move(z._f) ), _yet(z.has_yet())
	,	_p(  z.is_evaluated() ? new(_buf) type( Move(*z._p) ) : nullptr  )
	{}

	~Lazy()
	{
		if(_p != nullptr)
			_p->~type(),  _p = nullptr;
		
		_yet = false;
	}


	void clear()
	{
		this->~Lazy();

		for(auto& c : _buf)
			c = char(0);
	}

	auto get() const-> type const&{  return _deref(*this);  }
	auto get()-> type&{  return _deref(*this);  }

	void operator=(Lazy const&) = delete;
	auto operator=(type const& t)-> Lazy&{  return void(get() = t),  *this;  }
	auto operator=(type&& t) noexcept-> Lazy&{  return void( get() = Move(t) ),  *this;  }

	auto operator*() const-> type const&{  return get();  }
	auto operator*()-> type&{  return get();  }
	operator type&(){  return get();  }
	operator type const&() const{  return get();  }

	bool has_yet() const{  return _yet;  }
	bool is_destructed() const{  return !has_yet() && _p == nullptr;  }
	bool is_evaluated() const{  return !has_yet() && _p != nullptr;  }


private:
	char mutable _buf[sizeof(type)] = {0, };
	F _f;
	bool mutable _yet;
	type mutable *_p;
	

	template<class Z>
	static auto _deref(Z&& z)-> decltype(*z._p)
	{
		if(z.has_yet())
			z._p = new(z._buf) type(z._f()),  z._yet = false;

		return *z._p;
	}
};	


template<class F>
auto sgm::Make_Lazy(F&& f)-> Lazy< Referenceless_t<F> >{  return Forward<F>(f);  }


#endif // end of #ifndef _SGM_LAZY_
