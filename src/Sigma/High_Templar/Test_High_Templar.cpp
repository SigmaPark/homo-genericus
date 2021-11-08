#include "High_Templar.hpp"
#include "..\Specification\Specification.hpp"
#include "..\Pinweight\Pinweight.hpp"
#include "..\Avatar\Avatar.hpp"
#include <set>


namespace test_ht
{

	struct SHARE;
	struct CREFER;

	template<class RG1, class RG2>
	static bool are_same_ranges(RG1 &&rg1, RG2 &&rg2);

	static void MorphTest();
	static void FilterTest();
	static void FoldTest();
	static void Plait_Test();
	static void Plait_loop_Test();
	static void Test01();

}
//========//========//========//========//=======#//========//========//========//========//=======#


struct test_ht::SHARE 
:	sgm::Type_Decorator{  template<class T> using type = sgm::Pinweight<T>;  };

struct test_ht::CREFER 
:	sgm::Type_Decorator{  template<class T> using type = sgm::constAvatar<T>;  };


template<class RG1, class RG2>
bool test_ht::are_same_ranges(RG1 &&rg1, RG2 &&rg2)
{	
	auto itr1 = rg1.begin();
	auto itr2 = rg2.begin();

	while(itr1 != rg1.end() && itr2 != rg2.end() && *itr1 == *itr2)
		itr1++,  itr2++;

	return !(itr1 != rg1.end() || itr2 != rg2.end());
}


void test_ht::MorphTest()
{
	auto answer = sgm::Serial<double, 5>{ -1, -2, -3, -4, -5 };

	sgm::spec::is_True
	(	are_same_ranges
		(	sgm::ht::Morph<SHARE>
			(	sgm::Countable<int>(5, 1), [](int x)-> double{  return -x;  } 
			)
		,	answer
		)
	);

	auto const msr
	=	sgm::ht::Morph
		(	sgm::Countable<int>(5, 1), [](int x)-> double{  return -x;  } 
		) . eval< sgm::ht::Par<2> >();

	sgm::spec::is_True
	(	are_same_ranges(msr, answer)
	);

	auto const msr2
	=	sgm::ht::Morph
		(	std::set<int>{1,2,3,4,5}, [](int x)-> double{  return -x;  } 
		) .	eval();
}


void test_ht::FilterTest()
{
	auto is_even = [](int const& x)-> bool{  return x % 2 == 0;  };
	auto answer = sgm::Serial<int, 5>{2, 4, 6, 8, 10};

	sgm::Serial<int> const sr1 = sgm::Countable<int>(10, 1);

	sgm::spec::is_True
	(	are_same_ranges( sgm::ht::Filter(sr1, is_even), answer )
	&&	are_same_ranges( sgm::ht::Filter<CREFER>(sr1, is_even), answer )
	);

	sgm::Serial<int> const fsr = sgm::ht::Filter(sr1, is_even);

	sgm::spec::is_True( are_same_ranges(fsr, answer) );
}


void test_ht::FoldTest()
{
	auto minus = [](int res, int const& x)-> int{  return res - x;  };
	auto plus = [](int const res, int const x)-> int{  return res + x;  };

	bool res = false;

	using sgm::Countable;

	res 
	=	(	sgm::ht::Fold( Countable<int>(3, 1), minus, 10 ) == 4
		&&	sgm::ht::Fold( Countable<int>(3, 1), minus ) == -4
		&&	sgm::ht::Fold<SHARE>( Countable<int>(3, 1), minus ) == -4
		&&	sgm::ht::rFold( Countable<int>(3, 1), minus, 10 ) == 4
		&&	sgm::ht::rFold( Countable<int>(3, 1), minus ) == 0
		&&	(	sgm::ht::Fold<sgm::ht::Par<>>( Countable<int>(20, 1), plus, int(0) )
			==	sgm::ht::Fold( Countable<int>(20, 1), plus )
			)
		);

	sgm::spec::is_True(res);	
}


void test_ht::Plait_Test()
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

	sgm::spec::is_True(  are_same_ranges( sgm::ht::Plait(sr1, sr2, sr3), ans1 )  );
}


void test_ht::Plait_loop_Test()
{
	std::initializer_list<int> const iL{1, 3, 5, 7};
	sgm::Serial<int, 4> const sr1{2, 4, 6, 8};
	sgm::Serial<int> sr2{-3, -2, -1, 0};

	static_assert
	(	sgm::is_Same
		<	typename 
			std::iterator_traits
			<	sgm::Decay_t<decltype( sgm::ht::Plait(iL, sr1, sr2).begin() )>
			>::	iterator_category
		,	std::random_access_iterator_tag
		>::	value
	,	"iterator_traits tracking failed."
	);

	for( auto &&fam : sgm::ht::Plait(iL, sr1, sr2) )
		fam.get<2>() += fam.get<0>() + fam.get<1>();

	sgm::spec::is_True
	(	are_same_ranges(sr2, std::initializer_list<int>{0, 5, 10, 15})
	);
}


#include <vector>


void test_ht::Test01()
{
	std::vector<float> vec1{1,2,3}, vec2{10, 20, 30};

	sgm::spec::is_True
	(	are_same_ranges
		(	sgm::ht::Morph
			(	sgm::ht::Plait(vec1, vec2)
			,	[](auto const &fam){  return std::get<1>(fam)/std::get<0>(fam);  }
			)
		,	std::vector<float>{10, 10, 10}
		)
	);
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_High_Templar.hpp"


SGM_SPEC_TESTS(sgm_High_Templar)
{	test_ht::MorphTest
,	test_ht::FilterTest
,	test_ht::FoldTest
,	test_ht::Plait_Test
,	test_ht::Plait_loop_Test
,	test_ht::Test01
};