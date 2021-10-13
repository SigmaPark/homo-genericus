#pragma once
#include "..\Hamilton\Test_Hamilton.hpp"
#include "Affine.hpp"


namespace v3d::test
{
	
	enum class _Equiv_Affine_Tag;
	struct AffineTutorial;

}


struct v3d::test::AffineTutorial
{
	AffineTutorial() = delete;

	static void test();
};


template<>
struct v3d::test::_Equivalent<v3d::test::_Equiv_Affine_Tag>
{
	template<class L, class R, class...TYPES>
	static bool calc(L Lhs, R rhs, [[maybe_unused]]TYPES...args)
	{
		if constexpr( sizeof...(TYPES) > 0 )
			return calc(Lhs, rhs) && calc(Lhs, args...);
		else if constexpr(trait::is_AffineTr_v<L>)
			return calc(Lhs.mat(), rhs.mat()) && calc(Lhs.vec(), rhs.vec());
		else
			return _Equivalent<_Equiv_Hamilton_Tag>::calc(Lhs, rhs);
	}
};