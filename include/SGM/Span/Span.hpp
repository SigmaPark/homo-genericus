/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_SPAN_
#define _SGM_SPAN_


#include <limits>
#include "SGM/iterable/iterable.hpp"


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
		=	is_iterator< Decay_t<T1> >::value && is_iterator< Decay_t<T2> >::value ? 1
		:	is_iterator< Decay_t<T1> >::value && is_Convertible<T2, size_t>::value ? 2
		:	is_Bounded_Array< Decay_t<T1> >::value && is_None<T2>::value ? 3
		:	is_iterable< Decay_t<T1> >::value && is_None<T2>::value ? 4 
		:	/* otherwise */ 0		
	>
	struct _Dynamic_Span_Helper;


	template
	<	class Q, size_t S
	,	int
		=	is_Bounded_Array< Decay_t<Q> >::value ? 1
		:	is_iterator< Decay_t<Q> >::value ? 2
		:	is_iterable< Decay_t<Q> >::value ? 3
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
struct sgm::_Span_value_type_Helper::_value_type<ITR, true> 
:	public Decay_t<ITR>, Unconstructible{};


template<class ITR>
struct sgm::_Span_value_type_Helper::_value_type<ITR, false> 
:	Unconstructible{  using value_type = Decay_t< decltype(*Mock<ITR>()) >;  };
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class Q, std::size_t S, int ZERO>
struct sgm::_Static_Span_Helper
{
	static bool constexpr Compile_failed_v = static_cast<bool>(ZERO);

	static_assert(Compile_failed_v, "No method to create sgm::Span_t" );
};

template<class Q, std::size_t S>
struct sgm::_Static_Span_Helper<Q, S, 1> : Unconstructible
{
private:
	template<class A, std::size_t N>  /* Declaration Only */
	static auto constexpr _Try_arr_size( A(&)[N] ) 
	->	As_value_itself< std::size_t, (N <= S ? N : S) >;

	static std::size_t constexpr Size_v = decltype( _Try_arr_size(Mock<Q>()) )::value;


public:
	using res_t = Span_t<  Size_v, Decay_t< decltype( Address_of(Mock<Q>()[0]) ) >  >;

	template<class ARR>
	static auto Calc(ARR arr, None = {})-> SGM_DECLTYPE_AUTO(  res_t(arr)  )
};

template<class Q, std::size_t S>
struct sgm::_Static_Span_Helper<Q, S, 2> : Unconstructible
{
	using res_t = Span_t< S, Decay_t<Q> >;

	template<class ITR>
	static auto Calc(ITR const bi, None = {})-> SGM_DECLTYPE_AUTO(  res_t(bi)  )
};

template<class Q, std::size_t S>
struct sgm::_Static_Span_Helper<Q, S, 3> : Unconstructible
{
	using res_t = Span_t<  S, Decay_t< decltype( Begin(Mock<Q>()) ) >  >;
	
	template<class RG>
	static auto Calc(RG&& rg, None = {})-> SGM_DECLTYPE_AUTO(  res_t( Begin(rg) )  )
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T1, class T2, int ZERO>
struct sgm::_Dynamic_Span_Helper
{
	static bool constexpr Compile_failed_v = static_cast<bool>(ZERO);

	static_assert(Compile_failed_v, "No method to create sgm::Span_t" );
};

template<class T1, class T2>
struct sgm::_Dynamic_Span_Helper<T1, T2, 1> : Unconstructible
{
	using res_t = Span_t< SpanSize::DYNAMIC, Decay_t<T1> >;

	template<class ITR>
	static auto Calc(ITR const bi, ITR const ei)-> SGM_DECLTYPE_AUTO(  res_t(bi, ei)  )
};

template<class T1, class T2>
struct sgm::_Dynamic_Span_Helper<T1, T2, 2> : Unconstructible
{
	using res_t = Span_t< SpanSize::DYNAMIC, Decay_t<T1> >;

	template<class ITR, class S>
	static auto Calc(ITR const bi, S const s)
	->	SGM_DECLTYPE_AUTO(  res_t( bi, Next(bi, s) )  )
};

template<class T1, class T2>
struct sgm::_Dynamic_Span_Helper<T1, T2, 3>
:	_Static_Span_Helper< T1, std::numeric_limits<std::size_t>::max(), 1 >{};


template<class T1, class T2>
struct sgm::_Dynamic_Span_Helper<T1, T2, 4> : Unconstructible
{
	using res_t 
	=	Span_t<  SpanSize::DYNAMIC, Decay_t< decltype( Begin(Mock<T1>()) ) >  >;

	template<class RG, class _T2>
	static auto Calc(RG&& rg, _T2)-> SGM_DECLTYPE_AUTO(  res_t( Begin(rg), End(rg) )  )
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
	using _deref_t = decltype(*Mock<ITR>());


public:
	using value_type = typename _Span_value_type_Helper::type<ITR>;

	static size_t constexpr Size_v = S;

	ITR _begin;


	Span_t(ITR const bi) : _begin(bi){}


	auto data() const-> ITR{  return _begin;  }

	auto constexpr size() const-> size_t{  return Size_v;  }

	auto begin() const-> ITR{  return data();  }
	auto end() const-> ITR{  return Next(begin(), size());  }

	auto rbegin() const-> Reverse_iterator<ITR>{  return {Prev(end())};  }
	auto rend() const-> Reverse_iterator<ITR>{  return {Prev(begin())};  }

	auto front() const-> _deref_t{  return *begin();  }
	auto back() const-> _deref_t{  return *rbegin();  }


	auto operator[](size_t const idx) const-> _deref_t
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

	auto back() const-> typename _base_t::_deref_t{  return *rbegin();  }


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
	return _Span_Helper<T1, T2, SIZE>::Calc( Forward<T1>(t1), Forward<T2>(t2) );
}


#endif // end of #ifndef _SGM_SPAN_

