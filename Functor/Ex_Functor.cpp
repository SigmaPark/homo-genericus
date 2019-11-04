#include "Functor.h"
#include <iostream>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#


template<typename X, typename Y, typename Z>
decltype(auto) tfunc(X&& x, Y&& y, Z&& z)
{  
	return x*(y - z); 
}


int main()
{
	auto ftr = SGM_FUNCTOR(tfunc, 3);
	
	sgm::Functor times_2 
	=	[](auto&& x)
		{  
			return 2*x;  
		} / sgm::Dim<1>;
	
	auto ftr1 = SGM_FUNCTOR(tfunc, 3).rear(3.2, 0.2);	
	auto ftr_t1 = ftr.rear_tuple( std::tuple(3.2, 0.2) );

	auto ftr2 = ftr.front(2.1);
	auto ftr_t2 = ftr.front_tuple( std::tuple(2.1) );

	sgm::Functor ftr_c = times_2 <= ftr2;

	sgm::Functor ftr_a = ftr1 + ftr2 + ftr_c;
	auto res_a = ftr_a(2.1, 3.2, 0.2, 3.2, 0.2)();

	std::cout 
	<<	"ftr(2.1, 3.2, 0.2) = " << ftr(2.1, 3.2, 0.2) << '\n'
	<<	"ftr[ std::tuple(2.1, 3.2, 0.2) ] = " << ftr[std::tuple(2.1, 3.2, 0.2)] << '\n'
	<<	"ftr1(2.1) = " << ftr1(2.1) << '\n'
	<<	"ftr_t1(2.1) = " << ftr_t1(2.1) << '\n'
	<<	"ftr2(3,2, 0.2) = " << ftr2(3.2, 0.2) << '\n'
	<<	"ftr_t2(3.2, 0.2) = " << ftr_t2(3.2, 0.2) << '\n'
	<<	"ftr_c(3.2, 0.2) = " << ftr_c(3.2, 0.2) << '\n'
	<<	"ftr_a(2.1, 3.2, 0.2, 3.2, 0.2) = "
	<<	std::get<0>(res_a) << ' ' << std::get<1>(res_a) << ' ' << std::get<2>(res_a) << '\n'
	<<	"3.2 ^ftr2^ 0.2 = " << (3.2 ^ftr2^ 0.2) << '\n'
	;
	
	return 0;
}
