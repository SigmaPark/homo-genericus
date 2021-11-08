#include "Functional_iterator.hpp"
#include "..\Specification\Specification.hpp"


using sgm::spec::is_True;

template<unsigned>  static void Test();


struct AA
{
	int val[5] = {4, 3, 2, 1, 0};
};


template<class host_t>
struct _iter_functor
{
	_iter_functor(host_t *p) : _p(p){}

	decltype(auto) operator()(size_t const i){  return _p->val[i];  }

private:
	host_t *_p;
};


template<>
void Test<1>()
{
	AA aa;
	auto const con = sgm::as_iterable( _iter_functor(&aa), 5 );

	for(auto &x : con)
		x *= 2;

	is_True
	(	aa.val[0] == 8
	&&	aa.val[1] == 6
	&&	aa.val[2] == 4
	&&	aa.val[3] == 2
	&&	aa.val[4] == 0
	);
}


#include <algorithm>
#include <functional>


template<>
void Test<2>()
{
	AA aa;

	auto con 
	=	sgm::as_iterable< std::function<int&(size_t const)> >
		(	[&aa](size_t const i)-> int&{  return aa.val[i];  }
		,	5
		);

	std::for_each( con.rbegin(), con.rend(), [](int &x){  x *= 2;  } );

	is_True
	(	aa.val[0] == 8
	&&	aa.val[1] == 6
	&&	aa.val[2] == 4
	&&	aa.val[3] == 2
	&&	aa.val[4] == 0
	);


	auto ci = con.cbegin(), ci2 = ++con.cbegin();
	auto i = con.begin(),  i2 = ++con.begin();

	//i = ci;
	ci2 = i;
	i2 = i;
	ci2 = ci;

	//static_cast<decltype(i)>(ci);
	ci = static_cast<decltype(ci)>(i);
}


template<>
void Test<3>()
{
	AA aa;

	auto con 
	=	sgm::as_iterable< std::function<int&(size_t const)> >
		(	[&aa](size_t const i)-> int&{  return aa.val[i];  }
		,	5
		);
		
	std::sort(con.begin(), con.end());

	is_True
	(	aa.val[0] == 0
	&&	aa.val[1] == 1
	&&	aa.val[2] == 2
	&&	aa.val[3] == 3
	&&	aa.val[4] == 4
	);

	std::sort(con.rbegin(), con.rend());

	is_True
	(	aa.val[0] == 4
	&&	aa.val[1] == 3
	&&	aa.val[2] == 2
	&&	aa.val[3] == 1
	&&	aa.val[4] == 0
	);
}


#include "Test_Functional_iterator.hpp"


SGM_SPEC_TESTS(sgm_Functional_iterator)
{	::Test<1>
,	::Test<2>
,	::Test<3>
};