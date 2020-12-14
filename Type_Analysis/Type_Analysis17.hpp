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


#endif