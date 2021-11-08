#include "Chain.hpp"
#include "..\Specification\Specification.hpp"
#include "..\Avatar\Avatar.hpp"


using namespace sgm;


#include <vector>

static void Chain_Test()
{
	std::vector<int> const list = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	int const sum 
	=	[]( decltype(list) L, int res )
		{
			for( auto chain = Chaining(L);  chain;  res += *chain++ );

			return res;
		}(list, 0);


	spec::is_True(sum == 55);
}


#include "Test_Chain.hpp"


SGM_SPEC_TESTS(sgm_Chain)
{	//::Factorial_Test
	//::Fibonacci_Test
	Chain_Test
};