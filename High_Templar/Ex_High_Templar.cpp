#include <vector>
#include <array>
#include "..\Pinweight\Pinweight.hpp"
#include "..\Avatar\Avatar.hpp"
#include "Countable.hpp"


#include "High_Templar.hpp"

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


	struct SHARE : Flag<SHARE>, Type_Decorator
	{  
		template<class T> using type = Pinweight<T>;  
	};

	struct CREFER : Flag<CREFER>, Type_Decorator
	{
		template<class T> using type = constAvatar<T>;  
	};


public:
	static void CountableClass()
	{
		assert
		(	is_Equal( Countable<int>(5, 1), std::vector<int>{1, 2, 3, 4, 5} )
		&&	is_Equal( Countable<unsigned>(4), std::array<unsigned, 4>{0, 1, 2, 3} )
		);		
	}


	static void MorphTest()
	{
		auto negate = [](int x)-> int{  return -x;  };
		auto answer = std::vector<int>{ -1, -2, -3, -4, -5 };


		assert
		(	is_Equal(  ht::Morph( Countable<int>(5, 1), negate ), answer  )
		&&	is_Equal(  ht::Morph<SHARE>( Countable<int>(5, 1), negate ), answer  )
		&&	is_Equal
			(	ht::Morph( Countable<int>(100000, 1), negate )
			,	ht::Morph< ht::Par<4>, SHARE >( Countable<int>(100000, 1), negate )
			)
		);
	}


	static void FilterTest()
	{
		auto is_even = [](int const& x)-> bool{  return x % 2 == 0;  };
		auto answer = std::vector<int>{2, 4, 6, 8, 10};
			

		assert
		(	is_Equal(  ht::Filter( Countable<int>(10, 1), is_even ), answer  )
		&&	is_Equal(  ht::Filter<SHARE>( Countable<int>(10, 1), is_even ), answer  )
		&&	is_Equal
			(	ht::Filter( Countable<int>(100000, 1), is_even )
			,	ht::Filter< ht::Par<> >( Countable<int>(100000, 1), is_even )
			)
		);
	}

	
	static void FoldTest()
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




};
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#




int main()
{
	Tutorial::CountableClass();
	Tutorial::MorphTest();
	Tutorial::FilterTest();


	return 0;
}
