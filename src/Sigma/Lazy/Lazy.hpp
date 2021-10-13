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
	using Lazy_value_t = typename _lazy_t::value_type;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class F>
struct sgm::Lazy
{
public:
	using value_type = Referenceless_t< decltype(Declval<F>()()) >;


	Lazy(F f) : _f(f), _is_evaluated(false){}

	Lazy(Lazy const &z) : _f(z._f), _is_evaluated(z.is_evaluated())
	{
		if(z.is_evaluated())
			new(_buf) value_type(*z);
	}

	Lazy(Lazy &&z) noexcept : _f( Move(z._f) ), _is_evaluated(z.is_evaluated())
	{
		if(z.is_evaluated())
			new(_buf) value_type( Move(*z) );
	}

	~Lazy()
	{
		if(is_evaluated())
			(**this).~value_type(),
			_is_evaluated = false;
	}


	auto get() const-> value_type const&{  return _deref(*this);  }
	auto get()-> value_type&{  return _deref(*this);  }

	bool is_evaluated() const{  return _is_evaluated;  }

	operator value_type&(){  return get();  }
	operator value_type const&() const{  return get();  }

	void operator=(Lazy const&) = delete;
	auto operator=(value_type const &t)-> Lazy&{  return void(get() = t),  *this;  }
	auto operator=(value_type &&t) noexcept-> Lazy&{  return void( get() = Move(t) ),  *this;  }

	auto operator*() const-> value_type const&{  return *reinterpret_cast<value_type*>(_buf);  }
	auto operator*()-> value_type&{  return *reinterpret_cast<value_type*>(_buf);  }


private:
	char mutable _buf[sizeof(value_type)] = {0, };
	F _f;
	bool mutable _is_evaluated;
	

	template<class ME>
	static auto _deref(ME &me)
	->	Selective_t< is_immutable<ME>::value, value_type const, value_type >&
	{
		if(!me.is_evaluated())
			new(me._buf) value_type(me._f()),  me._is_evaluated = true;

		return *me;
	}
};	


template<class F>
auto sgm::Make_Lazy(F &&f)-> Lazy< Referenceless_t<F> >{  return Forward<F>(f);  }


#endif // end of #ifndef _SGM_LAZY_
