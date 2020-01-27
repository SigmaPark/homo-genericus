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

		auto operator=(Specimen const& spec)-> Specimen const&
		{
			val = spec.val,
			std::wcout << L"allocation\n ";

			return *this;
		}

		auto operator=(Specimen&& spec)-> Specimen const&
		{
			val = spec.val,
			std::wcout << L"move allocation\n ";

			return *this;
		}

		auto operator=(int x)-> Specimen const&
		{
			val = x,
			std::wcout << L"int allocation\n";

			return *this;
		}

		auto value() const-> int const&{  return val;  }
		auto value()->int&{  return val;}

	private:
		int val;
	};


public:
	UnitTest() = delete;


	static void Test1()
	{
		Specimen x = 3;
		Specimen const cx = 10, cy = 50;

		sgm::Avatar<Specimen> avt1 = x;
		sgm::Avatar<Specimen const> avt2 = cx;

		avt1.value() = 30;

		assert(avt1.value().value() == 30 && avt2.value().value() == 10);

	}




};
//========//========//========//========//=======#//========//========//========//========//=======#


int main()
{
	UnitTest::Test1();


	return 0;
}


#if 0

vector<T> tvec;


vector< array<float, 3> > vaf;
...

func1( Dynamic_CArr< Static_Carr<float, 3> >(vaf) )



sgm::Carrier<T> tcrr = method1( sgm::Carrier<T>(tvec) ); 

#endif
