#include "Lazy.hpp"
#include "..\Specification\Specification.hpp"


template<unsigned>
static void Test();


using sgm::Lazy;
using sgm::Make_Lazy;
using sgm::spec::Specimen;


template<>
void Test<2>()
{
	int number = 333;

	auto Lz = Make_Lazy( [&number]{  return Specimen(number);  } );

	number = 3;

	sgm::spec::is_True( *Lz == Specimen(3) );

	sgm::Lazy_value_t<decltype(Lz)> s1 = Lz;

	s1 = Lz;

	static_cast<Specimen&>(Lz) = Specimen(24);

	sgm::spec::is_True( *Lz == Specimen(24) );

	auto const Lz2 = Lz;

	Specimen& rz = Lz;

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
		//::Test<1>();
		::Test<2>();

		std::wcout << L"Lazy Test Complete.\n";
	}
	catch(...)
	{
		std::wcout << L"Lazy Test Failed.\n";

		throw;
	}
}