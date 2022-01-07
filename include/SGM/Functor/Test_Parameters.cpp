/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Parameters.hpp"
#include "Test_Parameters.hpp"


using sgm::spec::Specimen;


static void Flatten()
{
	{
		Specimen s0(2), s1(22), s2(222);

		auto pp = sgm::fp::Parameters( s0, sgm::immut(s1), sgm::Move(s2) );

		static_assert
		(	sgm::is_Same
			<	decltype(pp)
			,	sgm::fp::Param_Pack<Specimen&, Specimen const&, Specimen&&>
			>::value
		);
	}
	{
		Specimen s0(2), s1(22), s2(222);
		
		auto pp 
		=	sgm::fp::Parameters
			(	s0, sgm::fp::Parameters( sgm::immut(s0), s1 ), sgm::Move(s2) 
			);

		static_assert
		(	sgm::is_Same
			<	decltype(pp)
			,	sgm::fp::Param_Pack<Specimen&, Specimen const&, Specimen&, Specimen&&>
			>::value	
		);
	}
}


static void Merge()
{
	Specimen s0(2), s1(22), s2(222);

	auto pp1 = sgm::fp::Parameters( s0, sgm::immut(s1) );
	auto pp2 = sgm::fp::Param_Pack<Specimen, Specimen&&>( s1, sgm::Move(s2) );

	auto pp = sgm::fp::Merge_Params(pp1, pp2);

	static_assert
	(	sgm::is_Same
		<	decltype(pp)
		,	sgm::fp::Param_Pack<Specimen&, Specimen const&, Specimen, Specimen&&>
		>::	value
	);
}


static void Apply()
{
	auto Specimen_sum_f
	=	[](Specimen s1, Specimen& s2, Specimen&& s3)
		->	Specimen{  return {s1.value() + s2.value() + s3.value()};  };

	Specimen s0(2), s1(22), s2(222);

	auto const res 
	=	sgm::fp::Apply_Params
		(	Specimen_sum_f, sgm::fp::Parameters( s0, s1, sgm::Move(s2) )  
		);

	sgm::spec::is_True(res.value() == 246);
}


static void Structured_Binding()
{
	{
		auto&& [a, b, c] = sgm::fp::Param_Pack<int, double, int*>( 3, 0.14, (int*)nullptr );

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
		=	sgm::fp::Parameters(3, 0.14, x, y, &x, px);

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
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::fp::spec::Test_, Parameters, /**/)
{	::Flatten
,	::Merge
,	::Apply
,	::Structured_Binding
};