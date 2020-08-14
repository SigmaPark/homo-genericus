#include <vector>
#include <array>
#include "..\Pinweight\Pinweight.hpp"
#include "..\Avatar\Avatar.hpp"

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
	static void CountableTest()
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
			,	ht::Filter< ht::Par<>, SHARE >( Countable<int>(100000, 1), is_even )
			)
		);

		Serial<int> const sr1 = Countable<int>(10, 1);
		
		assert(  is_Equal( ht::Filter<CREFER>(sr1, is_even), answer )  );
	}

	
	static void FoldTest()
	{
		auto minus = [](int res, int const& x)-> int{  return res - x;  };
		auto plus = [](int const res, int const x)-> int{  return res + x;  };


		assert
		(	ht::Fold( Countable<int>(3, 1), minus, 10 ) == 4
		&&	ht::Fold( Countable<int>(3, 1), minus ) == -4
		&&	ht::Fold<SHARE>( Countable<int>(3, 1), minus ) == -4
		&&	ht::rFold( Countable<int>(3, 1), minus, 10 ) == 4
		&&	ht::rFold( Countable<int>(3, 1), minus ) == 0
		&&	(	ht::Fold<ht::Par<>>( Countable<int>(1000, 1), plus )
			==	ht::Fold( Countable<int>(1000, 1), plus )
			)
		);
	}


	static void Ex_LeibnizTest()
	{
		using namespace sgm::ht;

		size_t const N = 1000000;

		auto is_odd = [](size_t const n)-> bool{  return n % 2 == 1;  };
		auto plus = [](double const res, double const x)-> double{  return res + x;  };

		auto altered_harmonic 
		=	[](size_t const n)-> double
			{
				auto const den = static_cast<double>(n);

				return n % 4 == 1 ? 1.0 / den : -1.0 / den;
			};

		double const 
			quater_pi = acos(.0) * .5,
			Leibniz 
			=	Fold<Par<>>
				(	Morph<Par<>>(  Filter( Countable<>(N, 1), is_odd ), altered_harmonic  )
				,	plus
				);
			
		assert( abs(quater_pi - Leibniz) < 0.0001);
	}




};
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#


int main()
{
	Tutorial::CountableTest();
	Tutorial::MorphTest();
	Tutorial::FilterTest();
	Tutorial::FoldTest();

	Tutorial::Ex_LeibnizTest();

	return 0;
}
