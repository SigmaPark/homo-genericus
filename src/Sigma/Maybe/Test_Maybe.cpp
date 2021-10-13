#include "Maybe.hpp"
#include "..\Specification\Specification.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


using sgm::spec::is_True;
using stype = sgm::spec::Specimen;

template<unsigned>  static void Test();


template<>  
void Test<1>()
{
	stype s(3);

	sgm::Maybe<stype> ys1, ys2(stype{}), ys3(s), ys4 = sgm::NullMyb;

	is_True
	(	ys1.is_null() 
	&&	!ys2.is_null() && ys2.get() == stype() 
	&&	ys3 && *ys3 == s
	&&	ys4.is_null()
	);
}


template<>
void Test<2>()
{
	{
		sgm::Maybe<stype> ys1(3), ys2 = ys1;

		is_True
		(	ys1 && ys1.get() == 3
		&&	ys2 && *ys2 == 3 && ys2.get().state == stype::State::COPY_CONSTRUCTION
		);
	}
	{
		auto ys1 = sgm::Maybe<stype>{4};
		sgm::Maybe<stype> ys2 = Move(ys1);

		is_True(ys2 && *ys2 == 4);
		is_True(ys2->state == stype::State::MOVE_CONSTRUCTION);
	}
	{
		sgm::Maybe<stype> ys1,  ys2 = ys1;

		is_True(!ys1 && !ys2);
	}
	{
		sgm::Maybe<int> ys1(2);
		sgm::Maybe<size_t> ys2 = ys1;

		is_True(ys2 && ys2.get() == 2);
	}
}


template<>
void Test<3>()
{
	{
		sgm::Maybe<stype> ys1, ys2(4);

		ys1 = ys2;

		is_True( ys1 && *ys1 == 4 && ys1.get_or(55) == 4 );
	}
	{
		sgm::Maybe<stype> ys1, ys2(4);

		ys2 = ys1;

		is_True( ys2.is_null() && ys2.get_or(44) == 44 );
	}
	{
		sgm::Maybe<int> ys1(2);
		sgm::Maybe<size_t> ys2 = sgm::NullMyb;

		ys2 = ys1;

		is_True(ys2 && ys2.get() == 2);		
	}
	{
		sgm::Maybe<int> ys1(2);
		sgm::Maybe<size_t> ys2 = sgm::NullMyb;

		ys1 = ys2;

		is_True(!ys1);
	}
}


template<>
void Test<4>()
{
	struct
	{
		int x;

		auto getx()->int&{  return x;  }
	}	foo{52};

	sgm::Maybe<decltype(foo)> yfoo(foo);

	is_True(yfoo && yfoo->getx() == 52);
}


template<>
void Test<5>()
{
	sgm::Maybe<stype> ys1(3);

	ys1 = sgm::NullMyb;

	is_True(!ys1 && ys1->state != stype::State::DESTRUCTED);

	ys1.~Maybe();

	is_True(!ys1 && ys1->state == stype::State::DESTRUCTED);
}


#include "Test_Maybe.hpp"
#include <iostream>


void Test_sgm_Maybe::test()
{
	try
	{
		::Test<1>();
		::Test<2>();
		::Test<3>();
		::Test<4>();
		::Test<5>();

		std::wcout << L"Maybe Test Complete. \n";
	}
	catch(...)
	{
		std::wcerr << L"Maybe Test Failed. \n";

		throw;
	}
}