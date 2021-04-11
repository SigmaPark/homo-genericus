#include "idiom.hpp"
#include "..\Specification\Specification.hpp"


using namespace sgm;
using spec::is_True;

#include <map>
#include <string>

static void Test01()
{

	static_assert
	(	(	Has_Operator_index< std::map<int, int> >::value
		&&	!Has_Operator_index< std::map<std::string, int> >::value
		)
	,	""
	);


	static_assert
	(	(	!is_LvalueReference<int>::value
		&&	is_LvalueReference<int const&>::value
		&&	!is_LvalueReference<int&&>::value
		&&	is_LvalueReference<int&>::value
		)
	,	""
	);


	static_assert
	(	(	is_Convertible<int, double>::value
		&&	is_Convertible<int, int>::value
		&&	!is_Convertible<int const&, int&>::value
		)
	,	""
	);

	class AA{};
	class BB : public AA{};

	static_assert
	(	(	is_Class<AA>::value && !is_Class<double>::value
		&&	is_inherited_from<BB, AA>::value
		&&	!is_inherited_from<AA, BB>::value
		&&	is_inherited_from<AA, AA>::value
		)
	,	""
	);
}


static void Test02()
{
	class AA : public Limited<2, AA>{};
	class BB : public Limited<2, BB>{};


	AA aa1, aa2 = aa1;

	is_True(AA::nof_instance() == 2);

	try
	{
		AA aa3;

		is_True(false);
	}
	catch(Violation const& v)
	{
		is_True( dynamic_cast<Over_instantiation_Violation const*>(&v) != nullptr );
	}

	BB bb1;


	is_True(BB::nof_instance() == 1); 
}


class KK;

static auto operator^(int x, KK const &k)-> KK;
static auto operator^(int x, KK &k)-> KK;

//static auto operator+=(KK &kk, int x)-> KK&;

struct JJ{  int j;  };


class KK
{
public: 
	KK(int x) : _x(x){}

	bool operator!() const{  return _x == 0;  }
	auto operator++(int)-> int{  return _x++;  }

	//auto operator^(unsigned power) const-> KK
	//{
	//	int res = 1;

	//	while(power-->0)
	//		res *= _x;

	//	return {res};
	//}

	auto operator->()-> JJ*{  return &_j;  }

	auto operator*() const-> int const&{  return _x;  }
	//auto operator*()-> int&{  return _x;  }

	auto operator+=(int &d)-> KK&{  return _x + d,  *this;  }

	auto operator()(double, int const*)-> KK&{  return *this;  }

private:
	int _x;
	JJ _j = {33};

	friend auto ::operator^(int, KK const&)-> KK;
	friend auto ::operator^(int, KK&)-> KK;

	//friend auto ::operator+=(KK &kk, int x)-> KK&;
};

auto operator^(int x, KK const &k_)-> KK
{
	int res = 1;
	auto k = k_;

	while(k._x-->0)
		res *= x;

	return res;
}

auto operator^(int x, KK &k_)-> KK
{
	int res = 1;
	auto k = k_;

	while(k._x-->0)
		res *= x;

	return res;
}

//auto operator+=(KK &kk, int x)-> KK&{  return kk._x += x,  kk;  }


class pxKK : public sgm::Operator_interface<KK>
{
private:
	using pxk_t = sgm::Operator_interface<KK>;

public:
	pxKK(KK *pkk) : pxk_t(pkk){}

};


static void Test03()
{
	KK kk(3);

	pxKK px(&kk);
	pxKK const cpx(&kk);

	is_True(!px == false);
	px++;  is_True(*px == 4);

	px->j = 21;

	is_True(px->j == 21);
	is_True( *(2^px) == 16 );

	//is_True(cpx->j == 21);	// Compile Error.
	int s = 2;
	px += s;

	px(2.3, nullptr);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class Bar
{
public:
	Bar(int k = 0) : _k(k){}

	auto get() const-> int const&{  return _k;  }
	auto get()-> int&{  return _k;  }

private:
	int _k;
};


struct OpFoo
{
	int *p;
	Bar *pbar;

	auto operator+() const-> int{  return +(*p);  }
	auto operator-() const-> int{  return -(*p);  }
	auto operator++()-> OpFoo&{  return ++(*p),  *this;  }
	auto operator--()-> OpFoo&{  return --(*p),  *this;  }
	auto operator++(int)-> OpFoo{  return (*p)++,  *this;  }
	auto operator--(int)-> OpFoo{  return (*p)--,  *this;  }
	auto operator*() const-> int const&{  return *p;  }
	auto operator*()-> int&{  return *p;  }
	auto operator&() const-> int const*{  return p;  }
	auto operator&()-> int*&{  return p;  }
	auto operator!() const-> bool{  return *p == 0;  }
	auto operator~() const-> bool{  return *p < 0;  }
	auto operator->() const-> Bar const*{  return pbar;  }
	auto operator->()-> Bar*{  return pbar;  }

	auto operator[](int) const-> int{  return *p;  }
	auto operator[](int)-> int&{  return *p;  }
	
	auto operator+(int x) const-> int{  return *p + x;  }
	auto operator-(int x) const-> int{  return *p - x;  }
	auto operator*(int x) const-> int{  return *p * x;  }
	auto operator/(int x) const-> int{  return *p / x;  }
	auto operator%(int x) const-> int{  return *p % x;  }
	auto operator==(int x) const-> bool{  return *p == x;  }
	auto operator!=(int x) const-> bool{  return *p != x;  }
	auto operator<(int x) const-> bool{  return *p < x;  }
	auto operator>(int x) const-> bool{  return *p > x;  }
	auto operator<=(int x) const-> bool{  return *p <= x;  }
	auto operator>=(int x) const-> bool{  return *p >= x;  }
	auto operator&&(bool b) const-> bool{  return *p != 0 && b;  }
	auto operator||(bool b) const-> bool{  return *p != 0 || b;  }
	auto operator&(int x) const-> int{  return *p + 10*x;  }
	auto operator|(int x) const-> int{  return *p + 100*x;  }
	auto operator^(int x) const-> int{  return *p + 1000*x;  }
	auto operator<<(int x) const-> int{  return *p + 10000*x;  }
	auto operator>>(int x) const-> int{  return *p + 100000*x;  }
	auto operator->*(Bar b) const-> int{  return *p + b.get();  }
	auto operator->*(Bar b)-> int&{  return *p = *p + b.get(),  *p;  }

	auto operator+=(int x)-> OpFoo&{  return *p += x,  *this;  }
	auto operator-=(int x)-> OpFoo&{  return *p -= x,  *this;  }
	auto operator*=(int x)-> OpFoo&{  return *p *= x,  *this;  }
	auto operator/=(int x)-> OpFoo&{  return *p /= x,  *this;  }
	auto operator%=(int x)-> OpFoo&{  return *p %= x,  *this;  }
	auto operator&=(int x)-> OpFoo&{  return *p += 10*x,  *this;  }
	auto operator|=(int x)-> OpFoo&{  return *p += 100*x,  *this;  }
	auto operator^=(int x)-> OpFoo&{  return *p += 1000*x,  *this;  }
	auto operator<<=(int x)-> OpFoo&{  return *p += 10000*x,  *this;  }
	auto operator>>=(int x)-> OpFoo&{  return *p += 100000*x,  *this;  }
	auto operator,(int x) const-> int{  return *p*x;  }

	auto operator()() const-> int{  return 30000;  }
	auto operator()(int, double, int)-> int&{  return *p = -30000;  }
};


class Lon : public sgm::Operator_interface<OpFoo>
{
public:
	Lon(OpFoo *p) : sgm::Operator_interface<OpFoo>(p){}
};


static void Test04()
{
	int a;
	Bar bar;
	OpFoo *pfoo = new OpFoo{&a, &bar};
	Lon L = pfoo;
	Lon const &cL = L;
	
	auto Reset_f 
	=	[&a, &bar, &pfoo]
		{
			a = 3,  bar.get() = 7; 
			pfoo->p = &a,  pfoo->pbar = &bar;
		};

	Reset_f();

	is_True(*cL == 3);
	*L = 5;  is_True(*L == 5);  Reset_f();

	is_True(+cL == +3);  is_True(-cL == -3);

	is_True( *(++L) == 3 + 1 );  Reset_f();  is_True( *(--L) == 3 - 1 );  Reset_f();
	L++;  is_True(*L == 3 + 1);  Reset_f();  L--;  is_True(*L == 3 - 1);  Reset_f();

	is_True(&cL == &a);
	&L = nullptr;  is_True(&L == nullptr);  Reset_f();

	is_True(!L == false);  is_True(~L == false);

	is_True(cL->get() == bar.get());
	L->get() = -2;  is_True(bar.get() == -2);  Reset_f();

	is_True(cL[2] == 3);
	L[1] = 44;  is_True(*L == 44);  Reset_f();

	is_True(cL + 22 == 25);  is_True(cL - 1 == 2);  
	is_True(cL * 4 == 12);  is_True(cL / 2 == 1);  is_True(cL % 2 == 1);

	is_True(cL == 3);  is_True(cL != -3);
	is_True(cL < 4);  is_True(cL > 2);
	is_True(cL <= 3);  is_True(cL >= 3);
	
	is_True(cL && true);  is_True(cL || false);
	is_True( (cL & 2) == 23 );  is_True( (cL | 4) == 403 );  is_True( (cL ^ 5) == 5003 );  
	is_True( (cL << 7) == 70003 );  is_True( (cL >> 8) == 800003 );

	is_True(cL->*bar == 10);
	L->*bar;  is_True(*L == 10);  Reset_f();

	L += 6;  is_True(*L == 9);  Reset_f();
	L -= 6;  is_True(*L == -3);  Reset_f();
	L *= 4;  is_True(*L == 12);  Reset_f();
	L /= 2;  is_True(*L == 1);  Reset_f();
	L %= 2;  is_True(*L == 1);  Reset_f();
	L &= 2;  is_True(*L == 23);  Reset_f();
	L |= 2;  is_True(*L == 203);  Reset_f();
	L ^= 2;  is_True(*L == 2003);  Reset_f();
	L <<= 2;  is_True(*L == 20003);  Reset_f();
	L >>= 2;  is_True(*L == 200003);  Reset_f();

	is_True( (cL, 5) == 15 );

	is_True( cL() == 30000 );
	L(2, 22.0, 4);  is_True(*L == -30000);  Reset_f();

	delete pfoo,  pfoo = nullptr;
}


class CLon : public sgm::Operator_interface<OpFoo const>
{
public:
	CLon(OpFoo const *p) : sgm::Operator_interface<OpFoo const>(p){}
};


static void Test05()
{
	int a;
	Bar bar;
	OpFoo const *pfoo = new OpFoo{&a, &bar};
	CLon cL = pfoo;

	cL + 3;

	//cL += 3;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#include "Test_idiom.hpp"
#include <iostream>


void Test_sgm_idiom::test()
{
	try
	{
		::Test01();
		::Test02();
		::Test03();
		::Test04();
		::Test05();

		std::wcout << L"idiom Test Complete.\n";
	}
	catch(...)
	{
		std::wcout << L"idiom Test Failes.\n";

		throw;
	}
}