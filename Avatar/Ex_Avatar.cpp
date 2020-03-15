#include "Avatar.hpp"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

class UnitTest
{
	class No_DftGen_t
	{
	public:
		No_DftGen_t(int x) : x(x) {}

		int x;
	};



public:
	UnitTest() = delete;


	static void Test1()
	{
		double x = 3;
		double const cx = 6;

		{
			sgm::Avatar<double> avt = x;

			avt = 30;

			assert(avt == x && x == 30.0);
		}
		{
			sgm::Avatar<double const> cavt = cx;

			
		}

	}


	static void Test2()
	{

	}


	static void Test3()
	{


	}


	static void Test4()
	{

	}


	static void Test5()
	{

	}


};
//========//========//========//========//=======#//========//========//========//========//=======#


int main()
{
	UnitTest::Test1();
	UnitTest::Test2();
	UnitTest::Test3();
	UnitTest::Test4();
	UnitTest::Test5();

	return 0;
}

