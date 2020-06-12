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

		Avatar< Avatar<int const> > avt1;

		int x = 9;

		avt1(x);

		x = 4;

		assert(avt1 == 4);

		avt1.~Avatar_t();

		assert(avt1.has_gone());
	}


	static void Test4()
	{
		using namespace sgm;

		int x = 3;

		Avatar<int const> avt1;

		avt1(x);

		assert(avt1 == 3);

		int const cx = 4;

		avt1(cx);

		Avatar<int> avt2;

		assert(avt1 == 4 && avt2.is_yet());
	}


	static void Test5()
	{
		using namespace sgm;

		int x = 3;

		Avatar<  Avatar< Avatar<int> const >  > avt1 = x;

		static_assert
		(	(	Avatar<  Avatar< Avatar<int> const >  >::IS_CONST
			&&	Avatar<  Avatar< Avatar<int const> >  >::IS_CONST
			//&&	constAvatar< Avatar<int> >::IS_CONST
			//&&	Avatar<  Avatar< Avatar<int> > const  >::IS_CONST
			)
		,	""
		);
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

