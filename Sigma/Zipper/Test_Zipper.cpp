#include "Zipper.hpp"
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

	for(  auto&& [a, b, c] : sgm::Zipper( sgm::immut(s), sgm::immut(Li), vec )  )
		c += a+b;

	sgm::spec::is_True( vec.at(0) == 0 && vec.at(1) == 5 && vec.at(2) == 10 && vec.at(3) == 15 );
}


#include <chrono>
#include "..\High_Templar\High_Templar.hpp"
#include <iostream>


template<>
void Test<2>()
{
#ifdef _DEBUG
	auto constexpr size = static_cast<size_t>(1e6);
#else
	size_t constexpr size = 0x0000'0000'00ff'0000ULL;
#endif
	using namespace std::chrono;
	
	std::vector<size_t> vec1(size);
	sgm::Countable<size_t> cnt(size);

	auto tpoint = system_clock::now();

	for( auto[itr1, itr2] = std::pair(vec1.begin(), cnt.begin());  itr2 != cnt.end();  ++itr1,  ++itr2 )
		*itr1 = *itr2;

	duration<double> dt1 = system_clock::now() - tpoint;

	tpoint = system_clock::now();

	for( auto&& [x, c] : sgm::Zipper(vec1, cnt) )
		x = c;

	duration<double> dt2 = system_clock::now() - tpoint;

	//std::wcout << L"dt1 = " << dt1.count() << L",    dt2 = " << dt2.count() << std::endl;
	//sgm::spec::is_True( dt2.count()/dt1.count() < 1.1 );
}


#include "Test_Zipper.hpp"



void Test_sgm_Zipper::test()
{
	try
	{
		::Test<1>();
		::Test<2>();

		std::wcout << L"Zipper Test Complete.\n";
	}
	catch(...)
	{
		std::wcerr << L"Zipper Test Failed.\n";

		throw;
	}
}