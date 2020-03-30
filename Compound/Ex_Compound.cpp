#include "Compound.hpp"
#include <iostream>
#include <cassert>
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

struct UnitTest 
{
	UnitTest() = delete;

	template<unsigned> static void Case();

	template<>
	static void Case<1>()
	{
		using namespace sgm;

		Compound<int, double, wchar_t const*> cmp1(34, 0.43, L"Compound");

		std::cout 
		<<	Get<0>(cmp1) << std::endl
		<<	Get<1>(cmp1) << std::endl;

		std::wcout << Get<2>(cmp1) << std::endl;

		char const* str = "str";

		auto cmp2 = Make_Compound(4, Get<1>(cmp1), "str");
		
		auto cmp3 = Forward_as_Compound(4, Get<1>(cmp1), str);
	}


	template<>
	static void Case<2>()
	{
		using namespace sgm;

		//Compound<int, double> cmp1(2, 4.2), cmp2;

		//cmp2 = cmp1;


		//int x = 2;
		//double d = 3.4;

		//Bond(x, d) = Forward_as_Compound(5, 4.2);		// under debugging

		//assert(x == 5 && d == 4.2);


	}


};

int main()
{
	UnitTest::Case<1>();
	UnitTest::Case<2>();

	return 0;
}