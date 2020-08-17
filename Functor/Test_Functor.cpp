#include "Functor.h"
#include "Test_Functor.hpp"
#include <iostream>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#


template<typename X, typename Y, typename Z>
decltype(auto) tfunc(X&& x, Y&& y, Z&& z)
{  
	return x*(y - z); 
}


void Test_sgm_Functor::test()
{
	using namespace sgm;

	Functor ftr = SGM_FUNCTOR(tfunc, 3);
	Functor ftr2 = ftr(__, -1);
	Functor ftr1 = ftr(2, 3, __);

	std::cout
	<<	ftr(2, 3, -1) << '\n'
	<<	ftr1(-1) << '\n'
	<<	ftr2(2, 3) << '\n'
	<<	(2 ^ftr2^ 3) << '\n'
	<<	SGM_FUNCTOR(tfunc, 3)(2, 3, -1) << '\n'
	<<	SGM_FUNCTOR(tfunc, 3)(__, -1)(2, 3) << '\n'
	<<	SGM_FUNCTOR(tfunc, 3)(2, 3, __)(-1) << '\n'

	<<	[](auto&& x, auto&& y, auto&& z)
		{
			return x*(y - z);
		} / Dim<3>(2, 3, -1)
	<<	'\n'
	<<	(	[](auto&& x, auto&& y, auto&& z)
			{
				return x*(y - z);
			} / Dim<3>(2, 3, __)
		)(-1)
	<<	'\n'
	<<	(	[](auto&& x, auto&& y, auto&& z)
			{
				return x*(y - z);
			} / Dim<3>(__, -1)
		)(2, 3)
	<<	'\n'
	;

	
	Functor ftrL 
	=	[](auto&& x, auto&& y, auto&& z)
		{
			return x*(y - z);
		} / Dim<3>;

	std::cout
	<<	( ftrL | Pass<3> )(2, 3, -1) << '\n'
	<<	( ftrL | Pass<-3> )(-1, 3, 2) << '\n'
	<<	(	[](auto&& x)
			{  
				std::cout<< "you got " << x << ".\n"; 
				return x; 
			} / Dim<1>
		|	ftrL
		|	Pass<2> + [](auto&& x){  return x/2;  } / Dim<1>
		)(2, 3, -2)
	<<	'\n'
	;
}
