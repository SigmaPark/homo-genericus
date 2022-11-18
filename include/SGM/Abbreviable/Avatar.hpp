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
	template<class Q>
	static auto _calc(int)
	->	SFINAE_t< decltype(Declval<Q>().operator&()), decltype(Declval<Q>().operator&()) >;

	template<class Q>
	static auto _calc(...)-> T*;


public:
	using type = decltype( _calc<T>(0) );
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
	using _base_t = Operators_of<element_t>;

public:
	template<  class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	Abbreviable_t(Q& q) noexcept: _base_t(q){}

	Abbreviable_t(T const&&) = delete;

	template<  class _M, class = Enable_if_t< is_const_v || !Avatar_t<T, _M>::is_const_v >  >
	Abbreviable_t(Avatar_t<T, _M> const& avt) : _base_t(avt.get()){}

	template<  class _M, class = Enable_if_t< !is_Same<M, _M>::value >  >
	auto operator=(Avatar_t<T, _M> avt)-> Abbreviable_t&
	{
		static_assert(!is_const_v, "cannot bind to const Avatar_t");

		get() = avt.get();

		return *this;
	}	

	auto operator=(Abbreviable_t const& avt)-> Abbreviable_t&
	{
		static_assert(!is_const_v, "cannot bind to const Avatar_t");

		get() = avt.get();

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
		get() = Forward<Q>(q);

		return *this;
	}
	

	auto cget() const noexcept
	->	element_t const&{  return *static_cast<element_t const*>(this->_p);  }

	auto get() const noexcept-> SGM_DECLTYPE_AUTO(  cget()  )

	auto get() noexcept
	->	element_t&{  return *static_cast<element_t*>(this->_p);  }


	auto operator&() const
	->	typename _Avatar_Ref_Op_Helper<element_t const>::type{  return &get();  }

	auto operator&() 
	->	typename _Avatar_Ref_Op_Helper<element_t>::type{  return &get();  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template<  class T, class = Enable_if_t< !is_Avatar<T>::value >  >
	static auto Refer(T& t) noexcept-> Avatar< Referenceless_t<T> >{  return t;  }

	template<class T, class M>
	static auto Refer(Avatar_t<T, M> avt) noexcept-> Avatar_t<T, M>{  return avt;  }

	template<class T>
	static auto CRefer(T&& t)-> SGM_DECLTYPE_AUTO(  Refer( immut(t) )  )


	template<class T, class M>
	static auto Move(Avatar_t<T, M> avt) noexcept
	->	typename Avatar_t<T, M>::element_t&&{  return Move(avt.get());  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_AVATAR_