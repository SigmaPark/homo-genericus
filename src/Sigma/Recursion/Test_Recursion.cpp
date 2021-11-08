#include "Recursion.hpp"
#include "..\Specification\Specification.hpp"


using namespace sgm;
using spec::is_True;


static void Factorial_Test()
{
	int const answer = 5 * 4 * 3 * 2 * 1;

	is_True
	(	[n = 5, res = 1]() mutable
		{
			while(n > 1)
				res *= n--;

			return res;
		}()
	==	answer
	);

	is_True
	(	[] RECURSION(n, res)
		{
			if(n == 1)  return res;
			else  return_RECURSION(n - 1, n*res);
		}(5, 1)
	==	answer
	);
}


static void Fibonacci_Test()
{
	int const answer = 55;

	is_True
	(	[n = 10, prev = 0, next = 1]() mutable
		{
			while(n-->1)
				sgm::Tie(prev, next) = sgm::Forward_as_Family(next, prev + next);

			return next;
		}()
	==	answer	// fibonacci sequance : 1, 1, 2, 3, 5, 8, 13, 21, 34, 55
	);

	is_True
	(	[] RECURSION(n, prev, next)
		{
			if(n > 1)  return_RECURSION(n - 1, next, prev + next);
			else  return next;
		}(10, 0, 1)
	==	answer
	);
}


static void Range_Check()
{
	size_t constexpr size = 5;
	int const arr1[size] = {0, 1, 2, 3, 4},  arr2[size] = {0, 1, 2, 3, 4};

	is_True
	(	[e1 = arr1 + size,  e2 = arr2 + size] RECURSION(ptr1, ptr2)
		{
			if(bool const on1 = ptr1 != e1,  on2 = ptr2 != e2;  !on1 || !on2 || *ptr1 != *ptr2)
				return !(on1 || on2);
			else
				return_RECURSION(++ptr1, ++ptr2);
		}(&arr1[0], &arr2[0])
	);
}


#include "Test_Recursion.hpp"


SGM_SPEC_TESTS(sgm_Recursion)
{	::Factorial_Test
,	::Fibonacci_Test
,	::Range_Check
};