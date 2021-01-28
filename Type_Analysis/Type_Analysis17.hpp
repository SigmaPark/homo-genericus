#pragma once

#ifndef _SGM_TYPE_ANALYSIS17_
#define _SGM_TYPE_ANALYSIS17_


#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version of language support is required.
#endif

#include "Type_Analysis.hpp"


#ifndef SGM_COMPILE_FAILED
	#define SGM_COMPILE_FAILED(...) \
		static_assert([]() constexpr{  return false;  }(), #__VA_ARGS__)
#else
	#error SGM_COMPILE_FAILED was already defined somewhere else.
#endif
//========//========//========//========//=======#//========//========//========//========//=======#


#ifndef SGM_USER_DEFINED_TYPE_CHECK
	#define SGM_USER_DEFINED_TYPE_CHECK(NAME, TEM_SIGNATURES, TEM_PARAMS)		\
		template<class>	\
		struct _is_##NAME : sgm::False_t{  _is_##NAME() = delete;  };	\
		\
		template<TEM_SIGNATURES>	\
		struct _is_##NAME< NAME<TEM_PARAMS> > : sgm::True_t{  _is_##NAME() = delete;  };	\
		\
		template<class T>	\
		static bool constexpr is_##NAME##_v = _is_##NAME< sgm::Decay_t<T> >::value

#else
	#error SGM_USER_DEFINED_TYPE_CHECK was already defined somewhere else.
#endif


#endif