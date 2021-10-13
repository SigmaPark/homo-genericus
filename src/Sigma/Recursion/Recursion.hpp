#pragma once
#ifndef _SGM_RECURSION17_
#define _SGM_RECURSION17_


#include "..\Family\Family.hpp"

//	minimum versions for C++17 main syntex support.
#ifndef SGM_SYNTEX_VERSION_CPP17
	#error C++17 or higher version of language support is required.
#endif


#define RECURSION(...)	\
	(auto...args) mutable	\
	{	\
		auto _tied = sgm::Tie(args...);	\
		auto &[__VA_ARGS__] = _tied;	\
	_recursion_begin:


#define return_RECURSION(...)	\
			{	\
				_tied = sgm::Forward_as_Family(__VA_ARGS__);	\
				goto _recursion_begin;	\
			}	\
		}	\
		\
		if constexpr(false)	\
			0


#endif