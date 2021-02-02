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


#if defined(SGM_USER_DEFINED_TYPE_CHECK) && !defined(SGM_USER_DEFINED_TYPE_CHECK17)
	#define SGM_USER_DEFINED_TYPE_CHECK17(PRE, TITLE, TEM_SIGNATURES, TEM_PARAMS)	\
		SGM_USER_DEFINED_TYPE_CHECK(PRE, TITLE, TEM_SIGNATURES, TEM_PARAMS);	\
		\
		template<class...ARGS>	\
		static auto constexpr is##PRE##TITLE##_v = is##PRE##TITLE< Decay_t<ARGS>... >::value

#else
	#error SGM_USER_DEFINED_TYPE_CHECK17 was already defined somewhere else.
#endif
//========//========//========//========//=======#//========//========//========//========//=======#


#if !defined(_SGM_TEMPLATE_ALIAS) && !defined(_SGM_DECAY_TEMPLATE_ALIAS)
	#define _SGM_TEMPLATE_ALIAS(PRE, TITLE)	\
		template<class...ARGS>	\
		static auto constexpr is##PRE##TITLE##_v = is##PRE##TITLE<ARGS...>::value

	#define _SGM_DECAY_TEMPLATE_ALIAS(PRE, TITLE)	\
		template<class...ARGS>	\
		static auto constexpr is##PRE##TITLE##_v = is##PRE##TITLE< Decay_t<ARGS>... >::value


	namespace sgm
	{
		_SGM_TEMPLATE_ALIAS(_, Same);
		_SGM_TEMPLATE_ALIAS(_, Convertible);
		_SGM_TEMPLATE_ALIAS(_, Const);
		_SGM_TEMPLATE_ALIAS(_, LvalueReference);
		_SGM_TEMPLATE_ALIAS(_, RvalueReference);
		_SGM_TEMPLATE_ALIAS(_, Reference);
		_SGM_TEMPLATE_ALIAS(_, Volatile);

		_SGM_DECAY_TEMPLATE_ALIAS(_, Pointer);
		_SGM_DECAY_TEMPLATE_ALIAS(_, Class);
		_SGM_DECAY_TEMPLATE_ALIAS(_, inherited_from);
		_SGM_DECAY_TEMPLATE_ALIAS(_, Void);
		_SGM_DECAY_TEMPLATE_ALIAS(_, None);
		_SGM_DECAY_TEMPLATE_ALIAS(_, Mutability);
		_SGM_DECAY_TEMPLATE_ALIAS(_, immutable);
	}
		

	#undef _SGM_DECAY_TEMPLATE_ALIAS
	#undef _SGM_TEMPLATE_ALIAS
#else
	#error _SGM_TEMPLATE_ALIAS/_SGM_DECAY_TEMPLATE_ALIAS was already defined \
	somewhere else.
#endif


#endif