/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Family.hpp"
#include "Test_Family.hpp"


using sgm::spec::Specimen;
using sgm::spec::is_True;


static void Constructor()
{
	Specimen s1 = 1, s2 = 3;
	Specimen const cs1 = 5;

	sgm::Family<Specimen, Specimen&, Specimen const&> fam1(-1, s1, cs1);

	auto fam2 = fam1;

	is_True
	(	fam1 == sgm::Make_Family( Specimen(-1), s1, cs1 )
	&&	fam2 == sgm::Make_Family( Specimen(-1), s1, cs1 )
	);
}


static void Move_Rvalue_Member()
{
	Specimen s1 = 1, s2 = 3;
	Specimen const cs1 = 5;

	sgm::Family<Specimen, Specimen&, Specimen const&, Specimen&&> fam1
	(	-1, s1, cs1, sgm::Move(s2)
	);

	auto fam2 = sgm::Move(fam1);

	is_True
	(	fam2 == sgm::Make_Family( Specimen(-1), s1, cs1, Specimen(3) )
	&&	std::get<0>(fam1) == Specimen::State::MOVE_AWAY
	&&	std::get<1>(fam1) == s1
	&&	std::get<2>(fam1) == cs1
	&&	std::get<3>(fam1) == 3
	);
}


static void Assignment()
{
	Specimen s1 = 1, s2 = 3;
	Specimen const cs1 = 5;

	sgm::Family<Specimen, Specimen&, Specimen&> fam1( -1, s1, s2 ), fam2( -3, s2, s1 );

	fam2 = fam1;

	is_True( fam2 == sgm::Family<Specimen, Specimen, Specimen>{-1, 1, 1} );
}


static void Move_Assignment()
{
	Specimen s1 = 1, s2 = 3, s3 = 5, s4 = 7;
	Specimen const cs1 = 9;

	sgm::Family<Specimen, Specimen&, Specimen&&>
		fam1( -1, s1, sgm::Move(s2) ), fam2( -3, s4, sgm::Move(s3) );

	fam2 = sgm::Move(fam1);

	is_True
	(	fam2 == sgm::Family<Specimen, Specimen, Specimen>{-1, 1, 3}
	&&	std::get<0>(fam1) == Specimen::State::MOVE_AWAY
	&&	std::get<1>(fam1) == 1
	&&	std::get<2>(fam1) == Specimen::State::MOVE_AWAY
	);
}


static void Merge_Family_Test()
{
	Specimen s1 = 1, s2 = 3;

	sgm::Family<Specimen&, Specimen> fam1(s2, 3);
	sgm::Family<Specimen, Specimen&&> fam2( -4, sgm::Move(s1) );


	auto fam3 = sgm::Merge_Families(fam1, fam2);

	static_assert
	(	sgm::is_Same
		<	decltype(fam3), sgm::Family<Specimen&, Specimen, Specimen, Specimen&&> 
		>::	value
	,	""
	);

	is_True(fam3 == sgm::Family<Specimen, Specimen, Specimen, Specimen>{s2, 3, -4, 1});
}


static void Harden_Test()
{
	Specimen s1 = 1, s2 = 3;
	sgm::Family<Specimen&, Specimen&&> fam1( s1, sgm::Move(s2) );

	auto fam2 = sgm::Harden(fam1);

	static_assert
	(	sgm::is_Same< decltype(fam2), sgm::Family<Specimen&, Specimen> >::value
	,	""
	);

	is_True
	(	std::get<0>(fam2) == s1
	&&	std::get<1>(fam2) == 3
	&&	s2 == Specimen::State::MOVE_AWAY
	);
}


static void As_it_is_Test()
{
	Specimen s1 = 1, s2 = 3;

	sgm::Tie(s1, s2) = sgm::Forward_as_Family(5, sgm::as_it_is);

	is_True( s1 == Specimen(5) && s2 == 3 );
}


static void Structured_Binding()
{
	{
		auto&& [a, b, c] = sgm::Family<int, double, int*>( 3, 0.14, (int*)nullptr );

		static_assert
		(	sgm::is_Same<decltype(a), int>::value
		&&	sgm::is_Same<decltype(b), double>::value
		&&	sgm::is_Same<decltype(c), int*>::value
		);
	}
	{
		int x = 0,  *px = &x;
		double const y = 5.5;

		auto&& [t1, t2, t3, t4, t5, t6] 
		=	sgm::Forward_as_Family(3, 0.14, x, y, &x, px);

		static_assert
		(	sgm::is_Same<decltype(t1), int&&>::value
		&&	sgm::is_Same<decltype(t2), double&&>::value
		&&	sgm::is_Same<decltype(t3), int&>::value
		&&	sgm::is_Same<decltype(t4), double const&>::value
		&&	sgm::is_Same<decltype(t5), int*&&>::value
		&&	sgm::is_Same<decltype(t6), int*&>::value
		);
	}
}


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Family, /**/)
{	::Constructor
,	::Move_Rvalue_Member
,	::Assignment
,	::Move_Assignment
,	::Merge_Family_Test
,	::Harden_Test
,	::As_it_is_Test
,	::Structured_Binding
};