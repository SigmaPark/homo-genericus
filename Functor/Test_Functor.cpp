#if 1

#include "..\Functor\Functor.hpp"
#include "..\Specification\Specification.hpp"


struct Test_Case
{
	static void Made_from_Generic_Lambda();
	static void Made_from_Template_Object();
	static void Made_from_Other_Functor();
	static void Pass_and_Permute();


	template<class X1, class X2, class X3>
	static auto tfunc(X1&& x1, X2&& x2, X3&& x3){  return x1*(x2 - x3);  }
};


using namespace sgm::fp;
using sgm::spec::is_True;



void Temporary_test()
{
	using sgm::spec::Specimen;

	auto s1 = Specimen(4), &s2 = s1 ;
	auto&& al1 = Transfer<Specimen>(s1);
	Specimen s = al1;

	int const y = 3;
	auto tu = Transfer_as_Tuple<int const>(y);

	//is_True(s == Specimen::State::DESTRUCTED);
}


void Test_Case::Made_from_Generic_Lambda()
{
#if 1
	is_True
	(
		[](auto&& x, auto&& y, auto&& z){  return x*(y - z);  } / Dim<3>(2, 3, -1) == 8
	&&	(	[](auto&& x, auto&& y, auto&& z){  return x*(y - z);  } / Dim<3>(2, 3, __) 
		)(-1)
		== 8
	&&	(	[](auto&& x, auto&& y, auto&& z){  return x*(y - z);  } / Dim<3>(__, -1)
		)(2, 3)
		== 8
	);
#endif
}


void Test_Case::Made_from_Template_Object()
{
#if 1
	is_True
	(	SGM_FUNCTOR(Test_Case::tfunc, 3)(2, 3, -1) == 8
	&&	SGM_FUNCTOR(Test_Case::tfunc, 3)(__, -1)(2, 3) == 8
	&&	SGM_FUNCTOR(Test_Case::tfunc, 3)(2, 3, __)(-1) == 8
	);
#endif
}


void Test_Case::Made_from_Other_Functor()
{
#if 1
	Functor const ftr = SGM_FUNCTOR(Test_Case::tfunc, 3);
	Functor const ftr1 = ftr(2, 3, __);
	Functor const ftr2 = ftr(__, -1);


	is_True
	(	ftr(2, 3, -1)	== 8
	&&	ftr1(-1)	== 8
	&&	ftr2(2, 3) == 8
	);
#endif
}


void Test_Case::Pass_and_Permute()
{
	Functor const ftr3 = [](auto&& x, auto&& y, auto&& z){  return x*(y - z);  } / Dim<3>;


	is_True
	(	(ftr3 | Pass<3>)(2, 3, -1) == 8
	&&	(ftr3 | Pass<-3>)(-1, 3, 2) == 8
	&&	( ftr3 | Pass<2> + [](auto&& x){  return x/2;  } / Dim<1> )(2, 3, -2) == 8
	);


	int x = 3, *px = &x;
	auto const& cx = x;
	double y = 0.618;

	auto permuted = Permute<1, 2, 3, 0>(3, px, cx, y);
	
	is_True
	(	std::get<0>(permuted) == px
	&&	std::get<1>(permuted) == cx
	&&	std::get<2>(permuted) == y
	&&	std::get<3>(permuted) == 3
	);	
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Functor.hpp"
#include <iostream>


void Test_sgm_Functor::test()
{
	try
	{
		Temporary_test();

		Test_Case::Made_from_Generic_Lambda();
		Test_Case::Made_from_Template_Object();
		Test_Case::Made_from_Other_Functor();
		Test_Case::Pass_and_Permute();

		std::wcout << L"Functor Test Complete.\n";
	}
	catch(std::exception const& e)
	{
		std::cerr << e.what() << "Functor Test Failed.\n";

		throw e;
	}
}

#else

#include "Functor.h"
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
		(	ftr(2, 3, -1)	== 8
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


		int x = 3, *px = &x;
		auto const& cx = x;
		double y = 0.618;

		auto permuted = Permute<1, 2, 3, 0>(3, px, cx, y);
		
		is_True
		(	std::get<0>(permuted()) == px
		&&	std::get<1>(permuted()) == cx
		&&	std::get<2>(permuted()) == y
		&&	std::get<3>(permuted()) == 3
		);


		std::wcout << L"Functor Test Complete.\n";
	}
	catch(...)
	{
		std::wcout << L"Functor Test Failed.\n";

		throw;
	}

}

#endif