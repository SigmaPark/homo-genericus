/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Recursion.hpp"
#include "Test_Recursion.hpp"


using sgm::spec::is_True;


static void Factorial()
{
	auto factorial_f
	=	[] SGM_RECURSION(n, res)
		{
			if(n == 1)
				return res;
			else 
				return_RECURSION(n-1, n*res);
		} FROM_INITIAL_FUNCTION(int n){  return recursion(n, 1);  };


	is_True
	(	factorial_f(1) == 1
	&&	factorial_f(2) == 2*1
	&&	factorial_f(3) == 3*2*1
	&&	factorial_f(4) == 4*3*2*1
	&&	factorial_f(5) == 5*4*3*2*1
	&&	factorial_f(6) == 6*5*4*3*2*1
	);

	is_True
	(	[] SGM_RECURSION(n, res)
		{	
			if(n == 1)
				return res;
			else 
				return_RECURSION(n-1, n*res);
		} FROM_INITIAL_FUNCTION{  return recursion(6, 1);  }()
	==	6*5*4*3*2*1
	);
}


static void Fibonacci_Test()
{
	auto fibonacci_f
	=	[] SGM_RECURSION(n, prev, next)
		{	
			if(n > 1)
				return_RECURSION(n-1, next, prev+next);
			else
				return next;
		} FROM_INITIAL_FUNCTION(auto n){  return recursion(n, 0, 1);  };


	is_True
	(	fibonacci_f(1) == 1
	&&	fibonacci_f(2) == 1	
	&&	fibonacci_f(3) == 2	
	&&	fibonacci_f(4) == 3	
	&&	fibonacci_f(5) == 5	
	&&	fibonacci_f(6) == 8	
	&&	fibonacci_f(7) == 13
	);
	
	is_True
	(	[] SGM_RECURSION(n, prev, next)
		{	
			if(n > 1)
				return_RECURSION(n-1, next, prev+next);
			else
				return next;
		} FROM_INITIAL_FUNCTION{  return recursion(7, 0, 1);  }()
	==	13
	);
}


static void Upper_Bound_Test()
{
	int const arr[] = {1, 2, 4, 5, 5, 6};


	auto upper_bound_f
	=	[] SGM_RECURSION(bp, ep, val)
		{
			if(bp == ep)
				return bp;
			else if( auto const mp = bp + (ep - bp)/2;  val < *mp )
				return_RECURSION(bp, mp, val);
			else
				return_RECURSION(mp+1, ep, val);
		} FROM_INITIAL_FUNCTION(auto const val_){  return recursion(arr, arr + 6, val_);  };


	auto test_f
	=	[pbegin = arr](int const* const ptr, long long const answer_diff)
		->	bool{  return ptr - pbegin == answer_diff;  };

	is_True
	(	test_f( upper_bound_f(0), 0 )
	&&	test_f( upper_bound_f(1), 1 )
	&&	test_f( upper_bound_f(2), 2 )
	&&	test_f( upper_bound_f(3), 2 )
	&&	test_f( upper_bound_f(4), 3 )
	&&	test_f( upper_bound_f(5), 5 )
	&&	test_f( upper_bound_f(6), 6 )
	&&	test_f( upper_bound_f(7), 6 )
	);
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Recursion, /**/)
{	::Factorial
,	::Fibonacci_Test
,	::Upper_Bound_Test
};