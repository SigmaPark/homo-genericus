/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Queue.hpp"
#include "Queue.hpp"


using sgm::spec::Specimen;


static void Constructions()
{
	sgm::Queue<Specimen> qu;

	sgm::spec::is_True( qu.is_empty() );
}


static void Push_and_Pop()
{
	{
		sgm::Queue<Specimen> qu;

		qu.push( Specimen(1) );
		qu.push( Specimen(3) );

		sgm::spec::is_True(qu.size() == 2 && qu.front() == 1 && qu.back() == 3);

		qu.push( Specimen(5) );

		sgm::spec::is_True(qu.size() == 3 && qu.front() == 1 && qu.back() == 5);

		qu.pop();

		sgm::spec::is_True(qu.size() == 2 && qu.front() == 3 && qu.back() == 5);

		qu.pop();

		sgm::spec::is_True(qu.size() == 1 && qu.front() == 5 && qu.back() == 5);
	}
}



//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Queue, /**/)
{	::Constructions
,	::Push_and_Pop
};