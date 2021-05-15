#include "..\Specification\Specification.hpp"
#include "Trader.hpp"


using sgm::Trader;
using sgm::spec::is_True;


template<unsigned>  static void Test();

template<>
void Test<1>()
{
	int x = 1, y = 0;

	{
		auto &&t = sgm::make_Trader(x, [&y](int const &a){  y = 2*a + 3;  });

		is_True(t == 1);

		t = 3;
	}

	is_True(y == 2*3 + 3);
}


template<>
void Test<2>()
{
	int x = 5;

	[](int &t){  t = 10;  }
	(	std::ref( sgm::make_Trader(x, [&x](int const &a){  x = 3*a;  }) )
	);

	is_True(x == 3*10);
}


struct AA
{
	int x;

	auto value()-> int&{  return x;  }
};


template<>
void Test<3>()
{
	AA aa{1};
	int y = 0;

	sgm::make_Trader(aa, [&y](AA const &aa){  y = aa.x*2;  }).value() = 3;

	is_True(y == 2*3);
}


template<>
void Test<4>()
{
	class BB
	{
	public:
		BB(AA aa) : _a(aa){}

		auto value()
		{
			return 
			sgm::make_Trader
			(	AA{_a.value() * 10} , [&r = _a](AA const &t){  r.value() = t.x / 10;  }
			);
		}

		auto aval() const{  return _a.x;  }

	private:
		AA _a;
	};

	BB bb(AA{3});

	[](AA &a){  a.value() = a.value() + 20;  }
	(	std::ref(bb.value())
	);

	is_True(bb.aval() == 5);
}


#include "Test_Trader.hpp"
#include <iostream>


void Test_sgm_Trader::test()
{
	try
	{
		::Test<1>();
		::Test<2>();
		::Test<3>();
		::Test<4>();

		std::wcout << L"Trader Test Complete.\n";
	}
	catch(...)
	{
		std::wcout << L"Trader Test Failed.\n";

		throw;
	}
}