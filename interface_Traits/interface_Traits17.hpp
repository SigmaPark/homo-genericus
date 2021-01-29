#pragma once

#ifndef _SGM_INTERFACE_TRAITS17_
#define _SGM_INTERFACE_TRAITS17_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version of language support is required.
#endif

#include "..\Type_Analysis\Type_Analysis17.hpp"
#include "interface_Traits.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	
	template<class FUNC, class...ARGS>
	static size_t constexpr _Nof_Arguments();


	template<class FUNC, class...ARGS>
	static size_t constexpr nof_Arguments_v = _Nof_Arguments<FUNC, ARGS...>();

}


#include <type_traits>


template<class FUNC, class...ARGS>
size_t constexpr sgm::_Nof_Arguments()
{
	size_t constexpr NOF_ARGS = sizeof...(ARGS);

	if constexpr(std::is_invocable_v<FUNC, ARGS...>)
		return NOF_ARGS;
	else if constexpr
	(	size_t constexpr MAX_NOF_ARGS = 30, NOT_INVOCABLE = ULLONG_MAX
	;	NOF_ARGS > MAX_NOF_ARGS || NOF_ARGS == 0
	)
		return NOT_INVOCABLE;
	else
		return _Nof_Arguments< FUNC, First_t<ARGS...>, ARGS... >();
}


#endif