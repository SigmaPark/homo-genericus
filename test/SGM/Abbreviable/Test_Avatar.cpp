/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Abbreviable/Avatar.hpp"
#include "Test_Avatar.hpp"


using sgm::h2u::Specimen;


static void Test01()
{
	//{
	//	sgm::Avatar<int> constexpr avt{};

	//	SGM_H2U_ASSERT(!avt.is_valid());
	//}
	{
		Specimen s(2);
		auto avt = sgm::Refer(s);

		s = 22;

		SGM_H2U_ASSERT(avt == s && s == 22);
	}
	{
		Specimen s(2);
		Specimen const cs(4);

		auto cavt = sgm::Refer(cs);

		//cavt = 44;	// Compile Ban

		SGM_H2U_ASSERT(cavt == cs);
	}
	{
		Specimen s(2);

		auto cavt = sgm::CRefer(s);

		static_assert
		(	(	sgm::is_constAvatar<decltype(cavt)>::value
			&&	sgm::is_Same<decltype(cavt.v()), Specimen const&>::value
			)
		,	""
		);
	}
}


static void Test02()
{
	Specimen s(2);
	sgm::constAvatar<Specimen> cavt = sgm::Avatar<Specimen>(s);

	//cavt = Avatar<Specimen>();	// Compile Failed.

	static_assert( sgm::is_constAvatar<decltype(cavt)>::value, " cavt is not constant." );
}


static void Test03()
{
	static_assert
	(	(	std::is_convertible< sgm::Avatar<double>, sgm::Avatar<double const> >::value
		&&	!std::is_convertible< sgm::Avatar<double const>, sgm::Avatar<double> >::value
		&&	std::is_convertible< sgm::Avatar<double>, sgm::constAvatar<double> >::value
		&&	!std::is_convertible< sgm::constAvatar<double>, sgm::Avatar<double> >::value
		)
	,	""
	);
}


static void Test04()
{
	Specimen x(2);
	sgm::constAvatar<Specimen> avt1(x);

	x = 4;

	SGM_H2U_ASSERT(avt1 == 4);
}


static void Test05()
{
	using sgm::Avatar;
	using sgm::constAvatar;

	static_assert
	(	(	sgm::is_constAvatar<   Avatar<  Avatar< Avatar<Specimen> const >  >   >::value
		&&	sgm::is_constAvatar<   Avatar<  Avatar< Avatar<Specimen const> >  >   >::value
		&&	sgm::is_constAvatar<  constAvatar< Avatar<Specimen> >  >::value
		&&	sgm::is_constAvatar<   Avatar<  Avatar< Avatar<Specimen> > const  >   >::value
		&&	!sgm::is_constAvatar<  Avatar< Avatar<Specimen> > const  >::value
		)
	,	""
	);


	Specimen s(2);

	constAvatar<Specimen> avt1 = s;

	auto avt2 = sgm::Refer(avt1);

	SGM_H2U_ASSERT(avt1 == 2 && avt2 == 2);

	static_assert( sgm::is_Same<decltype(avt2.v()), Specimen const&>::value, "" );
}


static void Test06()
{
	int x1 = 2, x2 = 4;
	Specimen s(6);

	sgm::Avatar<int> avt1 = x1, avt2 = x2;
	sgm::Avatar<Specimen> avts = s;

	auto y1 = avt1 + avt2;
	auto y2 = avt2 - avt1;
	auto b = avt1 > avt2;

	static_assert
	(	(	sgm::is_Same<decltype(y1), int>::value
		&&	sgm::is_Same<decltype(y2), int>::value
		&&	sgm::is_Same<decltype(b), bool>::value
		)
	,	""
	);

	SGM_H2U_ASSERT(y1 == 2+4 && y2 == 4-2 && b == false && avts.v().value() == 6);
}


static void Test07()
{
	Specimen s1(2), s2(4), s3(6), s4(8), s5(10);
	sgm::Avatar<Specimen> avt1 = s1, avt2 = s2, avt3 = s3, avt4 = s4, avt5 = s5;

	[](Specimen t1, Specimen& t2, Specimen const& t3, Specimen&& t4, Specimen const&& t5)
	{
		SGM_H2U_ASSERT(t1 == 2 && t2 == 4 && t3 == 6 && t4 == 8 && t5 == 10);
	}( s1, s2, s3, sgm::Move(s4), sgm::Move(s5) );
}


static void Test08()
{
	Specimen s(2);

	auto avt = sgm::Refer(s);

	static_assert( sgm::is_Same<decltype(&s), decltype(&avt)>::value, "" );

	SGM_H2U_ASSERT(&s == &avt);
}


static void Test09()
{
	{
		Specimen s(2);

		auto avt = sgm::Refer(s);

		Specimen s2 = sgm::Move(avt);

		SGM_H2U_ASSERT(s2 == 2 && s == Specimen::State::MOVE_AWAY);
	}
	{
		Specimen s(2);

		auto avt = sgm::Refer(s);

		Specimen s2 = sgm::Move(avt);

		SGM_H2U_ASSERT(s2 == 2 && s == Specimen::State::MOVE_AWAY);
	}
}


static void Test10()
{
	{
		Specimen s(2);

		sgm::Avatar<void> va = s;

		static_cast<Specimen&>(va).value() = 5;

		SGM_H2U_ASSERT(s == 5);
	}
	{
		Specimen s(2);

		sgm::constAvatar<void> cva = s;

		Specimen const& rs = static_cast<Specimen const&>(cva);

		SGM_H2U_ASSERT(rs == 2);
		
		s.value() = 5;

		SGM_H2U_ASSERT(rs == 5);
	}
	{
		Specimen s(2);

		sgm::Avatar<void> va = sgm::Refer(s);

		static_cast<Specimen&>(va).value() = 5;

		SGM_H2U_ASSERT(s == 5);
	}
	{
		Specimen s(2);

		sgm::constAvatar<void> cva = sgm::CRefer(s);

		Specimen const& rs = static_cast<Specimen const&>(cva);

		SGM_H2U_ASSERT(rs == 2);
		
		s.value() = 5;

		SGM_H2U_ASSERT(rs == 5);
	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_HOW2USE_TESTS(sgm::h2u::Test_, Avatar, /**/)
{	::Test01
,	::Test02
,	::Test03
,	::Test04
,	::Test05
,	::Test06
,	::Test07
,	::Test08
,	::Test09
,	::Test10
};