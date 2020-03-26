#include "Avatar.hpp"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

class UnitTest
{
public:
	UnitTest() = delete;


	static void Test1()
	{
		double x = 3;
		double const cx = 6;

		{
			//sgm::Avatar<double> avt = x;
			auto avt = sgm::make_Avatar(x);

			avt = 30;

			assert(avt == x && x == 30.0);
		}
		{
			//sgm::Avatar<double const> cavt = cx;
			auto cavt = sgm::make_Avatar(cx);

			//cavt = 23;	// error

			assert(cavt == cx);

			cavt(x);

			assert(cavt == x);

			x = 50;

			assert(cavt == 50.0);
		} 

	}


	static void Test2()
	{
		using namespace sgm;

		static_assert
		(	std::is_convertible
			<	Avatar<double>, Avatar<double const>
			>::value
		&&	!std::is_convertible
			<	Avatar<double const>, Avatar<double>
			>::value
		&&	!std::is_convertible
			<	Avatar< Avatar<double> const>, Avatar<double>
			>::value
		&&	std::is_convertible
			<	Avatar<int>, Avatar< Avatar<int const> >
			>::value
		,	""
		);
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

