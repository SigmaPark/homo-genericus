#include "Avatar.hpp"
#include "..\Carrier\Carrier.hpp"
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
			
			auto avt = sgm::make_Avatar(x);

			x = 30.0;

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
		,	""
		);

		Avatar<double const> cavt = Avatar<double>();

		//cavt = Avatar<double>();

		static_assert( decltype(cavt)::IS_CONST, " cavt is not constant." );
		assert(cavt.is_yet());

	}


	static void Test3()
	{
		using namespace sgm;

		int dx = 3;

		Carrier< Avatar<int> > crravt(10);

		crravt.emplace_back(dx);
		//crravt >> dx;


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

