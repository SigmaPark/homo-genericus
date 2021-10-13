#pragma once
#include "..\Hamilton\Test_Hamilton.hpp"
#include "Quaternion.hpp"


namespace v3d::test
{

	enum class _Equiv_Quaternion_Tag;
	struct QuaternionTutorial;

}


struct v3d::test::QuaternionTutorial
{
	QuaternionTutorial() = delete;

	static void test();
};


template<>
struct v3d::test::_Equivalent<v3d::test::_Equiv_Quaternion_Tag>
{
	template<class LHS, class RHS, class...ARGS>
	static bool calc(LHS Lhs, RHS rhs, [[maybe_unused]]ARGS...args)
	{
		if constexpr( sizeof...(ARGS) > 0 )
			return calc(Lhs, rhs) && calc(Lhs, args...);
		else if constexpr(trait::is_v3dMat_v<LHS>)
			return _Equivalent<_Equiv_Hamilton_Tag>::calc(Lhs, rhs);
		else if constexpr(trait::is_Quaternion_v<LHS>)
			return calc(Lhs.w(), rhs.w()) && calc(Lhs.v(), rhs.v());
		else
			return _Equivalent<_Equiv_Number_Tag>::calc(Lhs, rhs);
	}
};