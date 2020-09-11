#include <tuple>
#include "..\Specification\Specification.hpp"


using namespace sgm;
using spec::is_True;


static void Factorial_Test()
{
	is_True
	(	[](auto n, auto res)
		{
			while(n > 1)
				std::tie(n, res) = std::tuple(n - 1, n*res);

			return res;
		}(5, 1)
	==	5 * 4 * 3 * 2 * 1
	);
}


static void Fibonacci_Test()
{
	is_True
	(	[](auto n, auto prev, auto next)
		{
			while(n > 1)
				std::tie(n, prev, next) = std::tuple(n - 1, next, prev + next);

			return next;
		}(10, 0, 1)
	==	55	// fibonacci sequance : 1, 1, 2, 3, 5, 8, 13, 21, 34, 55
	);
}


#include <iostream>
#include "Test_Recursion17.hpp"


void Test_sgm_Recursion::test()
{
	try
	{
		::Factorial_Test();
		::Fibonacci_Test();
		
		std::wcout << L"Recursion Test Complelete.\n";
	}
	catch(...)
	{
		std::wcout << L"Recursion Test Failed.\n";

		throw;
	}
}
