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


using sgm::spec::is_True;


void Test_Case::Made_from_Generic_Lambda()
{
	using sgm::fp::Dim;
	using sgm::fp::__;

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
}


void Test_Case::Made_from_Template_Object()
{
	using sgm::fp::__;

	is_True
	(	SGM_FUNCTOR(Test_Case::tfunc, 3)(2, 3, -1) == 8
	&&	SGM_FUNCTOR(Test_Case::tfunc, 3)(__, -1)(2, 3) == 8
	&&	SGM_FUNCTOR(Test_Case::tfunc, 3)(2, 3, __)(-1) == 8
	);
}


void Test_Case::Made_from_Other_Functor()
{
	using sgm::fp::__;

	sgm::fp::Functor const ftr = SGM_FUNCTOR(Test_Case::tfunc, 3);
	sgm::fp::Functor const ftr1 = ftr(2, 3, __);
	sgm::fp::Functor const ftr2 = ftr(__, -1);


	is_True
	(	ftr(2, 3, -1)	== 8
	&&	ftr1(-1)	== 8
	&&	ftr2(2, 3) == 8
	);
}


void Test_Case::Composition()
{
	sgm::fp::Functor const ftr = SGM_FUNCTOR(Test_Case::tfunc, 3);
	sgm::fp::Functor const halfer = [](auto&& x) {  return x / 2;  } / sgm::fp::Dim<1>;

	is_True(	(halfer | ftr)(2, 3, -1) == 4 );
}


static void stbinding_by_family()
{
	auto fam = sgm::Family<int, double>(3, .14);

	auto[x1, x2] = fam;

	is_True( x1 == 3 && abs(x2 - .14) < 1e-7 );
}


void Test_Case::Codomain_Merge()
{
	sgm::fp::Functor const ftr = [](auto&& x, auto&& y){  return x*y;  } / sgm::fp::Dim<2>;
	sgm::fp::Functor const ftr2 = [](auto&& x, auto&& y){  return x-y;  } / sgm::fp::Dim<2>;

	int x1 = 3, x2 = 2, x3 = 4, x4 = 1;

	auto[a1, a2] = *(ftr + ftr2)(x1, x2, x3, x4);	

	is_True(a1 == 6 && a2 == 3);
	auto a = (ftr2 | ftr + ftr2)(x1, x2, x3, x4);

	is_True(a == 3);
}


void Test_Case::Pass_and_Permute()
{
	sgm::fp::Functor const ftr3 
	=	[](auto&& x, auto&& y, auto&& z){  return x*(y - z);  } / sgm::fp::Dim<3>;

	int const val = 2;

	is_True
	(	(ftr3 | sgm::fp::Pass<3>)(val, 3, -1) == 8
	&&	(ftr3 | sgm::fp::Pass<-3>)(-1, 3, 2) == 8
	&&	( ftr3 | sgm::fp::Pass<2> + [](auto&& x){  return x/2;  } / sgm::fp::Dim<1> )(2, 3, -2) 
		==	8
	);


	int x = 3, *px = &x;
	auto const& cx = x;
	double y = 0.618;

	auto permuted = sgm::fp::Permute<1, 2, 3, 0>(3, px, cx, y);
	
	is_True
	(	permuted.get<0>() == px
	&&	permuted.get<1>() == cx
	&&	permuted.get<2>() == y
	//&&	permuted.get<3>() == 3		// already destructed because it's an rvalue reference.
	);
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Functor.hpp"
#include <iostream>


void Test_sgm_Functor::test()
{
	try
	{
		::stbinding_by_family();

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
