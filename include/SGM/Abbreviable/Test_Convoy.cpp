/*  SPDX-FileCopyrightText: (c) 2023 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Convoy.hpp"
#include "Test_Convoy.hpp"


using sgm::spec::Specimen;


static void Construction()
{
	{
		static_assert
		(	!sgm::Has_Operator_New< sgm::Convoy<Specimen>, Specimen const& >::value
		,	""
		);
	}
	{
		Specimen s{3};

		sgm::Convoy<Specimen> cvy = sgm::Move(s);

		SGM_SPEC_ASSERT
		(	cvy == 3
		&&	s == Specimen::State::MOVE_AWAY
		);
	}
}


//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Convoy, /**/)
{	Construction
};