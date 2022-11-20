/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_PINWEIGHT_
#define _SGM_PINWEIGHT_


#include "Abbreviable.hpp"
#include "../Operators_of/Operators_of.hpp"
#include <atomic>


namespace sgm
{

	SGM_ABBREVIABLE_WIZARD(Pinweight);


	template<class T>  
	class _Base_Pinweight;
	
	template<class T>  
	struct _Pinweight_Data;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
struct sgm::_Pinweight_Data
{
public:
	_Pinweight_Data() = delete;


private:
	friend class sgm::_Base_Pinweight<T>;


	T _val;
	std::atomic<size_t> _count{1};
	bool _inplace;


	template<class Q>
	_Pinweight_Data(Q&& q, bool const inp) : _val( Forward<Q>(q) ), _inplace(inp){}	


	template<class Q>
	static auto Construct(Q&& q)
	->	_Pinweight_Data*{  return new _Pinweight_Data( Forward<Q>(q), false );  }

	template<class Q>
	static auto Construct(Q&& q, void* vp)
	->	_Pinweight_Data*{  return new(vp) _Pinweight_Data( Forward<Q>(q), true );  }


	static void Destroy(_Pinweight_Data* pdata)
	{
		if(pdata == nullptr)
			return;
		
		if(pdata->_inplace)
			pdata->_val.~T();
		else
			delete pdata;

		pdata = nullptr;
	}


	static void CountDown(_Pinweight_Data* pdata)
	{
		if(pdata != nullptr && --pdata->_count == 0)
			Destroy(pdata);
	}

	static auto CountUp(_Pinweight_Data* pdata)-> _Pinweight_Data*
	{
		if(pdata != nullptr)
			++pdata->_count;

		return pdata;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::_Base_Pinweight : public Operators_of<T const>
{
private:
	using _data_t = _Pinweight_Data<T>;


public:
	using value_type = T;
	static size_t constexpr data_size_value = sizeof(T) + 2*sizeof(size_t);


	_Base_Pinweight() 
	:	_pdata( _data_t::Construct(T()) ){  _update_val();  }

	_Base_Pinweight(T const& t)  
	:	_pdata( _data_t::Construct(t) ){  _update_val();  }

	_Base_Pinweight(T&& t) 
	:	_pdata(  _data_t::Construct( Move(t) )  ){  _update_val();  }

	_Base_Pinweight(void* vp) 
	:	_pdata( _data_t::Construct(T(), vp) ){  _update_val();  }

	_Base_Pinweight(T const& t, void* vp)  
	:	_pdata( _data_t::Construct(t, vp) ){  _update_val();  }

	_Base_Pinweight(T&& t, void* vp) 
	:	_pdata(  _data_t::Construct( Move(t), vp )  ){  _update_val();  }

	_Base_Pinweight(_Base_Pinweight const& pwb) 
	:	_pdata( _data_t::CountUp(pwb._pdata) ){  _update_val();  }

	_Base_Pinweight(_Base_Pinweight&& pwb) 
	:	_pdata(pwb._pdata){  _update_val(),  pwb._pdata = nullptr;  }


	~_Base_Pinweight(){  _data_t::CountDown(_pdata);  }


	template<  class Q, class = Enable_if_t< !is_Pinweight<Q>::value >  >
	auto operator=(Q&& q)
	->	_Base_Pinweight&{  return *this = _Base_Pinweight( Forward<Q>(q) );  }

	auto operator=(_Base_Pinweight const& pwb)-> _Base_Pinweight&
	{
		if( !share_with(pwb) )
			_update( _data_t::CountUp(pwb._pdata) );

		return *this;
	}

	auto operator=(_Base_Pinweight&& pwb)-> _Base_Pinweight&
	{
		if( !share_with(pwb) )
			_update(pwb._pdata),  
			pwb._pdata = nullptr;

		return *this;
	}


	auto v() const noexcept-> T const&{  return Operators_of<T const>::v();  }

	bool share_with(_Base_Pinweight const& pw) const{  return _pdata == pw._pdata;  }
	size_t share_count() const{  return _pdata->_count;  }


private:
	_data_t* _pdata;


	void _update_val(){  Operators_of<T const>::_p = &_pdata->_val;  }

	void _update(_data_t* pdata)
	{
		_data_t::CountDown(_pdata);
		_pdata = pdata;
		_update_val();
	}


protected:
	auto _mut_ref()-> T&{  return _pdata->_val;  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Abbreviable_t
<	sgm::Pinweight_tag
,	T, sgm::invariable_t, sgm::Pinweight_Pedigree<T, sgm::invariable_t, false, false>
> 
:	public _Base_Pinweight<T>
{
private:
	using _base_t = _Base_Pinweight<T>;


	template<class Q>
	using _condition_t 
	=	Boolean
		<	!Has_Same_Origin<Q, Abbreviable_t>::value
		&&	(is_Convertible<Q&&, T>::value || is_inherited<_base_t, Q>::value)
		>;


public:
	Abbreviable_t() = default;

	template<  class Q, class = Enable_if_t< _condition_t<Q>::value >  >
	Abbreviable_t(Q&& q) : _base_t( Forward<Q>(q) ){}

	template<  class Q, class = Enable_if_t< _condition_t<Q>::value >  >
	Abbreviable_t(Q&& q, void* vp) : _base_t( Forward<Q>(q), vp ){}

	Abbreviable_t(Abbreviable_t const& pw) : _base_t(pw){}
	Abbreviable_t(Abbreviable_t&& pw) : _base_t( Move(pw) ){}


	template<class Q>  
	auto operator=(Q)-> Abbreviable_t& = delete;

	auto operator=(Abbreviable_t const&)-> Abbreviable_t& = delete;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Abbreviable_t
<	sgm::Pinweight_tag
,	T, sgm::Variable_t, sgm::Pinweight_Pedigree<T, sgm::Variable_t, false, false>
> 
:	public _Base_Pinweight<T>
{
private:
	using _base_t = _Base_Pinweight<T>;


	template<class Q>
	using _condition_t 
	=	Boolean
		<	!Has_Same_Origin<Q, Abbreviable_t>::value
		&&	(is_Convertible<Q&&, T>::value || is_inherited<_base_t, Q>::value)
		>;


public:
	Abbreviable_t() = default;

	template<  class Q, class = Enable_if_t< _condition_t<Q>::value >  >
	Abbreviable_t(Q&& q) : _base_t( Forward<Q>(q) ){}

	template<  class Q, class = Enable_if_t< _condition_t<Q>::value >  >
	Abbreviable_t(Q&& q, void* vp) : _base_t( Forward<Q>(q), vp ){}

	Abbreviable_t(Abbreviable_t const& pw) : _base_t(pw){}
	Abbreviable_t(Abbreviable_t&& pw) : _base_t( Move(pw) ){}


	template
	<	class Q, class = Enable_if_t<  !is_Same< Decay_t<Q>, Abbreviable_t >::value  >   
	>
	auto operator=(Q&& q)
	->	Abbreviable_t&{  return static_cast<_base_t&>(*this) = Forward<Q>(q),  *this;  }

	auto operator=(Abbreviable_t const& pw)
	->	Abbreviable_t&{  return *this = static_cast<_base_t const&>(pw);  }

	auto operator=(Abbreviable_t&& pw)
	->	Abbreviable_t&{  return *this = static_cast<_base_t&&>( Move(pw) );  }


	auto mut()-> typename _base_t::value_type&
	{
		if(_base_t::share_count() > 1)
			*this = Abbreviable_t(_base_t::v());
			
		return _base_t::_mut_ref();
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template<class T>
	static auto Share(T&& t)
	->	Pinweight< Referenceless_t<T> >{  return {Forward<T>(t)};  }

	template<class T>
	static auto Share(T&& t, void* vp)
	->	Pinweight< Referenceless_t<T> >{  return {Forward<T>(t), vp};  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_PINWEIGHT_