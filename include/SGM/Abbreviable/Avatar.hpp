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

}
//========//========//========//========//=======#//========//========//========//========//=======#


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
	constexpr Abbreviable_t() : _opof_type(){}

	template< class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	Abbreviable_t(Q& q) : _opof_type(q){}

	Abbreviable_t(T const&&) = delete;

	template<  class _M, class = Enable_if_t< is_const_v || !Avatar_t<T, _M>::is_const_v >  >
	Abbreviable_t(Avatar_t<T, _M> const& avt) : _opof_type( static_cast<T const&>(avt) ){}


	auto operator=(Abbreviable_t const& avt)-> Abbreviable_t&
	{
		static_assert(!is_const_v, "cannot bind to const Avatar_t");

		assert(is_valid() || avt.is_valid());

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
		assert(is_valid() && L"Avatar_t has nothing");

		static_cast<T&>(*this) = Forward<Q>(q);

		return *this;
	}


	bool is_valid() const{  return this->_p != nullptr;  }


	auto get() const-> T const&
	{
		assert(is_valid());

		return static_cast<T const&>(*this);  
	}

	auto get()-> element_t&
	{
		assert(is_valid());

		return static_cast<element_t&>(*this);  
	}
	

	template<class _M>
	auto reset(Avatar_t<T, _M> avt)-> Abbreviable_t
	{
		static_assert
		(	is_const_v || !Avatar_t<T, _M>::is_const_v
		,	"cannot bind to constAvatar"
		);

		static_cast<_opof_type&>(*this) = avt.get();

		return *this;
	}

	template< class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	auto reset(Q& q)-> Abbreviable_t
	{
		static_assert
		(	is_const_v || !is_immutable<Q>::value
		,	"cannot bind to constAvatar"
		);

		static_cast<_opof_type&>(*this) = q;

		return *this;
	}

	template< class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	auto reset(Q const&&)-> Abbreviable_t = delete;
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

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#




#endif // end of #ifndef _SGM_AVATAR_