#include "..\Specification\Specification.hpp"


using namespace sgm;


static void Test_No_Making()
{
	class AA : No_Making{};

	//AA aa;		// Compile Error
}


static void Test_Declval()
{
	class AA : No_Making{};
	class BB;

	static_assert
	(	(	is_Same<AA, decltype(Declval<AA>())>::value
		&&	is_Same<BB, decltype(Declval<BB>())>::value
		)
	,	""
	);
}


#include "Test_Type_Analysis.hpp"
#include <iostream>


void Test_sgm_Type_Analysis::test()
{
	try
	{
		::Test_No_Making();
		::Test_Declval();


		std::wcout << L"Type_Analysis Test Complete.\n";
	}
	catch(...)
	{
		std::wcerr << L"Type_Analysis Test Failed.\n";

		throw;
	}
}