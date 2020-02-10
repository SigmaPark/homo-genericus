#include "Compound.hpp"
#include <iostream>
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

int main()
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
	
	return 0;
}