#include "Avatar.hpp"
#include "..\Specification\Specification.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


using namespace sgm;
using spec::is_True;


static void Test1()
{
	double x = 3;
	double const cx = 6;

	{
		auto avt = sgm::Refer(x);

		x = 30.0;

		is_True(avt == x && x == 30.0);
	}
	{
		auto cavt = sgm::Refer(cx);

		//cavt = 23;	// error

		is_True(cavt == cx);

		cavt(x);

		is_True(cavt == x);

		x = 50;

		is_True(cavt == 50.0);
	} 

}


static void Test2()
{
	spec::All_True
	<	std::is_convertible< Avatar<double>, Avatar<double const> >::value
	,	!std::is_convertible< Avatar<double const>, Avatar<double> >::value
	>();


	Avatar<double const> cavt = Avatar<double>();

	//cavt = Avatar<double>();	// Compile Failed.

	static_assert( is_constAvatar<decltype(cavt)>::value, " cavt is not constant." );
	is_True(cavt.is_yet());

}


static void Test3()
{
	Avatar< Avatar<int const> > avt1;

	int x = 9;

	avt1(x);

	x = 4;

	is_True(avt1 == 4);

	avt1.~Avatar_t();

	is_True(avt1.has_gone());
}


static void Test4()
{
	int x = 3;

	Avatar<int const> avt1;

	avt1(x);

	is_True(avt1 == 3);

	int const cx = 4;

	avt1(cx);

	Avatar<int> avt2;

	is_True(avt1 == 4 && avt2.is_yet());
}


static void Test5()
{
	int x = 3;

	Avatar<  Avatar< Avatar<int> const >  > avt1 = x;

	spec::All_True
	<	is_constAvatar<   Avatar<  Avatar< Avatar<int> const >  >   >::value
	,	is_constAvatar<   Avatar<  Avatar< Avatar<int const> >  >   >::value
	,	is_constAvatar<  constAvatar< Avatar<int> >  >::value
	,	is_constAvatar<   Avatar<  Avatar< Avatar<int> > const  >   >::value
	,	is_constAvatar< Avatar< Avatar<int> > const  >::value
	>();

//	int const cx = 40;

//	constAvatar< Avatar<int> > avt_ = cx;	// Compile Fails.

	auto avt2 = Refer(avt1);

	is_True(avt1 == 3 && avt2 == 3);
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Avatar.hpp"
#include <iostream>


void Test_sgm_Avatar::test() 
{
	try
	{
		::Test1();
		::Test2();
		::Test3();
		::Test4();
		::Test5();
	}
	catch(spec::Exception e)
	{
		std::wcout << L"Avatar Test Failed.\n";

		throw e;
	}

	std::wcout << L"Avatar Test Complete.\n";
}
