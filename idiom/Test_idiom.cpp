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



class KK
{
public: 
	KK(int x) : _x(x){}

	bool operator!() const{  return _x == 0;  }
	auto operator++(int){  return _x++;  }

	//auto operator^(unsigned power) const-> KK
	//{
	//	int res = 1;

	//	while(power-->0)
	//		res *= _x;

	//	return {res};
	//}

private:
	int _x;
};

template<class T>
class _Proxy
{
private:
	T *_p;


	template<class Q>
	struct _Help
	{
		static auto calc(Q *p) SGM_DECLTYPE_AUTO(!*p)	
	};


public:
	_Proxy(T *p) : _p(p){}

	template< class H = _Help<T> >
	auto operator!(void) const SGM_DECLTYPE_AUTO( H::calc(_p) )


	template<class Q>
	struct _increase_Helper
	{
		static auto calc(Q *p) SGM_DECLTYPE_AUTO( (*p)++ )
	};

	template< class Q = _increase_Helper<T> >
	auto operator++(int) SGM_DECLTYPE_AUTO( Q::calc(_p) )


	template<class RHS>
	auto operator^(RHS const& rhs) const{  return *_p ^ rhs;  }
};

class pxKK : public _Proxy<KK>
{
private:
	using pxk_t = _Proxy<KK>;

public:
	pxKK(KK *pkk) : pxk_t(pkk){}

	//bool operator!() const{  return !pxk_t::operator!();  }
	//bool operator!() const = delete;
};


static void Test03()
{
	KK kk(3);

	pxKK px(&kk);

	bool const b = !px;

	px++;

	//auto px2 = px^3;
}


#include "Test_idiom.hpp"
#include <iostream>


void Test_sgm_idiom::test()
{
	try
	{
		::Test01();
		::Test02();
		::Test03();

		std::wcout << L"idiom Test Complete.\n";
	}
	catch(...)
	{
		std::wcout << L"idiom Test Failes.\n";

		throw;
	}
}