#include "Extremum.hpp"
#include "Sigma\Specification\Specification.hpp"


using sgm::spec::is_True;
using namespace sgm;


static void Test01()
{
	static_assert
	(	num::Constants::Decimal_v<float, 0> == 1
	&&	num::Constants::Decimal_v<float, 1> == 10
	&&	num::Constants::Decimal_v<float, 2> == 100
	&&	num::Constants::Decimal_v<float, 3> == 1000
	);
}


template< class X, class Y = X, class pair_t = num::XY_Pair<X, Y> >
struct _Compare
{
	pair_t _thresholds;

	
	_Compare(X x_thres, Y y_thres) : _thresholds( {std::abs(x_thres), std::abs(y_thres)} ){}
	_Compare(X thres) : _Compare(thres, thres){}

	bool operator()(pair_t A, pair_t B) const
	{
		return std::abs(A.x - B.x) < _thresholds.x && std::abs(A.y - B.y) < _thresholds.y;
	}
};


static void Test02()
{
	auto parabola_f = [](double x)-> double{  return pow(x - 1, 2) - 1;  };
	double const threshold = 1e-5;
	auto domain = {.0, 5.0};

	num::XY_Pair<double, double> const
		answer{1, parabola_f(1)},
		gs_searched 
		=	num::GoldenSection<num::Extreme::MINIMUM>::search
			(	parabola_f, domain, threshold
			);

	is_True( _Compare<double>(threshold)(gs_searched, answer) );
}


static void Test03()
{
	auto sine_f = [](double x)-> double{  return sin(x);  };
	double const threshold = 1e-5, &pi = num::Constants::Pi_v<double>;

	num::XY_Pair<double, double> const
		answer{pi/2, sin(pi/2)},
		gs_searched
		=	num::GoldenSection<num::Extreme::MAXIMUM>::search
			(	sine_f, std::initializer_list<double>{0.0, pi}, threshold
			);

	is_True( _Compare<double>(threshold)(gs_searched, answer) );
}


static void Test04()
{
	auto parabola_f
	=	[](double x1, double x2, double x3)-> double
		{
			return pow(x1 - 1, 2) + pow(x2 - 2, 2) + pow(x3 + 1, 2) - 1;
		};

	enum : unsigned{DIMENSION = 3};

	unsigned const maximum_iteration = 30;
	double const search_radius = 3.0, epsilon = 1e-8;
	auto xinit = { -.8, 4.5, -2.7};
	double x[DIMENSION];
	//double y;	// optional

	auto converged
	=	num::Powell<num::Extreme::MINIMUM>::template search
		(	parabola_f, std::move(xinit), search_radius, epsilon, maximum_iteration
		,	std::begin(x) //, y	// optional 
		);

	is_True
	(	converged
	&&	abs(x[0] - 1.0) < 5*epsilon
	&&	abs(x[1] - 2.0) < 5*epsilon
	&&	abs( x[2] - (-1.0) ) < 5*epsilon
	);
}


#include "Test_Extrimum.hpp"


SGM_SPEC_TESTS(num_Extrimum)
{	::Test01
,	::Test02
,	::Test03
,	::Test04
};
