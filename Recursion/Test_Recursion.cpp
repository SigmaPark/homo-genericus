#include "Recursion.hpp"
#include "..\Specification\Specification.hpp"


using namespace sgm;
using spec::is_True;


static void Factorial_Test()
{
	int const N = 5;

	auto const answer 
	=	[N](int res)
		{	
			for(auto d = N;  d > 1;)
				res*=d,  --d;

			return res;
		}(1);


	is_True
	(	(	[](int n, int res)
			{
				for( auto recur = Recursion(n, res);	  n > 1; )
					recur(n - 1, n*res);
						
				return res;
			}(N, 1)
		==	answer
		)
	&&	(	[](  decltype( Recursion(N, 1) ) recur  )
			{
				while(recur.first() > 1)
					recur(recur.first() - 1, recur.first()*recur.last());

				return recur.last();
			}( Recursion(N, 1) )
		==	answer
		)
	);
}


static void Fibonacci_Test()
{
	is_True
	(	[](int n, int prev, int next)
		{
			for( auto recur = Recursion(n, prev, next);  n > 1; )
				recur(n - 1, next, prev + next);

			return next;
		}(10, 0, 1)
	==	55	// fibonacci sequance : 1, 1, 2, 3, 5, 8, 13, 21, 34, 55
	);
}


static void N_sum_Test()
{
	auto arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	
	is_True
	(	[](decltype( make_Chain(arr) ) chain, int res)
		{
			for( auto recur = Recursion(chain, res);  chain; )
				recur(chain.body(), res + chain.head());

			return res;
		}( make_Chain(arr), 0 )
	==	1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10
	);
}


#include <iostream>
#include "Test_Recursion.hpp"


void Test_sgm_Recursion::test()
{
	try
	{
		::Factorial_Test();
		::Fibonacci_Test();
		::N_sum_Test();
	}
	catch(...)
	{
		std::wcout << L"Recursion Test Failed.\n";

		throw;
	}

	std::wcout << L"Recursion Test Complelete.\n";
}
