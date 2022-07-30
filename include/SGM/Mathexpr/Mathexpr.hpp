/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_MATHEXPR_
#define _SGM_MATHEXPR_


#include "../interface_Traits/interface_Traits.hpp"
#include <cstdint>


namespace sgm
{

	struct Mathexpr;

}
//========//========//========//========//=======#//========//========//========//========//=======#


struct sgm::Mathexpr : Unconstructible
{
private:
	template<class T>
	struct _Base_of_Exp
	{
		static T constexpr value 
		=	static_cast<T>(2.71828182845904523536028747135266249775724709369996L);
	};


public:
	template<class T>
	static auto constexpr pi()-> T
	{
		return static_cast<T>(3.14159265358979323846264338327950288419716939937511L);
	}


	template<class BASE, std::int64_t POWER, class RES = decltype(BASE::value)>
	static auto constexpr pow()-> RES
	{
		if constexpr(POWER == 0)
			return 1;
		else if constexpr(BASE::value == 0)
			return 0;
		else if constexpr(POWER < 0)
			return RES(1) / pow<BASE, -POWER, RES>();
		else
			return BASE::value*pow<BASE, POWER-1, RES>();
	}


	template<class T, std::int64_t BASE, std::int64_t POWER>
	static auto constexpr int_pow()-> T
	{
		return pow< As_value_itself<std::int64_t, BASE>, POWER, T >();
	}


	template<class T, std::size_t N>
	static auto constexpr factorial()-> T
	{
		if constexpr(N == 0)
			return 1;
		else
			return factorial<T, N-1>();
	}


	template<class RES, std::int64_t POWER>
	static auto constexpr exp()-> RES
	{
		return pow< _Base_of_Exp<RES>, POWER >();
	}
};


#endif // end of #ifndef _SGM_MATHEXPR_