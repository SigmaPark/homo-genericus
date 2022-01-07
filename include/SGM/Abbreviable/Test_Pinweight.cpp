/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Pinweight.hpp"
#include "Test_Pinweight.hpp"


using sgm::spec::Specimen;
using sgm::spec::is_True;


static void Test01()
{
	sgm::Pinweight<Specimen> pw;

	pw = Specimen(3);

	is_True(pw == 3 && pw.share_count() == 1);
}


static void Test02()
{
	Specimen const cs(3);
	sgm::Pinweight<Specimen> pw1(5), pw2(cs);

	is_True(pw1 == 5 && pw2 == 3);

	pw1 = 7, pw2 = 9;

	is_True(pw1 == 7 && pw2 == 9 && cs == 3);
	is_True(pw1.share_count() == 1 && pw2.share_count() == 1);
}


static void Test03()
{
	sgm::Pinweight<Specimen> pw1(3), pw2 = pw1;

	is_True(pw1 == 3 && pw2 == 3);
	is_True( pw1.share_with(pw2) && pw1.share_count() == 2 && pw2.share_count() == 2 );

	pw2 = Specimen(3);

	is_True(pw1 == 3 && pw2 == 3);
	is_True( !pw1.share_with(pw2) && pw1.share_count() == 1 && pw2.share_count() == 1 );
}


static void Test04()
{
	sgm::Pinweight<Specimen> pw1(3), pw2 = sgm::Pinweight<Specimen>(pw1);

	is_True( pw1.share_with(pw2) );

	sgm::Pinweight<Specimen> const pw3 = pw1;
	sgm::Pinweight<Specimen const> cpw = pw3;

	is_True( pw1.share_with(pw3) && pw2.share_with(pw3) );
	is_True(pw1.share_count() == 4);
}


static void Test05()
{
	sgm::constPinweight<Specimen> cpw1(3);

	is_True(cpw1.get().value() == 3);
}


template<class CPW>
struct _constPinweight_Test_Helper
{
private:
	template<class _CPW>
	static auto _calc(int)-> sgm::SFINAE_t< decltype(sgm::Declval<_CPW>().get() = 7) >;

	template<class...>
	static auto _calc(...)-> sgm::False_t;

public:
	static bool constexpr cannot_assign_to_constPinweight_value 
	=	!decltype( _calc<CPW>(0) )::value;
};


static void Test06()
{
	sgm::Pinweight<  sgm::Pinweight< sgm::Pinweight<Specimen> >  > ppw(3);
	sgm::constPinweight<Specimen> cpw(5);


	static_assert
	(	(	sgm::is_Same< decltype(ppw.get()), Specimen const& >::value
		&&	sgm::is_Same< decltype(cpw.get()), Specimen const& >::value
		&&	::_constPinweight_Test_Helper
			<	sgm::constPinweight<Specimen> 
			>::	cannot_assign_to_constPinweight_value
		)
	,	""
	);
}


struct Foo
{
	int x;
	Foo(int x_) : x(x_){}

	void set(int a){  x = a;  }
	auto value() const-> int{  return x;  }
	auto value()-> int&{  return x;  }
};


static void Test07()
{
	auto pw1 = sgm::Share( ::Foo(1) );
	sgm::Pinweight<::Foo> pw2 = pw1;
	sgm::constPinweight<::Foo> pw3 = pw1;

	static_assert
	(	(	sgm::is_constPinweight<decltype(pw3)>::value
		&&	sgm::is_constPinweight< sgm::Pinweight<::Foo const> >::value
		&&	!sgm::is_constPinweight< sgm::Pinweight<::Foo> const >::value
		&&	sgm::is_constPinweight<  sgm::Pinweight< sgm::constPinweight<::Foo> >  >::value
		&&	sgm::is_constPinweight<  sgm::Pinweight< sgm::Pinweight<::Foo> const >  >::value
		)
	,	""
	);

	is_True( pw1.share_with(pw2) );

	pw1.mut().set(3);

	is_True
	(	!pw1.share_with(pw2) 
	&&	pw1.get().value() == 3
	&&	pw3.get().value() == 1
	);
}


static void Test08()
{
	sgm::constPinweight<int> pwi1 = 3, pwi2 = 5;

	auto y1 = pwi1 + pwi2;
	auto y2 = pwi2 - pwi1;
	auto b = pwi1 < pwi2;

	static_assert
	(	(	sgm::is_Same<decltype(y1), int>::value
		&&	sgm::is_Same<decltype(y2), int>::value
		&&	sgm::is_Same<decltype(b), bool>::value
		)
	,	""
	);

	is_True(y1 == 8 && y2 == 2 && b == true);
	
	sgm::Pinweight<int> pw3 = pwi2;

	is_True( pw3.share_with(pwi2) );
	
	pw3.mut()++;

	is_True( pwi2 == 5 && pw3 == 5+1 && !pw3.share_with(pwi2) );
}


static void Test09()
{
	enum class Byte : unsigned char{};
	using Pinweight_bytes = Byte[sgm::Pinweight<Specimen>::data_size_value];

	Pinweight_bytes buf[2];

	for(int i = 0;  i < 10;  ++i)
	{
		auto pw1 = sgm::Share( Specimen(i), buf[0] );
		sgm::constPinweight<Specimen> pw2(i, buf[1]);

		[[maybe_unused]] auto pw3 = pw1;
		sgm::Pinweight<Specimen> pw4(8);

		is_True
		(	pw1 == i && pw2 == i
		&&	&pw1.get() == reinterpret_cast<Specimen const*>(&buf[0])
		&&	&pw2.get() == reinterpret_cast<Specimen const*>(&buf[1])
		);
	}

	is_True
	(	reinterpret_cast<Specimen*>(buf[0])->state() == Specimen::State::DESTRUCTION
	&&	reinterpret_cast<Specimen*>(buf[1])->state() == Specimen::State::DESTRUCTION
	);
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Pinweight, /**/)
{	::Test01
,	::Test02
,	::Test03
,	::Test04
,	::Test05
,	::Test06
,	::Test07
,	::Test08
,	::Test09
};