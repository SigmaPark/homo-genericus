/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_MATHEXPR_
#define _SGM_MATHEXPR_


#include "../Type_Analysis/Type_Analysis.hpp"
#include <cstdint>


namespace sgm
{

	struct Mathexpr;

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	namespace _Mathexpr_pow_detail
	{
	
		template
		<	class BASE, std::int64_t POWER, class RES = decltype(BASE::value)
		,	int MODE
			=	POWER == 0 ? 1
			:	BASE::value == 0 ? 2
			:	(POWER < 0) ? 3
			:	/* otherwise */ 0
		>
		struct Power;

		template<class BASE, std::int64_t POWER, class RES>
		struct Power<BASE, POWER, RES, 0> : Unconstructible
		{  
			static RES constexpr value = BASE::value*Power<BASE, POWER-1, RES>::value;  
		};

		template<class BASE, std::int64_t POWER, class RES>
		struct Power<BASE, POWER, RES, 1> 
		:	Unconstructible{  static RES constexpr value = 1;  };

		template<class BASE, std::int64_t POWER, class RES>
		struct Power<BASE, POWER, RES, 2> 
		:	Unconstructible{  static RES constexpr value = 0;  };

		template<class BASE, std::int64_t POWER, class RES>
		struct Power<BASE, POWER, RES, 3> : Unconstructible
		{ 
			static RES constexpr value = RES(1) / Power<BASE, -POWER, RES>::value;  
		};

	}
}


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
	static auto constexpr pi() noexcept
	->	T{  return static_cast<T>(3.14159265358979323846264338327950288419716939937511L);  }


	template<class BASE, std::int64_t POWER, class RES = decltype(BASE::value)>
	static auto constexpr pow() noexcept
	->	RES{  return _Mathexpr_pow_detail::Power<BASE, POWER, RES>::value;  }


	template<class T, std::int64_t BASE, std::int64_t POWER>
	static auto constexpr int_pow() noexcept
	->	T{  return pow< As_value_itself<std::int64_t, BASE>, POWER, T >();  }


	template<class T, std::size_t N>
	static auto constexpr factorial() noexcept
	->	Enable_if_t<N == 0, T>{  return 1;  }

	template<class T, std::size_t N>
	static auto constexpr factorial() noexcept
	->	Enable_if_t<N != 0, T>{  return N*factorial<T, N-1>();  }


	template<class RES, std::int64_t POWER>
	static auto constexpr exp() noexcept
	->	RES{  return pow< _Base_of_Exp<RES>, POWER >();  }
};


#endif // end of #ifndef _SGM_MATHEXPR_