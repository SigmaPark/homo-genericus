#pragma once

#ifndef _SGM_NUM_MATHEMATICS_
#define _SGM_NUM_MATHEMATICS_

#include <cfloat>

namespace sgm
{
	namespace num
	{
		struct Constants;
	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


struct sgm::num::Constants
{
	sgm::num::Constants() = delete;


	template<unsigned POWER>
	struct Decimal{  enum{value = 10 * Decimal<POWER - 1>::value};  };

	template<>
	struct Decimal<0>{  enum{value = 1};  };


	template<class T, unsigned TOLERANCE_LEVEL = 2>
	static auto Epsilon()-> T const&
	{
		static T const ep 
		=	static_cast<T>(Decimal<TOLERANCE_LEVEL>::value) * std::numeric_limits<T>::epsilon();

		return ep;
	}


	template<class T = float>
	static auto Pi()-> T const&
	{
		static T const pi = 2 * acos( T(0) );

		return pi;
	}

};

#endif