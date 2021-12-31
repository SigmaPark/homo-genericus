/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#include "../Hamilton/Test_Hamilton.hpp"
#include "Affine.hpp"


namespace s3d::spec
{
	
	enum class _Equiv_Affine_Tag;
	
	SGM_SPECIFICATION_CLASS(Test_, Affine, /**/);

}


template<>
struct s3d::spec::_Equivalent<s3d::spec::_Equiv_Affine_Tag>
{
	template<class L, class R, class...TYPES>
	static bool calc(L Lhs, R rhs, [[maybe_unused]] TYPES...args)
	{
		if constexpr( sizeof...(TYPES) > 0 )
			return calc(Lhs, rhs) && calc(Lhs, args...);
		else if constexpr(trait::is_AffineTr<L>::value)
			return calc(Lhs.mat(), rhs.mat()) && calc(Lhs.vec(), rhs.vec());
		else
			return _Equivalent<_Equiv_Hamilton_Tag>::calc(Lhs, rhs);
	}
};