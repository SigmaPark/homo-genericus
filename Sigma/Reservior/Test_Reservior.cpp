#include "Reservior.hpp"





#include "Test_Reservior.hpp"
#include <iostream>


void Test_sgm_Reservior::test()
{
	try
	{


		std::wcout << L"Reservior Test Complete.\n";
	}
	catch(...)
	{
		std::wcerr << L"Reservior Test Failed.\n";

		throw;
	}
}