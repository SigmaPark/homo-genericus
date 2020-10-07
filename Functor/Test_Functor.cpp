#include "Functor.h"
#include "..\Specification\Specification.hpp"
#include "Test_Functor.hpp"
#include <iostream>


template<typename X, typename Y, typename Z>
decltype(auto) tfunc(X&& x, Y&& y, Z&& z)
{  
	return x*(y - z); 
}
 

void Test_sgm_Functor::test()
{
	using namespace sgm;
	using spec::is_True;
	using blk::__;

	try
	{
		Functor const ftr = SGM_FUNCTOR(tfunc, 3);
		Functor const ftr1 = ftr(2, 3, __);
		Functor const ftr2 = ftr(__, -1);

		// 2*( 3 - (-1) ) = 8
		is_True
		(	8	
		&&	ftr(2, 3, -1)	== 8
		&&	ftr1(-1)	== 8
		&&	ftr2(2, 3) == 8
		&&	2 ^ftr2^ 3 == 8
		&&	SGM_FUNCTOR(tfunc, 3)(2, 3, -1) == 8
		&&	SGM_FUNCTOR(tfunc, 3)(__, -1)(2, 3) == 8
		&&	SGM_FUNCTOR(tfunc, 3)(2, 3, __)(-1) == 8
		&&	[](auto&& x, auto&& y, auto&& z){  return x*(y - z);  } / Dim<3>(2, 3, -1) == 8
		&&	(	[](auto&& x, auto&& y, auto&& z){  return x*(y - z);  } / Dim<3>(2, 3, __) 
			)(-1)
			== 8
		&&	(	[](auto&& x, auto&& y, auto&& z){  return x*(y - z);  } / Dim<3>(__, -1)
			)(2, 3)
			== 8
		);

		Functor const ftr3 = [](auto&& x, auto&& y, auto&& z){  return x*(y - z);  } / Dim<3>;

		is_True
		(	(ftr3 | Pass<3>)(2, 3, -1) == 8
		&&	(ftr3 | Pass<-3>)(-1, 3, 2) == 8
		&&	( ftr3 | Pass<2> + [](auto&& x){  return x/2;  } / Dim<1> )(2, 3, -2) == 8
		);

		std::wcout << L"Functor Test Complete.\n";
	}
	catch(...)
	{
		std::wcout << L"Functor Test Failed.\n";

		throw;
	}

}
