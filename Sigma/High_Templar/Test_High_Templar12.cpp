#include "High_Templar12.hpp"
#include "..\Specification\Specification.hpp"
#include "Countable.hpp"
#include "..\Pinweight\Pinweight.hpp"
#include "..\Avatar\Avatar.hpp"


namespace test_ht12
{

	struct SHARE;
	struct CREFER;

	template<class RG1, class RG2>
	static bool are_same_ranges(RG1 &&rg1, RG2 &&rg2);

	static void MorphTest();
	static void FilterTest();
	static void FoldTest();
	static void Plait_Test();

}
//========//========//========//========//=======#//========//========//========//========//=======#


struct test_ht12::SHARE 
:	sgm::Type_Decorator{  template<class T> using type = sgm::Pinweight<T>;  };

struct test_ht12::CREFER 
:	sgm::Type_Decorator{  template<class T> using type = sgm::constAvatar<T>;  };


template<class RG1, class RG2>
bool test_ht12::are_same_ranges(RG1 &&rg1, RG2 &&rg2)
{	
	auto itr1 = rg1.begin();
	auto itr2 = rg2.begin();

	while(itr1 != rg1.end() && itr2 != rg2.end() && *itr1 == *itr2)
		itr1++,  itr2++;

	return !(itr1 != rg1.end() || itr2 != rg2.end());
}


void test_ht12::MorphTest()
{
	auto answer = std::initializer_list<double>{ -1, -2, -3, -4, -5 };

	sgm::spec::is_True
	(	are_same_ranges
		(	sgm::ht12::Morph<SHARE>
			(	sgm::Countable<int>(5, 1), [](int x)-> double{  return -x;  } 
			)
		,	answer
		)
	);

	sgm::Serial<double> const msr
	=	sgm::ht12::Morph
		(	sgm::Countable<int>(5, 1), [](int x)-> double{  return -x;  } 
		);

	sgm::spec::is_True
	(	are_same_ranges(msr, answer)
	);
}


void test_ht12::FilterTest()
{
	auto is_even = [](int const& x)-> bool{  return x % 2 == 0;  };
	auto answer = std::initializer_list<int>{2, 4, 6, 8, 10};

	sgm::Serial<int> const sr1 = sgm::Countable<int>(10, 1);

	sgm::spec::is_True
	(	are_same_ranges( sgm::ht12::Filter(sr1, is_even), answer )
	&&	are_same_ranges( sgm::ht12::Filter<CREFER>(sr1, is_even), answer )
	);

	sgm::Serial<int> const fsr = sgm::ht12::Filter(sr1, is_even);

	sgm::spec::is_True( are_same_ranges(fsr, answer) );
}


void test_ht12::FoldTest()
{
	auto minus = [](int res, int const& x)-> int{  return res - x;  };
	auto plus = [](int const res, int const x)-> int{  return res + x;  };

	bool res = false;

	using sgm::Countable;

	res 
	=	(	sgm::ht12::Fold( Countable<int>(3, 1), minus, 10 ) == 4
		&&	sgm::ht12::Fold( Countable<int>(3, 1), minus ) == -4
		&&	sgm::ht12::Fold<SHARE>( Countable<int>(3, 1), minus ) == -4
		&&	sgm::ht12::rFold( Countable<int>(3, 1), minus, 10 ) == 4
		&&	sgm::ht12::rFold( Countable<int>(3, 1), minus ) == 0
		&&	(	sgm::ht12::Fold<sgm::ht12::Par<>>( Countable<int>(20, 1), plus, int(0) )
			==	sgm::ht12::Fold( Countable<int>(20, 1), plus )
			)
		);

	sgm::spec::is_True(res);	
}


void test_ht12::Plait_Test()
{
	using sgm::Serial;
	using sgm::Family;
	using sgm::Pinweight;
	
	Serial<int> sr1{1, 4, 7, 10};
	Serial<double> sr2{-1, -2, -3, -4};	
	Serial<char> sr3{'a', 'b', 'c', 'd'};

	using fam_t = Family<int, double, char>;

	auto ans1
	=	Serial<fam_t>
		{	fam_t(1, -1.0, 'a'), fam_t(4, -2.0, 'b')
		,	fam_t(7, -3.0, 'c'), fam_t(10, -4.0, 'd')
		};

	sgm::spec::is_True(  are_same_ranges( sgm::ht12::Plait(sr1, sr2, sr3), ans1 )  );

	//auto const sr = sgm::ht12::cPlait(sr1, sr2, sr3).eval();
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_High_Templar12.hpp"
#include <iostream>


void Test_sgm_High_Templar12::test()
{
	try
	{
		using namespace test_ht12;

		MorphTest();
		FilterTest();
		FoldTest();
		Plait_Test();

		std::wcout << L"High Templar 12 Test Complete.\n";
	}
	catch(sgm::spec::Exception e)
	{
		std::wcout << L"High Templar 12 Test Failed.\n";

		throw e;
	}
}