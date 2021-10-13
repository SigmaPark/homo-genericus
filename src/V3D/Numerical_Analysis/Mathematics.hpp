#pragma once

#ifndef _SGM_NUM_MATHEMATICS_
#define _SGM_NUM_MATHEMATICS_


#ifndef _USE_MATH_DEFINES
	#define _USE_MATH_DEFINES_FOR_THE_FIRST
	#define _USE_MATH_DEFINES
#endif

#include <cfloat>
#include <cmath>
#include <climits>

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
private:
	template<class T, unsigned BASE, unsigned POWER>
	struct _Exponential
	{
		static T constexpr value = BASE * _Exponential<T, BASE, POWER - 1>::value;
	};

	template<class T, unsigned BASE>
	struct _Exponential<T, BASE, 0>{  static T constexpr value = 1;  };


public:
	Constants() = delete;


	template<class T, unsigned POWER>
	static auto constexpr Decimal_v = _Exponential<T, 10, POWER>::value;

	template<class T, unsigned POWER>
	static auto constexpr Binary_v = _Exponential<T, 2, POWER>::value;


	template<class T>
	static auto constexpr Pi_v = static_cast<T>(M_PI);
};


#ifdef _USE_MATH_DEFINES_FOR_THE_FIRST
	#undef _USE_MATH_DEFINES
	#undef _USE_MATH_DEFINES_FOR_THE_FIRST
#endif

#endif // end of #ifndef _SGM_NUM_MATHEMATICS_