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

	template<class T>  class _PinweightBase;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
class sgm::_PinweightBase : public Operator_interface<T const>
{
public:
	using count_t = std::atomic<size_t>;
	using value_t = T;


	_PinweightBase() 
	:	_cpval(new value_t()), _pcount( new count_t(1) ){  _update_ptr();  }

	_PinweightBase(T const &t) 
	:	_cpval( new value_t(t) ), _pcount( new count_t(1) ){  _update_ptr();  }

	_PinweightBase(T &&t) 
	:	_cpval(  new value_t( Move(t) )  ), _pcount( new count_t(1) ){  _update_ptr();  }

	_PinweightBase(_PinweightBase const &pwb)
	:	_cpval(pwb._cpval), _pcount( _pcount_up(pwb._pcount) ){  _update_ptr();  }

	_PinweightBase(_PinweightBase &&pwb) 
	:	_cpval(pwb._cpval), _pcount(pwb._pcount)
	{
		_update_ptr(),  pwb._cpval = nullptr,  pwb._pcount = nullptr;
	}

	~_PinweightBase(){  _share_count_down();  }


	template<  class Q, class = Enable_if_t< !is_Pinweight<Q>::value >  >
	auto operator=(Q &&q)-> _PinweightBase&
	{
		_share_count_down();

		_update_cpval(  new value_t( Forward<Q>(q) )  );
		_pcount = new count_t(1);

		return *this;
	}

	auto operator=(_PinweightBase const &pwb)-> _PinweightBase&
	{
		if( !share_with(pwb) )
		{
			_share_count_down(),  _pcount_up(pwb._pcount);

			_update_cpval(pwb._cpval);
			_pcount = pwb._pcount;
		}

		return *this;
	}

	auto operator=(_PinweightBase &&pwb)-> _PinweightBase&
	{
		if( !share_with(pwb) )
		{
			_share_count_down();

			_update_cpval(pwb._cpval);
			_pcount = pwb._pcount;

			pwb._cpval = nullptr,  pwb._pcount = nullptr;
		}

		return *this;
	}


	auto get() const-> T const&{  return *_cpval;  }

	bool share_with(_PinweightBase const &pw) const{  return _cpval == pw._cpval;  }
	size_t share_count() const{  return *_pcount;  }


private:
	T mutable *_cpval;
	count_t mutable *_pcount;


	static auto _pcount_up(count_t *pn)-> count_t*
	{
		if(pn != nullptr)
			++*pn;

		return pn;
	}

	void _share_count_down() const
	{
		if( _pcount != nullptr && --*_pcount == 0 )
			delete _cpval, _cpval = nullptr,
			delete _pcount, _pcount = nullptr;
	}


	void _update_ptr(){  static_cast< Operator_interface<T const>& >(*this) = _cpval;  }
	void _update_cpval(T *p){  _cpval = p,  _update_ptr();  }

protected:
	auto _mutable_ref()-> T&{  return *_cpval;  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Pinweight_t
<	T, sgm::invar, sgm::Pinweight_T_Helper<T, sgm::invar, false, false, false> 
> :	public _PinweightBase<T>
{
private:
	using _base_t = _PinweightBase<T>;

public:
	Pinweight_t() = default;

	template
	<	class Q, class _DQ = Decay_t<Q>
	,	class 
		=	Enable_if_t
			<	!is_Same<_DQ, Pinweight_t>::value
			&&	(is_Convertible<Q&&, T>::value || is_inherited_from<_DQ, _base_t>::value) 
			>
	>
	Pinweight_t(Q &&q) : _base_t( Forward<Q>(q) ){}

	Pinweight_t(Pinweight_t const &pw) : _base_t(pw){}
	Pinweight_t(Pinweight_t &&pw) : _base_t( Move(pw) ){}


	template<class Q>  
	auto operator=(Q)-> Pinweight_t& = delete;

	auto operator=(Pinweight_t const&)-> Pinweight_t& = delete;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Pinweight_t< T, sgm::Var, sgm::Pinweight_T_Helper<T, sgm::Var, false, false, false> >
:	public _PinweightBase<T>
{
private:
	using _base_t = _PinweightBase<T>;

public:
	Pinweight_t() = default;

	template
	<	class Q, class _DQ = Decay_t<Q>
	,	class 
		=	Enable_if_t
			<	!is_Same<_DQ, Pinweight_t>::value
			&&	(is_Convertible<Q&&, T>::value || is_inherited_from<_DQ, _base_t>::value) 
			>
	>
	Pinweight_t(Q &&q) : _base_t( Forward<Q>(q) ){}

	Pinweight_t(Pinweight_t const &pw) : _base_t(pw){}
	Pinweight_t(Pinweight_t &&pw) : _base_t( Move(pw) ){}


	template<   class Q, class = Enable_if_t<  !is_Same< Decay_t<Q>, Pinweight_t >::value  >   >
	auto operator=(Q &&q)
	->	Pinweight_t&{  return static_cast<_base_t&>(*this) = Forward<Q>(q),  *this;  }

	auto operator=(Pinweight_t const &pw)
	->	Pinweight_t&{  return *this = static_cast<_base_t const&>(pw);  }

	auto operator=(Pinweight_t &&pw)
	->	Pinweight_t&{  return *this = static_cast<_base_t&&>( Move(pw) );  }


	auto mut()-> typename _base_t::value_t&
	{
		if(_base_t::share_count() > 1)
			*this = Pinweight_t(_base_t::get());
			
		return _base_t::_mutable_ref();
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template<class T>
	static auto Share(T &&t)-> Pinweight< Referenceless_t<T> >{  return Forward<T>(t);  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_PINWEIGHT_

