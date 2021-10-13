#pragma once
#include "V3D\Test_v3d.hpp"
#include "Euclid.hpp"


namespace v3d::test
{
	
	enum class _Equiv_Euclid_Tag;
	struct EuclidTutorial;

}


struct v3d::test::EuclidTutorial
{
	EuclidTutorial() = delete;

	static void test();
};


template<>
struct v3d::test::_Equivalent<v3d::test::_Equiv_Euclid_Tag>
{
	template<class L, class R, class...TYPES>
	static bool calc(L Lhs, R rhs, [[maybe_unused]]TYPES...args)
	{
		if constexpr( sizeof...(TYPES) > 0 )
			return calc(Lhs, rhs) && calc(Lhs, args...);
		else if constexpr(trait::has_Matrix_interface_v<L> && trait::has_Matrix_interface_v<R>)
		{
			bool res = Lhs.size() == rhs.size();

			for(size_t idx = Lhs.size();  res && idx-->0;)
				res = _Equivalent<_Equiv_Number_Tag>::calc( Lhs(idx), rhs(idx) );

			return res;
		}
		else if constexpr(trait::is_Plane_v<L> && trait::is_Plane_v<R>)
			return 
			(	Direction::are_parallel(Lhs.normal(), rhs.normal()) 
			&&	(	calc(rhs.position(), Lhs.position())
				||	Direction::are_orthogonal(Lhs.normal(), rhs.position() - Lhs.position())
				)
			);
		else if constexpr(trait::is_Line_v<L> && trait::is_Line_v<R>)
			return 
			(	Direction::are_parallel(Lhs.tangent(), rhs.tangent()) 
			&&	(	calc(rhs.position(), Lhs.position())
				||	Direction::are_parallel(Lhs.tangent(), rhs.position() - Lhs.position())
				)
			);			
		else
			return _Equivalent<_Equiv_Number_Tag>::calc(Lhs, rhs);
	}
};
