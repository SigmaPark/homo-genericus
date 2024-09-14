/*  SPDX-FileCopyrightText: (c) 2023 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_CONVOY_
#define _SGM_CONVOY_


#include "Abbreviable.hpp"
#include "SGM/Operators_of/Operators_of.hpp"


namespace sgm
{

	SGM_ABBREVIABLE_WIZARD(Convoy);

}


template<class T>
class sgm::Abbreviable_t
<	sgm::Convoy_tag
,	T, sgm::Variable_t, sgm::Convoy_Pedigree<T, sgm::Variable_t, false, false>
> : public Operators_of<T const>
{
public:
	using element_t = T const;

private:
	using _base_t = Operators_of<element_t>;


public:
	template<  class Q, class = Enable_if_t< is_Convertible<Q, element_t>::value >  >
	Abbreviable_t(Q&) = delete;

	Abbreviable_t(T&& t) noexcept : _value( Move(t) ){  _base_t::_p = Address_of(_value);  }

	Abbreviable_t(Abbreviable_t const&) = delete;
	Abbreviable_t(Abbreviable_t&& cvy) noexcept : Abbreviable_t(move()){}
	
	auto operator=(element_t&)-> Abbreviable_t& = delete;
	auto operator=(Abbreviable_t const&)-> Abbreviable_t& = delete;
	
	auto operator=(T&& t) noexcept-> Abbreviable_t&
	{
		_base_t::_p = Address_of( _value = Move(t) );

		return *this;
	}

	auto operator=(Abbreviable_t&& cvy) noexcept-> Abbreviable_t&{  return *this = cvy.move();  }

	auto move() noexcept-> T&&{  return _base_t::_p = nullptr,  Move(_value);  }

	explicit operator T&&() noexcept{  return move();  }

private:
	T _value; 
};


template<class T>
class sgm::Abbreviable_t
<	sgm::Convoy_tag
,	T, sgm::invariable_t, sgm::Convoy_Pedigree<T, sgm::invariable_t, false, false>
> 
:	public Unconstructible{};


#endif // end of #ifndef _SGM_CONVOY_