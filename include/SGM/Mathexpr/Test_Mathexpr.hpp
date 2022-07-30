/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#include "../Specification/Specification.hpp"
#include "Mathexpr.hpp"


namespace sgm::spec
{
	
	static auto constexpr e = sgm::Mathexpr::exp<double, 1>();
	static_assert( 2.7182818 < e && e < 2.7182819 ); 

}