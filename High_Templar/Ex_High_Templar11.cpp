#include <iterator>
#include <vector>
#include <functional>
#include "High_Templar11.hpp"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

class Tutorial : public sgm::No_Making
{
public:
	template<unsigned> static void Case();

	template<> static void Case<1>()
	{
		using namespace sgm;

		std::vector<int> v1{2, 4, 8};

		auto const a1 = Morph(v1, [](int i)-> int{  return 2*i;  });

		auto itr = v1.cbegin();
		
		for(auto const& x : a1)
			assert(x == 2 * *itr++);
	}


	template<> static void Case<2>()
	{
		auto domain = sgm::indices(5);
		
		auto itr = domain.cbegin();
		 
		for(size_t n = 0; n < domain.size(); n++)
			assert(*itr++ == n);
	}


	template<> static void Case<3>()
	{
		using namespace sgm;

		assert(  Fold( indices<double>(10, 1), std::plus<double>() ) == 55.0  );
		assert(  Fold( indices<double>(10, 1), std::plus<double>(), 5.0 ) == 60.0  );

		assert
		(	Fold
			(	indices<float>(10, 1), indices<float>(10, 1)
			,	std::plus<float>(), std::multiplies<float>()
			) 
		==	385.f
		);

		assert
		(	Fold
			(	indices<float>(10, 1), indices<float>(10, 1)
			,	std::plus<float>(), std::multiplies<float>()
			,	15.f
			) 
		==	400.f
		);
	}

};
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#


int main()
{
	Tutorial::Case<1>();
	Tutorial::Case<2>();
	Tutorial::Case<3>();

	return 0;
}

