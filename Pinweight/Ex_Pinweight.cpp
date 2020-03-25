#include "Pinweight.hpp"
#include <cassert>


class UnitTest
{
public:
	UnitTest() = delete;


	static void Test1()
	{
		sgm::Pinweight<float> pw;

		pw = 32.4f;

		assert(pw == 32.4f);				
	}


	static void Test2()
	{
		int const cx = 45;

		sgm::Pinweight<int> pw1(32), pw2(cx);

		assert(pw1 == 32 && pw2 == 45);

		pw1 = 23, pw2 = 54;

		assert(pw1 == 23 && pw2 == 54);
	}


	static void Test3()
	{
		sgm::Pinweight<int> pw1 = 3;
		sgm::Pinweight<int> pw2 = pw1;

		assert(	pw1 == 3 && pw2 == 3 && pw1.share_with(pw2) );

		pw2 = 3;

		assert( pw1 == 3 && pw2 == 3 && !pw1.share_with(pw2) );
	}


	static void Test4()
	{
		sgm::Pinweight<int> pw1 = 44, pw2 = sgm::Pinweight<int>(pw1);

		assert( pw1.share_with(pw2) );

		sgm::Pinweight<int> const cpw3 = pw1;
		sgm::Pinweight<int const> cpw = cpw3;

		assert( pw1.share_with(cpw3) && pw2.share_with(cpw3) );
	}


	static void Test5()
	{
		class T5{  public: void method() const{}  };

		sgm::Pinweight<T5> const pw1;

		pw1.value().method();

		sgm::Pinweight<T5> pw2 = pw1;

		static_cast<T5>(pw2).method();

		assert( pw1.share_with(pw2) );

		pw2.value().method();

		assert( pw1.share_with(pw2) );
	}


	static void Test6()
	{
		using namespace sgm;

		Pinweight<int> pw;

		Pinweight<  Pinweight< Pinweight<int> >  > ppw = 24;
		//Pinweight<int> ppw = 24;

		std::is_same< std::decay_t<decltype(ppw.value())>, int >::value;

		pw = ppw;

		assert(pw == 24);
	}

};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


int main()
{
	UnitTest::Test1();
	UnitTest::Test2();
	UnitTest::Test3();
	UnitTest::Test4();
	UnitTest::Test5();
	UnitTest::Test6();
	
	return 0;
}

