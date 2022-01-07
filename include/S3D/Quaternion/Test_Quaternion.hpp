/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#include "../Hamilton/Test_Hamilton.hpp"
#include "Quaternion.hpp"


namespace s3d::spec
{

	enum class _Equiv_Quaternion_Tag;
	
	SGM_SPECIFICATION_CLASS(Test_, Quaternion, /**/);

}


template<>
struct s3d::spec::_Equivalent<s3d::spec::_Equiv_Quaternion_Tag>
{
	template<class LHS, class RHS, class...ARGS>
	static bool calc(LHS Lhs, RHS rhs, [[maybe_unused]] ARGS...args)
	{
		if constexpr( sizeof...(ARGS) > 0 )
			return calc(Lhs, rhs) && calc(Lhs, args...);
		else if constexpr(trait::Has_Matrix_interface<LHS>::value)
			return _Equivalent<_Equiv_Hamilton_Tag>::calc(Lhs, rhs);
		else if constexpr(trait::is_Quaternion<LHS>::value)
			return calc(Lhs.w(), rhs.w()) && calc(Lhs.v(), rhs.v());
		else
			return _Equivalent<_Equiv_Number_Tag>::calc(Lhs, rhs);
	}
};