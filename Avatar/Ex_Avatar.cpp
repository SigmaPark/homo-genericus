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
			sgm::Avatar<double, sgm::Var> avt = x;

			avt = 30;

			assert(avt == x && x == 30.0);
		}
		{
			sgm::Avatar<double const> cavt = cx;

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
			<	Avatar<  Avatar< Avatar<int> >  >, Avatar<int>
			>::value
		&&	!std::is_convertible
			<	Avatar<double>, Avatar<double, Var>
			>::value
		&&	std::is_convertible
			<	Avatar< Avatar<double, Var> const >, Avatar<double>
			>::value
		&&	std::is_convertible
			<	Avatar<int, invar>, Avatar<  Avatar< Avatar<int, Var>, Var >, Var  >
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

template< template<class...> class...TQS >
class ManyQuestions
{
};


int main()
{
	UnitTest::Test1();
	UnitTest::Test2();
	UnitTest::Test3();
	UnitTest::Test4();
	UnitTest::Test5();

	using MQ = ManyQuestions<std::is_integral, std::is_const, std::is_reference>;

	return 0;
}

