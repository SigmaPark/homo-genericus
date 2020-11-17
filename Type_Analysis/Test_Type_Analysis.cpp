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
	(	(	std::is_same<AA, decltype(Declval<AA>())>::value
		&&	std::is_same<BB, decltype(Declval<BB>())>::value
		)
	,	""
	);
}


#include <climits>


static void Test_Size_info()
{
	static_assert
	(	(	CHAR_MAX == Size_info<char>::MAXIMUM && CHAR_MIN == Size_info<char>::MINIMUM
		&&	SHRT_MAX == Size_info<short>::MAXIMUM && SHRT_MIN == Size_info<short>::MINIMUM
		&&	INT_MAX == Size_info<int>::MAXIMUM && INT_MIN == Size_info<int>::MINIMUM
		&&	LLONG_MAX == Size_info<long long>::MAXIMUM 
		&&	LLONG_MIN == Size_info<long long>::MINIMUM
		&&	UCHAR_MAX == Size_info<unsigned char>::MAXIMUM
		&&	USHRT_MAX == Size_info<unsigned short>::MAXIMUM
		&&	UINT_MAX == Size_info<unsigned>::MAXIMUM
		&&	ULLONG_MAX == Size_info<unsigned long long>::MAXIMUM
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
		::Test_Size_info();

		std::wcout << L"Type_Analysis Test Complete.\n";
	}
	catch(...)
	{
		std::wcerr << L"Type_Analysis Test Failed.\n";

		throw;
	}
}