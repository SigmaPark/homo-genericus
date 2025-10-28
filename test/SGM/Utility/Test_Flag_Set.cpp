/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Utility/Flag_Set.hpp"
#include "Test_Flag_Set.hpp"
#include <type_traits>


static void Test01()
{
	auto fs = sgm::Flags(0.f, 0.0, 0);

	auto x1 = sgm::Satisfying_Flag<std::is_integral>(fs);
	auto x2 = sgm::Satisfying_Flag<std::is_pointer>(fs);
	auto x3 = sgm::Pick_Flag<float>(fs);
	auto x4 = sgm::Pick_Flag<int*>(fs);

	static_assert
	(	(	std::is_same<decltype(x1), int>::value
		&&	sgm::is_None<decltype(x2)>::value
		&&	std::is_same<decltype(x3), float>::value
		&&	sgm::is_None<decltype(x4)>::value
		)
	,	"" 
	);	
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_HOW2USE_TESTS(sgm::h2u::Test_, Flag_Set, /**/)
{	::Test01
};