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
	static auto Span(T1&& t1, T2&& t2 = {})
	noexcept(  noexcept( _Span_Helper<T1&&, T2&&, SIZE>::Calc(Mock<T1&&>(), Mock<T2&&>()) )  )
	->	typename _Span_Helper<T1&&, T2&&, SIZE>::res_t;

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
	static bool constexpr is_NXCT = true;

	static_assert(Compile_failed_v, "No method to create sgm::Span_t" );
};

template<class Q, std::size_t S>
struct sgm::_Static_Span_Helper<Q, S, 1> : Unconstructible
{
private:
	template<class A, std::size_t N>  /* Declaration Only */
	static auto constexpr _Try_arr_size( A(&)[N] ) noexcept
	->	As_value_itself< std::size_t, (N <= S ? N : S) >;

	static std::size_t constexpr Size_v = decltype( _Try_arr_size(Mock<Q>()) )::value;

public:
	using res_t = Span_t<  Size_v, Decay_t< decltype( Address_of(Mock<Q>()[0]) ) >  >;

	template<class ARR>
	static auto Calc(ARR arr, None = {}) noexcept-> SGM_DECLTYPE_AUTO(  res_t(arr)  )
};

template<class Q, std::size_t S>
struct sgm::_Static_Span_Helper<Q, S, 2> : Unconstructible
{
	using res_t = Span_t< S, Decay_t<Q> >;

	template<class ITR>
	static auto Calc(ITR const bi, None = {}) 
	noexcept(  noexcept( res_t(Mock<ITR const>()) )  )
	->	SGM_DECLTYPE_AUTO(  res_t(bi)  )
};

template<class Q, std::size_t S>
struct sgm::_Static_Span_Helper<Q, S, 3> : Unconstructible
{
	using res_t = Span_t<  S, Decay_t< decltype( fBegin<Q>(Mock<Q>()) ) >  >;
	
	template<class RG>
	static auto Calc(RG&& rg, None = {})
	noexcept(   noexcept(  res_t( fBegin<RG>(Mock<RG&&>()) )  )   )
	->	SGM_DECLTYPE_AUTO(   res_t(  fBegin<RG>( Forward<RG>(rg) )  )   )
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T1, class T2, int ZERO>
struct sgm::_Dynamic_Span_Helper
{
	static bool constexpr Compile_failed_v = static_cast<bool>(ZERO);
	static bool constexpr is_NXCT = true;

	static_assert(Compile_failed_v, "No method to create sgm::Span_t" );
};

template<class T1, class T2>
struct sgm::_Dynamic_Span_Helper<T1, T2, 1> : Unconstructible
{
	using res_t = Span_t< SpanSize::DYNAMIC, Decay_t<T1> >;

	template<class ITR>
	static auto Calc(ITR const bi, ITR const ei)
	noexcept(  noexcept( res_t(Mock<ITR const>(), Mock<ITR const>()) )  )
	->	SGM_DECLTYPE_AUTO(  res_t(bi, ei)  )
};

template<class T1, class T2>
struct sgm::_Dynamic_Span_Helper<T1, T2, 2> : Unconstructible
{
	using res_t = Span_t< SpanSize::DYNAMIC, Decay_t<T1> >;

	template<class ITR, class S>
	static auto Calc(ITR const bi, S const s)
	noexcept
	(	noexcept
		(	res_t
			(	Mock<ITR const>()
			,	Next(Mock<ITR const>(), Mock<S const>())
			)
		)
	)->	SGM_DECLTYPE_AUTO(  res_t( bi, Next(bi, s) )  )
};

template<class T1, class T2>
struct sgm::_Dynamic_Span_Helper<T1, T2, 3>
:	_Static_Span_Helper< T1, std::numeric_limits<std::size_t>::max(), 1 >{};


template<class T1, class T2>
struct sgm::_Dynamic_Span_Helper<T1, T2, 4> : Unconstructible
{
	using res_t 
	=	Span_t<  SpanSize::DYNAMIC, Decay_t< decltype( fBegin<T1>(Mock<T1&&>()) ) >  >;

	template<class RG, class _T2>
	static auto Calc(RG&& rg, _T2)
	noexcept(   noexcept(  res_t( fBegin<RG>(Mock<RG&&>()), fEnd<RG>(Mock<RG&&>()) )  )   )
	->	SGM_DECLTYPE_AUTO
	(
		res_t(  fBegin<RG>( Forward<RG>(rg) ), fEnd<RG>( Forward<RG>(rg) )  )  
	)
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T1, class T2>
struct sgm::_Span_Helper<T1, T2, sgm::SpanSize::DYNAMIC, true> : _Dynamic_Span_Helper<T1, T2>{};

template<class T1, class T2, std::size_t SIZE>
struct sgm::_Span_Helper<T1, T2, SIZE, false> : _Static_Span_Helper<T1, SIZE>
{
	static_assert(is_None<T2>::value, "");
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<std::size_t S, class ITR>
class sgm::Span_t
{
protected:
	using _deref_t = decltype(*Mock<ITR>());

	static bool constexpr _is_NXCT_copyable = noexcept( ITR(Mock<ITR>()) );

public:
	using value_type = typename _Span_value_type_Helper::type<ITR>;

	static size_t constexpr Size_v = S;

	ITR _begin;


	Span_t(ITR const bi) noexcept(_is_NXCT_copyable) : _begin(bi){}

	auto data() const noexcept(_is_NXCT_copyable)-> ITR{  return _begin;  }

	auto constexpr size() const noexcept-> size_t{  return Size_v;  }

	auto begin() const noexcept(_is_NXCT_copyable)-> ITR{  return data();  }

	auto end() const noexcept(  noexcept( Next(Span_t::begin(), Span_t::size()) )  )
	->	ITR{  return Next(begin(), size());  }

	auto rbegin() const 
	noexcept
	(	noexcept( Prev(Span_t::end()) )
	&&	noexcept( Reverse_iterator<ITR>(Mock<ITR const>()) )  
	)->	Reverse_iterator<ITR>{  return {Prev(end())};  }
	
	auto rend() const
	noexcept
	(	noexcept( Prev(Span_t::begin()) )
	&&	noexcept( Reverse_iterator<ITR>(Mock<ITR const>()) )  
	)->	Reverse_iterator<ITR>{  return {Prev(begin())};  }

	auto front() const noexcept( noexcept(*Span_t::begin()) )-> _deref_t{  return *begin();  }

	auto back() const noexcept( noexcept(*Span_t::rbegin()) )-> _deref_t{  return *rbegin();  }


	auto operator[](size_t const idx) const noexcept( noexcept(Span_t::begin()[0]) )-> _deref_t
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
	using _base_t::_is_NXCT_copyable;

public:
	Span_t() = delete;
	Span_t(Span_t const&) = default;

	auto end() const noexcept(_is_NXCT_copyable)-> ITR{  return _end;  }

	auto size() const noexcept(  noexcept( Difference(_base_t::begin(), Span_t::end()) )  )
	->	size_t{  return Difference(_base_t::begin(), end());  }

	auto rbegin() const	
	noexcept
	(	noexcept( Prev(Span_t::end()) )
	&&	noexcept( Reverse_iterator<ITR>(Mock<ITR const>()) )  
	)->	Reverse_iterator<ITR>{  return {Prev(end())};  }

	auto back() const noexcept( noexcept(*Span_t::rbegin()) )
	->	typename _base_t::_deref_t{  return *rbegin();  }

private:
	ITR _end;

	template<class T1, class T2, int>
	friend struct sgm::_Dynamic_Span_Helper;

	Span_t(ITR const bi, ITR const ei) noexcept(_is_NXCT_copyable)
	:	Span_t<SpanSize::INTERFACE, ITR>(bi),  _end(ei){}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<std::size_t SIZE, class T1, class T2>
auto sgm::Span(T1&& t1, T2&& t2)
noexcept(  noexcept( _Span_Helper<T1&&, T2&&, SIZE>::Calc(Mock<T1&&>(), Mock<T2&&>()) )  )
->	typename _Span_Helper<T1&&, T2&&, SIZE>::res_t
{
	return _Span_Helper<T1&&, T2&&, SIZE>::Calc( Forward<T1>(t1), Forward<T2>(t2) );
}


#endif // end of #ifndef _SGM_SPAN_

