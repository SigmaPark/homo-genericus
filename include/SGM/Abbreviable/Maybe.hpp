/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_MAYBE_
#define _SGM_MAYBE_


#include "Abbreviable.hpp"
#include "../Exception/Exception.hpp"
#include <new>


namespace sgm
{

	SGM_ABBREVIABLE_WIZARD(Maybe);


	enum class NullMyb_t : bool{} constexpr NullMyb{};

	struct Bad_Access_to_Maybe;


	template<class T>
	static auto make_Maybe(T&& t) noexcept(is_Rvalue_Reference<T&&>::value)
	->	Selective_t
		<	is_immutable<T>::value
		,	constMaybe< Decay_t<T> >, Maybe< Decay_t<T> >
		>;


	template<class T>
	class _Base_Maybe;

}
//========//========//========//========//=======#//========//========//========//========//=======#


struct sgm::Bad_Access_to_Maybe : public Exception
{
	auto what() const-> char const* override{  return "you accessed invalid sgm::Maybe .\n";  }

private:
	template<class T>
	friend class sgm::_Base_Maybe;


	Bad_Access_to_Maybe() = default;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::_Base_Maybe
{
private:
	enum class byte : unsigned char{Zero = 0};


public:
	using value_type = Decay_t<T>;

	
	static_assert
	(	!is_Primitive_Array<value_type>::value
	,	"Maybe::value_type should not be an array type . "
	);


	_Base_Maybe() : _has_value(false){}
	_Base_Maybe(NullMyb_t const) : _Base_Maybe(){}

	template
	<	class...ARGS
	,	class
		=	Enable_if_t
			<	sizeof...(ARGS) >= 2
			||	(	!is_Maybe< First_t<ARGS...> >::value
				&&	!is_Same<  Decay_t< First_t<ARGS...> >, _Base_Maybe  >::value
				&&	!is_Same<  Decay_t< First_t<ARGS...> >, NullMyb_t  >::value
				)
			>
	>
	_Base_Maybe(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	:	_has_value(true){  _alloc( Forward<ARGS>(args)... );  }


	_Base_Maybe(_Base_Maybe const& myb) : _has_value(myb.has_value())
	{
		if(myb.has_value())
			_alloc(*myb);
	}

	_Base_Maybe(_Base_Maybe&& myb) noexcept : _has_value(myb.has_value())
	{
		if(myb.has_value())
			_alloc( Move(*myb) );
	}

	template
	<	class Q, class = Enable_if_t< !is_Same<Q, T>::value && is_Convertible<Q, T>::value >
	>
	_Base_Maybe(_Base_Maybe<Q> const& myb) : _has_value(myb.has_value())
	{
		if(myb.has_value())
			_alloc(*myb);
	}


	~_Base_Maybe()
	{
		if(has_value())
			(**this).~T(),  
			_has_value = false;  
	}


	template
	<	class Q
	,	class 
		=	Enable_if_t
			<	is_Convertible<Q&&, T>::value
			&&	!is_Maybe<Q>::value
			&&	!is_Same< Decay_t<Q>, _Base_Maybe >::value
			&&	!is_Same< Decay_t<Q>, NullMyb_t >::value
			>
	>
	auto operator=(Q&& q) noexcept(is_Rvalue_Reference<Q&&>::value)-> _Base_Maybe&
	{
		**this = Forward<Q>(q); 
		_has_value = true;

		return *this;
	}

	auto operator=(NullMyb_t const)-> _Base_Maybe&{  return this->~_Base_Maybe(),  *this;  }

	template
	<	class Q, class = Enable_if_t< !is_Same<Q, T>::value && is_Convertible<Q, T>::value >
	>
	auto operator=(_Base_Maybe<Q> const& myb)
	->	_Base_Maybe&{  return myb.has_value() ? *this = *myb : *this = NullMyb;   }

	template
	<	class Q, class = Enable_if_t< !is_Same<Q, T>::value && is_Convertible<Q, T>::value >
	>
	auto operator=(_Base_Maybe<Q>&& myb)
	->	_Base_Maybe&{  return myb.has_value() ? *this = Move(*myb) : *this = NullMyb;  }

	auto operator=(_Base_Maybe const& myb)
	->	_Base_Maybe&{  return myb.has_value() ? *this = *myb : *this = NullMyb;  }

	auto operator=(_Base_Maybe&& myb) noexcept
	->	_Base_Maybe&{  return myb.has_value() ? *this = Move(*myb) : *this = NullMyb;  }


	auto has_value() const-> bool{  return _has_value;  }
	auto operator!() const-> bool{  return !has_value();  }

	auto operator*() const-> T const&{  return *reinterpret_cast<T const*>(_buf);  }
	auto operator*()-> T&{  return *reinterpret_cast<T*>(_buf);  }

	auto operator->() const-> T const*{  return &**this;  }
	auto operator->()-> T*{  return &**this;  }

	auto get() const noexcept(false)-> T const&
	{
		if(has_value())
			return **this;
		else
			throw Bad_Access_to_Maybe{};
	}

	auto get() noexcept(false)-> T&
	{
		if(has_value())
			return **this;
		else
			throw Bad_Access_to_Maybe{};
	}

	auto get_or(T const& t) const-> T const&{  return has_value() ? **this : t;  }
	auto get_or(T& t)-> T&{  return has_value() ? **this : t;  }



private:
	bool _has_value;
	byte _buf[sizeof(T)] = {byte::Zero, };


	template<class...ARGS>
	void _alloc(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	{
		new(_buf) value_type( Forward<ARGS>(args)... );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Abbreviable_t
<	sgm::Maybe_tag
,	T, sgm::invariable_t, sgm::Maybe_Pedigree<T, sgm::invariable_t, false, false>
>
:	public _Base_Maybe<T>
{
private:
	using _base_type = _Base_Maybe<T>;


public:
	template<class...ARGS>
	Abbreviable_t(ARGS&&...args) : _base_type( Forward<ARGS>(args)... ){}

	Abbreviable_t(Abbreviable_t const& abrv) 
	:	_base_type( static_cast<_base_type const&>(abrv) ){}

	Abbreviable_t(Abbreviable_t&& abrv) noexcept 
	:	_base_type( static_cast<_base_type&&>(abrv) ){}


	template<class Q>  
	auto operator=(Q)-> Abbreviable_t& = delete;

	auto operator=(Abbreviable_t const&)-> Abbreviable_t& = delete;

	auto operator*() const-> T const&{  return _base_type::operator*();  }
	auto operator->() const-> T const*{  return _base_type::operator->();  }
	auto get() const noexcept(false)-> T const&{  return _base_type::get();  }
	auto get_or(T const& t) const-> T const&{  return _base_type::get_or(t);  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Abbreviable_t
<	sgm::Maybe_tag
,	T, sgm::Variable_t, sgm::Maybe_Pedigree<T, sgm::Variable_t, false, false>
>
:	public _Base_Maybe<T>
{
private:
	using _base_type = _Base_Maybe<T>;


public:
	template<class...ARGS>
	Abbreviable_t(ARGS&&...args) : _base_type( Forward<ARGS>(args)... ){}

	Abbreviable_t(Abbreviable_t const& abrv) 
	:	_base_type( static_cast<_base_type const&>(abrv) ){}

	Abbreviable_t(Abbreviable_t&& abrv) noexcept 
	:	_base_type( static_cast<_base_type&&>(abrv) ){}


	template<class Q>
	auto operator=(Q&& q) noexcept(is_Rvalue_Reference<Q&&>::value)
	->	Abbreviable_t&{  return _base_type::operator=( Forward<Q>(q) ),  *this;  }


	auto operator=(Abbreviable_t const& myb)
	->	Abbreviable_t&{  return _base_type::operator=(myb),  *this;  }

	auto operator=(Abbreviable_t&& myb) noexcept
	->	Abbreviable_t&{  return _base_type::operator=( Move(myb) ),  *this;  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
auto sgm::make_Maybe(T&& t) noexcept(is_Rvalue_Reference<T&&>::value)
->	Selective_t
	<	is_immutable<T>::value
	,	constMaybe< Decay_t<T> >, Maybe< Decay_t<T> >
	>
{  
	return Forward<T>(t);  
}


#endif // end of #ifndef _SGM_MAYBE_
