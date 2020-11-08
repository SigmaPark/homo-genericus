#include <vector>
#include "..\Pinweight\Pinweight.hpp"
#include "..\Avatar\Avatar.hpp"

#include "High_Templar.hpp"
#include "..\Specification\Specification.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


using namespace sgm;
using spec::Judge;
using spec::is_True;


struct SHARE : Flag<SHARE>, Type_Decorator
{
	template<class T> using type = Pinweight<T>;
};

struct CREFER : Flag<CREFER>, Type_Decorator
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
#ifndef __INTELLISENSE__
	res
	=	(	Judge::have_the_same(  ht::Morph( Countable<int>(5, 1), negate ), answer  )
		&&	Judge::have_the_same(  ht::Morph<SHARE>( Countable<int>(5, 1), negate ), answer  )
		&&	Judge::have_the_same
			(	ht::Morph( Countable<int>(100000, 1), negate )
			,	ht::Morph< ht::Par<4>, SHARE >( Countable<int>(100000, 1), negate )
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
#ifndef __INTELLISENSE__
	res 
	=	(	Judge::have_the_same(  ht::Filter( Countable<int>(10, 1), is_even ), answer  )
		&&	Judge::have_the_same
			(	ht::Filter<SHARE>( Countable<int>(10, 1), is_even ), answer  
			)
		&&	Judge::have_the_same
			(	ht::Filter( Countable<int>(100000, 1), is_even )
			,	ht::Filter<ht::Par<>, SHARE>( Countable<int>(100000, 1), is_even )
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
#ifndef __INTELLISENSE__
	res 
	=	(	ht::Fold( Countable<int>(3, 1), minus, 10 ) == 4
		&&	ht::Fold( Countable<int>(3, 1), minus ) == -4
		&&	ht::Fold<SHARE>( Countable<int>(3, 1), minus ) == -4
		&&	ht::rFold( Countable<int>(3, 1), minus, 10 ) == 4
		&&	ht::rFold( Countable<int>(3, 1), minus ) == 0
		&&	(	ht::Fold<ht::Par<>>( Countable<int>(1000, 1), plus, int(0) )
			==	ht::Fold( Countable<int>(1000, 1), plus )
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


static void Zip_Test()
{
	Serial<int> sr1{1, 4, 7, 10};
	Serial<double> sr2{-1, -2, -3, -4};

	bool res = false;
#ifndef __INTELLISENSE__
	res
	=	(	Judge::have_the_same
			(	ht::Zip(sr1, sr2)
			,	Serial< Family<int, double> >
				{	Family<int, double>{1, -1.0}, Family<int, double>{4, -2.0}
				,	Family<int, double>{7, -3.0}, Family<int, double>{10, -4.0}
				}
			)
		&&	Judge::have_the_same
			(	ht::Zip< ht::Par<2> >(sr1, sr2)
			,	Serial< Family<int, double> >
				{	Family<int, double>{1, -1.0}, Family<int, double>{4, -2.0}
				,	Family<int, double>{7, -3.0}, Family<int, double>{10, -4.0}
				}
			)
		);
#endif
	is_True(res);
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

		Zip_Test();

		std::wcout << L"High Templar Test Complete.\n";
	}
	catch(spec::Exception e)
	{
		std::wcout << L"High Templar Test Failed.\n";

		throw e;
	}
}
