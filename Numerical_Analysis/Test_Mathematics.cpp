#include "Mathematics.hpp"
#include "Extremum.hpp"
#include "..\Specification\Specification.hpp"


using namespace sgm;


static void Test01()
{
	spec::All_True
	<	num::Constants::Decimal<0>::value == 1
	,	num::Constants::Decimal<1>::value == 10
	,	num::Constants::Decimal<2>::value == 100
	,	num::Constants::Decimal<3>::value == 1000
	>();


	spec::is_True
	(	FLT_EPSILON == num::Constants::Epsilon<float, 0>() 
	,	DBL_EPSILON == num::Constants::Epsilon<double, 0>()
	);
}

#include <iostream>


static void Test02()
{
	//std::wcout << L"answer = "
	//<<	num::GoldenSection<num::Extreme::MINIMUM>::search
	//	(	[](float x)-> float{  return (x*x - 1) - 1;  }
	//	,	{0.f, 3.f}, 0.0001f
	//	).first
	//<<	std::endl;
	//
}


#include "Test_Mathematics.hpp"



void Test_sgm_Mathematics::test()
{
	try
	{
		::Test01();
		::Test02();
	}
	catch (std::exception e)
	{
		std::cerr << e.what() << std::endl;

		throw e;
	}

	std::wcout << L"Mathematics Test Complete.\n";
}