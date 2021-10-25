#include "Pinweight.hpp"
#include "..\Specification\Specification.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


using namespace sgm;
using spec::is_True;


static void Test01()
{
	sgm::Pinweight<float> pw;

	pw = 32.4f;

	is_True(pw == 32.4f && pw.share_count() == 1);
}


static void Test02()
{
	int const cx = 45;

	sgm::Pinweight<int> pw1(32), pw2(cx);

	is_True(pw1 == 32 && pw2 == 45);

	pw1 = 23, pw2 = 54;

	is_True(pw1 == 23 && pw2 == 54);
}


static void Test03()
{
	sgm::Pinweight<int> pw1 = 3, pw2 = pw1;

	is_True(	pw1 == 3 && pw2 == 3 && pw1.share_with(pw2) );

	pw2 = 3;

	is_True( pw1 == 3 && pw2 == 3 && !pw1.share_with(pw2) );
}


static void Test04()
{
	sgm::Pinweight<int> pw1 = 44, pw2 = sgm::Pinweight<int>(pw1);

	is_True( pw1.share_with(pw2) );

	sgm::Pinweight<int> const cpw3 = pw1;
	sgm::Pinweight<int const> cpw = cpw3;

	is_True( pw1.share_with(cpw3) && pw2.share_with(cpw3) );
	is_True( pw1.share_count() == 4 );
}


static void Test05()
{
	class T5{  public: void method() const{}  };

	sgm::Pinweight<T5> const pw1;

	pw1.get().method();

	sgm::Pinweight<T5> pw2 = pw1;

	static_cast<T5>(pw2).method();

	is_True( pw1.share_with(pw2) );

	pw2.get().method();

	is_True( pw1.share_with(pw2) && pw1.share_count() == 2 );

	pw2 = T5();

	is_True( !pw1.share_with(pw2) && pw1.share_count() == 1 );
}


static void Test06()
{
	Pinweight<int> pw;

	Pinweight<  Pinweight< Pinweight<int> >  > ppw = 24;
	Pinweight<int const> cpw = 53;

//	cpw = pw;	// Compile Failed because it is deleted
//	cpw = 32;	// Compile Failed because it is deleted

	ppw = cpw;

	static_assert( std::is_same< std::decay_t<decltype(ppw.get())>, int >::value, "" );

	pw = ppw;

	is_True(pw == 53);
	is_True(pw.share_count() == 3);
}


static void Test07()
{
	Pinweight<double const> pw = 3;

	{
		Pinweight<double> pw2;

		pw2 = pw;

		is_True(pw2 == 3 && pw.share_count() == 2);
	}

	is_True(pw.share_count() == 1);

	{
		constPinweight<double> pw2 = pw;

		is_True(pw2 == 3 && pw.share_count() == 2);
	}

	is_True(pw.share_count() == 1);
}


struct AA
{
	int _a;

	AA(int a = 1) : _a(a){}

	int Get() const{  return _a;  }
	int& Get(){  return _a;  }
	void Set(int a){  _a = a;  }
};


static void Test08()
{
	auto pw1 = Share(AA()), pw2 = pw1;
	constPinweight<AA> pw3 = pw1;

	is_True( pw1.share_with(pw2) );

	pw1.mut().Set(11);

	is_True
	(	!pw1.share_with(pw2) 
	&&	pw1.get().Get() == 11
	&&	pw3.get().Get() == 1
	);
}


static void Test09()
{
	constPinweight<int> pwi1 = 3, pwi2 = 5;

	auto i3 = pwi1 + pwi2, i4 = pwi2 - pwi1;
	auto b1 = pwi1 > pwi2;

	is_True(i3 == 8 && i4 == 2 && b1 == false);

	
	Pinweight<int> pw3 = pwi2;

	is_True( pw3.share_with(pwi2) );
	
	pw3.mut()++;

	is_True( pwi2 == 5 && pw3 == 6);
}


static void Test10()
{
	using sgm::spec::Specimen;

	enum class Byte : char{} buf[2][PinweightMemory<Specimen>::value];

	for(int i = 0;  i < 10;  ++i)
	{
		auto pw1 = Share( Specimen(50 + i), buf[0] );
		auto pw2 = pw1;
		constPinweight<Specimen> pw3(50 + i, buf[1]);

		is_True
		(	pw2 == 50 + i && pw3 == 50 + i 
		&&	pw1.share_with(pw2) && !pw2.share_with(pw3) 
		);
	}

	is_True
	(	reinterpret_cast<Specimen*>(buf[0])->state == Specimen::State::DESTRUCTED
	&&	reinterpret_cast<Specimen*>(buf[1])->state == Specimen::State::DESTRUCTED
	);
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Pinweight.hpp"
#include <iostream>
#include <functional>


SGM_SPEC_TESTS(sgm_Pinweight)
{	::Test01
,	::Test02
,	::Test03
,	::Test04
,	::Test05
,	::Test06
,	::Test07
,	::Test08
,	::Test09
,	::Test10
};