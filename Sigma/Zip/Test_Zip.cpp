#include "Zip.hpp"
#include "..\Specification\Specification.hpp"


template<unsigned>  static void Test();


#include <vector>
#include <list>
#include <set>


template<>
void Test<1>()
{
	std::vector<int> vec{1, 3, 5, 7};
	std::list<int> Li{2, 4, 6, 8};
	std::set<int> s{-3, -2, -1, 0};

	//for(  auto&& [a, b, c] : sgm::Zipper( sgm::immut(s), sgm::immut(Li), vec )  )
	//	c += a+b;


	//sgm::spec::is_True( vec.at(0) == 0 && vec.at(1) == 5 && vec.at(2) == 10 && vec.at(3) == 15 );
}


#include "Test_Zip.hpp"
#include <iostream>


void Test_sgm_Zip::test()
{
	try
	{
		::Test<1>();

		std::wcout << L"Zip Test Complete.\n";
	}
	catch(...)
	{
		std::wcerr << L"Zip Test Failed.\n";

		throw;
	}
}