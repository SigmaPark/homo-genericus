#pragma once

#ifndef _SGM_HIGH_TEMPLAR_INTERFACE_
#define _SGM_HIGH_TEMPLAR_INTERFACE_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

#include "..\Type_Analysis\Type_Analysis.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{
	

	enum : size_t{DYNAMIC = -1};

	template<class T, size_t N = DYNAMIC> class _Sequance_t;


	template<  class T = size_t, class = std::enable_if_t< std::is_scalar<T>::value >  >
	static auto indices(size_t N, T offset = 0)-> _Sequance_t<T>
	{
		_Sequance_t<T> res(N);

		while(N-->0)
			res >> offset++;

		return res;
	}


	


}

#endif