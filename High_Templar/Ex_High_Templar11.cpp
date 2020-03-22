#include <iterator>
#include <vector>
#include <functional>
#include <omp.h>
#include "High_Templar11.hpp"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

class Tutorial : sgm::No_Making
{
public:
	template<unsigned> static void Case();

	template<> static void Case<1>()
	{
		using namespace sgm;

		std::vector<int> v1 = indices(10000, 1);

		auto const a1 = Morph<PWT, PAR>(v1, [](int i)-> int{  return 2*i;  });

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
		assert(  Fold( indices<double>(10, 1), 5.0, std::plus<double>() ) == 60.0  );
	}


	template<> static void Case<4>()
	{
		using namespace sgm;

		auto const domain = sgm::indices(1000, 1);
		auto is_odd = [](int n)-> bool {  return n % 2 == 1;  };
		auto const odds = Filter<PAR>(domain, is_odd);

		for(auto x : odds)
			assert( is_odd(x) );
	}


};
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#



int main()
{
	Tutorial::Case<1>();
	Tutorial::Case<2>();
	Tutorial::Case<3>();
	Tutorial::Case<4>();



	return 0;
}

