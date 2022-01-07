/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once

#include "../Test_s3d.hpp"
#include "Hamilton.hpp"
#include "SGM/Specification/Specification.hpp"


namespace s3d::spec
{
	
	enum class _Equiv_Hamilton_Tag;

	SGM_SPECIFICATION_CLASS(Test_, Hamilton, /**/);

}


template<>
struct s3d::spec::_Equivalent<s3d::spec::_Equiv_Hamilton_Tag>
{
	template<class L, class R, class...TYPES>
	static bool calc(L Lhs, R rhs, TYPES...args)
	{
		if constexpr( sizeof...(TYPES) > 0 )
			return calc(Lhs, rhs) && calc(Lhs, args...);
		else if constexpr(trait::Has_Matrix_interface<L>::value)
		{
			bool res = Lhs.rows() == rhs.rows() && Lhs.cols() == rhs.cols();
				
			for(size_t i = Lhs.rows();  i-->0;)
				for(size_t j = Lhs.cols();  res && j-->0;)
					res = calc( Lhs(i, j), rhs(i, j) );

			return res;
		}
		else if constexpr(trait::is_complex<L>::value || trait::is_complex<R>::value)
		{
			using real_t = Decay_t<decltype( std::real(Lhs) )>;
			using complex_t = std::complex<real_t>;

			auto const c1 = static_cast<complex_t>(Lhs), c2 = static_cast<complex_t>(rhs);

			return calc(c1.real(), c2.real()) && calc(c1.imag(), c2.imag());
		}
		else
			return _Equivalent<_Equiv_Number_Tag>::calc(Lhs, rhs);
	}
};