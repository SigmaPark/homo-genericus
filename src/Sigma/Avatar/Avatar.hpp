#pragma once

#ifndef _SGM_AVATAR_
#define _SGM_AVATAR_

#include "..\Abbreviable\Abbreviable.hpp"
#include "..\interface_Traits\interface_Traits.hpp"
#include <cassert>
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	SGM_ABBREVIABLE_PROXY(Avatar);
}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, class M>
class sgm::Avatar_t< T, M, sgm::Avatar_T_Helper<T, M, false, false, false> >
:	public 
	Operator_interface
	<	Selective_t
		<	is_constAvatar
			<	Avatar_t< T, M, sgm::Avatar_T_Helper<T, M, false, false, false> >
			>::	value
		,	T const
		,	T
		>
	>
{
public:
	static bool constexpr IS_CONST = is_constAvatar<Avatar_t>::value;

	using value_t = Selective_t< IS_CONST, T const, T >;

private:
	using _opintfc_t = Operator_interface<value_t>;


public:
	Avatar_t() : _opintfc_t(nullptr), _state(State::YET){}

	template< class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	Avatar_t(Q &q) : _opintfc_t(&q), _state(State::OWNING){}

	Avatar_t(T&&) = delete;

	template<  class _M, class = Enable_if_t< IS_CONST || !Avatar_t<T, _M>::IS_CONST >  >
	Avatar_t(Avatar_t<T, _M> const &avt) : _opintfc_t(&avt.get()), _state( get_state(avt) ){}


	void distruct() noexcept
	{  
		static_cast<_opintfc_t&>(*this) = nullptr,  _state = State::GONE;  
	}

	~Avatar_t(){  distruct();  }


	auto operator=(Avatar_t const &avt)-> Avatar_t&
	{
		static_assert(!IS_CONST, "cannot bind to const Avatar_t");

		assert(is_yet() && !avt.has_gone() || is_owning() && avt.is_owning());

		static_cast<T&>(*this) = avt.get(),  _state = avt._state;

		return *this;
	}


	template
	<	class Q
	,	class 
		=	Enable_if_t< !IS_CONST && !is_Avatar<Q>::value && is_Convertible<Q, T>::value >  
	>
	auto operator=(Q &&q)-> Avatar_t&
	{
		assert(is_owning() && L"Avatar_t has nothing");

		static_cast<T&>(*this) = Forward<Q>(q);

		return *this;
	}


	bool is_yet() const{  return _state == State::YET;  }
	bool is_owning() const{  return _state == State::OWNING;  }
	bool has_gone() const{  return _state == State::GONE;  }

	auto get() const-> T const&{  return static_cast<T const&>(*this);  }
	auto get()-> value_t&{  return static_cast<value_t&>(*this);  }
	

	template<class _M>
	auto reset(Avatar_t<T, _M> avt)-> Avatar_t
	{
		static_assert(IS_CONST || !Avatar_t<T, _M>::IS_CONST, "cannot bind to const Avatar_t");

		static_cast<_opintfc_t&>(*this) = &avt.get(),  _state = get_state(avt);

		return *this;
	}

	template< class Q, class = Enable_if_t< !is_Avatar<Q>::value >  >
	auto reset(Q &q)-> Avatar_t
	{
		static_assert(IS_CONST || !is_immutable<Q>::value, "cannot bind to const Avatar_t");

		assert(!has_gone() && L"Avatar_t was released already");

		static_cast<_opintfc_t&>(*this) = &q,  _state = State::OWNING;

		return *this;
	}


private:
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
	static auto Refer(T &t)-> Avatar< Referenceless_t<T> >{  return t;  }

	template<  class T, class = Enable_if_t< is_Avatar<T>::value >  >
	static auto Refer(T t)-> Avatar<typename T::value_t>{  return t.get();  }

	template<class T>
	static auto CRefer(T &&t)-> SGM_DECLTYPE_AUTO(  Refer( static_cast<T const&>(t) )  )

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_AVATAR_