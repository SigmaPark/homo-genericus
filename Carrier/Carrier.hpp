#pragma once

#ifndef _SGM_CARRIER_
#define _SGM_CARRIER_

#include "..\Abbreviable\Abbreviable.hpp"
#include <atomic>


namespace sgm
{

	SGM_ABBREVIABLE_PROXY(Carrier);

	template<class T>
	static auto Carry(T&& t)-> Carrier< std::remove_reference_t<T> >{  return std::move(t);  }

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
class sgm::Carrier_t< T, sgm::invar, sgm::Carrier_T_Helper<T, sgm::invar, false, false, false> >
:	No_Making		//	Moving immutable object is Nonsense.
{};


template<class T>
class sgm::Carrier_t< T, sgm::Var, sgm::Carrier_T_Helper<T, sgm::Var, false, false, false> >
{
public:
	Carrier_t(T&& t) : _val( std::move(t) ), _is_carrying(true){}
	Carrier_t(Carrier_t const&) = delete;
	Carrier_t(Carrier_t&& crr) : _val(crr.deliver()), _is_carrying(true){}


	auto operator=(Carrier_t const&)-> Carrier_t& = delete;

	auto operator=(Carrier_t&& crr)-> Carrier_t&
	{
		*_carrying(*this) = _carrying(crr).deliver();

		return *this;
	}

	auto operator=(T&& t)-> Carrier_t&{  return _carrying(*this) = Carry(t);  }


	auto operator*() const-> T const&{  return _deref(this);  }
	auto operator*()-> T&{  return _deref(this);  }


	operator bool() const{  return _is_carrying;  }

	operator T&&(){  return deliver();  }

	auto move()-> Carrier_t{  return this->deliver();  }


	auto deliver()-> T&&
	{
		_carrying(*this)._is_carrying = false;

		return std::move(_val);
	}


private:
	T _val;
	std::atomic<bool> _is_carrying;


	static auto _carrying(Carrier_t& crr)-> Carrier_t&
	{
		assert(crr && L"the content was already delivered.\n");

		return crr;
	}


	template<class Q>
	static auto _deref(Q* const pcrr)-> decltype(**pcrr){  return _carrying(*pcrr)._val;  }
};


#endif