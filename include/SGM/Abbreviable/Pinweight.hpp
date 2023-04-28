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
	_Pinweight_Data(Q&& q, bool const inp) 
	noexcept(  Aleph_Check<Q&&>::value || noexcept( T(q) )  )
	:	_val( Forward<Q>(q) ), _inplace(inp){}	


	template<class Q>
	static auto Construct(Q&& q)
	SGM_TRY_NOEXCEPT_DECLTYPE_AUTO(  new _Pinweight_Data( Forward<Q>(q), false )  )  

	template<class Q>
	static auto Construct(Q&& q, void* vp)
	SGM_TRY_NOEXCEPT(  _Pinweight_Data( Forward<Q>(q), false )  )
	->	_Pinweight_Data*{  return new(vp) _Pinweight_Data( Forward<Q>(q), true );  }


	static auto Destroy(_Pinweight_Data* pdata) noexcept-> void
	{
		if(pdata == nullptr)
			return;
		
		if(pdata->_inplace)
			pdata->_val.~T();
		else
			delete pdata;

		pdata = nullptr;
	}


	static auto CountDown(_Pinweight_Data* pdata) noexcept-> void
	{
		if(pdata != nullptr && --pdata->_count == 0)
			Destroy(pdata);
	}

	static auto CountUp(_Pinweight_Data* pdata) noexcept-> _Pinweight_Data*
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


	_data_t* _pdata;


	auto _update_val() noexcept-> void{  Operators_of<T const>::_p = Address_of(_pdata->_val);  }

	auto _update(_data_t* pdata)
	noexcept(  noexcept( _data_t::CountDown(pdata) ) && noexcept(_update_val())  )
	->	void
	{
		_data_t::CountDown(_pdata);
		_pdata = pdata;
		_update_val();
	}


protected:
	auto _mut_ref() noexcept-> T&{  return _pdata->_val;  }


public:
	using value_type = T;
	static size_t constexpr data_size_value = sizeof(T) + 2*sizeof(size_t);


	_Base_Pinweight() 
	noexcept(  noexcept( _data_t::Construct(Mock<T>()) ) && noexcept(_update_val())  )
	:	_pdata( _data_t::Construct(T()) ){  _update_val();  }

	_Base_Pinweight(T const& t) 
	noexcept(  noexcept( _data_t::Construct(t) ) && noexcept(_update_val())  )
	:	_pdata( _data_t::Construct(t) ){  _update_val();  }

	_Base_Pinweight(T&& t) noexcept
	:	_pdata(  _data_t::Construct( Move(t) )  ){  _update_val();  }

	_Base_Pinweight(void* vp) 
	noexcept(  noexcept( _data_t::Construct(Mock<T>(), vp) ) && noexcept(_update_val())  )
	:	_pdata( _data_t::Construct(T(), vp) ){  _update_val();  }

	_Base_Pinweight(T const& t, void* vp)  
	noexcept(  noexcept( _data_t::Construct(t, vp) ) && noexcept(_update_val())  )
	:	_pdata( _data_t::Construct(t, vp) ){  _update_val();  }

	_Base_Pinweight(T&& t, void* vp) noexcept
	:	_pdata(  _data_t::Construct( Move(t), vp )  ){  _update_val();  }

	_Base_Pinweight(_Base_Pinweight const& pwb) 
	noexcept(  noexcept( _data_t::CountUp(Mock<_data_t*>()) ) && noexcept(_update_val())  )
	:	_pdata( _data_t::CountUp(pwb._pdata) ){  _update_val();  }

	_Base_Pinweight(_Base_Pinweight&& pwb) SGM_TRY_NOEXCEPT(_update_val())
	:	_pdata(pwb._pdata){  _update_val(),  pwb._pdata = nullptr;  }


	~_Base_Pinweight() noexcept{  _data_t::CountDown(_pdata);  }


	auto v() const noexcept-> T const&{  return Operators_of<T const>::v();  }

	auto share_with(_Base_Pinweight const& pw) const noexcept
	->	bool{  return _pdata == pw._pdata;  }
	
	auto share_count() const noexcept-> size_t{  return _pdata->_count;  }


	auto operator=(_Base_Pinweight const& pwb)
	noexcept
	(	noexcept( !share_with(pwb) ) 
	&&	noexcept(  _update( _data_t::CountUp(pwb._pdata) )  ) 
	)
	->	_Base_Pinweight&
	{
		if( !share_with(pwb) )
			_update( _data_t::CountUp(pwb._pdata) );

		return *this;
	}

	auto operator=(_Base_Pinweight&& pwb)
	noexcept(  noexcept( !share_with(pwb) ) && noexcept( _update(pwb._pdata) )  )
	->	_Base_Pinweight&
	{
		if( !share_with(pwb) )
			_update(pwb._pdata),  
			pwb._pdata = nullptr;

		return *this;
	}

	template<  class Q, class = Enable_if_t< !is_Pinweight<Q>::value >  >
	auto operator=(Q&& q) 
	SGM_TRY_NOEXCEPT(  Mock<_Base_Pinweight>() = _Base_Pinweight( Forward<Q>(q) )  )
	->	_Base_Pinweight&{  return *this = _Base_Pinweight( Forward<Q>(q) );  }

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
	Abbreviable_t() SGM_TRY_NOEXCEPT(_base_t()) = default;

	template<  class Q, class = Enable_if_t< _condition_t<Q>::value >  >
	Abbreviable_t(Q&& q) SGM_TRY_NOEXCEPT(  _base_t( Forward<Q>(q) )  ) 
	:	_base_t( Forward<Q>(q) ){}

	template<  class Q, class = Enable_if_t< _condition_t<Q>::value >  >
	Abbreviable_t(Q&& q, void* vp) SGM_TRY_NOEXCEPT(  _base_t( Forward<Q>(q), vp )  )
	:	_base_t( Forward<Q>(q), vp ){}

	Abbreviable_t(Abbreviable_t const& pw) SGM_TRY_NOEXCEPT( _base_t(pw) ) : _base_t(pw){}
	
	Abbreviable_t(Abbreviable_t&& pw) SGM_TRY_NOEXCEPT(  _base_t( Move(pw) )  ) 
	:	_base_t( Move(pw) ){}


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
	Abbreviable_t() SGM_TRY_NOEXCEPT(_base_t()) = default;

	template<  class Q, class = Enable_if_t< _condition_t<Q>::value >  >
	Abbreviable_t(Q&& q) SGM_TRY_NOEXCEPT(  _base_t( Forward<Q>(q) )  ) 
	:	_base_t( Forward<Q>(q) ){}

	template<  class Q, class = Enable_if_t< _condition_t<Q>::value >  >
	Abbreviable_t(Q&& q, void* vp) SGM_TRY_NOEXCEPT(  _base_t( Forward<Q>(q), vp )  ) 
	:	_base_t( Forward<Q>(q), vp ){}

	Abbreviable_t(Abbreviable_t const& pw) SGM_TRY_NOEXCEPT( _base_t(pw) ) : _base_t(pw){}
	
	Abbreviable_t(Abbreviable_t&& pw) SGM_TRY_NOEXCEPT(  _base_t( Move(pw) )  ) 
	:	_base_t( Move(pw) ){}


	template
	<	class Q, class = Enable_if_t<  !is_Same< Decay_t<Q>, Abbreviable_t >::value  >   
	>
	auto operator=(Q&& q) SGM_TRY_NOEXCEPT( Mock<_base_t&>() = Forward<Q>(q) )
	->	Abbreviable_t&{  return static_cast<_base_t&>(*this) = Forward<Q>(q),  *this;  }

	auto operator=(Abbreviable_t const& pw) 
	SGM_TRY_NOEXCEPT(Mock<Abbreviable_t>() = Mock<_base_t const&>())
	->	Abbreviable_t&{  return *this = static_cast<_base_t const&>(pw);  }

	auto operator=(Abbreviable_t&& pw)
	SGM_TRY_NOEXCEPT(Mock<Abbreviable_t>() = Mock<_base_t&&>())
	->	Abbreviable_t&{  return *this = static_cast<_base_t&&>( Move(pw) );  }


	auto mut()
	noexcept
	(	noexcept(Mock<_base_t>().share_count())
	&&	noexcept( Mock<Abbreviable_t>() = Abbreviable_t(Mock<_base_t>().v()) )
	)
	->	typename _base_t::value_type&
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
	static auto Share(T&& t) SGM_TRY_NOEXCEPT( Pinweight< Referenceless_t<T> >{Forward<T>(t)} )
	->	Pinweight< Referenceless_t<T> >{  return {Forward<T>(t)};  }

	template<class T>
	static auto Share(T&& t, void* vp)
	SGM_TRY_NOEXCEPT( Pinweight< Referenceless_t<T> >{Forward<T>(t), vp} )
	->	Pinweight< Referenceless_t<T> >{  return {Forward<T>(t), vp};  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_PINWEIGHT_