/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_AVATAR_
#define _SGM_AVATAR_


#include "Abbreviable.hpp"
#include "../Operators_of/Operators_of.hpp"
#include <cassert>


namespace sgm
{
	
	SGM_ABBREVIABLE_WIZARD(Avatar);


	template<class T>
	struct _Avatar_Ref_Op_Helper;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
struct sgm::_Avatar_Ref_Op_Helper
{
private:
	template<class Q, bool B>
	struct _type_with_v
	{
		using type = Q;
		static bool constexpr is_noexcept_v = B;
	};


	template<class Q>
	static auto _calc(int)
	->	SFINAE_t
		<	decltype(Declval<Q>().operator&())
		,	_type_with_v
			<	decltype(Declval<Q>().operator&())
			,	noexcept(Declval<Q>().operator&()) 
			>
		>;


	template<class Q>
	static auto _calc(...)-> _type_with_v<T*, true>;


	using res_t = decltype( _calc<T>(0) );

public:
	using type = typename res_t::type;

	static bool constexpr is_noexcept_v = res_t::is_noexcept_v;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, class M>
class sgm::Abbreviable_t
<	sgm::Avatar_tag
,	T, M, sgm::Avatar_Pedigree<T, M, false, false> 
>
:	public 
	Operators_of
	<	Selective_t
		<	is_constAvatar
			<	Avatar_t< T, M, sgm::Avatar_Pedigree<T, M, false, false> >
			>::	value
		,	T const
		,	T
		>
	>
{
public:
	static bool constexpr is_const_v = is_constAvatar<Abbreviable_t>::value;

	using element_t = Selective_t<is_const_v, T const, T>;

private:
	using _opof_type = Operators_of<element_t>;


public:
	template< class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	Abbreviable_t(Q& q) : _opof_type(q){}

	Abbreviable_t(T const&&) = delete;

	template<  class _M, class = Enable_if_t< is_const_v || !Avatar_t<T, _M>::is_const_v >  >
	Abbreviable_t(Avatar_t<T, _M> const& avt) : _opof_type( static_cast<T const&>(avt) ){}


	auto operator=(Abbreviable_t const& avt)-> Abbreviable_t&
	{
		static_assert(!is_const_v, "cannot bind to const Avatar_t");

		static_cast<T&>(*this) = avt.get();

		return *this;
	}


	template
	<	class Q
	,	class 
		=	Enable_if_t
			<	!is_const_v && !is_Avatar<Q>::value && is_Convertible<Q, T>::value 
			>  
	>
	auto operator=(Q&& q) noexcept(Aleph_Check<Q&&>::value)-> Abbreviable_t&
	{
		static_cast<T&>(*this) = Forward<Q>(q);

		return *this;
	}
	

	auto get() const noexcept
	->	element_t const&{  return *static_cast<element_t const*>(this->_p);  }

	auto get() noexcept
	->	element_t&{  return *static_cast<element_t*>(this->_p);  }


	auto operator&() const noexcept(_Avatar_Ref_Op_Helper<element_t const>::is_noexcept_v)
	->	typename _Avatar_Ref_Op_Helper<element_t const>::type{  return &get();  }

	auto operator&() noexcept(_Avatar_Ref_Op_Helper<element_t>::is_noexcept_v)
	->	typename _Avatar_Ref_Op_Helper<element_t>::type{  return &get();  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template<  class T, class = Enable_if_t< !is_Avatar<T>::value >  >
	static auto Refer(T& t)-> Avatar< Referenceless_t<T> >{  return t;  }

	template<  class T, class = Enable_if_t< is_Avatar<T>::value >  >
	static auto Refer(T t)-> Avatar<typename T::element_t>{  return t.get();  }

	template<class T>
	static auto CRefer(T&& t)-> SGM_DECLTYPE_AUTO(  Refer( immut(t) )  )

	template<  class T, class = Enable_if_t< !is_Avatar<T>::value >  >
	static auto Move(Avatar_t<T, Variable_t> avt)-> T&&{  return Move(avt.get());  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#




#endif // end of #ifndef _SGM_AVATAR_