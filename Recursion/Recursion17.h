/**	Macro trick for tail-recusive implementation
*/
#pragma once

#ifndef _SGM_RECURSION_
#define _SGM_RECURSION_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif


#include <tuple>

//========//========//========//========//=======#//========//========//========//========//=======#


#if defined(SGM_RECURSION) || defined(ESCAPE_IF)
	#error macro SGM_RECURSION or ESCAPE_IF (or both) is already defined elsewhere.
#else

	#define SGM_RECURSION(...)		\
		[&](auto&&...args)		\
		{	\
			auto [__VA_ARGS__] = std::tuple(args...);	\
			\
			auto self	\
			=	[&](auto&&...params)	\
				{	\
					std::tie(__VA_ARGS__) = std::forward_as_tuple(params...);	\
				};	\
			\
			auto _recursion \
			=	[&]


	
	#define ESCAPE_IF(...)		\
				};	\
			\
			while( !(__VA_ARGS__) )	\
				_recursion();

#endif
//========//========//========//========//=======#//========//========//========//========//=======#

#endif // End of #ifndef _SGM_RECURSION_