/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_AVATAR_
#define _SGM_AVATAR_


#include "Abbreviable.hpp"
#include "SGM/Operators_of/Operators_of.hpp"
#include <cassert>


namespace sgm
{
	
	SGM_ABBREVIABLE_WIZARD(Avatar);

	template<class T>
	struct is_VoidAvatar;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
class sgm::Abbreviable_t
<	sgm::Avatar_tag
,	T, sgm::Variable_t, sgm::Avatar_Pedigree<T, sgm::Variable_t, false, false>
> :	public Operators_of<T>
{
public:
	using element_t = T;


private:
	using _base_t = Operators_of<element_t>;


public:
	template
	<	class Q, class = Enable_if_t< !is_Avatar<Q>::value && !is_immutable<Q&>::value >  
	>
	Abbreviable_t(Q& q) noexcept : _base_t(q){}

	template
	<	class Q
	,	class 
		=	Enable_if_t
			<	is_Avatar<Q>::value 
			&&	!is_constAvatar<Q>::value 
			&&	!is_Const< Referenceless_t<Q> >::value
			>
	>
	Abbreviable_t(Q&& avt) noexcept : _base_t(avt.v()){}

	template<  class Q, class = Enable_if_t< is_constAvatar<Q>::value >  >
	Abbreviable_t(Q const&) = delete;

	Abbreviable_t(T const&&) = delete;


	auto operator=(constAvatar<T> const& avt)-> Abbreviable_t&
	{
		_base_t::v() = avt.v();

		return *this;
	}	

	auto operator=(Abbreviable_t const& avt)
	->	Abbreviable_t&{  return *this = constAvatar<T>(avt);  }

	template
	<	class Q
	,	class 
		=	Enable_if_t< !is_Avatar<Q>::value && is_Convertible<Q, T>::value >  
	>
	auto operator=(Q&& q) noexcept(Aleph_Check<Q&&>::value)
	->	Abbreviable_t&
	{
		_base_t::v() = Forward<Q>(q);

		return *this;
	}


	auto operator&() const noexcept-> SGM_DECLTYPE_AUTO(  &_base_t::v()  )
	auto operator&() noexcept-> SGM_DECLTYPE_AUTO(  &_base_t::v()  )
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Abbreviable_t
<	sgm::Avatar_tag
,	T, sgm::invariable_t, sgm::Avatar_Pedigree<T, sgm::invariable_t, false, false>
> :	public Operators_of<T const>
{
public:
	using element_t = T const;
	using value_type = T;

private:
	using _base_t = Operators_of<element_t>;


public:
	template<  class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	Abbreviable_t(Q& q) noexcept : _base_t(q){}

	template<  class Q, class = Enable_if_t< is_Avatar<Q>::value >  >
	Abbreviable_t(Q const& avt) noexcept : _base_t(avt.v()){}

	Abbreviable_t(T const&&) = delete;


	auto operator=(Abbreviable_t const&)-> Abbreviable_t& = delete;
	auto operator=(T const&)-> Abbreviable_t& = delete;

	auto operator&() const noexcept-> SGM_DECLTYPE_AUTO(  &_base_t::v()  )
	auto operator&() noexcept-> SGM_DECLTYPE_AUTO(  &_base_t::v()  )
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
struct sgm::is_VoidAvatar
{
private:
	/* Declaration Only */
	template< class Q, bool B = is_Avatar<Q>::value, class = Enable_if_t<!B> >
	static auto _calc(int)-> Boolean<B>;

	/* Declaration Only */
	template< class Q, bool B = is_Avatar<Q>::value, class = Enable_if_t<B> >
	static auto _calc(...)-> is_Void< typename Referenceless_t<Q>::value_type >;


public:
	static bool constexpr value = decltype( _calc<T>(0) )::value;

	using type = Boolean<value>;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<>
class sgm::Abbreviable_t
<	sgm::Avatar_tag
,	void, sgm::Variable_t, sgm::Avatar_Pedigree<void, sgm::Variable_t, false, false>
>
{
public:
	using element_t = void;
	using value_type = void;


	template
	<	class Q, class = Enable_if_t< !is_Avatar<Q>::value && !is_immutable<Q&>::value >  
	>
	Abbreviable_t(Q& q) noexcept : _vp( Address_of(q) ){}

	template
	<	class Q
	,	class 
		=	Enable_if_t
			<	is_Avatar<Q>::value && !Has_Same_Origin<Q, Abbreviable_t>::value
			&&	!is_constAvatar<Q>::value 
			&&	!is_Const< Referenceless_t<Q> >::value
			>
	>
	Abbreviable_t(Q&& avt) noexcept : Abbreviable_t(avt.v()){}

	template<  class Q, class = Enable_if_t< is_constAvatar<Q>::value >  >
	Abbreviable_t(Q const&) = delete;

	template<  class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	Abbreviable_t(Q const&&) = delete;

	Abbreviable_t(Abbreviable_t const&) = default;


	template<  class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	explicit operator Q&() noexcept{	  return *reinterpret_cast<Q*>(_vp);  }

	template<class Q>
	explicit operator Avatar<Q>() noexcept
	{
		return static_cast< typename Avatar<Q>::element_t& >(*this);  
	}

	template<class Q>
	explicit operator constAvatar<Q>() const noexcept
	{
		return static_cast< typename constAvatar<Q>::element_t& >(*this);  
	}

	template<class Q>
	explicit operator constAvatar<Q>() noexcept
	{
		return static_cast< typename constAvatar<Q>::element_t& >(*this);  
	}

	
	auto operator&() const noexcept-> void const*{  return _vp;  }
	auto operator&() noexcept-> void*{  return _vp;  }


	auto v() const noexcept
	->	Omni_Convertible const&{  return *reinterpret_cast<Omni_Convertible const*>(_vp);  }

	auto v() noexcept
	->	Omni_Convertible&{  return *reinterpret_cast<Omni_Convertible*>(_vp);  }


private:
	void* const _vp;
};


template<>
class sgm::Abbreviable_t
<	sgm::Avatar_tag
,	void, sgm::invariable_t, sgm::Avatar_Pedigree<void, sgm::invariable_t, false, false>
>
{
public:
	using element_t = void const;
	using value_type = void;


	template<  class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	Abbreviable_t(Q& q) noexcept: _cvp( Address_of(q) ){}

	template
	<  	class Q
	, 	class = Enable_if_t< is_Avatar<Q>::value && !Has_Same_Origin<Q, Abbreviable_t>::value >  
	>
	Abbreviable_t(Q const& avt) noexcept : Abbreviable_t(avt.v()){}

	template<  class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	Abbreviable_t(Q const&&) = delete;

	Abbreviable_t(Abbreviable_t const&) = default;

	Abbreviable_t
	(	Abbreviable_t
		<	Avatar_tag, void, Variable_t
		,	Avatar_Pedigree<void, Variable_t, false, false>
		>	va
	)	noexcept : _cvp(&va){}


	template<  class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	explicit operator Q const&() const noexcept{  return *reinterpret_cast<Q const*>(_cvp);  }

	template<class Q>
	explicit operator constAvatar<Q>() const noexcept
	{
		return static_cast< typename constAvatar<Q>::element_t& >(*this);  
	}


	auto operator&() const noexcept-> void const*{  return _cvp;  }

	auto v() const noexcept
	->	Omni_Convertible const&{  return *reinterpret_cast<Omni_Convertible const*>(_cvp);  }


private:
	void const* const _cvp;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template<  class T, class = Enable_if_t< !is_Avatar<T>::value >  >
	static auto Refer(T& t) noexcept
	->	Selective_t
		<	is_immutable<T&>::value
		,	constAvatar< Decay_t<T> >, Avatar< Decay_t<T> >
		>
	{
		return t;  
	}

	template<class T, class M>
	static auto Refer(Avatar_t<T, M> avt) noexcept-> SGM_DECLTYPE_AUTO(  Refer(avt.v())  )

	template<class T>
	static auto CRefer(T&& t) noexcept-> SGM_DECLTYPE_AUTO(  Refer( immut(t) )  )


	template<class T, class M>
	static auto Move(Avatar_t<T, M> avt) noexcept
	->	typename Avatar_t<T, M>::element_t&&{  return Move(avt.v());  }

	template<class, class T>
	static auto Forward(constAvatar<T> avt) noexcept
	->	typename constAvatar<T>::element_t&{  return avt.v();  }

	template<class, class T>
	static auto Forward(Avatar<T> avt) noexcept
	->	typename Avatar<T>::element_t&{  return avt.v();  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_AVATAR_