#pragma once


#ifndef _SGM_TRADER_
#define _SGM_TRADER_


#include <type_traits>
#include "..\interface_Traits\interface_Traits.hpp"


namespace sgm
{
	
	template
	<	class T, class TRD
	,	bool = std::is_class_v<T> && !std::is_final_v<T> && std::is_copy_assignable_v<T>
	>
	struct Trader;

	template<class T, class TRD, bool B>
	struct _Trader_base;


	template<class _T, class _TRD>
	static auto make_Trader(_T &&t, _TRD &&trader)
	->	Trader< std::remove_reference_t<_T>, std::remove_reference_t<_TRD> >;

	template<class _T, class _TRD>
	static auto eval(Trader<_T, _TRD> const &trd)-> _T;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, class TRD>
struct sgm::Trader<T, TRD, false> : public Operator_interface<T>
{
public:
	using value_type = T;
	using type = Trader;

	Trader(Trader const&) = delete;
	~Trader(){  _trader(ref());  }

	template
	<	class Q
	,	class
		=	std::enable_if_t
			<	std::is_convertible_v<Q&&, T> && !std::is_same_v< std::decay_t<Q>, Trader >  
			>
	>
	auto operator=(Q &&q)-> Trader&{  return ref() = std::forward<Q>(q),  *this;  }

	auto operator=(Trader const &trd)-> Trader&{  return ref() = trd.eval(),  *this;  }

	auto ref()-> T&{  return _t;  }
	auto eval() const-> T{  return _t;  }

private:
	T _t;
	TRD _trader;


	template<class _T, class _TRD>
	friend auto make_Trader(_T &&t, _TRD &&trader)
	->	Trader< std::remove_reference_t<_T>, std::remove_reference_t<_TRD> >;

	template
	<	class Q
	,	class
		=	std::enable_if_t
			<	std::is_convertible_v<Q&&, T> && !std::is_same_v< std::decay_t<Q>, Trader >  
			>
	>
	Trader(Q &&q, TRD trader) : _t( std::forward<Q>(q) ), _trader(trader)
	{
		static_cast< Operator_interface<T>& >(*this) = &_t;
	}
};


template<class T, class TRD>
struct sgm::Trader<T, TRD, true> : public T
{
public:
	using value_type = T;
	using type = Trader;

	Trader(Trader const&) = delete;
	~Trader(){  _trader(ref());  }

	template
	<	class Q
	,	class
		=	std::enable_if_t
			<	std::is_convertible_v<Q&&, T> && !std::is_same_v< std::decay_t<Q>, Trader >  
			>
	>
	auto operator=(Q &&q)-> Trader&{  return ref() = std::forward<Q>(q),  *this;  }

	auto operator=(Trader const &trd)-> Trader&{  return ref() = trd.eval(),  *this;  }

	auto ref()-> T&{  return *this;  }
	auto eval() const-> T{  return *this;  }

private:
	TRD _trader;


	template<class _T, class _TRD>
	friend auto make_Trader(_T &&t, _TRD &&trader)
	->	Trader< std::remove_reference_t<_T>, std::remove_reference_t<_TRD> >;

	template
	<	class Q
	,	class
		=	std::enable_if_t
			<	std::is_convertible_v<Q&&, T> && !std::is_same_v< std::decay_t<Q>, Trader >  
			>
	>
	Trader(Q &&q, TRD trader) : T( std::forward<Q>(q) ), _trader(trader){}
};


template<class _T, class _TRD>
auto sgm::make_Trader(_T &&t, _TRD &&trader)
->	Trader< std::remove_reference_t<_T>, std::remove_reference_t<_TRD> >
{
	return{std::forward<_T>(t), std::forward<_TRD>(trader)};
}


template<class _T, class _TRD>
auto sgm::eval(sgm::Trader<_T, _TRD> const &trd)-> _T{  return trd.eval();  }


#include <utility>


namespace std
{

	template<class T, class TRD, bool B>
	auto ref(sgm::Trader<T, TRD, B> &&trd)-> T&{  return trd.ref();  }

}


#endif // end of #ifndef _SGM_TRADER_