#include <vector>
#include <array>
#include "..\Pinweight\Pinweight.hpp"
#include "..\Avatar\Avatar.hpp"
#include "Countable.hpp"


#define USE_STL_VECTOR_AND_ARRAY
#include "High_Templar.hpp"
#undef USE_STL_VECTOR_AND_ARRAY
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

using namespace sgm;


class Tutorial : No_Making
{
private:
	template<class C1, class C2>
	static bool is_Equal(C1 con1, C2 con2)
	{
		auto itr1 = con1.cbegin();
		auto itr2 = con2.cbegin();

		bool res = con1.size() == con2.size();

		while(itr1 != con1.cend() && res)
			res = *itr1++ == *itr2++;

		return res;
	}


	struct SHARE{  template<class T> using type = Pinweight<T>;  };


public:
	template<unsigned> static void Case();

	template<> static void Case<1>()
	{
		assert
		(	is_Equal( Countable<int>(5, 1), std::vector<int>{1, 2, 3, 4, 5} )
		&&	is_Equal( Countable<unsigned>(4), std::array<unsigned, 4>{0, 1, 2, 3} )
		);
	}


	template<> static void Case<2>()
	{
		auto negate = [](int x)-> int{  return -x;  };
		auto answer = std::vector<int>{ -1, -2, -3, -4, -5 };

		//assert
		//(	is_Equal(  Morph( indices(5, 1), negate ), answer  )
		//&&	is_Equal(  Morph( SHARE(), indices(5, 1), negate ), answer  )
		//&&	is_Equal(  Morph<SHARE>( indices(5, 1), negate ), answer  )
		//&&	is_Equal
		//	(	Morph( indices(100000, 1), negate )
		//	,	Morph<Parallel_Proc>( indices(100000, 1), negate )
		//	)
		//);
	}


	template<> static void Case<3>()
	{
		auto is_even = [](int const& x)-> bool{  return x % 2 == 0;  };
		auto answer = std::vector<int>{2, 4, 6, 8, 10};
		
		//assert
		//(	is_Equal(  Filter( indices(10, 1), is_even ), answer  )
		//&&	is_Equal(  Filter( SHARE(), indices(10, 1), is_even ), answer  )
		//&&	is_Equal(  Filter<SHARE>( indices(10, 1), is_even ), answer  )
		//&&	is_Equal
		//	(	Filter( indices(100000, 1), is_even )
		//	,	Filter<Parallel_Proc>( indices(100000, 1), is_even )
		//	)
		//);
	}


	template<> static void Case<4>()
	{
		auto minus = [](int res, int const& x)-> int{  return res - x;  };

		//assert
		//(	Fold( indices(3, 1), minus, 10 ) == 4
		//&&	Fold( indices(3, 1), minus ) == -4
		//&&	Fold<SHARE>( indices(3, 1), minus ) == -4
		//&&	rFold( indices(3, 1), minus, 10 ) == 4
		//&&	rFold( indices(3, 1), minus ) == 0
		//&&	rFold( SHARE(), indices(3, 1), minus, 10 ) == 4
		//);
	}


	template<> static void Case<5>()
	{
		//assert(  is_Equal( Countable<size_t>(5), indexable<size_t>{0, 1, 2, 3, 4} )  );
	}


};
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#




int main()
{
	Tutorial::Case<1>();
	Tutorial::Case<2>();
	Tutorial::Case<3>();
	Tutorial::Case<4>();
	Tutorial::Case<5>();

	return 0;
}
