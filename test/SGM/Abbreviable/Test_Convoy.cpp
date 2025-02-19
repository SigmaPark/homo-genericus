/*  SPDX-FileCopyrightText: (c) 2023 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Abbreviable/Convoy.hpp"
#include "Test_Convoy.hpp"
#include "SGM/interface_Traits/interface_Traits.hpp"


using sgm::h2u::Specimen;


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

		SGM_H2U_ASSERT
		(	cvy == 3
		&&	s == Specimen::State::MOVE_AWAY
		);
	}
}


//========//========//========//========//=======#//========//========//========//========//=======#


SGM_HOW2USE_TESTS(sgm::h2u::Test_, Convoy, /**/)
{	Construction
};