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


struct _Recursor
{
	size_t const *b1, *b2;
	size_t const *const e1, *const e2;
};


static void Range_Check(size_t const N)
{
	size_t *const arr = new size_t[N], *const end = arr + N;
	
	for(size_t n = 0;  n < N;  ++n)
		arr[n] = n;

	struct Range{  size_t *const begin,  *const end;  }  rg{arr, end};

	is_True
	(	[](auto &&rg1, auto &&rg2)
		{
			auto b1 = rg1.begin;
			auto b2 = rg2.begin;
			auto const e1 = rg1.end;
			auto const e2 = rg2.end;			

			while(b1 != e1 && b2 != e2 && *b1 == *b2)
				std::tie(b1, b2) = std::tuple(b1 + 1, b2 + 1);

			return !(b1 != e1 || b2 != e2);
		//top:
		//	if(bool const on1 = b1 != e1, on2 = b2 != e2;  on1 && on2 && *b1++ == *b2++)
		//		goto top;
		//	else
		//		return !(on1 || on2);
		}(rg, rg)
	);


#define TAIL_RECURSION(...)	\
	[&, __VA_ARGS__]() mutable




	//TAIL_RECURSION(b1 = range1.begin, b2 = range2.begin, e1 = range1.end, e2 = range2.end)
	//{
	//	UNTIL(bool const on1 = b1 != e1, on2 = b2 != e2;  on1 && on2 && *b1 == *b2)
	//		replace(b1, b2).with(b1+1, b2+1);
	//	else
	//		return !(on1 || on2);
	//}();


	[&, range1 = rg, range2 = rg]() mutable
	{
		auto b1 = range1.begin;
		auto b2 = range2.begin;
		auto const e1 = range1.end;
		auto const e2 = range2.end;

		//auto recursion 
		//=	[&b1, &b2](auto &&_1, auto &&_2)
		//	{
		//		std::tie(b1, b2) = std::tuple(_1, _2);
		//	};

		//if(b1 != e1 && b2 != e2 && *b1 == *b2)
		//	recursion(b1+1, b2+1);

		bool const on1 = b1 != e1,  on2 = b2 != e2;

		return !(on1 || on2);
	}();

	delete[] arr;
}


#include <iostream>
#include "Test_Recursion17.hpp"


void Test_sgm_Recursion::test()
{
	try
	{
		::Factorial_Test();
		::Fibonacci_Test();
		::Range_Check(10);
		
		std::wcout << L"Recursion Test Complelete.\n";
	}
	catch(...)
	{
		std::wcout << L"Recursion Test Failed.\n";

		throw;
	}
}
