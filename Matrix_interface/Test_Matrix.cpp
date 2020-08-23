#include "Matrix.hpp"


using namespace sgm;
using mxi::Matrix, mxi::Vector;


template
<	class T, class U
,	class = std::enable_if_t< std::is_convertible_v<T, U> || std::is_convertible_v<U, T> >
>
static bool is_Equal(T t, U u)
{
	if constexpr(std::is_integral_v<T>)
		return t == u;
	else if constexpr(std::is_floating_point_v<T>)
		return abs(t - u) < T(1e-5);
	else if constexpr(mxi::MxTraits::is_mxiMatrix_v<T>)
	{
		bool res = t.rows() == u.rows() && t.cols() == u.cols();

		for(auto i = t.rows(); i-->0;)
			for(auto j = t.cols(); res && j-->0;)
				res = is_Equal( t(i, j), u(i, j) );

		return res;
	}
	else if constexpr(mxi::MxTraits::is_mxiVector_v<T>)
	{
		bool res = t.size() == u.size();

		for(auto idx = t.size(); res && idx-->0;)
			res = is_Equal( t(idx), u(idx) );

		return res;
	}
	else SGM_COMPILE_FAILED(no method to compare them.);
}


template<class T> static bool constexpr is_modable_v = std::is_lvalue_reference_v<T>;
//========//========//========//========//=======#//========//========//========//========//=======#


struct Matrix_Case : No_Making
{
	static void Constructions();
	static void Substitutions();
	static void Element();
	static void Row_and_Column();
	static void Block();
	static void inversion();
	static void Transpose();
	static void Resize();
	static void internal_Array();
	static void Algebra();
	static void Special_Matrices();
};


struct Vector_Case : No_Making
{
	static void Constructions();
	static void Substitutions();
	static void Element();
	static void Resize();
	static void Head_and_Tail();
	static void Normalization();
	static void Algebra();
	static void Special_Vectors();
};
//========//========//========//========//=======#//========//========//========//========//=======#


#include "..\Specification\Specification.hpp"
#include <vector>


using spec::is_True;


void Matrix_Case::Constructions()
{
	Matrix<float, 2, 2> 
		X1,
		X2
		{	3, 4
		,	1, 2
		},
		X3 = X2;

	is_True( is_Equal(X2, X3) );


	Matrix<float> 
		X4
		=	Matrix<float, 2, 2>
			{	3, 4
			,	1, 2
			},
		X5 = X2;

	is_True
	(	is_Equal(X4, X5) 
	&&	is_Equal
		(	X4
		,	Matrix<float, 2, 2>
			{	3, 4
			,	1, 2
			}
		) 
	);
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Matrix.hpp"
#include <iostream>


void Test_sgm_Matrix::test()
{
	try
	{
		Matrix_Case::Constructions();

	}
	catch(...)
	{
		std::wcout << L"Matrix Test Failed.\n";

		throw;
	}

	std::wcout << L"Matrix Test Complete.\n";
}