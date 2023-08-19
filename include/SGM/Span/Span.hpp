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
		enum : size_t{DYNAMIC = 0, INTERFACE = std::numeric_limits<size_t>::max()};  
	};


	template<size_t S, class ITR>
	class Span_t;

}


namespace sgm
{
	namespace _span_detail
	{

		struct Span_value_type_Helper;


		template
		<	class T1, class T2
		,	int
			=	is_iterator< Decay_t<T1> >::value && is_iterator< Decay_t<T2> >::value ? 1
			:	is_iterator< Decay_t<T1> >::value && is_Convertible<T2, size_t>::value ? 2
			:	is_Bounded_Array< Decay_t<T1> >::value && is_None<T2>::value ? 3
			:	is_iterable< Decay_t<T1> >::value && is_None<T2>::value ? 4 
			:	/* otherwise */ 0		
		>
		struct Dynamic_Span_Helper;


		template
		<	class Q, size_t S
		,	int
			=	is_Bounded_Array< Decay_t<Q> >::value ? 1
			:	is_iterator< Decay_t<Q> >::value ? 2
			:	is_iterable< Decay_t<Q> >::value ? 3
			:	/* otherwise */ 0
		>
		struct Static_Span_Helper;


		template<class T1, class T2, size_t SIZE, bool = SIZE == SpanSize::DYNAMIC>
		struct Span_Helper;

	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


struct sgm::_span_detail::Span_value_type_Helper : Unconstructible
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
struct sgm::_span_detail::Span_value_type_Helper::_value_type<ITR, true>
:	public Decay_t<ITR>, Unconstructible{};


template<class ITR>
struct sgm::_span_detail::Span_value_type_Helper::_value_type<ITR, false>
:	Unconstructible{  using value_type = Decay_t< decltype(*Mock<ITR>()) >;  };
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class Q, std::size_t S, int ZERO>
struct sgm::_span_detail::Static_Span_Helper
{
	static bool constexpr Compile_failed_v = static_cast<bool>(ZERO);

	static_assert(Compile_failed_v, "No method to create sgm::Span_t" );
};

template<class Q, std::size_t S>
struct sgm::_span_detail::Static_Span_Helper<Q, S, 1> : Unconstructible
{
private:
	template<class A, std::size_t N>  /* Declaration Only */
	static auto constexpr _Try_arr_size( A(&)[N] ) noexcept
	->	As_value_itself< std::size_t, (N <= S ? N : S) >;

	static std::size_t constexpr _Size_v = decltype( _Try_arr_size(Mock<Q>()) )::value;


public:
	using res_t = Span_t<  _Size_v, Decay_t< decltype( Address_of(Mock<Q>()[0]) ) >  >;

	template< class ARR, bool _NXCT = nxct::is_Nxct_initialization<ARR, res_t>::value >
	static auto Calc(ARR arr, None = {}) noexcept(_NXCT)-> SGM_DECLTYPE_AUTO(  res_t(arr)  )
};

template<class Q, std::size_t S>
struct sgm::_span_detail::Static_Span_Helper<Q, S, 2> : Unconstructible
{
	using res_t = Span_t< S, Decay_t<Q> >;

	template< class ITR, bool _NXCT = nxct::is_Nxct_initialization<ITR const&, res_t>::value >
	static auto Calc(ITR const& bi, None = {}) noexcept(_NXCT)-> SGM_DECLTYPE_AUTO(  res_t(bi)  )
};

template<class Q, std::size_t S>
struct sgm::_span_detail::Static_Span_Helper<Q, S, 3> : Unconstructible
{
	using res_t = Span_t<  S, Decay_t< decltype( Begin(Mock<Q>()) ) >  >;
	
	template< class RG, bool _NXCT = noexcept(  res_t( Begin(Mock<RG&&>()) )  ) >
	static auto Calc(RG&& rg, None = {}) noexcept(_NXCT)
	->	SGM_DECLTYPE_AUTO(  res_t( Begin(rg) )  )
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T1, class T2, int ZERO>
struct sgm::_span_detail::Dynamic_Span_Helper
{
	static bool constexpr Compile_failed_v = static_cast<bool>(ZERO);

	static_assert(Compile_failed_v, "No method to create sgm::Span_t" );
};

template<class T1, class T2>
struct sgm::_span_detail::Dynamic_Span_Helper<T1, T2, 1> : Unconstructible
{
	using res_t = Span_t< SpanSize::DYNAMIC, Decay_t<T1> >;

	template< class ITR, bool _NXCT = noexcept( res_t(Mock<ITR const&>(), Mock<ITR const&>()) ) >
	static auto Calc(ITR const& bi, ITR const& ei) noexcept(_NXCT)
	->	SGM_DECLTYPE_AUTO(  res_t(bi, ei)  )
};

template<class T1, class T2>
struct sgm::_span_detail::Dynamic_Span_Helper<T1, T2, 2> : Unconstructible
{
	using res_t = Span_t< SpanSize::DYNAMIC, Decay_t<T1> >;

	template
	<	class ITR, class S
	,	bool _NXCT 
		=	noexcept
			(	res_t
				(	Mock<ITR const&>()
				,	Next(Mock<ITR const&>(), Mock<S const>())
				)
			)
	>
	static auto Calc(ITR const& bi, S const s) noexcept(_NXCT)
	->	SGM_DECLTYPE_AUTO(  res_t( bi, Next(bi, s) )  )
};

template<class T1, class T2>
struct sgm::_span_detail::Dynamic_Span_Helper<T1, T2, 3>
:	Static_Span_Helper< T1, SpanSize::INTERFACE, 1 >{};


template<class T1, class T2>
struct sgm::_span_detail::Dynamic_Span_Helper<T1, T2, 4> : Unconstructible
{
	using res_t = Span_t<  SpanSize::DYNAMIC, Decay_t< decltype( Begin(Mock<T1>()) ) >  >;

	template
	<	class RG, class _T2
	,	bool _NXCT = noexcept(  res_t( Begin(Mock<RG&&>()), End(Mock<RG&&>()) )  )
	>
	static auto Calc(RG&& rg, _T2) noexcept(_NXCT)
	->	SGM_DECLTYPE_AUTO(  res_t( Begin(rg), End(rg) )  )
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T1, class T2>
struct sgm::_span_detail::Span_Helper<T1, T2, sgm::SpanSize::DYNAMIC, true> 
:	Dynamic_Span_Helper<T1, T2>{};

template<class T, std::size_t SIZE>
struct sgm::_span_detail::Span_Helper<T, sgm::None, SIZE, false> 
:	Static_Span_Helper<T, SIZE>{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<std::size_t S, class ITR>
class sgm::Span_t
{
protected:
	using _deref_t = decltype(*Mock<ITR>());

	static bool constexpr _is_nxct_deref_v = noexcept(*Mock<ITR>());


public:
	using value_type = typename _span_detail::Span_value_type_Helper::type<ITR>;

	static size_t constexpr Size_v = S;

	ITR _begin;


	Span_t(ITR const& bi) noexcept(nxct::is_Nxct_initialization<ITR const&, ITR>::value) 
	:	_begin(bi){}
	
	Span_t(Span_t const&) noexcept(nxct::is_Nxct_initialization<ITR const&, ITR>::value) 
	=	default;
	
	Span_t(Span_t&&) noexcept(nxct::is_Nxct_initialization< Decay_t<ITR>&&, ITR >::value) 
	=	default;

	auto operator=(Span_t const&) noexcept(nxct::is_Nxct_Assignment<ITR const&, ITR&>::value)
	->	Span_t& = default;

	auto operator=(Span_t&&) noexcept(nxct::is_Nxct_Assignment< Decay_t<ITR>&&, ITR& >::value)
	->	Span_t& = default;


	auto data() const noexcept-> ITR const&{  return _begin;  }

	auto constexpr size() const noexcept-> size_t{  return Size_v;  }

	auto begin() const noexcept( noexcept(Mock<Span_t const>().data()) )
	->	ITR const&{  return data();  }
	
	auto end() const 
	noexcept(  noexcept( Next(Mock<Span_t const>().begin(), Mock<Span_t const>().size()) )  )
	->	ITR{  return Next(begin(), size());  }

	auto rbegin() const 
	noexcept(   noexcept(  Reverse_iterator<ITR>( Prev(Mock<Span_t const>().end()) )  )   )
	->	Reverse_iterator<ITR>{  return {Prev(end())};  }
	
	auto rend() const 
	noexcept(   noexcept(  Reverse_iterator<ITR>( Prev(Mock<Span_t const>().begin()) )  )   )
	->	Reverse_iterator<ITR>{  return {Prev(begin())};  }

	auto front() const noexcept( noexcept(Mock<Span_t const>().begin()) && _is_nxct_deref_v )
	->	_deref_t{  return *begin();  }
	
	auto back() const noexcept( noexcept(Mock<Span_t const>().rbegin()) && _is_nxct_deref_v )
	->	_deref_t{  return *rbegin();  }


	auto operator[](size_t const idx) const 
	noexcept( noexcept(Mock<Span_t const>().begin()) && _is_nxct_deref_v )
	->	_deref_t
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
	
	Span_t(Span_t const&) noexcept(nxct::is_Nxct_initialization<_base_t const&, _base_t>::value)
	=	default;
	
	Span_t(Span_t&&) noexcept(nxct::is_Nxct_initialization< Decay_t<_base_t>&&, _base_t >::value)
	=	default;

	auto operator=(Span_t const&)
	noexcept(nxct::is_Nxct_Assignment<_base_t const&, _base_t&>::value)
	->	Span_t& = default;

	auto operator=(Span_t&&) 
	noexcept(nxct::is_Nxct_Assignment< Decay_t<_base_t>&&, _base_t& >::value)
	->	Span_t& = default;


	using _base_t::begin;
	
	auto end() const noexcept-> ITR const&{  return _end;  }

	auto rbegin() const 
	noexcept(   noexcept(  Reverse_iterator<ITR>( Prev(Mock<Span_t const>().end()) )  )   )
	->	Reverse_iterator<ITR>{  return {Prev(end())};  }

	using _base_t::rend;


	auto size() const 
	noexcept
	(	noexcept( Difference(Mock<Span_t const>().begin(), Mock<Span_t const>().end()) )  
	)
	->	size_t{  return Difference(begin(), end());  }


	using _base_t::front;
	
	auto back() const 
	noexcept( noexcept(Mock<Span_t const>().rbegin()) && _base_t::_is_nxct_deref_v )
	->	typename _base_t::_deref_t{  return *rbegin();  }


private:
	ITR _end;


	template<class T1, class T2, int>
	friend struct sgm::_span_detail::Dynamic_Span_Helper;

	Span_t(ITR const& bi, ITR const& ei) 
	noexcept(nxct::is_Nxct_initialization<ITR const&, _base_t>::value)
	:	_base_t(bi), _end(ei){}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template
	<	size_t SIZE = SpanSize::DYNAMIC, class T1, class T2 = None
	,	class _SH = _span_detail::Span_Helper<T1, T2, SIZE>
	,	bool _NXCT = noexcept( _SH::Calc(Mock<T1&&>(), Mock<T2&&>()) )
	>
	static auto Span(T1&& t1, T2&& t2 = {}) noexcept(_NXCT)
	->	typename _SH::res_t{  return _SH::Calc( Forward<T1>(t1), Forward<T2>(t2) );  }

}


#endif // end of #ifndef _SGM_SPAN_

