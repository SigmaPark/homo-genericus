#include "Matrix.hpp"


using namespace sgm;
using mxi::Matrix, mxi::Vector;


template<class T, class U, class...TYPES>
static bool is_Equal(T t, U u, TYPES...types)
{
	if constexpr( sizeof...(TYPES) > 0 )
		return is_Equal(t, u) && is_Equal(u, types...);
	else if constexpr(std::is_integral_v<T>)
		return t == u;
	else if constexpr(std::is_floating_point_v<T>)
		return abs(t - u) < T(1e-5);
	else if constexpr(mxi::MxTraits::is_mxiMatrix_v<T>)
	{
		bool res = t.rows() == u.rows() && t.cols() == u.cols();

		for(auto i = t.rows();  i-->0;)
			for(auto j = t.cols();  res && j-->0;)
				res = is_Equal( t(i, j), u(i, j) );

		return res;
	}
	else if constexpr(mxi::MxTraits::is_mxiVector_v<T>)
	{
		bool res = t.size() == u.size();

		for(auto idx = t.size();  res && idx-->0;)
			res = is_Equal( t(idx), u(idx) );

		return res;
	}
	else SGM_COMPILE_FAILED(no method to compare them.);
}


template<class T> static bool constexpr Not_modable_v = !std::is_lvalue_reference_v<T>;
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
	static void Special_Matrices();
	static void Algebra();
	static void Space();
	static void internal_Array();
};


struct Vector_Case : No_Making
{
	static void Constructions();
	static void Substitutions();
	static void Element();
	static void Resize();
	static void Head_and_Tail();
	static void Conversion_to_Matrix();
	static void Normalization();
	static void Algebra();
	static void Special_Vectors();
};


struct Matrix_Vector_Operation : No_Making
{
	static void Test01();
	static void Test02();
};


struct UnitVector_Case : No_Making
{
	static void Constructions();
	static void Substitutions();
};


struct OrthonormalMatrix_Case : No_Making
{
	static void Constructions();
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


	Matrix<float> 
		X4
		=	Matrix<float, 2, 2>
			{	3, 4
			,	1, 2
			},
		X5
		(	std::vector<float>{3, 4, 1, 2}
		,	2, 2
		),
		X6 = X2;


	Matrix<float, 2, 2> const
		CX1,
		CX2
		{	3, 4
		,	1, 2
		},
		CX3 = X2;


	Matrix<float> const
		CX4
		=	Matrix<float, 2, 2>
			{	3, 4
			,	1, 2
			},
		CX5
		(	std::vector<float>{3, 4, 1, 2}
		,	2, 2
		),
		CX6 = X2;


	Matrix<float> X7(2, 2);


	is_True
	(	is_Equal(CX1, X1) 
	&&	is_Equal
		(	X2, X3, X4, X5, X6, CX2, CX3, CX4, CX5, CX6
		,	Matrix<float, 2, 2>
			{	3, 4
			,	1, 2
			}
		)
	&&	X7.rows() == 2 && X7.cols() == 2
	);
}


void Matrix_Case::Substitutions()
{
	Matrix<float> X1, X2, X3, X4(6, 4); 

	X1 = Matrix<float>(std::vector<float>{3, 4, 1, 2}, 2, 2);
	X2 
	=	Matrix<float, 2, 2>
		{	3, 4
		,	1, 2
		},
	X3 = X2,
	X4 = X3;

	Matrix<float, 2, 2> X5;

	X5
	=	{	3, 4
		,	1, 2
		};

	is_True
	(	is_Equal
		(	X1, X2, X3, X4, X5
		,	Matrix<float, 2, 2>
			{	3, 4
			,	1, 2
			}
		)
	);
}


void Matrix_Case::Element()
{
	Matrix<float, 3, 3> const CX1
	{	2, 4, 6
	,	8, 10, 12
	,	14, 16, 18
	};

	is_True
	(	is_Equal( CX1(0, 1), 4.f )
	&&	is_Equal( CX1(2, 1), 16.f )
	);

	static_assert
	(	Not_modable_v<decltype( CX1(0, 1) )> && Not_modable_v<decltype( CX1(2, 1) )>
	);


	Matrix<float, 3, 3> X2 = CX1;

	X2(0, 0) = 20.f,  X2(2, 1) = 1.6f;

	is_True
	(	is_Equal
		(	X2
		,	Matrix<float, 3, 3>
			{	20.f, 4.f, 6.f
			,	8.f, 10.f, 12.f
			,	14.f, 1.6f, 18.f
			}
		)
	);
}


void Matrix_Case::Row_and_Column()
{
	Matrix<float, 3, 3> const CX1
	{	3, 6, 9
	,	12, 15, 18
	,	21, 24, 27
	};

	is_True
	(	is_Equal( CX1.row(0), Matrix<float, 1, 3>{3, 6, 9} )
	&&	is_Equal
		(	CX1.col(2)
		,	Matrix<float, 3, 1>
			{	9
			,	18
			,	27
			} 
		)
	);

	static_assert
	(	Not_modable_v<decltype( CX1.row(0) )> && Not_modable_v<decltype( CX1.col(2) )>
	);


	Matrix<float> X2 = CX1;

	X2.row(1) = Matrix<float, 1, 3>{120, 150, 180};

	X2.col(1) 
	=	Matrix<float, 3, 1>
		{	600
		,	1500
		,	2400
		};

	is_True
	(	is_Equal
		(	X2
		,	Matrix<float, 3, 3>
			{	3, 600, 9
			,	120, 1500, 180
			,	21, 2400, 27
			}
		)
	);
}


void Matrix_Case::Block()
{
	Matrix<float, 4, 3> const CX1
	{	4, 8, 12
	,	16, 20, 24
	,	28, 32, 36
	,	40, 44, 48
	};

	is_True
	(	is_Equal
		(	CX1.block(0, 0, 3, 2)
		,	Matrix<float, 3, 2>
			{	4, 8
			,	16, 20
			,	28, 32
			}
		)
	&&	is_Equal
		(	CX1.block(2, 1, 2, 2)
		,	Matrix<float, 2, 2>
			{	32, 36
			,	44, 48
			}
		)
	&&	is_Equal( CX1.block(1, 0, 1, 3), CX1.row(1) )
	&&	is_Equal( CX1.block(0, 2, 4, 1), CX1.col(2) )
	);

	static_assert
	(	Not_modable_v<decltype( CX1.block(0, 0, 3, 2) )>
	&&	Not_modable_v<decltype( CX1.block(1, 0, 1, 3) )>
	);


	Matrix<float> X2 = CX1;

	X2.block(2, 0, 2, 2)
	=	Matrix<float, 2, 2>
		{	280, 320
		,	400, 440
		};

	X2.block(0, 2, 3, 1)
	=	Matrix<float, 3, 1>
		{	-12
		,	-24
		,	-36
		};

	is_True
	(	is_Equal
		(	X2
		,	Matrix<float, 4, 3>
			{	4, 8, -12
			,	16, 20, -24
			,	280, 320, -36
			,	400, 440, 48
			}
		)
	);
}


void Matrix_Case::inversion()
{
	Matrix<float, 2, 2> const CX1
	{	.5f, 0.f
	,	0.f, .5f
	};

	is_True
	(	is_Equal
		(	CX1.inversed()
		,	Matrix<float, 2, 2>
			{	2, 0
			,	0, 2
			}
		)
	);


	Matrix<float> X2 = CX1;

	static_assert
	(	Not_modable_v<decltype(CX1.inversed())>
	&&	Not_modable_v<decltype(X2.inversed())>
	);
}


void Matrix_Case::Transpose()
{
	Matrix<float, 2, 3> const CX1
	{	3, 6, 9
	,	12, 15, 18
	};

	Matrix<float, 3, 2> X2 = CX1.transposed();

	is_True
	(	is_Equal
		(	X2
		,	Matrix<float, 3, 2>
			{	3, 12
			,	6, 15
			,	9, 18
			}
		)
	);

	static_assert( Not_modable_v<decltype(CX1.transposed())> );


	X2.transposed()
	=	Matrix<float, 2, 3>
		{	-3, 6, -9
		,	12, -15, 18
		};

	is_True
	(	is_Equal
		(	X2
		,	Matrix<float, 3, 2>
			{	-3, 12
			,	6, -15
			,	-9, 18
			}
		)
	);
}


void Matrix_Case::Resize()
{
	Matrix<float> X1
	(	std::vector<float>{3, 4, 1, 2}, 2, 2
	);

	X1.resize(3, 2);

	is_True
	(	X1.rows() == 3 && X1.cols() == 2
	&&	!is_Equal
		(	X1.block(0, 0, 2, 2)
		,	Matrix<float, 2, 2>
			{	3, 4
			,	1, 2
			}
		)
	);


	X1.resize(3, 3).col(2) = Matrix<float, 1, 3>{10, 20, 30}.transposed();

	X1.block(0, 0, 3, 2)
	=	Matrix<float, 3, 2>
		{	-3, -4
		,	-1, -2
		,	7, 8
		};

	is_True
	(	is_Equal
		(	X1
		,	Matrix<float, 3, 3>
			{	-3, -4, 10
			,	-1, -2, 20
			,	7, 8, 30
			}
		)
	);
}


void Matrix_Case::Special_Matrices()
{
	is_True
	(	is_Equal
		(	Matrix<float, 3, 3>::identity()
		,	Matrix<float>::identity(3)
		,	Matrix<float, 3, 3>
			{	1, 0, 0
			,	0, 1, 0
			,	0, 0, 1
			}
		)
	&&	is_Equal
		(	Matrix<float, 2, 3>::zero()
		,	Matrix<float>::zero(2, 3)
		,	Matrix<float, 2, 3>
			{	0, 0, 0
			,	0, 0, 0
			}
		)	
	);
}


void Matrix_Case::Algebra()
{
	Matrix<float, 2, 2> X1
	{	2, 4
	,	6, 8
	};

	decltype(X1) const X2
	{	4, 3
	,	2, 1
	};


	is_True
	(	is_Equal
		(	X1 + X2 + 3.f*Matrix<float, 2, 2>::identity()
		,	Matrix<float, 2, 2>
			{	9, 7
			,	8, 12
			}
		)
	);
}


void Matrix_Case::Space()
{
	std::vector< Vector<float> > const vec1
	{	Vector<float>{1, 2}, Vector<float>{3, 4}, Vector<float>{5, 6}
	};

	is_True
	(	is_Equal
		(	sgm::mxi::column_space(vec1)
		,	Matrix<float, 2, 3>
			{	1, 3, 5
			,	2, 4, 6
			}
		)
	);


	std::vector< Vector<float, 2> > const vec2
	{	Vector<float>{1, 2}, Vector<float>{3, 4}, Vector<float>{5, 6}
	};

	is_True
	(	is_Equal
		(	sgm::mxi::row_space(vec1)
		,	Matrix<float, 3, 2>
			{	1, 2
			,	3, 4
			,	5, 6
			}
		)
	);
}


void Matrix_Case::internal_Array()
{
	auto copied
	=	[](auto* arr, size_t s)
		{
			std::vector< std::decay_t<decltype(arr[0])> > res(s);

			std::copy(arr, arr + s, res.begin());

			return res;
		};

	Matrix<float, 3, 3> const CX1
	{	8, 6, 4
	,	2, 0, -2
	,	-4, -6, -8
	};


	std::vector<float> 
		vec1 = copied(CX1.data(), CX1.size()), 
		vec2 = copied(CX1.transposed().data(), CX1.size());


	using spec::Judge;

	is_True
	(	Judge::have_the_same(vec1, std::vector<float>{8, 2, -4, 6, 0, -6, 4, -2, -8})
	&&	Judge::have_the_same(vec2, vec1)
	&&	!Judge::have_the_same(vec2, std::vector<float>{8, 6, 4, 2, 0, -2, -4, -6, -8})
	);


	Matrix<float> X2 = CX1.transposed();

	is_True
	(	Judge::have_the_same
		(	copied(X2.data(), X2.size()) 
		,	std::vector<float>{8, 6, 4, 2, 0, -2, -4, -6, -8}
		)
	);

	
	X2.data()[1] = 60,  X2.data()[3] = 20;

	is_True
	(	Judge::have_the_same
		(	copied(X2.data(), X2.size()) 
		,	std::vector<float>{8, 60, 4, 20, 0, -2, -4, -6, -8}
		)
	);
}


void Vector_Case::Constructions()
{
	Vector<float, 3> 
		V1,
		V2{2, 4, 6},
		V3 = std::vector<float>{2, 4, 6},
		V4 = Matrix<float, 1, 3>{2, 4, 6},
		V5 
		=	Matrix<float, 3, 1>
			{	2
			,	4
			,	6
			},
		V6 = V3;

	Vector<float> V7(3);

	is_True
	(	V1.size() == 3
	&&	is_Equal(V2, V3, V4, V5, V6)
	&&	V7.size() == 3
	);
}


void Vector_Case::Substitutions()
{
	Vector<float, 4> V1, V2, V3;

	V1 = Vector<float>{2, 4, 6, 8};
	V2 = {2, 4, 6, 8};

	//V3 = Matrix<float, 4, 1>{2, 4, 6, 8};	// Compile Fails : implicit type conversion
	V3 = static_cast< Vector<float> >(Matrix<float, 4, 1>{2, 4, 6, 8});


	is_True( is_Equal(V1, V2, V3, Vector<float>{2, 4, 6, 8}) ); 
}


void Vector_Case::Element()
{
	Vector<int, 3> const CV1{3, 6, 9};

	is_True( CV1(0) == 3 && CV1(1) == 6 && CV1(2) == 9 );

	Vector<int> V2 = CV1;

	V2(0) = -3,  V2(1) = 60;

	is_True( is_Equal(V2, Vector<int>{-3, 60, 9}) );
}


void Vector_Case::Resize()
{
	Vector<float> V1;

	is_True(V1.size() == 0);

	V1.resize(3),  V1(0) = 1,  V1(1) = 3,  V1(2) = 5;

	is_True( is_Equal(V1, Vector<float>{1, 3, 5}) );

	V1.resize(4) = {-1, 3, -5, 7};

	is_True( is_Equal(V1, Vector<float>{-1, 3, -5, 7}) );
}


void Vector_Case::Head_and_Tail()
{
	Vector<float> const CV1{1, 1, 2, 3, 5, 8, 13};

	is_True
	(	is_Equal(CV1.head(3), Vector<float>{1, 1, 2})
	&&	is_Equal(CV1.tail(4), Vector<float>{3, 5, 8, 13})
	);

	Vector<float> V2 = CV1;

	V2.head(4) = Vector<float>{-1, 1, 2, -3},
	V2.tail(3) = Vector<float>{50, 80, 130};

	is_True( is_Equal(V2, Vector<float>{-1, 1, 2, -3, 50, 80, 130}) );
}


void Vector_Case::Conversion_to_Matrix()
{
	Matrix<float, 3, 4> X1
	{	2, 4, 6, 8
	,	10, 12, 14, 16
	,	18, 20, 22, 24
	};

	Vector<float> const 
		CV1{-10, -12, -14, -16},
		CV2{60, 140, 220};

	X1.row(1) = CV1.rowVec(),
	X1.col(2) = CV2.colVec();

	is_True
	(	is_Equal
		(	X1
		,	Matrix<float, 3, 4>
			{	2, 4, 60, 8
			,	-10, -12, 140, -16
			,	18, 20, 220, 24
			}
		) 
	);
}


void Vector_Case::Normalization()
{
	Vector<float, 3> const CV1{1, 2, 3}, CV2 = CV1.normalized();

	is_True
	(	is_Equal(  CV1.norm(), sqrt( CV1(0)*CV1(0) + CV1(1)*CV1(1) + CV1(2)*CV1(2) )  )
	&&	is_Equal(CV1 / CV1.norm(), CV2)
	);
}


void Vector_Case::Special_Vectors()
{
	is_True
	(	is_Equal
		(	Vector<float, 3>::zero()
		,	Vector<float>::zero(3)
		,	Vector<float>{0, 0, 0}
		)
	&&	is_Equal
		(	Vector<float, 5>::ones()
		,	Vector<float>::ones(5)
		,	Vector<float>{1, 1, 1, 1, 1}
		)
	);
}


void Vector_Case::Algebra()
{
	is_True
	(	is_Equal
		(	Vector<float>{1, 2, 3} + Vector<float>{-2, 10, 5} - 3.f*Vector<float, 3>::ones()
		,	Vector<float>{-4, 9, 5}
		)
	&&	is_Equal( Vector<float>{1, 0, 0}.dot(Vector<float>{0, 1, 0}), 0.f )
	&&	is_Equal( Vector<float>{1, 0, 0}.dot(Vector<float>{1, 0, 0}), 1.f )
	&&	is_Equal( Vector<float>{1, 0, 0}.cross(Vector<float>{0, 1, 0}), Vector<float>{0, 0, 1} )
	&&	is_Equal
		(	Vector<float>{1, 0, 0}.dyad(Vector<float>{0, 1, 0})
		,	Vector<float>{1, 0, 0}.colVec() * Vector<float>{0, 1, 0}.rowVec()
		)
	);
}
//========//========//========//========//=======#//========//========//========//========//=======#


void Matrix_Vector_Operation::Test01()
{
	Matrix<float, 2, 2> m1
	{	1, 2
	,	3, -1
	};

	Vector<float, 2> v1{1, -1};

	is_True
	(	is_Equal(v1 * m1, Vector<float>{-2, 3})
	&&	is_Equal(m1 * v1, Vector<float>{-1, 4})
	);

	Matrix<float, 1, 1> x = (v1 * m1).rowVec() * v1.colVec();

	is_True(  is_Equal( x(0, 0), -5 )  );
}


void Matrix_Vector_Operation::Test02()
{
	Matrix<float, 2, 2> m1
	{	-1, 2
	,	1, -1
	};
}
//========//========//========//========//=======#//========//========//========//========//=======#


using sgm::mxi::UnitVector;


void UnitVector_Case::Constructions()
{
	UnitVector<float, 3> u0;

	is_True( is_Equal(u0.vec(), Vector<float>{1, 0, 0}) );

	UnitVector<float, 3> u1{1, 1, 1};

	is_True( is_Equal(u1.vec(), Vector<float>{1, 1, 1}.normalized()) );

	UnitVector<float> u2 = 2*u1;	// Vector-> UnitVector : implicit conversion

	is_True(  is_Equal( u2.vec(), (2*u1).normalized() )  );

	Vector<float, 3> v1 = -u1.vec();	// UnitVector-> Vector : explicit conversion only

	is_True(	is_Equal(v1, -u1.vec()) );	
}


void UnitVector_Case::Substitutions()
{
	UnitVector<float, 3> u1, u2{-1, 1, 0};
	 
	u1 = u2;

	float const len1 = sqrt(2.f)/2.f;

	is_True( is_Equal(u1.vec(), Vector<float>{-len1, len1, 0.f}) );

	u1 = UnitVector<float>{1, 1, 1};

	is_True( is_Equal(u1.vec(), Vector<float>{1, 1, 1}.normalized()) );

	u1 = 3*Vector<float>{0, 1, 1} + Vector<float>::ones(3);

	is_True( is_Equal(u1.vec(), Vector<float>{1, 4, 4}.normalized()) );
}
//========//========//========//========//=======#//========//========//========//========//=======#


using sgm::mxi::OrthonormalMatrix;


void OrthonormalMatrix_Case::Constructions()
{
	OrthonormalMatrix<float, 3> onx0;
	OrthonormalMatrix<float, 3> onx1 = 3*onx0;

	is_True
	(	is_Equal( onx1(0, 0), 1 )
	&&	is_Equal( onx1(1, 1), 1 )
	&&	is_Equal( onx1(2, 2), 1 )
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
		Matrix_Case::Substitutions();
		Matrix_Case::Element();
		Matrix_Case::Row_and_Column();
		Matrix_Case::Block();
		Matrix_Case::inversion();
		Matrix_Case::Transpose();
		Matrix_Case::Resize(); 
		Matrix_Case::Special_Matrices();
		Matrix_Case::Algebra();
		Matrix_Case::Space();
		Matrix_Case::internal_Array();

		Vector_Case::Constructions();
		Vector_Case::Substitutions();
		Vector_Case::Element();
		Vector_Case::Resize();
		Vector_Case::Head_and_Tail();
		Vector_Case::Conversion_to_Matrix();
		Vector_Case::Normalization();
		Vector_Case::Special_Vectors();
		Vector_Case::Algebra();

		Matrix_Vector_Operation::Test01();

		UnitVector_Case::Constructions();
		UnitVector_Case::Substitutions();

		OrthonormalMatrix_Case::Constructions();
		
		std::wcout << L"Matrix Test Complete.\n";
	}
	catch(...)
	{
		std::wcout << L"Matrix Test Failed.\n";

		throw;
	}
}