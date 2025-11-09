/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Utility/Mathexpr.hpp"
#include "Test_Mathexpr.hpp"


using namespace h2u;


static void intro()
{
	mdo
	<<	Title(L"Introduction") 
	<<	L"sgm::Mathexpr supports some fundamental mathematical functions which operate "
	<<	L"in \"compile time\", not in runtime."
	<<	empty_line;
}


static void constexpr_pi()
{
	mdo << Title(LR"($ \pi $)");

BEGIN_CODE_BLOCK(constexpr_pi_ex)
	auto constexpr pi_f = sgm::Mathexpr::pi<float>();
	auto constexpr pi_d = sgm::Mathexpr::pi<double>();
	auto constexpr pi_ld = sgm::Mathexpr::pi<long double>();

	static_assert
	(	(	3.141592f < pi_f && pi_f < 3.141593f
		&&	3.1415926535897 < pi_d && pi_d < 3.1415926535898
		&&	3.1415926535897 < pi_ld && pi_ld < 3.1415926535898
		)
	,	"sgm::Mathexpr::pi gives pi as a compile time constant."
	);
END_CODE_BLOCK_AND_LOAD(constexpr_pi_ex)

	mdo << empty_line;
}


static void constexpr_factorial()
{
	mdo << Title(L"Factorial Function");

BEGIN_CODE_BLOCK(factorial_ex)
	auto constexpr n0 = sgm::Mathexpr::factorial<int, 0>();
	auto constexpr n1 = sgm::Mathexpr::factorial<unsigned int, 4>();
	auto constexpr n2 = sgm::Mathexpr::factorial<long long, 4>();

	static_assert
	(	(	std::is_same<decltype(n0), int const>::value && n0 == 1
		&&	std::is_same<decltype(n1), unsigned int const>::value && n1 == 4*3*2*1
		&&	std::is_same<decltype(n2), long long const>::value && n2 == 4*3*2*1
		)
	,	"factorial function in compile time."
	);
END_CODE_BLOCK_AND_LOAD(factorial_ex)

	mdo << empty_line;
}


BEGIN_CODE_BLOCK(1/2_as_base_ex)
struct one_over_two{  static double constexpr value = 0.5;  };
END_CODE_BLOCK(1/2_as_base_ex)

static void constexpr_pow()
{
	mdo 
	<<	Title(L"Power Functions with Integer Power")
	<<	Title(L"Real Number Base", 2)
	<<	Load_code_block(L"1/2_as_base_ex") << newl;

BEGIN_CODE_BLOCK(pow_with_real_base_ex)
	static_assert
	(	(	sgm::Mathexpr::pow<one_over_two, 0>() == 1.0
		&&	sgm::Mathexpr::pow<one_over_two, 3>() == 0.5*0.5*0.5
		&&	sgm::Mathexpr::pow<one_over_two, -3>() == 1.0/(0.5*0.5*0.5)
		)
	,	"power function with a base number object whose constexpr static member value is real."
	);
END_CODE_BLOCK_AND_LOAD(pow_with_real_base_ex)

	mdo << empty_line;


	mdo << Title(L"Integer Base", 2);

BEGIN_CODE_BLOCK(pow_with_int_base_ex)
	auto constexpr n1 = sgm::Mathexpr::int_pow<int, 2, 0>();
	auto constexpr n2 = sgm::Mathexpr::int_pow<wchar_t, 2, 7>();
	auto constexpr n3 = sgm::Mathexpr::int_pow<long long, 2, 10>();

	static_assert
	(	(	std::is_same<decltype(n1), int const>::value && n1 == 1
		&&	std::is_same<decltype(n2), wchar_t const>::value && n2 == 2*2*2*2*2*2*2
		&&	std::is_same<decltype(n3), long long const>::value 
		&&	n3 == 2*2*2*2*2*2*2*2*2*2
		)
	,	"sgm::Mathexpr::int_pow is easier to use when the base number is integral number."
	);
END_CODE_BLOCK_AND_LOAD(pow_with_int_base_ex)

	mdo << empty_line;


	mdo << Title(L"Exponential Function", 2);

BEGIN_CODE_BLOCK(expoential_ex)
	auto constexpr e1_d = sgm::Mathexpr::exp<double, 1>();
	auto constexpr e1_f = sgm::Mathexpr::exp<float, 1>();
	auto constexpr e3_d = sgm::Mathexpr::exp<double, 3>();

	static_assert
	(	(	std::is_same<decltype(e1_d), double const>::value 
		&&	2.7182818284 < e1_d && e1_d < 2.7182818285
		&&	std::is_same<decltype(e1_f), float const>::value
		&&	2.71828f < e1_f && e1_f < 2.718289f
		&&	e1_d*e1_d*e1_d - 1e-12 < e3_d && e3_d < e1_d*e1_d*e1_d + 1e-12
		)
	,	"same to std::exp but operates in compile time."
	);
END_CODE_BLOCK_AND_LOAD(expoential_ex)

	mdo << empty_line;
}
//========//========//========//========//=======#//========//========//========//========//=======#


H2U_HOW2USE_TESTS(sgm::test::Test_, Mathexpr, /**/)
{	::intro
,	::constexpr_pi
,	::constexpr_factorial
,	::constexpr_pow
};