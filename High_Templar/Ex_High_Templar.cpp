
#include "High_Templar.hpp"

#include "..\Pinweight\Pinweight.hpp"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

using namespace sgm;


class Tutorial : No_Making
{
private:
	template<class C1, class C2>
	static bool is_Equal(C1 c1, C2 c2)
	{
		bool res = c1.size() == c2.size();

		for
		(	auto Pa = std::make_pair(c1.begin(), c2.begin())
		;	res && Pa.first != c1.end()
		;	res = *Pa.first++ == *Pa.second++
		);

		return res;
	}


	struct SHARE{  template<class T> using type = Pinweight<T>;  };


public:
	template<unsigned> static void Case();

	template<> static void Case<1>()
	{
		assert
		(	is_Equal( indices(5, 1), std::vector<int>{1, 2, 3, 4, 5} )
		&&	is_Equal( indices<unsigned>(4), std::array<unsigned, 4>{0, 1, 2, 3} )
		);
	}


	template<> static void Case<2>()
	{
		auto negate = [](int x)-> int{  return -x;  };
		auto answer = std::vector<int>{ -1, -2, -3, -4, -5 };

		assert
		(	is_Equal(  Morph( indices(5, 1), negate ), answer  )
		&&	is_Equal(  Morph( SHARE(), indices(5, 1), negate ), answer  )
		&&	is_Equal(  Morph<SHARE>( indices(5, 1), negate ), answer  )
		&&	is_Equal
			(	Morph( indices(100000, 1), negate )
			,	Morph<Parallel_Proc>( indices(100000, 1), negate )
			)
		);
	}


	template<> static void Case<3>()
	{
		auto is_even = [](int const& x)-> bool{  return x % 2 == 0;  };
		auto answer = std::vector<int>{2, 4, 6, 8, 10};
		
		Filter<Parallel_Proc>( indices(100000, 1), is_even );

		assert
		(	is_Equal(  Filter( indices(10, 1), is_even ), answer  )
		&&	is_Equal(  Filter( SHARE(), indices(10, 1), is_even ), answer  )
		&&	is_Equal(  Filter<SHARE>( indices(10, 1), is_even ), answer  )
		//&&	is_Equal
		//	(	Filter( indices(100000, 1), is_even )
		//	,	Filter<Parallel_Proc>( indices(100000, 1), is_even )
		//	)
		);
	}


	template<> static void Case<4>()
	{
		auto minus = [](int res, int const& x)-> int{  return res - x;  };

		assert
		(	Fold( indices(3, 1), minus, 10 ) == 4
		&&	Fold( indices(3, 1), minus ) == -4
		&&	Fold<SHARE>( indices(3, 1), minus ) == -4
		&&	rFold( indices(3, 1), minus, 10 ) == 4
		&&	rFold( indices(3, 1), minus ) == 0
		&&	rFold( SHARE(), indices(3, 1), minus, 10 ) == 4
		);
	}


};
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

template<class T>
struct Base
{
	Base(T t) : val(t){}
	T val;

	auto get()-> T&{  return val;  }
	auto get() const-> T const&{  return val;  }
};


template<class T>
struct Derived : Base<T>
{
	using BB = Base<T>;

	Derived(T t) : BB(t){}

	auto operator[](int)-> T&{  return BB::get();  }
	auto operator[](int) const-> T const&{  return BB::get();  }
};



int main()
{
	Tutorial::Case<1>();
	Tutorial::Case<2>();
	Tutorial::Case<3>();
	Tutorial::Case<4>();

	Derived<int> x(3);
	Derived<int> const x2(2);

	x[3];
	x2[3];


	return 0;
}
