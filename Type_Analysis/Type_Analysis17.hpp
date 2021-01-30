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
		struct is_##NAME : sgm::False_t{  is_##NAME() = delete;  };	\
		\
		template<TEM_SIGNATURES>	\
		struct is_##NAME< NAME<TEM_PARAMS> > : sgm::True_t{  is_##NAME() = delete;  };	\
		\
		template<class T>	\
		static bool constexpr is_##NAME##_v = is_##NAME< sgm::Decay_t<T> >::value

#else
	#error SGM_USER_DEFINED_TYPE_CHECK was already defined somewhere else.
#endif
//========//========//========//========//=======#//========//========//========//========//=======#


#if !defined(_SGM_TEMPLATE_ALIAS) && !defined(_SGM_DECAY_TEMPLATE_ALIAS)
	#define _SGM_TEMPLATE_ALIAS(TITLE)	\
		template<class...ARGS>	\
		static auto constexpr is##TITLE##_v = is##TITLE<ARGS...>::value

	#define _SGM_DECAY_TEMPLATE_ALIAS(TITLE)	\
		template<class...ARGS>	\
		static auto constexpr is##TITLE##_v = is##TITLE< Decay_t<ARGS>... >::value


	namespace sgm
	{
		_SGM_TEMPLATE_ALIAS(Same);
		_SGM_TEMPLATE_ALIAS(Convertible);
		_SGM_TEMPLATE_ALIAS(Const);
		_SGM_TEMPLATE_ALIAS(LvalueReference);
		_SGM_TEMPLATE_ALIAS(RvalueReference);
		_SGM_TEMPLATE_ALIAS(Volatile);

		_SGM_DECAY_TEMPLATE_ALIAS(Pointer);
		_SGM_DECAY_TEMPLATE_ALIAS(Class);
		_SGM_DECAY_TEMPLATE_ALIAS(_inherited_from);
		_SGM_DECAY_TEMPLATE_ALIAS(Void);
		_SGM_DECAY_TEMPLATE_ALIAS(None);
		_SGM_DECAY_TEMPLATE_ALIAS(_Mutability);
		_SGM_DECAY_TEMPLATE_ALIAS(_immutable);
	}
		

	#undef _SGM_DECAY_TEMPLATE_ALIAS
	#undef _SGM_TEMPLATE_ALIAS
#else
	#error _SGM_TEMPLATE_ALIAS/_SGM_DECAY_TEMPLATE_ALIAS was already defined \
	somewhere else.
#endif


#endif