/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_SPAN_
#define _SGM_SPAN_


#include "../iterable/iterable.hpp"


namespace sgm
{

	struct SpanSize
	:	Unconstructible{  enum : size_t{INTERFACE = MAXIMUM_VALUE_OF_PTRDIFF_TYPE, DYNAMIC = 0};  };


	template<class T, size_t S = SpanSize::DYNAMIC, class ITR = T*>
	class Span;


	struct _Span_value_type_Helper;


	template
	<	class T1, class T2
	,	int
		=	is_iterator<T1>::value && is_iterator<T2>::value ? 1
		:	is_iterator<T1>::value && is_Convertible<T2, size_t>::value ? 2
		:	is_iterable<T1>::value && is_None<T2>::value ? 3 
		:	/* otherwise */ 0		
	>
	struct _Dynamic_Span_by_Helper;


	template
	<	class Q, size_t S
	,	int
		=	is_iterator<Q>::value ? 1
		:	is_iterable<Q>::value ? 2
		:	/* otherwise */ 0
	>
	struct _Static_Span_by_Helper;

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
struct sgm::_Span_value_type_Helper::_value_type<ITR, false> : Unconstructible
{
	using value_type = Decay_t< decltype(*Declval<ITR>()) >;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class Q, std::size_t S, int ZERO>
struct sgm::_Static_Span_by_Helper
{
	static bool constexpr compile_failed_v = static_cast<bool>(ZERO);

	static_assert(compile_failed_v, "No method to create sgm::Span" );
};

template<class Q, std::size_t S>
struct sgm::_Static_Span_by_Helper<Q, S, 1> : Unconstructible
{
private:
	using _value_type = Referenceless_t< decltype(*Declval<Q>()) >;

public:
	using res_t = Span<_value_type, S, Q>;

	static auto calc(Q itr)-> SGM_DECLTYPE_AUTO(  res_t(itr)  )
};

template<class Q, std::size_t S>
struct sgm::_Static_Span_by_Helper<Q, S, 2> : Unconstructible
{
private:
	using _itr_t = decltype( Begin(Declval<Q>()) );
	using _value_type = Referenceless_t< decltype( *Declval<_itr_t>() ) >;

public:
	using res_t = Span<_value_type, S, _itr_t>;
	
	template<class RG>
	static auto calc(RG&& rg)-> SGM_DECLTYPE_AUTO(  res_t( Begin(rg) )  )
};


template<class T1, class T2, int ZERO>
struct sgm::_Dynamic_Span_by_Helper
{
	static bool constexpr compile_failed_v = static_cast<bool>(ZERO);

	static_assert(compile_failed_v, "No method to create sgm::Span" );
};

template<class T1, class T2>
struct sgm::_Dynamic_Span_by_Helper<T1, T2, 1> : Unconstructible
{
private:
	using _value_type = Referenceless_t< decltype(*Declval<T1>()) >;

public:
	using res_t = Span<_value_type, SpanSize::DYNAMIC, T1>;

	static auto calc(T1 itr1, T2 itr2)-> SGM_DECLTYPE_AUTO
	(
		res_t(  itr1, static_cast<size_t>( Difference(itr1, itr2) )  )
	)
};

template<class T1, class T2>
struct sgm::_Dynamic_Span_by_Helper<T1, T2, 2> : Unconstructible
{
private:
	using _value_type = Referenceless_t< decltype(*Declval<T1>()) >;

public:
	using res_t = Span<_value_type, SpanSize::DYNAMIC, T1>;

	static auto calc(T1 p, T2 s)-> SGM_DECLTYPE_AUTO
	(
		res_t( p, static_cast<size_t>(s) )
	)
};

template<class T1, class T2>
struct sgm::_Dynamic_Span_by_Helper<T1, T2, 3> : Unconstructible
{
private:
	using _itr_t = decltype( Begin(Declval<T1>()) );
	using _value_type = Referenceless_t< decltype( *Declval<_itr_t>() ) >;

public:
	using res_t = Span<_value_type, SpanSize::DYNAMIC, _itr_t>;

	template<class RG>
	static auto calc(RG&& rg, T2)-> SGM_DECLTYPE_AUTO
	(
		res_t( Begin(rg), Size(rg) )
	)
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, std::size_t S, class ITR>
class sgm::Span
{
protected:
	using _deref_t = decltype(*Declval<ITR>());


public:
	using value_type = typename _Span_value_type_Helper::type<ITR>;

	static size_t constexpr size_v = S;

	ITR _start;


	Span(ITR const p) : _start(p){}

	template<class Q>
	static auto by(Q&& q)-> typename _Static_Span_by_Helper<Q, S>::res_t
	{
		return _Static_Span_by_Helper<Q, S>::calc( Forward<Q>(q) );
	}


	auto data() const-> ITR{  return _start;  }

	auto size() const-> size_t{  return size_v;  }

	auto begin() const-> ITR{  return data();  }
	auto end() const-> ITR{  return begin() + size();  }

	auto rbegin() const-> Reverse_iterator<ITR>{  return {Prev(end())};  }
	auto rend() const-> Reverse_iterator<ITR>{  return {Prev(begin())};  }

	auto operator[](size_t const idx) const-> _deref_t const{  return *(begin() + idx);  }
	auto operator[](size_t const idx)-> _deref_t{  return *(begin() + idx);  }

	auto front() const-> _deref_t const{  return *begin();  }
	auto front()-> _deref_t{  return *begin();  }

	auto back() const-> _deref_t const{  return *rbegin();  }
	auto back()-> _deref_t{  return *rbegin();  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, class ITR>
class sgm::Span<T, sgm::SpanSize::DYNAMIC, ITR> : public Span<T, SpanSize::INTERFACE, ITR>
{
private:
	using _base_t = Span<T, SpanSize::INTERFACE, ITR>;


public:
	Span() = delete;
	Span(Span const&) = default;

	auto size() const-> size_t{  return _size;  }

	auto end() const-> ITR{  return _base_t::begin() + size();  }
	auto rbegin() const-> Reverse_iterator<ITR>{  return {Prev(end())};  }

	auto back() const-> typename _base_t::_deref_t const{  return *rbegin();  }
	auto back()-> typename _base_t::_deref_t{  return *rbegin();  }

private:
	size_t _size;


	template<class T1, class T2, int>
	friend struct sgm::_Dynamic_Span_by_Helper;


	Span(ITR const p, size_t const s) : Span<T, SpanSize::INTERFACE, ITR>(p),  _size(s){}


public:
	template<class T1, class T2 = None>
	static auto by(T1&& t1, T2&& t2= {})-> typename _Dynamic_Span_by_Helper<T1, T2>::res_t
	{
		return _Dynamic_Span_by_Helper<T1, T2>::calc( Forward<T1>(t1), Forward<T2>(t2) );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_SPAN_

