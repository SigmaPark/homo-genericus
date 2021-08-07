#include <vector>
#include "..\Pinweight\Pinweight.hpp"
#include "..\Avatar\Avatar.hpp"

#include "High_Templar.hpp"
#include "..\Specification\Specification.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


using namespace sgm;
using spec::Judge;
using spec::is_True;


struct SHARE : Type_Decorator
{
	template<class T> using type = Pinweight<T>;
};

struct CREFER : Type_Decorator
{
	template<class T> using type = constAvatar<T>;
};



static void CountableTest()
{
	is_True
	(	Judge::have_the_same( Countable<int>(5, 1), std::vector<int>{1, 2, 3, 4, 5} )
	&&	Judge::have_the_same( Countable<unsigned>(4), std::vector<unsigned>{0, 1, 2, 3} )
	);
}


static void MorphTest()
{
	auto negate = [](int x)-> int{  return -x;  };
	auto answer = std::vector<int>{ -1, -2, -3, -4, -5 };

	bool res = false;
#if 1// #ifndef __INTELLISENSE__
	res
	=	(	Judge::have_the_same(  ht::Morph( Countable<int>(5, 1), negate ), answer  )
		&&	Judge::have_the_same(  ht::Morph<SHARE>( Countable<int>(5, 1), negate ), answer  )
		&&	Judge::have_the_same
			(	ht::Morph( Countable<int>(20, 1), negate )
			,	ht::Morph< ht::Par<4>, SHARE >( Countable<int>(20, 1), negate )
			)
		);
#endif
	is_True(res);
}


static void FilterTest()
{
	auto is_even = [](int const& x)-> bool{  return x % 2 == 0;  };
	auto answer = std::vector<int>{2, 4, 6, 8, 10};
	
	Serial<int> const sr1 = Countable<int>(10, 1);

	bool res = false;
#if 1 //#ifndef __INTELLISENSE__
	res 
	=	(	Judge::have_the_same(  ht::Filter( Countable<int>(10, 1), is_even ), answer  )
		&&	Judge::have_the_same
			(	ht::Filter<SHARE>( Countable<int>(10, 1), is_even ), answer  
			)
		&&	Judge::have_the_same
			(	ht::Filter( Countable<int>(20, 1), is_even )
			,	ht::Filter<ht::Par<>, SHARE>( Countable<int>(20, 1), is_even )
			)
		&&	Judge::have_the_same( ht::Filter<CREFER>(sr1, is_even), answer )
		);
#endif
	is_True(res);
}


static void FoldTest()
{
	auto minus = [](int res, int const& x)-> int{  return res - x;  };
	auto plus = [](int const res, int const x)-> int{  return res + x;  };

	bool res = false;
#if 1 //#ifndef __INTELLISENSE__
	res 
	=	(	ht::Fold( Countable<int>(3, 1), minus, 10 ) == 4
		&&	ht::Fold( Countable<int>(3, 1), minus ) == -4
		&&	ht::Fold<SHARE>( Countable<int>(3, 1), minus ) == -4
		&&	ht::rFold( Countable<int>(3, 1), minus, 10 ) == 4
		&&	ht::rFold( Countable<int>(3, 1), minus ) == 0
		&&	(	ht::Fold<ht::Par<>>( Countable<int>(20, 1), plus, int(0) )
			==	ht::Fold( Countable<int>(20, 1), plus )
			)
		);
#endif
	is_True(res);
}


static void Ex_LeibnizTest()
{
	size_t const N = 1000000;

	auto is_odd = [](size_t const n)-> bool{  return n % 2 == 1;  };
	auto plus = [](double const res, double const x)-> double{  return res + x;  };

	auto altered_harmonic 
	=	[](size_t const n)-> double
		{
			auto const den = static_cast<double>(n);

			return n % 4 == 1 ? 1.0 / den : -1.0 / den;
		};

	bool res = false;
#ifndef __INTELLISENSE__
	double const 
		quater_pi = acos(.0) * .5,
		Leibniz 
		=	ht::Fold<ht::Par<>>
			(	ht::Morph<ht::Par<>>
				(	ht::Filter( Countable<>(N, 1), is_odd ), altered_harmonic  
				)
			,	plus
			);

	res = abs(quater_pi - Leibniz) < 0.0001;
#endif
	is_True(res);
}


static void Plait_Test()
{
	Serial<int> sr1{1, 4, 7, 10};
	Serial<double> sr2{-1, -2, -3, -4};	
	Serial<char> sr3{'a', 'b', 'c', 'd'};

	using fam1_t = Family<int, double, char>;
	using fam2_t = Family< Pinweight<int>, Pinweight<double>, Pinweight<char> >;

	auto ans1
	=	Serial<fam1_t>
		{	fam1_t(1, -1.0, 'a'), fam1_t(4, -2.0, 'b')
		,	fam1_t(7, -3.0, 'c'), fam1_t(10, -4.0, 'd')
		};

	auto ans2
	=	Serial<fam2_t>
		{	fam2_t(1, -1.0, 'a'), fam2_t(4, -2.0, 'b')
		,	fam2_t(7, -3.0, 'c'), fam2_t(10, -4.0, 'd')
		};

#if 1
	bool res = false;
#ifndef __INTELLISENSE__
	res
	=	(	Judge::have_the_same( ht::Plait(sr1, sr2, sr3), ans1 )
		&&	Judge::have_the_same( ht::Plait< ht::Par<2> >(sr1, sr2, sr3), ans1 )
		&&	Judge::have_the_same( ht::Plait< ht::Par<2>, SHARE >(sr1, sr2, sr3), ans2 )
		);
#endif
	is_True(res);
#endif
}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace test_ht12
{

	template<class RG1, class RG2>
	static auto are_same_ranges(RG1 &&rg1, RG2 &&rg2)-> bool
	{
		struct _res_t
		{
			decltype(rg1.begin()) const b1, e1;
			decltype(rg2.begin()) const b2, e2;
			bool const is_ongoing[2] = {b1 != e1, b2 != e2};

			operator bool() const
			{
				return 
				is_ongoing[0] && is_ongoing[1] && *b1 == *b2
				?	_res_t{sgm::Next(b1), e1, sgm::Next(b2), e2}
				:	!(is_ongoing[0] || is_ongoing[1]);
			}
		};

		return _res_t{rg1.begin(), rg1.end(), rg2.begin(), rg2.end()};
	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_High_Templar.hpp"
#include <iostream>


void Test_sgm_High_Templar::test()
{
	try
	{
		CountableTest();
		MorphTest();
		FilterTest();
		FoldTest();

		Ex_LeibnizTest();

		Plait_Test();

		is_True
		(	test_ht12::are_same_ranges
			(	std::initializer_list<int>{2, 5, 8, 11}, sgm::Serial<int>{2, 5, 8, 11}
			)
		);

		std::wcout << L"High Templar Test Complete.\n";
	}
	catch(spec::Exception e)
	{
		std::wcout << L"High Templar Test Failed.\n";

		throw e;
	}
}
