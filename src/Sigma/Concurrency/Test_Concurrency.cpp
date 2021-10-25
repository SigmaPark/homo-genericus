#include "Concurrency.hpp"
#include "..\Specification\Specification.hpp"


using namespace sgm::par;
using namespace sgm::spec;


template<unsigned>
static void Test();


template<>
void Test<1>()
{
	size_t sum = 0;

	auto func
	=	[&sum](size_t bidx, size_t const eidx, unsigned const)
		{
			for(;  bidx < eidx;  sum += ++bidx);
		};

	Parallel<4>()(100, func);

	is_True(sum == 5050);
}


template<>
void Test<2>()
{
	size_t sum = 0;

	auto func
	=	[&sum](size_t bidx, size_t const eidx, unsigned const)
		{
			for(;  bidx < eidx;  sum += ++bidx);
		};

	Parallel<>(4)(100, func);

	is_True(sum == 5050);
}


#include "Test_Concurrency.hpp"
#include <iostream>


SGM_SPEC_TESTS(sgm_Concurrency)
{	::Test<1>
,	::Test<2>
};