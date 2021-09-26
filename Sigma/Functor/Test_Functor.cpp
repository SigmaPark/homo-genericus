#include "..\Functor\Functor.hpp"
#include "..\Specification\Specification.hpp"


using sgm::spec::is_True;


struct TestF18
{
	static void Test01();
	static void Test02();
	static void Test03();
	static void Test04();
	static void Test05();
	static void Test06();
	static void Test07();
	static void Test08();
	static void Test09();
	static void Test10();
	static void Test11();

	template<class RG1, class RG2>
	static bool are_same_ranges(RG1 &&rg1, RG2 &&rg2)
	{	
		auto itr1 = rg1.begin();
		auto itr2 = rg2.begin();
	
		while(itr1 != rg1.end() && itr2 != rg2.end() && *itr1 == *itr2)
			itr1++,  itr2++;
	
		return !(itr1 != rg1.end() || itr2 != rg2.end());
	}
};


void TestF18::Test01()
{
	int y = 5;

	sgm::fp::Functor ftr1 = [](auto&& x, auto const& y){  return 2*x*y;  };
	
	auto res1 = ( [](auto&& x, auto const& y){  return 2*x*y;  } / sgm::fp::as_functor )(2, y);

	is_True( res1 == 20 && ftr1(2, y) == 20 );
}


void TestF18::Test02()
{
	int y = 5;

	is_True
	(	( [](int&& x, auto const& y){  return 2*x*y;  } / sgm::fp::as_functor )
		(sgm::fp::_, y)(2)
	==	20	
	);

	sgm::fp::Functor ftr1 = [](int&& x, auto const& y){  return 2*x*y;  };
	
	is_True
	(	ftr1(sgm::fp::_, y)(2) == 20
	&&	ftr1(2, sgm::fp::_)(y) == 20
	);
}


void TestF18::Test03()
{
	auto f1 = [](auto x, auto y){  return x+y;  };
	auto f2 = [](auto x){  return 2*x;  };

	auto y = (f2/sgm::fp::as_functor * f1)(3, 5);

	is_True(y == 16);

	sgm::fp::Functor ftr2 = f2;
	sgm::fp::Functor cps_ftr = ftr2 * f1;

	is_True( cps_ftr(6.0, 9.0) == 30.0 );
}


void TestF18::Test04()
{
	int const x = 3;
	double y = 1.0;

	auto&& [a0, a1, a2] = sgm::fp::Permute<1,2,0>(x, y, 'c');

	static_assert
	(	sgm::is_Same_v<decltype(a0), double&> 
	&&	sgm::is_Same_v<decltype(a1), char&&>
	&&	sgm::is_Same_v<decltype(a2), int const&>
	);
}


void TestF18::Test05()
{
	auto answer = sgm::Serial<double, 5>{ -1, -2, -3, -4, -5 };

	is_True
	(	are_same_ranges
		(	sgm::fp::Morph
			(	sgm::Countable<int>(5, 1), [](int x)-> double{  return -x;  } 
			)
		,	answer
		)
	);	
}


void TestF18::Test06()
{
	auto is_even = [](int const& x)-> bool{  return x % 2 == 0;  };
	auto answer = sgm::Serial<int, 5>{2, 4, 6, 8, 10};	
	sgm::Serial<int> const sr1 = sgm::Countable<int>(10, 1);

	is_True(  are_same_ranges( sgm::fp::Filter(sr1, is_even), answer )  );
}


void TestF18::Test07()
{
	auto minus = [](int res, int const& x)-> int{  return res - x;  };
	auto plus = [](int const res, int const x)-> int{  return res + x;  };

	bool res = false;

	using sgm::Countable;

	res 
	=	(	sgm::fp::Fold( Countable<int>(3, 1), minus, 10 ) == 4
		&&	sgm::fp::Fold( Countable<int>(3, 1), minus ) == -4
		&&	sgm::fp::rFold( Countable<int>(3, 1), minus, 10 ) == 4
		&&	sgm::fp::rFold( Countable<int>(3, 1), minus ) == 0
		&&	(	sgm::fp::Fold_par<sgm::ht::Par<>>( Countable<int>(20, 1), plus, int(0) )
			==	sgm::fp::Fold( Countable<int>(20, 1), plus )
			)
		);

	is_True(res);		
}


void TestF18::Test08()
{
	std::initializer_list<int> const iL{1, 3, 5, 7};
	sgm::Serial<int, 4> const sr1{2, 4, 6, 8};
	sgm::Serial<int> sr2{-3, -2, -1, 0};

	static_assert
	(	sgm::is_Same
		<	typename 
			std::iterator_traits
			<	sgm::Decay_t<decltype( sgm::fp::Plait(iL, sr1, sr2).begin() )>
			>::	iterator_category
		,	std::random_access_iterator_tag
		>::	value
	,	"iterator_traits tracking failed."
	);

	for( auto &&fam : sgm::fp::Plait(iL, sr1, sr2) )
		fam.get<2>() += fam.get<0>() + fam.get<1>();

	is_True
	(	are_same_ranges(sr2, std::initializer_list<int>{0, 5, 10, 15})
	);
}


#include <cmath>
#include <functional>


void TestF18::Test09()
{
	using namespace sgm::fp;

	size_t constexpr N = 10000;
	double const ans = std::pow( std::acos(0)*2, 2 )/6.0;

	Functor const ftr1
	=	Fold(_, std::plus<>())
	*	Morph(_, [](size_t const x){  return std::pow(x, -2.0);  })
	*	integers;

	auto const res = ftr1(N, 1);

	is_True( std::abs(res - ans) < 1e-4 );
}


void TestF18::Test10()
{
	using namespace sgm::fp;

	size_t constexpr N = 10000;
	double const ans = .5*std::acos(0);

	Functor const ftr1
	=	Fold(_, std::plus<>())
	*	Morph( _, [](size_t const x){  return (x % 2 == 1 ? -1.0 : 1.0)/double(2*x + 1);  } )
	*	integers;

	auto const res = ftr1(N);

	is_True( std::abs(res - ans) < 1e-4 );
}


void TestF18::Test11()
{
	using namespace sgm::fp;

	is_True( Functor()(1) == 1 );
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Functor.hpp"
#include <iostream>


void Test_sgm_Functor::test()
{
	try
	{
		TestF18::Test01();
		TestF18::Test02();
		TestF18::Test03();
		TestF18::Test04();
		TestF18::Test05();
		TestF18::Test06();
		TestF18::Test07();
		TestF18::Test08();
		TestF18::Test09();
		TestF18::Test10();
		TestF18::Test11();

		std::wcout << L"Functor Test Complete.\n";
	}
	catch(std::exception const &e)
	{
		std::cerr << e.what() << "Functor Test Failed.\n";

		throw e;
	}
}
