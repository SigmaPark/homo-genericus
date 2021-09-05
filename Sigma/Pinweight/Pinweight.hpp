#pragma once

#ifndef _SGM_PINWEIGHT_
#define _SGM_PINWEIGHT_

#include "..\Abbreviable\Abbreviable.hpp"
#include "..\interface_Traits\interface_Traits.hpp"
#include <atomic>
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	SGM_ABBREVIABLE_PROXY(Pinweight);

	template<class T>  class _Pinweight_interface;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
class sgm::_Pinweight_interface : public Operator_interface<T const>
{
public:
	using count_t = std::atomic<size_t>;
	using value_t = T;


	_Pinweight_interface() 
	:	_cpval(new value_t()), _pcount( new count_t(1) ){  _update_ptr();  }

	_Pinweight_interface(T const &t) 
	:	_cpval( new value_t(t) ), _pcount( new count_t(1) ){  _update_ptr();  }

	_Pinweight_interface(T &&t) 
	:	_cpval(  new value_t( Move(t) )  ), _pcount( new count_t(1) ){  _update_ptr();  }

	_Pinweight_interface(_Pinweight_interface const &pwimpl)
	:	_cpval(pwimpl._cpval), _pcount( _pcount_up(pwimpl._pcount) ){  _update_ptr();  }

	_Pinweight_interface(_Pinweight_interface &&pwimpl) 
	:	_cpval(pwimpl._cpval), _pcount(pwimpl._pcount)
	{
		_update_ptr(),  pwimpl._cpval = nullptr,  pwimpl._pcount = nullptr;
	}

	~_Pinweight_interface(){  _my_pcount_down();  }


	auto get() const-> T const&{  return *_cpval;  }

	bool share_with(_Pinweight_interface const &pw) const{  return _cpval == pw._cpval;  }
	size_t share_count() const{  return *_pcount;  }


protected:
	T mutable *_cpval;
	count_t mutable *_pcount;


	static auto _pcount_up(count_t *pn)-> count_t*
	{
		if(pn != nullptr)
			++(*pn);

		return pn;
	}

	void _my_pcount_down() const
	{
		if(_pcount == nullptr)
			return;
		else if( --(*_pcount) == 0 )
			delete _cpval, _cpval = nullptr,
			delete _pcount, _pcount = nullptr;
	}


	void _update_ptr(){  static_cast< Operator_interface<T const>& >(*this) = _cpval;  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Pinweight_t
<	T, sgm::invar, sgm::Pinweight_T_Helper<T, sgm::invar, false, false, false> 
> :	public _Pinweight_interface<T>
{
	using _itfc_t = _Pinweight_interface<T>;

	friend class Pinweight_t<T, Var>;

public:
	Pinweight_t() = default;

	Pinweight_t(T const &t) : _itfc_t(t){}
	Pinweight_t(T &&t) : _itfc_t( Move(t) ){}

	Pinweight_t(_Pinweight_interface<T> const &pwitfc) : _itfc_t(pwitfc){}
	Pinweight_t(_Pinweight_interface<T> &&pwitfc) : _itfc_t( Move(pwitfc) ){}

	Pinweight_t(Pinweight_t const &pw) : _itfc_t(pw){}
	Pinweight_t(Pinweight_t &&pw) : _itfc_t( Move(pw) ){}

	~Pinweight_t() = default;


	void operator=(T) = delete;
	void operator=(_Pinweight_interface<T>) = delete;
	auto operator=(Pinweight_t const&)-> Pinweight_t& = delete;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Pinweight_t< T, sgm::Var, sgm::Pinweight_T_Helper<T, sgm::Var, false, false, false> >
:	public _Pinweight_interface<T>
{
	using _itfc_t = _Pinweight_interface<T>;


	template<class Q>
	static auto _substitute(Pinweight_t &res, Q &&q)-> Pinweight_t&
	{
		res._my_pcount_down();
		
		res._cpval = new typename _itfc_t::value_t( Forward<Q>(q) ),  res._update_ptr();
		res._pcount = new typename _itfc_t::count_t(1);

		return res; 			
	}


public:
	Pinweight_t() = default;

	Pinweight_t(T const &t) : _itfc_t(t){}
	Pinweight_t(T &&t) : _itfc_t( Move(t) ){}

	Pinweight_t(_Pinweight_interface<T> const &pwitfc) : _itfc_t(pwitfc){}
	Pinweight_t(_Pinweight_interface<T> &&pwitfc) : _itfc_t( Move(pwitfc) ){}

	Pinweight_t(Pinweight_t const &pw) : _itfc_t(pw){}
	Pinweight_t(Pinweight_t &&pw) : _itfc_t( Move(pw) ){}

	~Pinweight_t() = default;


	auto operator=(T const &t)-> Pinweight_t&{  return _substitute(*this, t);  }
	auto operator=(T &&t)-> Pinweight_t&{  return _substitute( *this, Move(t) );  }


	auto operator=(Pinweight_t<T, invar> const &pw)-> Pinweight_t&
	{
		if(this->_cpval == pw._cpval)
			return *this;

		_itfc_t::_my_pcount_down(),  _itfc_t::_pcount_up(pw._pcount);

		this->_cpval = pw._cpval,  _itfc_t::_update_ptr(),
		this->_pcount = pw._pcount;

		return *this;
	}

	auto operator=(Pinweight_t<T, invar> &&pw)-> Pinweight_t&
	{
		if(this->_cpval == pw._cpval)
			return *this;

		_itfc_t::_my_pcount_down();

		this->_cpval = pw._cpval,  _itfc_t::_update_ptr(),  pw._cpval = nullptr,
		this->_pcount = pw._pcount,  pw._pcount = nullptr;

		return *this;
	}

	auto operator=(Pinweight_t const &pw)
	->	Pinweight_t&{  return *this = static_cast< Pinweight_t<T, invar> const& >(pw);  }

	auto operator=(Pinweight_t &&pw)
	->	Pinweight_t&{  return *this = static_cast< Pinweight_t<T, invar>&& >( Move(pw) );  }


	auto mut()-> typename _itfc_t::value_t&
	{
		if(_itfc_t::share_count() > 1)
			*this = Pinweight_t(_itfc_t::get());
			
		return *this->_cpval;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template<class T>
	static auto Pinweighting(T &&t)-> Pinweight< Referenceless_t<T> >{  return Forward<T>(t);  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_PINWEIGHT_

