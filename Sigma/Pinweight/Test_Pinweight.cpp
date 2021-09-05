#include "Pinweight.hpp"
#include "..\Specification\Specification.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


using namespace sgm;
using spec::is_True;


static void Test1()
{
	sgm::Pinweight<float> pw;

	pw = 32.4f;

	is_True(pw == 32.4f && pw.share_count() == 1);
}


static void Test2()
{
	int const cx = 45;

	sgm::Pinweight<int> pw1(32), pw2(cx);

	is_True(pw1 == 32 && pw2 == 45);

	pw1 = 23, pw2 = 54;

	is_True(pw1 == 23 && pw2 == 54);
}


static void Test3()
{
	sgm::Pinweight<int> pw1 = 3, pw2 = pw1;

	is_True(	pw1 == 3 && pw2 == 3 && pw1.share_with(pw2) );

	pw2 = 3;

	is_True( pw1 == 3 && pw2 == 3 && !pw1.share_with(pw2) );
}


static void Test4()
{
	sgm::Pinweight<int> pw1 = 44, pw2 = sgm::Pinweight<int>(pw1);

	is_True( pw1.share_with(pw2) );

	sgm::Pinweight<int> const cpw3 = pw1;
	sgm::Pinweight<int const> cpw = cpw3;

	is_True( pw1.share_with(cpw3) && pw2.share_with(cpw3) );
	is_True( pw1.share_count() == 4 );
}


static void Test5()
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


static void Test6()
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


static void Test7()
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


static void Test8()
{
	struct AA
	{
		int _a;

		AA(int a = 1) : _a(a){}

		int Get() const{  return _a;  }
		int& Get(){  return _a;  }
		void Set(int a){  _a = a;  }
	};


	auto pw1 = Pinweighting(AA()), pw2 = pw1;
	constPinweight<AA> pw3 = pw1;

	is_True( pw1.share_with(pw2) );

	pw1--->Set(11);

	is_True
	(	!pw1.share_with(pw2) 
	&&	pw1->Get() == 11
	&&	pw3->Get() == 1
	);


}


static void Test9()
{
	constPinweight<int> pwi1 = 3, pwi2 = 5;

	auto i3 = pwi1 + pwi2, i4 = pwi2 - pwi1;
	auto b1 = pwi1 > pwi2;

	is_True(i3 == 8 && i4 == 2 && b1 == false);
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Pinweight.hpp"
#include <iostream>
#include <functional>


void Test_sgm_Pinweight::test()
{
	try
	{
		::Test1();
		::Test2();
		::Test3();
		::Test4();
		::Test5();
		::Test6();
		::Test7();
		::Test8();
		::Test9();

		
		std::wcout << L"Pinweight Test Complete.\n";
	}
	catch(spec::Exception e)
	{
		std::wcout << L"Pinweight Test Failed.\n";

		throw e;
	}
}
