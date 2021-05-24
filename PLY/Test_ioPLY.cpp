#include "ioPLY.hpp"
#include "..\Specification\Specification.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

static void Test01()
{
	std::string const filename = "plyTest.ply";
	std::ifstream fs(filename);

	if(!fs.is_open())
		std::cout << "failed to open " << filename.c_str() << std::endl;
	else
	{
		std::string line;
		std::getline(fs, line);

		std::cout << line.c_str() << std::endl;

		fs.close();
	}
}


#include "Test_ioPLY.hpp"


void Test_sgm_ioPLY::test()
{
	try
	{
		::Test01();

		std::wcout << L"ioPLY Test Complete.\n";
	}
	catch(...)
	{
		std::wcerr << L"ioPLY Test Failed.\n";

		throw;
	}
}