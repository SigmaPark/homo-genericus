#include "Lazy.hpp"
#include "..\Specification\Specification.hpp"


template<unsigned>
static void Test();


template<class F>
struct AA
{
	AA(F f) : _f(f){}

	F _f;
};


template<>
void Test<1>()
{
	int x = 11;

	AA aa([&x]{  return double(x);  }), aa2 = aa;

	sgm::spec::is_True(aa2._f() == 11.0);
}


using sgm::Lazy;


template<>
void Test<2>()
{
	Lazy Lz([]{  return sgm::spec::Specimen(3);  });

	sgm::spec::is_True( *Lz == sgm::spec::Specimen(3) );

	Lz.destruct();

	sgm::spec::is_True( *Lz == sgm::spec::Specimen(3) );
}


#include "Test_Lazy.hpp"
#include <iostream>


void Test_sgm_Lazy::test()
{
	try
	{
		::Test<1>();
		::Test<2>();

		std::wcout << L"Lazy Test Complete.\n";
	}
	catch(...)
	{
		std::wcout << L"Lazy Test Failed.\n";

		throw;
	}
}