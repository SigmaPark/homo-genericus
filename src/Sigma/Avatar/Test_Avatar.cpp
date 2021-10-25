#include "Avatar.hpp"
#include "..\Specification\Specification.hpp"
#include <type_traits>
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

		cavt.reset(x);

		is_True(cavt == x);

		x = 50;

		is_True(cavt == 50.0);
	} 

}


static void Test2()
{
	static_assert
	(	(	std::is_convertible< Avatar<double>, Avatar<double const> >::value
		&&	!std::is_convertible< Avatar<double const>, Avatar<double> >::value
		)
	,	""
	);

	Avatar<double const> cavt = Avatar<double>();

	//cavt = Avatar<double>();	// Compile Failed.

	static_assert( is_constAvatar<decltype(cavt)>::value, " cavt is not constant." );
	is_True(cavt.is_yet());

}


static void Test3()
{
	Avatar< Avatar<int const> > avt1;

	int x = 9;

	avt1.reset(x);

	x = 4;

	is_True(avt1 == 4);

	avt1.distruct();

	is_True(avt1.has_gone());
}


static void Test4()
{
	int x = 3;

	Avatar<int const> avt1;

	avt1.reset(x);

	is_True(avt1 == 3);

	int const cx = 4;

	avt1.reset(cx);

	Avatar<int> avt2;

	is_True(avt1 == 4 && avt2.is_yet());
}


static void Test5()
{
	int x = 3;

	Avatar<  Avatar< Avatar<int> const >  > avt1 = x;

	static_assert
	(	(	is_constAvatar<   Avatar<  Avatar< Avatar<int> const >  >   >::value
		&&	is_constAvatar<   Avatar<  Avatar< Avatar<int const> >  >   >::value
		&&	is_constAvatar<  constAvatar< Avatar<int> >  >::value
		&&	is_constAvatar<   Avatar<  Avatar< Avatar<int> > const  >   >::value
		&&	is_constAvatar< Avatar< Avatar<int> > const  >::value
		)
	,	""
	);

//	int const cx = 40;

//	constAvatar< Avatar<int> > avt_ = cx;	// Compile Fails.

	auto avt2 = Refer(avt1);

	is_True(avt1 == 3 && avt2 == 3);
}


static void Test6()
{
	int x1 = 3, x2 = 5;
	Avatar<int> avt1 = x1, avt2 = x2;

	auto i3 = avt1 + avt2, i4 = avt2 - avt1;
	auto b1 = avt1 > avt2;

	is_True(i3 == 8 && i4 == 2 && b1 == false);

	[](int, int const&, int&){}(avt1, avt2, avt2);
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Avatar.hpp"
#include <iostream>


SGM_SPEC_TESTS(sgm_Avatar)
{	::Test1
,	::Test2
,	::Test3
,	::Test4
,	::Test5
,	::Test6
};