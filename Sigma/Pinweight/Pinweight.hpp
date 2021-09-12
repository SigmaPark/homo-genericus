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
	template<class T>  struct _PinweightCore;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
struct sgm::_PinweightCore
{
	T val;
	bool inplace;
	std::atomic<size_t> count{1};


	_PinweightCore() = delete;


	template<class Q>
	static auto Construct(Q &&q, void *vp)-> _PinweightCore*
	{
		if(vp != nullptr)
			return new(vp) _PinweightCore( Forward<Q>(q), true );
		else
			return new _PinweightCore( Forward<Q>(q), false );
	}

	static void Destruct(_PinweightCore *pcore)
	{
		if(pcore == nullptr)
			return;
		
		if(pcore->inplace)
			pcore->val.~T();
		else
			delete pcore;

		pcore = nullptr;
	}


	static void CountDown(_PinweightCore *pcore)
	{
		if(pcore != nullptr && --pcore->count == 0)
			Destruct(pcore);
	}

	static auto CountUp(_PinweightCore *pcore)-> _PinweightCore*
	{
		if(pcore != nullptr && pcore->count != 0)
			++pcore->count;

		return pcore;
	}

private:
	template<class Q>
	_PinweightCore(Q &&q, bool const inp) : val( Forward<Q>(q) ), inplace(inp){}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::_PinweightBase : public Operator_interface<T const>
{
	using _core_t = _PinweightCore<T>;
	using _count_t = std::atomic<size_t>;

public:
	using value_type = T;


	_PinweightBase(void *vp = nullptr) 
	:	_pcore( _core_t::Construct(T(), vp) ){  _update_ptr();  }

	_PinweightBase(T const &t, void *vp = nullptr)  
	:	_pcore( _core_t::Construct(t, vp) ){  _update_ptr();  }

	_PinweightBase(T &&t, void *vp = nullptr) 
	:	_pcore(  _core_t::Construct( Move(t), vp )  ){  _update_ptr();  }

	_PinweightBase(_PinweightBase const &pwb) 
	:	_pcore( _core_t::CountUp(pwb._pcore) ){  _update_ptr();  }

	_PinweightBase(_PinweightBase &&pwb) 
	:	_pcore(pwb._pcore){  _update_ptr(),  pwb._pcore = nullptr;  }


	~_PinweightBase(){  _core_t::CountDown(_pcore);  }


	template<  class Q, class = Enable_if_t< !is_Pinweight<Q>::value >  >
	auto operator=(Q &&q)-> _PinweightBase&{  return *this = _PinweightBase( Forward<Q>(q) );  }

	auto operator=(_PinweightBase const &pwb)-> _PinweightBase&
	{
		if( !share_with(pwb) )
			_update( _core_t::CountUp(pwb._pcore) );

		return *this;
	}

	auto operator=(_PinweightBase &&pwb)-> _PinweightBase&
	{
		if( !share_with(pwb) )
			_update(pwb._pcore),  pwb._pcore = nullptr;

		return *this;
	}


	auto get() const-> T const&{  return _pcore->val;  }

	bool share_with(_PinweightBase const &pw) const{  return _pcore == pw._pcore;  }
	size_t share_count() const{  return _pcore->count;  }


private:
	_core_t *_pcore;


	void _update_ptr(){  static_cast< Operator_interface<T const>& >(*this) = &_pcore->val;  }

	void _update(_core_t *pcore)
	{
		_core_t::CountDown(_pcore);
		_pcore = pcore;
		_update_ptr();
	}

protected:
	auto _mutable_ref()-> T&{  return _pcore->val;  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Pinweight_t
<	T, sgm::invar, sgm::Pinweight_T_Helper<T, sgm::invar, false, false, false> 
> :	public _PinweightBase<T>
{
private:
	using _base_t = _PinweightBase<T>;

	template< class Q, class _DQ = Decay_t<Q> >
	using _enif_t 
	=	Boolean_type
		<	!is_Same<_DQ, Pinweight_t>::value
		&&	(is_Convertible<Q&&, T>::value || is_inherited_from<_DQ, _base_t>::value) 
		>;

public:
	Pinweight_t() = default;

	template<  class Q, class = Enable_if_t< _enif_t<Q>::value >  >
	Pinweight_t(Q &&q) : _base_t( Forward<Q>(q) ){}

	template<  class Q, class = Enable_if_t< _enif_t<Q>::value >  >
	Pinweight_t(Q &&q, void *vp) : _base_t( Forward<Q>(q), vp ){}

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

	template< class Q, class _DQ = Decay_t<Q> >
	using _enif_t 
	=	Boolean_type
		<	!is_Same<_DQ, Pinweight_t>::value
		&&	(is_Convertible<Q&&, T>::value || is_inherited_from<_DQ, _base_t>::value) 
		>;

public:
	Pinweight_t() = default;

	template<  class Q, class = Enable_if_t< _enif_t<Q>::value >  >
	Pinweight_t(Q &&q) : _base_t( Forward<Q>(q) ){}

	template<  class Q, class = Enable_if_t< _enif_t<Q>::value >  >
	Pinweight_t(Q &&q, void *vp) : _base_t( Forward<Q>(q), vp ){}

	Pinweight_t(Pinweight_t const &pw) : _base_t(pw){}
	Pinweight_t(Pinweight_t &&pw) : _base_t( Move(pw) ){}


	template<   class Q, class = Enable_if_t<  !is_Same< Decay_t<Q>, Pinweight_t >::value  >   >
	auto operator=(Q &&q)
	->	Pinweight_t&{  return static_cast<_base_t&>(*this) = Forward<Q>(q),  *this;  }

	auto operator=(Pinweight_t const &pw)
	->	Pinweight_t&{  return *this = static_cast<_base_t const&>(pw);  }

	auto operator=(Pinweight_t &&pw)
	->	Pinweight_t&{  return *this = static_cast<_base_t&&>( Move(pw) );  }


	auto mut()-> typename _base_t::value_type&
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
	static auto Share(T &&t, void *vp = nullptr)
	->	Pinweight< Referenceless_t<T> >{  return {Forward<T>(t), vp};  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_PINWEIGHT_