#include "Multiple.hpp"
#include "..\Specification\Specification.hpp"


using namespace sgm;
using fp::Multiple, spec::Specimen, spec::is_True;
//========//========//========//========//=======#//========//========//========//========//=======#


static void Test01()
{
	Specimen s1 = 3;
	Specimen const cs1 = 80;

	auto const mtp1 = fp::Make_Multiple( s1, cs1, Specimen(-2) );
	auto const mtp2 = fp::Forward_as_Multiple( s1, cs1, Specimen(-2) );

	is_True
	(	mtp1.get<0>() == 3 && mtp1.get<1>() == 80 && mtp1.get<2>() == -2
	&&	mtp2.get<0>() == 3 && mtp2.get<1>() == 80 
	&&	mtp2.get<2>() == Specimen::State::DESTRUCTED
	);
}


static void Test02()
{
	Specimen s1 = 9, s2 = 8, s3 = 90;

	Multiple<Specimen, Specimen&, Specimen const&, Specimen&&> mtp1
	(	Specimen(7), s1, s2, std::move(s3)
	);

	static_assert
	(	std::is_same_v< decltype(mtp1.forward<0>()), Specimen >
	&&	std::is_same_v< decltype(mtp1.forward<1>()), Specimen& >
	&&	std::is_same_v< decltype(mtp1.forward<2>()), Specimen const& >
	&&	std::is_same_v< decltype(mtp1.forward<3>()), Specimen&& >
	);

	is_True(mtp1.forward<3>() == 90);

	auto s4 = mtp1.forward<3>();

	is_True(mtp1.get<3>() == Specimen::State::DESTRUCTED);
}


static void Test03()
{
	Specimen s1 = 9, s2 = 8, s3 = 90;
	
	std::tuple<Specimen, Specimen&, Specimen const&, Specimen&&> tu1
	(	Specimen(7), s1, s2, std::move(s3)
	);

	auto mtp1 = fp::to_Multiple(tu1);

	static_assert
	(	std::is_same_v
		<	decltype(mtp1)
		,	Multiple<Specimen, Specimen&, Specimen const&, Specimen&&>
		>
	);

	is_True(mtp1.forward<3>() == 90);

	auto s4 = mtp1.forward<3>();

	is_True(mtp1.get<3>() == Specimen::State::DESTRUCTED);
}


static void Test04()
{
	Specimen s1 = 9, s2 = 8, s3 = 90;

	Multiple<Specimen, Specimen&, Specimen const&, Specimen&&> mtp1
	(	Specimen(7), s1, s2, std::move(s3)
	);
	
	auto mtp2 = mtp1.hardened();

	static_assert
	(	std::is_same_v
		<	decltype(mtp2)
		,	Multiple<Specimen, Specimen&, Specimen const&, Specimen>
		>
	);

	is_True(mtp1.forward<3>() == Specimen::State::DESTRUCTED && mtp2.forward<3>() == 90);
}


static void Test05()
{
	Specimen s1 = 9, s2 = 8, s3 = 90;

	Multiple<Specimen, Specimen&&> mtp1( Specimen(-4), std::move(s1) );
	Multiple<Specimen&, Specimen const&> mtp2(s2, s3);

	auto mtp3 = mtp1 + mtp2;

	is_True
	(	mtp3.get<0>() == -4 && mtp3.get<1>() == 9 && mtp3.get<2>() == 8 && mtp3.get<3>() == 90
	);

	auto val = mtp3.forward<1>();

	is_True
	(	val == Specimen::State::MOVE_CONSTRUCTION 
	&&	mtp3.get<1>() == Specimen::State::DESTRUCTED
	);
}


static void Test06()
{
	Specimen s1 = 9, s2 = 8, s3 = 90;

	Multiple<Specimen, Specimen&&> mtp1( Specimen(-4), std::move(s1) );
	Multiple<Specimen&, Specimen const&> mtp2(s2, s3);

	auto mtp3 = Make_Flat_MTP(s1, mtp1);
	auto mtp4 = Forward_as_Flat_MTP(mtp1, mtp2, s3);

	static_assert
	(	std::is_same_v
		<	decltype(mtp3)
		,	Multiple<Specimen, Specimen, Specimen&&>
		>
	&&	std::is_same_v
		<	decltype(mtp4)
		,	Multiple<Specimen&&, Specimen&&, Specimen&, Specimen const&, Specimen&>
		>
	);

	auto val = mtp4.forward<0>();

	is_True
	(	val == Specimen::State::MOVE_CONSTRUCTION 
	&&	mtp4.get<0>() == Specimen::State::DESTRUCTED
	);
}


static void Test07()
{
	Specimen s1 = 9, s2 = 8, s3 = 90;

	auto[mtp1, mtp2]
	=	*fp::Forward_as_2FMTP<2, 2>
		(	Specimen(2), s1, static_cast<Specimen const&>(s2), std::move(s3)
		);


	bool test_result = false;
#ifndef __INTELLISENSE__
	static_assert
	(	std::is_same_v
		<	decltype(mtp1)
		,	Multiple<Specimen&&, Specimen&>
		>
	&&	std::is_same_v
		<	decltype(mtp2)
		,	Multiple<Specimen const&, Specimen&&>
		>
	);

	test_result
	=	(	mtp1.get<0>() == Specimen::State::DESTRUCTED
		&&	mtp1.get<1>() == 9
		&&	mtp2.get<0>() == 8
		&&	mtp2.get<1>() == 90
		);
#endif
	is_True(test_result);
}


static void Test08()
{
	Specimen s1 = 9, s2 = 7;
	Specimen const cs1 = -3;

	auto func 
	=	[](auto&& x1, auto&& x2, auto&& x3)
		{
			return x1.val + x2.val + x3.val;
		};

	Multiple<Specimen&, Specimen const&, Specimen&&> mtp1( s1, cs1, std::move(s2) );

	auto res = fp::Apply(func, mtp1);

	is_True(res == 13);
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Multiple.hpp"
#include <iostream>


void Test_sgm_Multiple::test()
{
	try
	{
		::Test01();
		::Test02(); 
		::Test03();
		::Test04();
		::Test05();
		::Test06();
		::Test07();
		::Test08();

		std::wcout << L"Multiple Test Complete.\n";
	}
	catch(...)
	{
		std::wcerr << L"Multiple Test Failed.\n";

		throw;
	}
}