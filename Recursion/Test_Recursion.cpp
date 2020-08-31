#include "Recursion.hpp"
#include "..\Specification\Specification.hpp"
#include "..\Avatar\Avatar.hpp"

using namespace sgm;
using spec::are_Equivalent;


static void Factorial_Test()
{
	int const N = 5;

	auto const answer 
	=	[N](int res)
		{	
			for(auto d = N;  d > 1;  --d)
				res*=d;

			return res;
		}(1);


	are_Equivalent
	(	[](int n, int res)
		{
			for
			(	auto recur = Recursion( std::ref(n), std::ref(res) )
			;	n > 1
			;	recur(n - 1, n * res)
			);
					
			return res;
		}(N, 1)
	,	[](  decltype( Recursion(N, 1) ) recur  )
		{
			while(recur._<1>() > 1)
				recur( recur._<1>() - 1, recur._<1>()*recur._<-1>() );

			return recur._<-1>();
		}( Recursion(N, 1) )
	,	answer
	);
}


static void Fibonacci_Test()
{
	are_Equivalent
	(	[](int n, int prev, int next)
		{
			for
			(	auto recur = Recursion( std::ref(n), std::ref(prev), std::ref(next) )
			;	n > 1
			;	recur(n - 1, next, prev + next)
			);

			return next;
		}(10, 0, 1)
	,	55
	);
}


static void N_sum_Test()
{
	auto arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	
	are_Equivalent
	(	[](decltype( Chaining(arr) ) chain, int res)
		{
			for
			(	auto recur = Recursion( chain, Refer(res) )
			;	chain
			;	recur(recur._<1>().body(), res + recur._<1>().head())
			);

			return res;
		}( Chaining(arr), 0 )
	,	1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10
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
