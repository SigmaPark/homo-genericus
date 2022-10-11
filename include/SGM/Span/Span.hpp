/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_SPAN_
#define _SGM_SPAN_


#include <limits>
#include "../iterable/iterable.hpp"


namespace sgm
{

	struct SpanSize : Unconstructible
	{
		enum : size_t{INTERFACE = std::numeric_limits<size_t>::max(), DYNAMIC = 0};  
	};


	template<size_t S, class ITR>
	class Span_t;


	struct _Span_value_type_Helper;


	template
	<	class T1, class T2
	,	int
		=	is_iterator<T1>::value && is_iterator<T2>::value ? 1
		:	is_iterator<T1>::value && is_Convertible<T2, size_t>::value ? 2
		:	is_iterable<T1>::value && is_None<T2>::value ? 3 
		:	/* otherwise */ 0		
	>
	struct _Dynamic_Span_Helper;


	template
	<	class Q, size_t S
	,	int
		=	is_iterator<Q>::value ? 1
		:	is_iterable<Q>::value ? 2
		:	/* otherwise */ 0
	>
	struct _Static_Span_Helper;


	template<class T1, class T2, size_t SIZE, bool = SIZE == SpanSize::DYNAMIC>
	struct _Span_Helper;


	template<size_t SIZE = SpanSize::DYNAMIC, class T1, class T2 = None>
	static auto Span(T1&& t1, T2&& t2 = {})-> typename _Span_Helper<T1, T2, SIZE>::res_t;

}
//========//========//========//========//=======#//========//========//========//========//=======#


struct sgm::_Span_value_type_Helper : Unconstructible
{
private:
	SGM_HAS_NESTED_TYPE(value_type);

	template<  class ITR, bool = Has_NestedType_value_type< Decay_t<ITR> >::value  >
	struct _value_type;


public:
	template<class ITR>
	using type = typename _value_type<ITR>::value_type;
};


template<class ITR>
struct sgm::_Span_value_type_Helper::_value_type<ITR, true> : public ITR, Unconstructible{};

template<class ITR>
struct sgm::_Span_value_type_Helper::_value_type<ITR, false> 
:	Unconstructible{  using value_type = Decay_t< decltype(*Declval<ITR>()) >;  };
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class Q, std::size_t S, int ZERO>
struct sgm::_Static_Span_Helper
{
	static bool constexpr compile_failed_v = static_cast<bool>(ZERO);

	static_assert(compile_failed_v, "No method to create sgm::Span_t" );
};

template<class Q, std::size_t S>
struct sgm::_Static_Span_Helper<Q, S, 1> : Unconstructible
{
	using res_t = Span_t<S, Q>;

	static auto calc(Q bi, None = {})-> SGM_DECLTYPE_AUTO(  res_t(bi)  )
};

template<class Q, std::size_t S>
struct sgm::_Static_Span_Helper<Q, S, 2> : Unconstructible
{
	using res_t = Span_t< S, decltype( Begin(Declval<Q>()) ) >;
	
	template<class RG>
	static auto calc(RG&& rg, None = {})-> SGM_DECLTYPE_AUTO(  res_t( Begin(rg) )  )
};


template<class T1, class T2, int ZERO>
struct sgm::_Dynamic_Span_Helper
{
	static bool constexpr compile_failed_v = static_cast<bool>(ZERO);

	static_assert(compile_failed_v, "No method to create sgm::Span_t" );
};

template<class T1, class T2>
struct sgm::_Dynamic_Span_Helper<T1, T2, 1> : Unconstructible
{
	using res_t = Span_t<SpanSize::DYNAMIC, T1>;

	static auto calc(T1 bi, T2 ei)-> SGM_DECLTYPE_AUTO(  res_t(bi, ei)  )
};

template<class T1, class T2>
struct sgm::_Dynamic_Span_Helper<T1, T2, 2> : Unconstructible
{
	using res_t = Span_t<SpanSize::DYNAMIC, T1>;

	static auto calc(T1 bi, T2 s)-> SGM_DECLTYPE_AUTO(  res_t( bi, Next(bi, s) )  )
};

template<class T1, class T2>
struct sgm::_Dynamic_Span_Helper<T1, T2, 3> : Unconstructible
{
	using res_t = Span_t< SpanSize::DYNAMIC, decltype( Begin(Declval<T1>()) ) >;

	template<class RG>
	static auto calc(RG&& rg, T2)-> SGM_DECLTYPE_AUTO(  res_t( Begin(rg), End(rg) )  )
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T1, class T2>
struct sgm::_Span_Helper<T1, T2, sgm::SpanSize::DYNAMIC, true> : _Dynamic_Span_Helper<T1, T2>{};

template<class T, std::size_t SIZE>
struct sgm::_Span_Helper<T, sgm::None, SIZE, false> : _Static_Span_Helper<T, SIZE>{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<std::size_t S, class ITR>
class sgm::Span_t
{
protected:
	using _deref_t = decltype(*Declval<ITR>());


public:
	using value_type = typename _Span_value_type_Helper::type<ITR>;

	static size_t constexpr size_v = S;

	ITR _begin;


	Span_t(ITR const bi) : _begin(bi){}


	auto data() const-> ITR{  return _begin;  }

	auto constexpr size() const-> size_t{  return size_v;  }

	auto begin() const-> ITR{  return data();  }
	auto end() const-> ITR{  return Next(begin(), size());  }

	auto rbegin() const-> Reverse_iterator<ITR>{  return {Prev(end())};  }
	auto rend() const-> Reverse_iterator<ITR>{  return {Prev(begin())};  }

	auto front() const-> _deref_t const{  return *begin();  }
	auto front()-> _deref_t{  return *begin();  }

	auto back() const-> _deref_t const{  return *rbegin();  }
	auto back()-> _deref_t{  return *rbegin();  }


	auto operator[](size_t const idx) const-> _deref_t const
	{
		static_assert(is_random_access_iterator<ITR>::value, "");

		return begin()[idx];  
	}

	auto operator[](size_t const idx)-> _deref_t
	{
		static_assert(is_random_access_iterator<ITR>::value, "");

		return begin()[idx];  
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class ITR>
class sgm::Span_t<sgm::SpanSize::DYNAMIC, ITR> : public Span_t<SpanSize::INTERFACE, ITR>
{
private:
	using _base_t = Span_t<SpanSize::INTERFACE, ITR>;


public:
	Span_t() = delete;
	Span_t(Span_t const&) = default;

	auto size() const-> size_t{  return Difference(_base_t::begin(), end());  }

	auto end() const-> ITR{  return _end;  }
	auto rbegin() const-> Reverse_iterator<ITR>{  return {Prev(end())};  }

	auto back() const-> typename _base_t::_deref_t const{  return *rbegin();  }
	auto back()-> typename _base_t::_deref_t{  return *rbegin();  }


private:
	ITR _end;


	template<class T1, class T2, int>
	friend struct sgm::_Dynamic_Span_Helper;

	Span_t(ITR const bi, ITR const ei) : Span_t<SpanSize::INTERFACE, ITR>(bi),  _end(ei){}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<std::size_t SIZE, class T1, class T2>
auto sgm::Span(T1&& t1, T2&& t2)-> typename _Span_Helper<T1, T2, SIZE>::res_t
{
	return _Span_Helper<T1, T2, SIZE>::calc( Forward<T1>(t1), Forward<T2>(t2) );
}


#endif // end of #ifndef _SGM_SPAN_

