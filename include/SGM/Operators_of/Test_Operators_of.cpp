/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Operators_of.hpp"
#include "Test_Operators_of.hpp"


using sgm::spec::Specimen;


struct Foo
{
	Specimen *p;

	auto operator*() const-> Specimen const&{  return *p;  }
	auto operator*()-> Specimen&{  return *p;  }
	auto operator->() const-> Specimen const*{  return p;  }
	auto operator->()-> Specimen*{  return p;  }
	auto operator&() const-> Specimen const*{  return p;  }
	auto operator&()-> Specimen*&{  return p;  }

	auto operator+() const-> int{  return +p->value();  }
	auto operator-() const-> int{  return -p->value();  }
	auto operator++()-> Foo&{  return ++p->value(),  *this;  }
	auto operator--()-> Foo&{  return --p->value(),  *this;  }
	auto operator++(int)-> Foo{  return p->value()++,  *this;  }
	auto operator--(int)-> Foo{  return p->value()--,  *this;  }
	auto operator!() const-> bool{  return p->value() == 0;  }
	auto operator~() const-> bool{  return p->value() < 0;  }

	auto operator[](int) const-> int{  return p->value();  }
	auto operator[](int)-> int&{  return p->value();  }

	auto operator+(int x) const-> int{  return p->value() + x;  }
	auto operator-(int x) const-> int{  return p->value() - x;  }
	auto operator*(int x) const-> int{  return p->value() * x;  }
	auto operator/(int x) const-> int{  return p->value() / x;  }
	auto operator%(int x) const-> int{  return p->value() % x;  }
	auto operator==(int x) const-> bool{  return p->value() == x;  }
	auto operator!=(int x) const-> bool{  return p->value() != x;  }
	auto operator<(int x) const-> bool{  return p->value() < x;  }
	auto operator>(int x) const-> bool{  return p->value() > x;  }
	auto operator<=(int x) const-> bool{  return p->value() <= x;  }
	auto operator>=(int x) const-> bool{  return p->value() >= x;  }
	auto operator&&(bool b) const-> bool{  return p->value() != 0 && b;  }
	auto operator||(bool b) const-> bool{  return p->value() != 0 || b;  }
	auto operator&(int x) const-> int{  return p->value() + 10*x;  }
	auto operator|(int x) const-> int{  return p->value() + 100*x;  }
	auto operator^(int x) const-> int{  return p->value() + 1000*x;  }
	auto operator<<(int x) const-> int{  return p->value() + 10000*x;  }
	auto operator>>(int x) const-> int{  return p->value() + 100000*x;  }
	
	auto operator->*(Specimen s) const-> int{  return p->value() + s.value();  }
	
	auto operator->*(Specimen s)
	->	int&{  return p->value() = p->value() + s.value(),  p->value();  }

	auto operator+=(int x)-> Foo&{  return p->value() += x,  *this;  }
	auto operator-=(int x)-> Foo&{  return p->value() -= x,  *this;  }
	auto operator*=(int x)-> Foo&{  return p->value() *= x,  *this;  }
	auto operator/=(int x)-> Foo&{  return p->value() /= x,  *this;  }
	auto operator%=(int x)-> Foo&{  return p->value() %= x,  *this;  }
	auto operator&=(int x)-> Foo&{  return p->value() += 10*x,  *this;  }
	auto operator|=(int x)-> Foo&{  return p->value() += 100*x,  *this;  }
	auto operator^=(int x)-> Foo&{  return p->value() += 1000*x,  *this;  }
	auto operator<<=(int x)-> Foo&{  return p->value() += 10000*x,  *this;  }
	auto operator>>=(int x)-> Foo&{  return p->value() += 100000*x,  *this;  }
	auto operator,(int x) const-> int{  return p->value()*x;  }

	auto operator()() const-> int{  return 777;  }
	auto operator()(int, double, int)-> int&{  return p->value() = -999;  }
};


class Bar : public sgm::Operators_of<::Foo>
{
public:
	Bar(Foo& foo) : sgm::Operators_of<::Foo>(foo){}
};


static void Test01()
{
	using sgm::spec::is_True;

	Specimen s{3};
	::Foo foo{&s};
	::Bar b(foo);
	::Bar const& cb = b;

	auto reset_f
	=	[&s, &foo]
		{
			s.value() = 3;

			foo.p = &s;
		};
	
	{
		is_True( *cb == Specimen(3) );
		
		*b = Specimen(33);

		is_True( *b == Specimen(33) );
	}

	reset_f();
	
	{
		is_True(+cb == +3 && -cb == -3 && +b == +3 && -b == -3);
	}
	{
		is_True( *(++b) == Specimen(3 + 1) );  reset_f();
		is_True( *(--b) == Specimen(3 - 1) );  reset_f();

		b++;
		is_True( *b == Specimen(3 + 1) );  reset_f();

		b--;
		is_True( *b == Specimen(3 - 1) ); 
	}
	
	reset_f();

	{
		is_True(&cb == &s);

		&b = nullptr;

		is_True(&b == nullptr);
	}
	
	reset_f();
	
	{
		is_True(!b == false && ~b == false);
	}
	{
		is_True(cb->value() == 3);

		b->value() = -4;

		is_True(b->value() == -4 && ~b == true);
	}

	reset_f();

	{
		is_True(cb[1] == 3);

		b[4] = -5;  

		is_True(b->value() == -5);
	}

	reset_f();

	{
		is_True
		(	cb + 30 == 33 && cb - 30 == -27 
		&&	cb * 5 == 15 && cb / 2 == 1 && cb % 2 == 1
		);
	}
	{
		is_True(cb == 3 && cb != 33 && cb < 5 && cb > 1 && cb <= 3 && cb >= 3);
	}
	{
		is_True(cb && true); 
		is_True(cb || false);

		is_True
		(	( (cb & 2) == 23 ) && ( (cb | 4) == 403 ) && ( (cb ^ 6) == 6003 )
		&&	( (cb << 8) == 80003 ) && ( (cb >> 9) == 900003 )
		);
	}
	{
		is_True( cb->*Specimen(30) == 33 );

		b->*Specimen(30);

		is_True(b->value() == 33);
	}

	reset_f();

	{
		b += 10;
		is_True(b->value() == 13);  reset_f();

		b -= 10;
		is_True(b->value() == -7);  reset_f();

		b *= 10;
		is_True(b->value() == 30);  reset_f();

		b /= 2;
		is_True(b->value() == 1);  reset_f();

		b %= 2;
		is_True(b->value() == 1);  reset_f();

		b &= 4;
		is_True(b->value() == 43);  reset_f();

		b |= 4;
		is_True(b->value() == 403);  reset_f();

		b ^= 6;
		is_True(b->value() == 6003);  reset_f();

		b <<= 8;
		is_True(b->value() == 80003);  reset_f();

		b >>= 8;
		is_True(b->value() == 800003);  reset_f();
	}
	{
		is_True(cb() == 777);

		b(1, 23.0, 4);

		is_True(b->value() == -999);
	}

	reset_f();
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Operators_of, /**/)
{	::Test01
};