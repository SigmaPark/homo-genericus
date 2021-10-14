#pragma once
#include <cfloat>
#include <type_traits>
#include "Sigma\Version\Version.hpp"

#ifndef SGM_SYNTEX_VERSION_CPP17
	#error C++17 or higher version of language support is required.
#endif


namespace v3d::test
{
	
	enum class _Equiv_Number_Tag;
	template<class TAG>  struct _Equivalent;

}


template<>
struct v3d::test::_Equivalent<v3d::test::_Equiv_Number_Tag>
{
private:
	template<class T>
	static auto constexpr _epsilon() noexcept
	{
		static_assert(std::is_floating_point_v<T>, "NOT a Real Number.");

		auto constexpr res 
		=	std::is_same_v<T, double> ? T(DBL_EPSILON)
		:	std::is_same_v<T, long double> ? T(LDBL_EPSILON)
		:	/* otherwise */ T(FLT_EPSILON);

		return res;
	}

public:
	template<class L, class R, class...TYPES>
	static bool calc(L Lhs, R rhs, TYPES...args)
	{
		if constexpr( sizeof...(TYPES) > 0 )
			return calc(Lhs, rhs) && calc(Lhs, args...);
		else if constexpr(std::is_integral_v<L> || std::is_pointer_v<L>)
			return Lhs == rhs;
		else if constexpr(std::is_floating_point_v<L>)
			return abs(Lhs - rhs) < static_cast<L>(1e3) * _epsilon<L>();
		else
			void(*const compile_fail)() = []{  static_assert(false, "no method to compare them.");  };
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------