#include "Multiple.hpp"
#include "..\Specification\Specification.hpp"


using namespace sgm;
using fp0::Multiple, spec::Specimen, spec::is_True;


static void Test01()
{
	Specimen s1 = 3;
	Specimen const cs1 = 80;

	auto const mtp1 = fp0::Make_Multiple( s1, cs1, Specimen(-2) );
	auto const mtp2 = fp0::Forward_as_Multiple( s1, cs1, Specimen(-2) );

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

	auto mtp1 = fp0::to_Multiple(tu1);

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

		std::wcout << L"Multiple Test Complete.\n";
	}
	catch(...)
	{
		std::wcerr << L"Multiple Test Failed.\n";

		throw;
	}
}