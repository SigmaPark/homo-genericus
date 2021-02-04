#pragma once

#ifndef _SGM_AVATAR_
#define _SGM_AVATAR_

#include <cassert>
#include "..\Abbreviable\Abbreviable.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	SGM_ABBREVIABLE_PROXY(Avatar);
}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, class M>
class sgm::Avatar_t< T, M, sgm::Avatar_T_Helper<T, M, false, false, false> >
{
public:
	enum : bool{IS_CONST = is_constAvatar<Avatar_t>::value};

	using value_t = Selective_t< IS_CONST, T const, T >;


	bool is_yet() const	{  return _state == State::YET;  }
	bool is_owning() const	{  return _state == State::OWNING;  }
	bool has_gone() const	{  return _state == State::GONE;  }


	Avatar_t() : _pval(nullptr), _state(State::YET){}

	template< class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	Avatar_t(Q& q) : _pval(&q), _state(State::OWNING){}

	Avatar_t(T&&) = delete;

	template<  class _M, class = Enable_if_t< IS_CONST || !Avatar_t<T, _M>::IS_CONST >  >
	Avatar_t(Avatar_t<T, _M> const& avt) : _pval(&avt.value()), _state( get_state(avt) ){}


	void distruct() SGM_NOEXCEPT{  _pval = nullptr, _state = State::GONE;  }
	~Avatar_t(){  distruct();  }


	auto value() const-> T const&	{  return *_pval;  }
	operator T const&() const		{  return value();  }

	auto value()-> Selective_t<IS_CONST, T const&, T&>{  return *_pval;  }


	auto operator=(Avatar_t const& avt)-> Avatar_t&
	{
		static_assert(!IS_CONST, "cannot bind to const Avatar_t");

		assert(is_yet() && !avt.has_gone() || is_owning() && avt.is_owning());

		*_pval = avt.value(), _state = avt._state;

		return *this;
	}


	template
	<	class Q
	,	class 
		=	Enable_if_t< !IS_CONST && !is_Avatar<Q>::value && is_Convertible<Q, T>::value >  
	>
	auto operator=(Q&& q)-> Avatar_t&
	{
		assert(is_owning() && L"Avatar_t has nothing");

		*_pval = Forward<Q>(q);

		return *this;
	}


	template<class _M>
	auto operator()(Avatar_t<T, _M> avt)-> Avatar_t
	{
		static_assert(IS_CONST || !Avatar_t<T, _M>::IS_CONST, "cannot bind to const Avatar_t");

		_pval = &avt.value(), _state = get_state(avt);

		return *this;
	}

	template< class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	auto operator()(Q& q)-> Avatar_t
	{
		static_assert
		(	IS_CONST || !is_immutable<decltype(q)>::value, "cannot bind to const Avatar_t"
		);

		assert(!has_gone() && L"Avatar_t was released already");

		_pval = &q, _state = State::OWNING;

		return *this;
	}


	template<class Q>
	bool operator==(Q&& q) const{  return *_pval == Forward<Q>(q);  }

	template<class Q>
	bool operator!=(Q&& q) const{  return !( *this == Forward<Q>(q) );  }


	template<class _M>
	bool operator==(Avatar_t<T, _M> avt) const
	{  
		assert(!has_gone() && !avt.has_gone() && L"Avatar_t was released already");

		return *this == avt.value(); 
	}

	template<class _M>
	bool operator!=(Avatar_t<T, _M> avt) const{  return !(*this == avt);  }


private:
	value_t* _pval;
	enum class State{YET, OWNING, GONE} _state;


	template<class _M>
	static auto get_state(Avatar_t<T, _M> avt)-> State
	{
		return avt.is_owning() ? State::OWNING : (avt.is_yet() ? State::YET : State::GONE);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template<  class T, class = Enable_if_t< !is_Avatar<T>::value >  >
	static auto Refer(T& t)-> Avatar< Referenceless_t<T> >
	{
		return t;
	}


	template<  class T, class = Enable_if_t< is_Avatar<T>::value >  >
	static auto Refer(T t)-> Avatar<typename T::value_t>
	{  
		return t.value();
	}


	template<class T>
	static auto CRefer(T&& t)-> decltype(  Refer( static_cast<T const&>(t) )  )
	{
		return Refer( static_cast<T const&>(t) );
	}

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_AVATAR_