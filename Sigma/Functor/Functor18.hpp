#ifndef _SGM_F18_FUNCTOR_
#define _SGM_F18_FUNCTOR_

#include "Multiple.hpp"


namespace sgm::f18
{
	
	using fp::Multiple;

	struct Blank;

	template<class closure_t, class invoker_t>
	struct FnExpr;

	struct As_Functor;

	template<class hFXP>
	class Functor;

	template<unsigned... INDICES>
	struct indexer;

	template<class mtp_t = Multiple<>, unsigned... INDICES>
	struct Closure;

}


namespace sgm::f18::trait
{
	
	template<class T>
	static bool constexpr is_Blank_v = is_Same_v< Decay_t<T>, Blank >;


	template<   class T, class = Boolean_type<  is_Same_v< T, Decay_t<T> >  >   >
	struct is_Functor;

	template<class T>
	static bool constexpr is_Functor_v = is_Functor<T>::value;


	
	template<unsigned QN, class>
	struct _Find_Heler;

}


template<class FN>
static decltype(auto) constexpr operator/(FN&& fn, sgm::f18::As_Functor) 
noexcept(sgm::is_RvalueReference<FN&&>::value);
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


template<class T>
struct sgm::f18::trait::is_Functor<T, sgm::False_t> : sgm::f18::trait::is_Functor< Decay_t<T> >{};

template<class T>
struct sgm::f18::trait::is_Functor<T, sgm::True_t> : False_t{};

template<class FXP>
struct sgm::f18::trait::is_Functor< sgm::f18::Functor<FXP>, sgm::True_t > : True_t{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<unsigned... INDICES>
struct sgm::f18::indexer
{
	constexpr indexer() = default;
};


#endif