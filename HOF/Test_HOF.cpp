#include "sgmHOF.hpp"
#include "..\Specification\Specification.hpp"


using namespace sgm::hof;


template<unsigned>
static void Test();


template<>
void Test<1>()
{
	auto Equal = [](auto x1, auto x2){  return abs(x1 - x2) < 0.001;  } / Dim<2>;
	auto const Basel = pow(2 * acos(0), 2) / 6.0;

	sgm::spec::is_True
	(	(	Equal(Basel, __)
		|	Fold_f<>( __, [](auto x1, auto x2){  return x1 + x2;  } )
		|	Morph_f<>( __, [](auto x){  return 1.0/double(x*x);  } )
		)( sgm::Countable<int>(10000, 1) )
	);
}


#include "Test_HOF.hpp"
#include <iostream>


void Test_sgm_HOF::test()
{
	try
	{
		::Test<1>();

		std::wcout << L"HOF Test Complete.\n";
	}
	catch(...)
	{
		std::wcerr << L"HOF Test Failed.\n";

		throw;
	}
}