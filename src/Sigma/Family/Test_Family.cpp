#include "Family.hpp"
#include "..\Specification\Specification.hpp"
#include <type_traits>

using namespace sgm;
using spec::Specimen;
using spec::is_True;
//========//========//========//========//=======#//========//========//========//========//=======#


static void Test01()
{
	Family<int, Specimen, double*> fam( 45, Specimen(2), nullptr );
	
	is_True
	(	std::get<0>(fam) == 45
	&&	std::get<1>(fam) == Specimen(2)
	&&	std::get<2>(fam) == nullptr
	);

	auto fam2 = Make_Family( 45, Specimen(2), static_cast<double*>(nullptr) );

	static_assert( std::is_same<decltype(fam), decltype(fam2)>::value, "" );

	is_True
	(	std::get<0>(fam2) == 45
	&&	std::get<1>(fam2) == Specimen(2)
	&&	std::get<2>(fam2) == nullptr	
	);
}


static void Test02()
{
	Specimen s1 = 3;
	Specimen const cs1 = 54;

	auto fam1 = Forward_as_Family( s1, cs1, Specimen(2) );

	static_assert
	(	std::is_same< decltype(fam1), Family<Specimen&, Specimen const&, Specimen&&> >::value
	,	""
	);

	is_True
	(	std::get<0>(fam1) == 3
	&&	std::get<1>(fam1) == 54
	&&	std::get<2>(fam1) == Specimen::State::DESTRUCTED
	);
}


static void Test03()
{
	Specimen s1 = 3, s2 = 5;
	Specimen cs1 = 10;

	Family<Specimen, Specimen&, Specimen const&> fam1( Specimen(-2), s1, cs1 );

	auto fam2 = fam1;

	is_True
	(	std::get<0>(fam2) == -2
	&&	std::get<1>(fam2) == s1
	&&	std::get<2>(fam2) == cs1
	&&	std::get<0>(fam1) == -2
	&&	std::get<1>(fam1) == s1
	&&	std::get<2>(fam1) == cs1
	);
}


static void Test04()
{
	Specimen s1 = 3, s2 = 5;
	Specimen const cs1 = 10;

	Family<Specimen, Specimen&, Specimen const&, Specimen&&> fam1
	(	Specimen(-2), s1, cs1, std::move(s2)
	);

	auto fam2 = std::move(fam1);

	is_True
	(	std::get<0>(fam2) == -2
	&&	std::get<1>(fam2) == s1
	&&	std::get<2>(fam2) == cs1
	&&	std::get<3>(fam2) == 5
	&&	std::get<0>(fam1) == Specimen::State::DESTRUCTED
	&&	std::get<1>(fam1) == s1
	&&	std::get<2>(fam1) == cs1
	&&	std::get<3>(fam1) == 5	
	);
}


static void Test05()
{
	Specimen s1 = 3, s2 = 5;
	Specimen const cs1 = 10;

	Family<Specimen, Specimen&, Specimen&>
		fam1( Specimen(-2), s1, s2 ),
		fam2( Specimen(-8), s2, s1 );

	fam2 = fam1;

	is_True
	(	std::get<0>(fam2) == -2
	&&	std::get<1>(fam2) == 3
	&&	std::get<2>(fam2) == 3	
	);
}


static void Test06()
{
	Specimen s1 = 3, s2 = 5, s3 = 33, s4 = 211;
	Specimen const cs1 = 10;

	Family<Specimen, Specimen&, Specimen&&>
		fam1( Specimen(-2), s1, std::move(s2) ),
		fam2( Specimen(-8), s4, std::move(s3) );

	fam2 = std::move(fam1);

	is_True
	(	std::get<0>(fam2) == -2
	&&	std::get<1>(fam2) == 3
	&&	std::get<2>(fam2) == 5
	&&	std::get<0>(fam1) == Specimen::State::DESTRUCTED
	&&	std::get<1>(fam1) == 3
	&&	std::get<2>(fam1) == Specimen::State::DESTRUCTED
	);
}


static void Test07()
{
	Specimen s1 = -2, s2 = 30;

	Family<Specimen&, Specimen> fam1(s2, 65);
	Family<Specimen, Specimen&&> fam2( -4, std::move(s1) );


	auto fam3 = sgm::Merge_Families(fam1, fam2);

	static_assert
	(	std::is_same
		<	decltype(fam3), Family<Specimen&, Specimen, Specimen, Specimen&&> 
		>::value
	,	""
	);

	is_True
	(	std::get<0>(fam3) == s2
	&&	std::get<1>(fam3) == 65
	&&	std::get<2>(fam3) == -4
	&&	std::get<3>(fam3) == -2
	);
}


static void Test08()
{
	Specimen s1 = -2, s2 = 30;
	Family<Specimen&, Specimen&&> fam1( s1, std::move(s2) );

	auto fam2 = sgm::Harden(fam1);

	static_assert
	(	std::is_same
		<	decltype(fam2), Family<Specimen&, Specimen> 
		>::value
	,	""
	);

	is_True
	(	std::get<0>(fam2) == s1
	&&	std::get<1>(fam2) == 30
	&&	s2 == Specimen::State::DESTRUCTED
	);
}


static void Test09()
{
	Specimen s1 = -2, s2 = 30;

	Tie(s1, s2) = Forward_as_Family(25, as_it_is);

	is_True( s1 == Specimen(25) && s2 == 30 );
}


#include "Test_Family.hpp"
#include <iostream>


SGM_SPEC_TESTS(sgm_Family)
{	::Test01
,	::Test02
,	::Test03
,	::Test04
,	::Test05
,	::Test06
,	::Test07
,	::Test08
,	::Test09
};