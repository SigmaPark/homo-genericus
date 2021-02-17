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
using sgm::spec::Specimen;


template<>
void Test<2>()
{
	Lazy Lz = []{  return Specimen(3);  };

	sgm::spec::is_True( *Lz == Specimen(3) );

	Specimen s1;

	s1 = Lz;

	static_cast<Specimen&>(Lz) = Specimen(24);

	sgm::spec::is_True( *Lz == Specimen(24) );

	Specimen& rz = Lz;
	Lazy const Lz2 = Lz;



	rz = 15;

	sgm::spec::is_True( *Lz == Specimen(15) && *Lz2 == 24 );


	Lz.~Lazy();

	sgm::spec::is_True(Lz.is_destructed() && s1 == 3);
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