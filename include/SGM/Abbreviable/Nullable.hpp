/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_NULLABLE_
#define _SGM_NULLABLE_


#include "Abbreviable.hpp"
#include "../Operators_of/Operators_of.hpp"
#include "../Exception/Exception.hpp"
#include <new>


namespace sgm
{
	
	SGM_ABBREVIABLE_WIZARD(Nullable);


	class Null_t;

	struct Bad_Access_to_Nullable;


	template<class T>
	class _Base_Nullable;

	template< class T, bool = is_Class_or_Union<T>::value >
	union _Nullable_Core;

}
//========//========//========//========//=======#//========//========//========//========//=======#


struct sgm::Bad_Access_to_Nullable : public Exception
{
	auto what() const-> char const* override{  return "you accessed invalid sgm::Nullable .";  }

private:
	template<class T>
	friend class sgm::_Base_Nullable;

	friend class sgm::Null_t;


	constexpr Bad_Access_to_Nullable() = default;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::Null_t
{
public:
	constexpr Null_t() = default;


	auto constexpr has_value() const noexcept-> bool{  return false;  }

	void v() const noexcept = delete;
	auto v_or() const noexcept(false)-> Omni_Convertible{  throw Bad_Access_to_Nullable{};  }

	template<class T>
	auto v_or(T&& t) const noexcept-> SGM_DECLTYPE_AUTO(  Forward<T>(t)  )
};


template<>
struct sgm::is_Abbreviable<sgm::Nullable_tag, sgm::Null_t, true> : True_t{};

template<>
struct sgm::is_constAbbreviable<sgm::Nullable_tag, sgm::Null_t, true> : True_t{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
union sgm::_Nullable_Core<T, false>
{
	Null_t n = {};
	T value;


	void clear() noexcept{}
};

template<class T>
union sgm::_Nullable_Core<T, true>
{
	Null_t n;
	T value;


	_Nullable_Core() : n(){}
	~_Nullable_Core(){}

	void clear() noexcept{  value.~T();  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::_Base_Nullable : public Operators_of<T>
{
private:
	using _core_t = _Nullable_Core< Decay_t<T> >;
	using _opof_t = Operators_of<T>;


public:
	using value_type = Decay_t<T>;


	static_assert
	(	!is_Primitive_Array<value_type>::value
	,	"Nullable::value_type should not be an array type ."
	);


	constexpr _Base_Nullable(Null_t const = {}){};

	template
	<	class...ARGS
	,	class
		=	Enable_if_t
			<	( sizeof...(ARGS) >= 2 )
			||	(	!is_Nullable< First_t<ARGS...> >::value
				&&	!Has_Same_Origin< First_t<ARGS...>, _Base_Nullable >::value
				&&	!Has_Same_Origin< First_t<ARGS...>, Null_t >::value
				)
			>
	>
	_Base_Nullable(ARGS&&...args)
	noexcept(Aleph_Check<ARGS&&...>::value){  _alloc( Forward<ARGS>(args)... );  }

	_Base_Nullable(_Base_Nullable const& nb){  _try_alloc(nb);  }
	_Base_Nullable(_Base_Nullable&& nb){  _try_alloc( Move(nb) );  }

	template
	<	class Q, class = Enable_if_t< is_Convertible_but_Different_Origin<Q, T>::value >
	>
	_Base_Nullable(_Base_Nullable<Q> const& nb){  _try_alloc(nb);  }

	template
	<	class Q, class = Enable_if_t< is_Convertible_but_Different_Origin<Q, T>::value >
	>
	_Base_Nullable(_Base_Nullable<Q>&& nb){  _try_alloc( Move(nb) );  }

	~_Base_Nullable(){  *this = Null_t{};  }


	template
	<	class Q
	,	class
		=	Enable_if_t
			<	is_Convertible_but_Different_Origin<Q, _Base_Nullable>::value
			&&	!is_Nullable<Q>::value
			>
	>
	auto operator=(Q&& q) noexcept(is_Rvalue_Reference<Q&&>::value)-> _Base_Nullable&
	{
		if(!has_value())
			_alloc( Forward<Q>(q) );
		else
			v() = Forward<Q>(q);

		return *this;
	}

	auto operator=(Null_t const)-> _Base_Nullable&
	{
		if(has_value())
			_core.clear(),
			_opof_t::_p = nullptr;

		return *this;
	}

	template
	<	class Q, class = Enable_if_t< is_Convertible_but_Different_Origin<Q, T>::value >
	>
	auto operator=(_Base_Nullable<Q> const& nb)
	->	_Base_Nullable&{  return nb.has_value() ? *this = nb.v() : *this = Null_t{};  }

	template
	<	class Q, class = Enable_if_t< is_Convertible_but_Different_Origin<Q, T>::value >
	>
	auto operator=(_Base_Nullable<Q>&& nb)
	->	_Base_Nullable&{  return nb.has_value() ? *this = Move(nb).v() : *this = Null_t{};  }

	auto operator=(_Base_Nullable const& nb)
	->	_Base_Nullable&{  return nb.has_value() ? *this = nb.v() : *this = Null_t{};  }

	auto operator=(_Base_Nullable&& nb)
	->	_Base_Nullable&{  return nb.has_value() ? *this = Move(nb).v() : *this = Null_t{};  }


	auto has_value() const noexcept-> bool{  return _opof_t::_p != nullptr;  }

	auto v() & noexcept-> T&{  return _core.value;  }
	auto v() const& noexcept-> T const&{  return _core.value;  }
	auto v() && noexcept-> T&&{  return Move(_core.value);  }
	auto v() const&& noexcept-> T const&&{  return Move(_core.value);  }

	auto v_or() & noexcept(false)-> T&{  return _get_or(*this);  }
	auto v_or() const& noexcept(false)-> T const&{  return _get_or(*this);  }
	auto v_or() && noexcept(false)-> T&&{  return _get_or( Move(*this) );  }
	auto v_or() const&& noexcept(false)-> T const&&{  return _get_or( Move(*this) );  }
	
	auto v_or(T& t) & noexcept-> T&{  return _get_or(*this, t);  }
	auto v_or(T const& t) const& noexcept-> T const&{  return _get_or(*this, t);  }
	auto v_or(T&& t) && noexcept-> T&&{  return _get_or( Move(*this), Move(t) );  }
	
	auto v_or(T const&& t) const&& noexcept
	->	T const&&{  return _get_or( Move(*this), Move(t) );  }


private:
	_core_t _core;


	template<class...ARGS>
	void _alloc(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	{
		_opof_t::_p = new(&_core) value_type( Forward<ARGS>(args)... );
	}

	template<class NB>
	void _try_alloc(NB&& nb) noexcept(is_Rvalue_Reference<NB&&>::value)
	{
		if(nb.has_value())
			_alloc( Forward<NB>(nb).v() );
		else
			*this = Null_t{};
	}


	template<class ME, class...ARGS>
	static auto _get_or(ME&& me, ARGS&&...args) noexcept(false)
	->	Enable_if_t< sizeof...(ARGS) == 0, Qualify_Like_t<ME&&, T> >
	{
		if(me.has_value())
			return Forward<ME>(me).v();
		else
			throw Bad_Access_to_Nullable{};
	}

	template<class ME, class...ARGS>
	static auto _get_or(ME&& me, ARGS&&...args) noexcept
	->	Enable_if_t< sizeof...(ARGS) == 1, Qualify_Like_t<ME&&, T> >
	{
		if(me.has_value())
			return Forward<ME>(me).v();
		else
			return Nth_Param<0>( Forward<ARGS>(args)... );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Abbreviable_t
<	sgm::Nullable_tag
,	T, sgm::invariable_t, sgm::Nullable_Pedigree<T, sgm::invariable_t, false, false>
>
:	public _Base_Nullable<T const>
{
private:
	using _base_t = _Base_Nullable<T const>;


public:
	using _base_t::_base_t;

	Abbreviable_t(Abbreviable_t const& abrv) : _base_t( static_cast<_base_t const&>(abrv) ){}
	Abbreviable_t(Abbreviable_t&& abrv) noexcept : _base_t( static_cast<_base_t&&>(abrv) ){}


	template<class Q>
	auto operator=(Q)-> Abbreviable_t& = delete;

	auto operator=(Abbreviable_t const&)-> Abbreviable_t& = delete;

	auto v() const noexcept-> T const&{  return _base_t::v();  }
	auto v_or() const noexcept(false)-> T const&{  return _base_t::v_or();  }
	auto v_or(T const& t) const noexcept-> T const&{  return _base_t::v_or(t);  }

	auto move() noexcept-> T const&&{  return Move(v());  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Abbreviable_t
<	sgm::Nullable_tag
,	T, sgm::Variable_t, sgm::Nullable_Pedigree<T, sgm::Variable_t, false, false>
>
:	public _Base_Nullable<T>
{
private:
	using _base_t = _Base_Nullable<T>;


public:
	using _base_t::_base_t;

	Abbreviable_t(Abbreviable_t const& abrv) : _base_t( static_cast<_base_t const&>(abrv) ){}
	Abbreviable_t(Abbreviable_t&& abrv) noexcept : _base_t( static_cast<_base_t&&>(abrv) ){}


	template<class Q>
	auto operator=(Q&& q) noexcept(is_Rvalue_Reference<Q&&>::value)
	->	Abbreviable_t&{  return _base_t::operator=( Forward<Q>(q) ),  *this;  }

	auto operator=(Abbreviable_t const& abrv)
	->	Abbreviable_t&{  return _base_t::operator=(abrv),  *this;  }

	auto operator=(Abbreviable_t&& abrv) noexcept
	->	Abbreviable_t&{  return _base_t::operator=( Move(abrv) ),  *this;  }


	auto move() noexcept-> T&&{  return Move(_base_t::v());  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template<class T>
	static auto make_Nullable(T&& t) noexcept(is_Rvalue_Reference<T&&>::value)
	->	Selective_t
		<	is_immutable<T>::value
		,	constNullable< Decay_t<T> >, Nullable< Decay_t<T> >
		>
	{
		return Forward<T>(t);
	}	

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#




#endif // end of #ifndef _SGM_NULLABLE_