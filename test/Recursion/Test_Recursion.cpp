/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Recursion/Recursion.hpp"
#include "Test_Recursion.hpp"


static void intro()
{
	sgm::spec::mdo
	<<	sgm::spec::Title(L"Introduction")
	<<	L"There are many cases that recursions are easier to understand and "
	<<	L"more effective to express than loop . "
	<<	L"However, implementations for recursion consume lots of callstack depth . "
	<<	L"The callstack depth is so limited that one should be careful "
	<<	L"to avoid stack overflow error when a recursive algorithm is used . "
	<<	L"SGM_RECURSION macro system gives you a fancy way to implement recursion codes "
	<<	L"without consuming any callstack . "
	<<	sgm::spec::empty_line;
}


static void How_to_use()
{
	sgm::spec::mdo 
	<<	sgm::spec::Title(L"How to Use")
	<<	L"SGM_RECURSION method consists of 2 lambdas . " << sgm::spec::newl;

	sgm::spec::mdo
	<<	LR"(
			[/* capture here like lambda does */] 
			SGM_RECURSION(param1, param2, param3)		// input parameter names without type
			{
				// ...

				if( /* halting condition */ )
				{
					// ...

					return /* result */ ;
				}
				else  /* recursion condition */
				{
					// ...

					return_RECURSION( new_param1, new_param2, new_param_3 ); // recursion here
				}
			}
			FROM_INITIAL_FUNCTION( /* input parameters declarations like general function */ )
			{
				// ...

				return recursion(initial_param1, initial_param2, initial_param3);
			};
		)"_code
	<<	sgm::spec::newl;

	sgm::spec::mdo
	<<	L"1st lambda-like macro starting with \" [...] SGM_RECURSION(...) \" defines "
	<<	L"how the recursion behaves . When you recurse again, be careful so that you must "
	<<	L"write \"return_RECURSION\", not \"return RECURSION\" or \"return recursion\" . "
	<<	sgm::spec::newl
	<<	L"2nd lambda starting with macro \"FROM_INITIAL_FUNCTION(...)\" enable you to set "
	<<	L"pre-actions and initial conditions for the recursion . "
	<<	L"Here, you can start recursion with a pre-defined function "
	<<	L"whose name is \"recursion()\" . "
	<<	sgm::spec::newl
	<<	L"Let's take a look how it works with some examples ."
	<<	sgm::spec::empty_line;

	sgm::spec::mdo << sgm::spec::Title(L"Examples");
}


static void Factorial()
{
	sgm::spec::mdo 
	<<	sgm::spec::Title(L"Factorial", 2)
	<<	sgm::spec::Load_code_block(L"factorial_example") << sgm::spec::newl;

BEGIN_CODE_BLOCK(factorial_example)
	auto factorial_f
	=	[] SGM_RECURSION(n, res)
		{
			if(n == 1)
				return res;
			else 
				return_RECURSION(n-1, n*res);
		} FROM_INITIAL_FUNCTION(int n){  return recursion(n, 1);  };


	SGM_SPEC_ASSERT
	(	factorial_f(1) == 1
	&&	factorial_f(2) == 2*1
	&&	factorial_f(3) == 3*2*1
	&&	factorial_f(4) == 4*3*2*1
	&&	factorial_f(5) == 5*4*3*2*1
	&&	factorial_f(6) == 6*5*4*3*2*1
	);


	int const factorial6
	=	[] SGM_RECURSION(n, res)
		{	
			if(n == 1)
				return res;
			else 
				return_RECURSION(n-1, n*res);
		} FROM_INITIAL_FUNCTION{  return recursion(6, 1);  }();


	SGM_SPEC_ASSERT(factorial6 == 6*5*4*3*2*1);
END_CODE_BLOCK(factorial_example)

	sgm::spec::mdo << sgm::spec::empty_line;
}


static void Fibonacci_Test()
{
	sgm::spec::mdo 
	<<	sgm::spec::Title(L"Fibonacci Sequence", 2)
	<<	sgm::spec::Load_code_block(L"fibonacci_example") << sgm::spec::newl;

BEGIN_CODE_BLOCK(fibonacci_example)
	auto fibonacci_f
	=	[] SGM_RECURSION(n, prev, next)
		{	
			if(n > 1)
				return_RECURSION(n-1, next, prev+next);
			else
				return next;
		} FROM_INITIAL_FUNCTION(auto n){  return recursion(n, 0, 1);  };


	SGM_SPEC_ASSERT
	(	fibonacci_f(1) == 1
	&&	fibonacci_f(2) == 1	
	&&	fibonacci_f(3) == 2	
	&&	fibonacci_f(4) == 3	
	&&	fibonacci_f(5) == 5	
	&&	fibonacci_f(6) == 8	
	&&	fibonacci_f(7) == 13
	);


	int const fibonacci7th
	=	[] SGM_RECURSION(n, prev, next)
		{	
			if(n > 1)
				return_RECURSION(n-1, next, prev+next);
			else
				return next;
		} FROM_INITIAL_FUNCTION{  return recursion(7, 0, 1);  }();
	

	SGM_SPEC_ASSERT(fibonacci7th == 13);
END_CODE_BLOCK(fibonacci_example)

	sgm::spec::mdo << sgm::spec::empty_line;
}


static void Upper_Bound_Test()
{
	sgm::spec::mdo 
	<<	sgm::spec::Title(L"Upper Bound of a Sequence", 2)
	<<	sgm::spec::Load_code_block(L"upperbound_example") << sgm::spec::newl;	

BEGIN_CODE_BLOCK(upperbound_example)
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

	SGM_SPEC_ASSERT
	(	test_f( upper_bound_f(0), 0 )
	&&	test_f( upper_bound_f(1), 1 )
	&&	test_f( upper_bound_f(2), 2 )
	&&	test_f( upper_bound_f(3), 2 )
	&&	test_f( upper_bound_f(4), 3 )
	&&	test_f( upper_bound_f(5), 5 )
	&&	test_f( upper_bound_f(6), 6 )
	&&	test_f( upper_bound_f(7), 6 )
	);
END_CODE_BLOCK(upperbound_example)

	sgm::spec::mdo << sgm::spec::empty_line;
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Recursion, /**/)
{	::intro
,	::How_to_use
,	::Factorial
,	::Fibonacci_Test
,	::Upper_Bound_Test
};