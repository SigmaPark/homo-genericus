/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Boomerang/Boomerang.hpp"
#include "Test_Boomerang.hpp"
#include "SGM/interface_Traits/interface_Traits.hpp"


using sgm::h2u::Specimen;


static void Test01()
{
	int x = 5, y = 0;

	{
		auto&& bar = sgm::throw_Boomerang( x, [&y](int const& t){  y = 2*t;  } );

		SGM_H2U_ASSERT(bar == 5);

		sgm::Move(bar) = 10;
	}

	SGM_H2U_ASSERT(y == 2*10);
}


static void Test02()
{
	Specimen s(1);
	int y = 0;

	sgm::throw_Boomerang( s, [&y](Specimen const& s){  y = s.value() * 2;  } ).value() = 3;

	SGM_H2U_ASSERT(y == 3*2);
}


static void Test03()
{
	Specimen s(2);
	int x = 0;

	auto foo_f = [](int& t){  t = 5;  };

	static_assert(!sgm::Has_Operator_invocation<decltype(foo_f), Specimen>::value, "");

	foo_f
	(	std::ref(  sgm::throw_Boomerang( x, [&s](int const t){  s.value() = t;  } )  )
	);

	SGM_H2U_ASSERT(s.value() == 5);
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_HOW2USE_TESTS(sgm::h2u::Test_, Boomerang, /**/)
{	::Test01
,	::Test02
,	::Test03
};