/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#include "../Test_s3d.hpp"
#include "SGM/Specification/Specification.hpp"
#include "Euclid.hpp"


namespace s3d::spec
{
	
	enum class _Equiv_Euclid_Tag;

	SGM_SPECIFICATION_CLASS(Test_, Euclid, /**/);

}


template<>
struct s3d::spec::_Equivalent<s3d::spec::_Equiv_Euclid_Tag>
{
	template<class L, class R, class...TYPES>
	static bool calc(L Lhs, R rhs, [[maybe_unused]] TYPES...args)
	{
		if constexpr( sizeof...(TYPES) > 0 )
			return calc(Lhs, rhs) && calc(Lhs, args...);
		else if constexpr
		(	trait::Has_Matrix_interface<L>::value 
		&&	trait::Has_Matrix_interface<R>::value
		)
		{
			bool res = Lhs.size() == rhs.size();

			for(size_t idx = Lhs.size();  res && idx-->0;)
				res = _Equivalent<_Equiv_Number_Tag>::calc( Lhs(idx), rhs(idx) );

			return res;
		}
		else if constexpr(trait::is_Plane<L>::value && trait::is_Plane<R>::value)
			return 
			(	Direction::are_parallel(Lhs.normal(), rhs.normal()) 
			&&	(	calc(rhs.position(), Lhs.position())
				||	Direction::are_orthogonal(Lhs.normal(), rhs.position() - Lhs.position())
				)
			);
		else if constexpr(trait::is_Line<L>::value && trait::is_Line<R>::value)
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
