//#include "Mathematics.hpp"
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


template< class X, class Y = X, class pair_t = num::XY_Pair<X, Y> >
struct _Compare
{
	pair_t _thresholds;

	
	_Compare(X x_thres, Y y_thres) : _thresholds( {abs(x_thres), abs(y_thres)} ){}
	_Compare(X thres) : _Compare(thres, thres){}

	bool operator()(pair_t A, pair_t B) const
	{
		return abs(A.x - B.x) < _thresholds.x && abs(A.y - B.y) < _thresholds.y;
	}
};


static void Test02()
{
	auto parabola_f = [](double x)-> double{  return pow(x - 1, 2) - 1;  };
	double const threshold = 1e-5;

	num::XY_Pair<double, double> const
		answer{1, parabola_f(1)},
		gs_searched 
		=	num::GoldenSection<num::Extreme::MINIMUM>::search
			(	parabola_f, num::Real_Range<double>{0.0, 5.0}, threshold
			);

	//spec::are_Equivalent_by( _Compare<double>(threshold), gs_searched, answer );
}


static void Test03()
{
	auto sine_f = [](double x)-> double{  return sin(x);  };
	double const threshold = 1e-5, &pi = num::Constants::Pi<double>();

	num::XY_Pair<double, double> const
		answer{pi/2, sin(pi/2)},
		gs_searched
		=	num::GoldenSection<num::Extreme::MAXIMUM>::search
			(	sine_f, num::Real_Range<double>{0.0, pi}, threshold
			);
	
	//spec::are_Equivalent_by( _Compare<double>(threshold), gs_searched, answer );
}


#include "Test_Mathematics.hpp"
#include <iostream>


void Test_sgm_Mathematics::test()
{
	try
	{
		::Test01();
		::Test02();
		::Test03();
		
		std::wcout << L"Mathematics Test Complete.\n";
	}
	catch (std::exception e)
	{
		std::cerr << e.what() << std::endl;

		throw e;
	}
}