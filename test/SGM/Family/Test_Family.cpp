/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Family/Family.hpp"
#include "Test_Family.hpp"


using sgm::h2u::Specimen;


static void Constructor()
{
	Specimen s1 = 1, s2 = 3;
	Specimen const cs1 = 5;

	sgm::Family<Specimen, Specimen&, Specimen const&> fam1(-1, s1, cs1);

	auto fam2 = fam1;

	SGM_H2U_ASSERT
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

	SGM_H2U_ASSERT
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

	SGM_H2U_ASSERT( fam2 == sgm::Family<Specimen, Specimen, Specimen>{-1, 1, 1} );
}


static void Move_Assignment()
{
	Specimen s1 = 1, s2 = 3, s3 = 5, s4 = 7;
	Specimen const cs1 = 9;

	sgm::Family<Specimen, Specimen&, Specimen&&>
		fam1( -1, s1, sgm::Move(s2) ), fam2( -3, s4, sgm::Move(s3) );

	fam2 = sgm::Move(fam1);

	SGM_H2U_ASSERT
	(	fam2 == sgm::Family<Specimen, Specimen, Specimen>{-1, 1, 3}
	&&	std::get<0>(fam1) == Specimen::State::MOVE_AWAY
	&&	std::get<1>(fam1) == 1
	&&	std::get<2>(fam1) == Specimen::State::MOVE_AWAY
	);
}


static void Get_and_Forward()
{
	Specimen s1(5);

	sgm::Family<Specimen, Specimen&> fam1( Specimen(2), s1 );

	static_assert
	(	sgm::Boolean_And
		<	sgm::is_Same< decltype(fam1.template get<0>()), Specimen& >
		,	sgm::is_Same< decltype(fam1.template get<1>()), Specimen& >
		,	sgm::is_Same< decltype( sgm::immut(fam1).template get<0>() ), Specimen const& >
		,	sgm::is_Same< decltype( sgm::immut(fam1).template get<1>() ), Specimen& >
		,	sgm::is_Same< decltype( sgm::Move(fam1).template get<0>() ), Specimen&& >
		,	sgm::is_Same< decltype( sgm::Move(fam1).template get<1>() ), Specimen& >
		>::	value
	,	""
	);

	static_assert
	(	sgm::Boolean_And
		<	sgm::is_Same< decltype(fam1.template forward<0>()), Specimen&& >
		,	sgm::is_Same< decltype(fam1.template forward<1>()), Specimen& >
		,	sgm::is_Same< decltype( sgm::immut(fam1).template forward<0>() ), Specimen const&& >
		,	sgm::is_Same< decltype( sgm::immut(fam1).template forward<1>() ), Specimen& >
		,	sgm::is_Same< decltype( sgm::Move(fam1).template forward<0>() ), Specimen&& >
		,	sgm::is_Same< decltype( sgm::Move(fam1).template forward<1>() ), Specimen& >
		>::	value
	,	""
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

	SGM_H2U_ASSERT(fam3 == sgm::Family<Specimen, Specimen, Specimen, Specimen>{s2, 3, -4, 1});
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

	SGM_H2U_ASSERT
	(	std::get<0>(fam2) == s1
	&&	std::get<1>(fam2) == 3
	&&	s2 == Specimen::State::MOVE_AWAY
	);
}


static void As_it_is_Test()
{
	Specimen s1 = 1, s2 = 3;

	sgm::Tie(s1, s2) = sgm::Forward_as_Family(5, sgm::as_it_is);

	SGM_H2U_ASSERT( s1 == Specimen(5) && s2 == 3 );
}


static void Structured_Binding()
{
#if SGM_CXX_STANDARD >= 2017
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
	{
		auto duo = sgm::Family<int, double>(3, 3.14);
		auto [a, b] = duo;
		
		static_assert
		(	sgm::is_Same<decltype(a), int>::value
		&&	sgm::is_Same<decltype(b), double>::value
		&&	sgm::is_Same<decltype(duo.first()), int&>::value
		&&	sgm::is_Same<decltype(duo.second()), double&>::value
		);
	}
	{
		auto trio = sgm::Family<int, double, wchar_t>(3, 3.14, ' ');
		auto [a, b, c] = trio;
		
		static_assert
		(	sgm::is_Same<decltype(a), int>::value
		&&	sgm::is_Same<decltype(b), double>::value
		&&	sgm::is_Same<decltype(c), wchar_t>::value
		&&	sgm::is_Same<decltype(trio.first()), int&>::value
		&&	sgm::is_Same<decltype(trio.second()), double&>::value
		&&	sgm::is_Same<decltype(trio.third()), wchar_t&>::value
		);
	}
#endif
}


SGM_HOW2USE_TESTS(sgm::h2u::Test_, Family, /**/)
{	::Constructor
,	::Move_Rvalue_Member
,	::Assignment
,	::Move_Assignment
,	::Get_and_Forward
,	::Merge_Family_Test
,	::Harden_Test
,	::As_it_is_Test
,	::Structured_Binding
};