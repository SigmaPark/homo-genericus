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
	);

	sgm::Serial<int> const fsr = sgm::ht12::Filter(sr1, is_even);

	sgm::spec::is_True( are_same_ranges(fsr, answer) );
}


void test_ht12::FoldTest()
{
	auto minus = [](int res, int const& x)-> int{  return res - x;  };
	auto plus = [](int const res, int const x)-> int{  return res + x;  };

	bool res = false;

	res 
	=	(	sgm::ht12::Fold( sgm::Countable<int>(3, 1), minus, 10 ) == 4
		&&	sgm::ht12::Fold( sgm::Countable<int>(3, 1), minus ) == -4
		&&	sgm::ht12::Fold<SHARE>( sgm::Countable<int>(3, 1), minus ) == -4
		&&	sgm::ht12::rFold( sgm::Countable<int>(3, 1), minus, 10 ) == 4
		&&	sgm::ht12::rFold( sgm::Countable<int>(3, 1), minus ) == 0
		&&	(	sgm::ht12::Fold<sgm::ht12::Par<>>( sgm::Countable<int>(20, 1), plus, int(0) )
			==	sgm::ht12::Fold( sgm::Countable<int>(20, 1), plus )
			)
		);

	sgm::spec::is_True(res);	
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_High_Templar12.hpp"
#include <iostream>


void Test_sgm_High_Templar12::test()
{
	try
	{
		test_ht12::MorphTest();
		test_ht12::FilterTest();
		test_ht12::FoldTest();

		std::wcout << L"High Templar 12 Test Complete.\n";
	}
	catch(sgm::spec::Exception e)
	{
		std::wcout << L"High Templar 12 Test Failed.\n";

		throw e;
	}
}