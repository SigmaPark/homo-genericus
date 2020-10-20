#if 1

#include "..\Functor\Functor.hpp"
#include "..\Specification\Specification.hpp"


struct Test_Case
{
	static void Made_from_Generic_Lambda();
	static void Made_from_Template_Object();
	static void Made_from_Other_Functor();

	static void Composition();
	static void Codomain_Merge();

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


	int const y = 3;

	int val = 2, &&rref = std::move(val);

	auto tu1 = std::tuple<int, int&, int&&>( 5, val, std::move(rref) );
	auto tu2 = std::tuple<int&&, int const&>( std::move(rref), val );
	auto mg_tu = Merged_Tuple(tu1, tu2);

	static_assert
	(	std::is_same_v<std::tuple_element_t<0, decltype(mg_tu)>, int&&>
	);

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


void Test_Case::Composition()
{
	Functor const ftr = SGM_FUNCTOR(Test_Case::tfunc, 3);
	Functor const halfer = [](auto&& x) {  return x / 2;  } / Dim<1>;

	is_True(	(halfer | ftr)(2, 3, -1) == 4 );
}


void Test_Case::Codomain_Merge()
{
	Functor const ftr = [](auto&& x, auto&& y){  return x*y;  } / Dim<2>;
	Functor const ftr2 = [](auto&& x, auto&& y){  return x-y;  } / Dim<2>;

	int x1 = 3, x2 = 2, x3 = 4, x4 = 1;

	//auto [a1, a2] = *( (ftr + ftr2)(x1, x2, x3, x4) );	// Todo : Fails!


	//auto a = (ftr2 | ftr + ftr2)(x1, x2, x3, x4);

	//is_True(a == 3);
}


void Test_Case::Pass_and_Permute()
{
	Functor const ftr3 = [](auto&& x, auto&& y, auto&& z){  return x*(y - z);  } / Dim<3>;

	int const x = 2;

	is_True
	(	(ftr3 | Pass<3>)(x, 3, -1) == 8
	&&	(ftr3 | Pass<-3>)(-1, 3, 2) == 8
	//&&	( ftr3 | Pass<2> + [](auto&& x){  return x/2;  } / Dim<1> )(2, 3, -2) == 8
	);


	//int x = 3, *px = &x;
	//auto const& cx = x;
	//double y = 0.618;

	//auto permuted = Permute<1, 2, 3, 0>(3, px, cx, y);
	//
	//is_True
	//(	std::get<0>(permuted) == px
	//&&	std::get<1>(permuted) == cx
	//&&	std::get<2>(permuted) == y
	//&&	std::get<3>(permuted) == 3
	//);	
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

		Test_Case::Composition();
		Test_Case::Codomain_Merge();

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