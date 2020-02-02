#include "Avatar.hpp"
#include <cassert>
#include <iostream>


class UnitTest
{
private:

	class Specimen
	{
	public: 
		Specimen() : val(0){  std::wcout << L"default construction\n";  }
		Specimen(int x) : val(x){  std::wcout << L"construction\n";  }

		Specimen(Specimen const& spec) : val(spec.val){  std::wcout << L"copy construction\n ";  }
		Specimen(Specimen&& spec) : val(spec.val){  std::wcout << L"move construction\n ";  }

		~Specimen(){  std::wcout << L"destruction\n";  }

		auto operator=(Specimen const& spec)-> Specimen&
		{
			val = spec.val,
			std::wcout << L"allocation\n ";

			return *this;
		}

		auto operator=(Specimen&& spec)-> Specimen&
		{
			val = spec.val,
			std::wcout << L"move allocation\n ";

			return *this;
		}

		auto operator=(int x)-> Specimen&
		{
			val = x,
			std::wcout << L"int allocation\n";

			return *this;
		}


		bool operator==(Specimen const& spec) const{  return val == spec.val;  }
		bool operator!=(Specimen const& spec) const{  return !(*this == spec);  }

		auto value() const-> int const&{  return val;  }
		auto value()->int&{  return val;  }

	private:
		int val;
	};


	template<unsigned N>
	static void title_bar()
	{
		std::wcout << L"-------- Test"<< N << "--------" << std::endl;  
	}


public:
	UnitTest() = delete;


	static void Test1()
	{
		title_bar<1>();

		Specimen x = 3;
		Specimen const cx = 10, cy = 50;

		sgm::Avatar<Specimen> avt1 = x;
		sgm::Avatar<Specimen const> avt2 = cx;

		avt1.value() = 30;

		assert(avt1.value().value() == 30 && avt2.value().value() == 10);
	}


	static void Test2()
	{
		title_bar<2>();

		Specimen x1 = 5, x2 = 10;

		sgm::Avatar<Specimen> avt;

		assert(avt.is_temporary());

		x1 = avt;	// default construction.

		x1 = 5;

		avt = x1;

		assert(avt == x1);

		avt = x2;

		assert(avt == x2);

		x2 = 15;

		assert(avt.value().value() == 15);
	}


	static void Test3()
	{
		title_bar<3>();

		Specimen x1 = 10, x2 = 20;

		sgm::Avatar<Specimen> avt1 = x1, avt2 = avt1;

		x1 = 5;

		assert(avt1.value().value() == 5 && avt2.value().value() == 5);

		avt1 = x2;

		assert(avt1.value().value() == 20 && avt2.value().value() == 5);

		avt2 = avt1;

		assert(avt1.value().value() == 20 && avt2.value().value() == 20);

	}


	static void Test4()
	{
		title_bar<4>();

		Specimen const cx1 = 10, cx2 = 20;

		sgm::Avatar<Specimen const> cavt1 = cx1;
		sgm::Avatar<Specimen const> const cavt2 = cx2;

		cavt1 = cx2;

		assert(cavt1.value().value() == 20 && cavt2.value().value() == 20);

		Specimen x1 = 5, x2 = 2;

		sgm::Avatar<Specimen> const avt1 = x1;

		//avt1.value() = x2;	// compile fails
		//avt1 = x2;	// compile fails

		sgm::Avatar<Specimen> avt2;

		Specimen x3 = avt2;

		avt2 = 32;

		assert(x3 == 0 && avt2.value().value() == 32);

		avt2 = x3;

		assert(avt2.value().value() == 0);

		try
		{
			avt2 = 23;
		}
		catch(bool)
		{
			std::wcout << "rvalue substitution failed\n";
		}
	}


	static void Test5()
	{
		title_bar<5>();

		Specimen x1 = 3, x3 = 9;
		Specimen cx2 = 6;

		sgm::Avatar<Specimen> avt1 = x1;
		sgm::Avatar<Specimen const> cavt2 = cx2;
		sgm::Avatar<Specimen> const avtc3 = x3;

		avt1.~Avatar();
		cavt2.~Avatar();
		avtc3.~Avatar();

		assert
		(	avt1.is_released() && x1 == 3
		&&	cavt2.is_released() && cx2 == 6
		&&	avtc3.is_released() && x3 == 9
		);

		try
		{
			avt1 = x1;
		}
		catch(bool)
		{
			std::wcout << "avt1 was released.\n";
		}
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

