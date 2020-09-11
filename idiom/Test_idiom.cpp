#include "idiom.hpp"
#include "..\Specification\Specification.hpp"


using namespace sgm;
using spec::is_True;


static void Test01()
{
	Freezer<int> fz(5);
}


static void Test02()
{
	class AA : public Limited<2, AA>{};
	class BB : public Limited<2, BB>{};


	AA aa1, aa2 = aa1;

	is_True(AA::nof_instance() == 2);

	try
	{
		AA aa3;

		is_True(false);
	}
	catch(Violation const& v)
	{
		is_True( dynamic_cast<Over_instantiation_Violation const*>(&v) != nullptr );
	}

	BB bb1;


	is_True(BB::nof_instance() == 1); 
}


#include "Test_idiom.hpp"
#include <iostream>


void Test_sgm_idiom::test()
{
	try
	{
		::Test01();
		::Test02();

		std::wcout << L"idiom Test Complete.\n";
	}
	catch(...)
	{
		std::wcout << L"idiom Test Failes.\n";

		throw;
	}
}