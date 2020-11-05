#include "Recursion.hpp"
#include "..\Specification\Specification.hpp"
#include "..\Avatar\Avatar.hpp"

using namespace sgm;


#if 0
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

	auto const 
		Ex1 
		=	[](int n, int res)
			{
				for
				(	auto recur = Recursion( Refer(n), Refer(res) )
				;	n > 1
				;	recur(n - 1, n * res)
				);
						
				return res;
			}(N, 1),
		Ex2
		=	[](  decltype( Recursion(N, 1) ) recur  )
			{
				while(recur._<1>() > 1)
					recur( recur._<1>() - 1, recur._<1>()*recur._<-1>() );

				return recur._<-1>();
			}( Recursion(N, 1) );


	are_Equivalent(Ex1, Ex2, answer);
}


static void Fibonacci_Test()
{
	are_Equivalent
	(	[](int n, int prev, int next)
		{
			for
			(	auto recur = Recursion( Refer(n), Refer(prev), Refer(next) )
			;	n > 1
			;	recur(n - 1, next, prev + next)
			);

			return next;
		}(10, 0, 1)
	,	55
	);
}
#endif

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


#include <iostream>
#include "Test_Recursion.hpp"


void Test_sgm_Recursion::test()
{
	try
	{
		//::Factorial_Test();
		//::Fibonacci_Test();
		Chain_Test();
		
		std::wcout << L"Recursion Test Complelete.\n";
	}
	catch(...)
	{
		std::wcout << L"Recursion Test Failed.\n";

		throw;
	}
}
