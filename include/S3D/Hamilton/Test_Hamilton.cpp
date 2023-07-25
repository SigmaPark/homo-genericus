/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Hamilton.hpp"
#include <vector>


template<class...TYPES>
static void _identical(TYPES...types)
{
	SGM_SPEC_ASSERT( s3d::spec::_Equivalent<s3d::spec::_Equiv_Hamilton_Tag>::calc(types...) );
}


static void _Construction_and_Resize()
{
	{
		s3d::Matrix<float, 2, 2> Mat;

		SGM_SPEC_ASSERT(Mat.rows() == 2 && Mat.cols() == 2 && Mat.size() == 2*2);
		::_identical(Mat, s3d::Matrix<float, 2, 2>::Zero());
	}
	{
		s3d::Matrix<float> Mat(2, 2);

		SGM_SPEC_ASSERT(Mat.rows() == 2 && Mat.cols() == 2 && Mat.size() == 2*2);
		::_identical( Mat, s3d::Matrix<float>::Zero(2, 2) );
	}
	{
		s3d::Matrix<float, 2, 3> const Mat
		{	2, 4, 6
		,	8, 10, 12
		};

		SGM_SPEC_ASSERT(Mat.rows() == 2 && Mat.cols() == 3 && Mat.size() == 2*3);
	}
	{
		s3d::Matrix<float> Mat;

		Mat.resize(2, 3);

		SGM_SPEC_ASSERT(Mat.rows() == 2 && Mat.cols() == 3 && Mat.size() == 2*3);
		::_identical( Mat, s3d::Matrix<float>::Zero(2, 3) );
	}
	{
		std::vector<float> const iterable{3, 6, 9, 12};

		s3d::Matrix<float, 2, 2> const Mat = iterable;

		SGM_SPEC_ASSERT(Mat.rows() == 2 && Mat.cols() == 2 && Mat.size() == 2*2);
	}
	{
		s3d::Matrix<float, 2, 2>
			Mat1,
			Mat2 = Mat1,
			Mat3 = s3d::Matrix<float, 2, 2>{};
		
		::_identical(Mat1, Mat2, Mat3, s3d::Matrix<float, 2, 2>::Zero());
	}
	{
		s3d::Vector<float, 2> Vec;

		SGM_SPEC_ASSERT(Vec.rows() == 2 && Vec.cols() == 1 && Vec.size() == 2);
		::_identical(Vec, s3d::Vector<float, 2>::Zero());
	}
	{
		s3d::RowVec<float, 2> RVec;
		s3d::ColVec<float, 2> CVec;

		SGM_SPEC_ASSERT(RVec.rows() == 1 && RVec.cols() == 2 && RVec.size() == 2);
		SGM_SPEC_ASSERT(CVec.rows() == 2 && CVec.cols() == 1 && CVec.size() == 2);

		::_identical(RVec, s3d::Matrix<float, 1, 2>::Zero());
		::_identical(CVec, s3d::Matrix<float, 2, 1>::Zero());
	}
	{
		s3d::Vector<float, 3> const
			Vec1{1, 3, 5},
			Vec2 = std::vector<float>{1, 3, 5},
			Vec3 = Vec1,
			Vec4 = s3d::Vector<float>{1, 3, 5};

		::_identical(Vec1, Vec2, Vec3, Vec4);
	}
	{
		s3d::Vector<float> Vec5(3), Vec6;

		Vec6.resize(3);

		::_identical(Vec5.size(), Vec6.size(), 3);
		::_identical( Vec5, Vec6, s3d::Vector<float>::Zero(3) );
	}
	{
		s3d::Matrix< std::complex<float>, 2, 2 > const Mat1
		{	std::complex<float>{0, 0}, std::complex<float>{0, 0}
		,	std::complex<float>{0, 0}, std::complex<float>{0, 0}
		};

		::_identical
		(	Mat1(0, 0), Mat1(0, 1), Mat1(1, 0), Mat1(1, 1), 0.f
		);
	}
	{
		s3d::Matrix< std::complex<float>, 2, 2 > const
			Mat1
			{	std::complex<float>{0, 0}, std::complex<float>{0, 1}
			,	std::complex<float>{1, 0}, std::complex<float>{1, 1}
			};

		s3d::Matrix< std::complex<float> > const Mat2 = Mat1;

		::_identical(Mat1, Mat2);
	}
	{
		s3d::Matrix<float, 2, 2> const Mat1
		{	0, 0
		,	1, -1
		};

		s3d::Matrix< std::complex<float> > const Mat2 = Mat1;

		::_identical(Mat1, Mat2);
	}
	{
		s3d::Matrix<float, 3, s3d::DYNAMIC> Mat(s3d::FIXED_SIZE, 5);

		SGM_SPEC_ASSERT(Mat.rows() == 3 && Mat.cols() == 5);

		Mat.resize(s3d::FIXED_SIZE, 10);

		SGM_SPEC_ASSERT(Mat.rows() == 3 && Mat.cols() == 10);
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _Substitution()
{
	s3d::Matrix<float, 2, 3> const ansMat
	{	1, 3, 5
	,	7, 9, 11
	};

	{
		s3d::Matrix<float, 2, 3> Mats;
		s3d::Matrix<float> Matd;

		Mats = ansMat;
		Matd = ansMat;

		::_identical(Mats, Matd, ansMat);
	}
	{
		s3d::Matrix<float, 2, 3> Mats;
		s3d::Matrix<float> Matd;

		Mats 
		=	{	1, 3, 5
			,	7, 9, 11
			};

		Matd.resize(2, 3)
		=	{	1, 3, 5
			,	7, 9, 11
			};

		::_identical(Mats, Matd, ansMat);
	}
	{
		s3d::Matrix<float, 2, 3> Mats;
		s3d::Matrix<float> Matd;
		
		std::vector<float> const iterable{1, 3, 5, 7, 9, 11};

		Mats = iterable;
		Matd.resize(2, 3) = iterable;

		::_identical(Mats, Matd, ansMat);	
	}


	s3d::Vector<float> const ansVec{1, 3, 5};

	{
		s3d::Vector<float, 3> Vecs;
		s3d::Vector<float> Vecd;

		Vecs = ansVec;
		Vecd = ansVec;

		::_identical(Vecs, Vecd, ansVec);
	}
	{
		s3d::Vector<float, 3> Vecs;
		s3d::Vector<float> Vecd;

		Vecs = s3d::Vector<float>{1, 3, 5};
		Vecd = s3d::Vector<float>{1, 3, 5};

		::_identical(Vecs, Vecd, ansVec);		
	}
	{
		s3d::Vector<float, 3> Vecs;
		s3d::Vector<float> Vecd;

		std::vector<float> const iterable{1, 3, 5};

		Vecs = iterable;
		Vecd = iterable;

		::_identical(Vecs, Vecd, ansVec);		
	}
	{
		s3d::Matrix< std::complex<float>, 2, 2 > const
			Mat1
			{	std::complex<float>{0, 0}, std::complex<float>{0, 1}
			,	std::complex<float>{1, 0}, std::complex<float>{1, 1}
			};

		s3d::Matrix< std::complex<float> > Mat2;

		Mat2 = Mat1;

		::_identical(Mat1, Mat2);
	}
	{
		s3d::Matrix<float, 2, 2> const Mat1
		{	0, 0
		,	1, -1
		};

		s3d::Matrix< std::complex<float> > Mat2;

		Mat2 = Mat1;

		::_identical(Mat1, Mat2);

	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _Element()
{
	{	
		s3d::Matrix<float, 3, 2> Mat
		{	2, 4
		,	6, 8
		,	10, 12
		};

		::_identical( Mat(0, 0), 2 );
		::_identical( Mat(2, 1), 12 );

		Mat(0, 0) = -2;

		::_identical( Mat(0, 0), -2 );
	}
	{
		s3d::Vector<float, 3> Vec{3, 6, 9};

		::_identical( Vec(0), 3 );
		::_identical( Vec(1), 6 );
		::_identical( Vec(2), 9 );

		Vec(1) = 10;

		::_identical(Vec, s3d::Vector<float>{3, 10, 9});
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _Partial_Access()
{
	s3d::Matrix<float, 4, 3> const Mat0
	{	2, 4, 6
	,	8, 10, 12
	,	14, 16, 18
	,	20, 22, 24
	};

	{
		s3d::Matrix<float, 4, 3> Mat = Mat0;

		::_identical
		(	Mat.block(1, 0, 3, 2)
		,	s3d::Matrix<float, 3, 2>
			{	8, 10
			,	14, 16
			,	20, 22
			}
		);

		::_identical
		(	Mat.block(2, 1, 2, 2)
		,	s3d::Matrix<float, 2, 2>
			{	16, 18
			,	22, 24
			}
		);

		Mat.block(2, 1, 2, 2)
		=	s3d::Matrix<float, 2, 2>
			{	-16, -18
			,	660, 420
			};

		::_identical
		(	Mat
		,	s3d::Matrix<float, 4, 3>
			{	2, 4, 6
			,	8, 10, 12
			,	14, -16, -18
			,	20, 660, 420
			}
		);
	}
	{
		s3d::Matrix<float, 4, 3> Mat = Mat0;

		::_identical( Mat.row(2), s3d::RowVec<float>{14, 16, 18} );
		::_identical( Mat.col(1), s3d::ColVec<float>{4, 10, 16, 22} );

		Mat.row(3) = s3d::RowVec<float>{-20, -22, -24};
		Mat.col(0) = s3d::ColVec<float>{0.2f, 0.8f, 1.4f, 2.f};

		static_assert
		(	std::is_same_v
			<	decltype(  s3d::Eval( Mat.row(3) )  )
			, s3d::RowVec<float, 3>
			>
		);

		::_identical
		(	Mat
		,	s3d::Matrix<float, 4, 3>
			{	0.2f, 4.f, 6.f
			,	0.8f, 10.f, 12.f
			,	1.4f, 16.f, 18.f
			,	2.f, -22.f, -24.f
			}
		);
	}
	{
		s3d::Vector<float> Vec{2, 4, 6, 8};

		::_identical( Vec.head(2), s3d::Vector<float>{2, 4} );
		::_identical( Vec.tail(3), s3d::Vector<float>{4, 6, 8} );

		Vec.head(2) = s3d::Vector<float>{-3, -5};
		Vec.tail(2) = s3d::Vector<float>{60, 80};

		::_identical( Vec, s3d::Vector<float>{-3, -5, 60, 80} );
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _inversion_and_transpose()
{
	{
		s3d::Matrix<float, 2, 2> const Mat
		{	0.5f, 0.f
		,	0.f, 0.5f
		};

		::_identical
		(	Mat.inv()
		,	s3d::Matrix<float, 2, 2>
			{	2, 0
			,	0, 2
			}
		);
	}
	{
		s3d::Matrix<float, 2, 3> const Mat
		{	3, 6, 9
		,	12, 15, 18
		};

		::_identical
		(	Mat.transpose()
		,	s3d::Matrix<float, 3, 2>
			{	3, 12
			,	6, 15
			,	9, 18
			}
		);
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _Static_Matrix()
{
	::_identical
	(	s3d::Matrix<float, 3, 3>::identity()
	,	s3d::Matrix<float>::identity(3)
	,	s3d::Matrix<float, 3, 3>
		{	1, 0, 0
		,	0, 1, 0
		,	0, 0, 1
		}
	);

	::_identical
	(	s3d::Matrix<float, 2, 3>::Zero()
	,	s3d::Matrix<float>::Zero(2, 3)
	,	s3d::Matrix<float, 2, 3>
		{	0, 0, 0
		,	0, 0, 0
		}
	);

	::_identical
	(	s3d::Matrix<float, 2, 3>::Ones()
	,	s3d::Matrix<float>::Ones(2, 3)
	,	s3d::Matrix<float, 2, 3>
		{	1, 1, 1
		,	1, 1, 1
		}
	);


	::_identical
	(	s3d::Vector<float, 3>::Zero()
	,	s3d::Vector<float>::Zero(3)
	,	s3d::Vector<float>{0, 0, 0}
	);

	::_identical
	(	s3d::Vector<float, 3>::Ones()
	,	s3d::Vector<float>::Ones(3)
	,	s3d::Vector<float>{1, 1, 1}
	);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _Algebra()
{
	{
		s3d::Matrix<float, 2, 2> const X1
		{	2, 4
		,	6, 8
		};
	
		s3d::Matrix<float, 2, 2> const X2
		{	4, 3
		,	2, 1
		};
	
		static_assert
		(	std::is_same_v
			<	decltype( s3d::Eval(X1 + X2 + 3.f * s3d::Matrix<float, 2, 2>::identity()) )
			,	s3d::Matrix<float, 2, 2>
			>
		);
	
		::_identical
		(	X1 + X2 + 3.f* s3d::Matrix<float, 2, 2>::identity()
		,	s3d::Matrix<float, 2, 2>
			{	9, 7
			,	8, 12
			}
		);
	}
	{
		s3d::Matrix< std::complex<float>, 2, 2 > const X1
		{	2.f, 4.f
		,	6.f, 8.f
		};
	
		s3d::Matrix< std::complex<float>, 2, 2 > const X2
		{	4.f, 3.f
		,	2.f, 1.f
		};
	
		static_assert
		(	std::is_same_v
			<	decltype( s3d::Eval(X1 + X2 + 3.f * s3d::Matrix<float, 2, 2>::identity()) )
			,	s3d::Matrix< std::complex<float>, 2, 2 >
			>
		);
	
		::_identical
		(	X1 + X2 + 3.f* s3d::Matrix< std::complex<float>, 2, 2 >::identity()
		,	s3d::Matrix<float, 2, 2>
			{	9, 7
			,	8, 12
			}
		);		
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _Vector_Computing()
{
	s3d::Vector<float, 3> const X1{1, 0, 0}, X2{0, 1, 0}, X3{2, 3, 4};

	::_identical
	(	X3.dot( X1.cross(X2) )
	,	X1.cross(X2).dot(X3)
	,	4
	);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _internal_Array()
{
	std::vector<float> const src_iterable{2, 4, 6, 8, 10, 12};

	s3d::Matrix<float, 2, 3> Mat1 = src_iterable;
	s3d::Matrix<float, 2, 3, s3d::Storing_Order::ROW_FIRST> Mat2 = src_iterable;
	
	::_identical
	(	Mat1, Mat2
	,	s3d::Matrix<float, 2, 3>
		{	2, 4, 6
		,	8, 10, 12
		}
	);

	float arr1[6], arr2[6];

	std::memcpy( arr1, Mat1.data(), sizeof(float)*Mat1.size() );
	std::memcpy( arr2, Mat2.data(), sizeof(float)*Mat2.size() );

	auto verify_array_f
	=	[](auto &arr, std::initializer_list<float> &&ans)
		{
			for( auto[p, itr] = std::pair(arr, ans.begin());  itr != ans.end();  ++p,  ++itr )
				::_identical(*p, *itr);
		};

	verify_array_f(arr1, {2, 8, 4, 10, 6, 12});
	verify_array_f(arr2, {2, 4, 6, 8, 10, 12});
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _Column_and_Row_Space()
{
	std::vector< s3d::Vector<float, 3> > Vecs1
	{	s3d::Vector<float, 3>{1, 1, 1}
	,	s3d::Vector<float, 3>{2, 2, 2}
	,	s3d::Vector<float, 3>{3, 3, 3}
	};

	s3d::Matrix<float, 3, 3> Mat1 = Vecs1;
	s3d::Matrix<float> Mat2 = Vecs1;

	::_identical
	(	Mat1, Mat2
	,	s3d::Matrix<float, 3, 3>
		{	1, 2, 3
		,	1, 2, 3
		,	1, 2, 3
		}
	);

	std::vector< s3d::RowVec<float, 3> > Vecs2
	{	s3d::RowVec<float>{1, 1, 1}
	,	s3d::RowVec<float>{2, 2, 2}
	,	s3d::RowVec<float>{3, 3, 3}
	};

	s3d::Matrix<float> Mat3 = Vecs2;

	::_identical
	(	Mat3
	,	s3d::Matrix<float, 3, 3>
		{	1, 1, 1
		,	2, 2, 2
		,	3, 3, 3
		}
	);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _UnitVector()
{
	{
		s3d::UnitVec<float> uVec1(3);
		s3d::UnitVec<float, 3> uVec2;

		::_identical(uVec1, uVec2, s3d::UnitVec<float>{0, 0, 1}, s3d::Vector<float, 3>{0, 0, 1});
	}
	{
		s3d::UnitVec<float> uVec1{1, 1, 1};
		s3d::UnitVec<float, 3> uVec2{2, 2, 2}, uVec3 = std::initializer_list<float>{3, 3, 3};

		::_identical(uVec1, uVec2, uVec3, s3d::Vector<float>{1, 1, 1}.normalized());
	}
	{
		s3d::UnitVec<float> uVec1(2);

		::_identical(uVec1, s3d::Vector<float>{0, 1});

		uVec1 = {0, 3};

		::_identical(uVec1, s3d::Vector<float>{0, 1});

		uVec1.resize(3) = {1, 1, 1};

		::_identical(uVec1, s3d::Vector<float>{1, 1, 1}.normalized());

	//	UnitVec<float, 3>().resize(5);	// Compile Ban
	}
	{
		s3d::UnitVec<float, 3> uVec1;

		::_identical(+uVec1, s3d::UnitVec<float>{0, 0, 1});
		::_identical(-uVec1, s3d::UnitVec<float>{0, 0, -1});


	//	uVec1 + UnitVec<float, 3>{1, 1, 1};		// Compile Ban
	//	uVec1 - Vector<float, 3>{2, 2, 2};		// Compile Ban
	//	uVec1 += UnitVec<float, 3>{1, 1, 1};		// Compile Ban
	//	uVec1 -= Vector<float, 3>{2, 2, 2};		// Compile Ban
	}
	{
		s3d::UnitVec<float, 3> uVec1;

		::_identical( uVec1(2), 1 );

	//	uVec1(0) = 2;	// Compile Ban
	}
	{
		s3d::Matrix<float, 2, 2> const Mat1
		{	0, -2
		,	2, 0
		};

		s3d::UnitVec<float, 2> uVec1;

		::_identical(Mat1 * uVec1, s3d::Vector<float>{-2, 0});

		s3d::UnitVec<float, 2> uVec2 = Mat1 * uVec1;

		::_identical(uVec2, s3d::Vector<float>{-2, 0}.normalized());
	}
	{
		s3d::UnitVec<float, 3> uVec1{3, 3, 3};

		::_identical( uVec1.head(2), s3d::Vector<float>{1, 1, 1}.normalized().head(2) );
	//	uVec1.head(2) = Vector<float>{1, 1};		// Compile Ban
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _OrthogonalMatrix()
{
	{
		s3d::OrthogonalMat<float, 3> otnMat1;
		s3d::OrthogonalMat<float> otnMat2(3);

		::_identical(otnMat1, otnMat2, s3d::Matrix<float, 3, 3>::identity());
	}
	{
		s3d::OrthogonalMat<float> otnMat1
		=	s3d::Matrix<float, 3, 3>
			{	1, 3, 1
			,	2, 0, 1
			,	0, 0, -1
			};

		s3d::OrthogonalMat<float, 3> otnMat2
		{	1, 3, 1
		,	2, 0, 1
		,	0, 0, -1
		};

		s3d::OrthogonalMat otnMat3 = otnMat2;

		otnMat3 = 3*otnMat2;

		::_identical(otnMat1, otnMat2, otnMat3);

		::_identical(otnMat1*otnMat1.transpose(), s3d::Matrix<float, 3, 3>::identity());
	}
	{
		s3d::OrthogonalMat<float> otnMat1(2);

		::_identical(otnMat1, s3d::Matrix<float, 2, 2>::identity());

		otnMat1.resize(3);

		::_identical(otnMat1, s3d::Matrix<float, 3, 3>::identity());
	}
	{
		s3d::OrthogonalMat<float, 2> otnMat1;

		::_identical(+otnMat1, otnMat1);
		::_identical(-otnMat1, -s3d::Matrix<float, 2, 2>::identity());

	//	otnMat1 + otnMat1;		// Compile Ban
	//	otnMat1 - otnMat1;		// Compile Ban
	}
	{
		s3d::OrthogonalMat<float, 2> otnMat1
		{	1, -1
		,	1, 1
		};

		auto const otnMat2 = otnMat1;

		otnMat1 *= otnMat1;

		::_identical
		(	otnMat1, otnMat2 * otnMat2
		,	s3d::Matrix<float, 2, 2>
			{	0, -1
			,	1, 0
			}
		);
	}
	{
		s3d::OrthogonalMat<float, 3> otnMat1;

		::_identical( otnMat1(0), otnMat1(0, 0), 1 );
	//	otnMat1(0) = 3;	// Compile Ban

		::_identical( otnMat1.col(1), s3d::Vector<float>{0, 1, 0} );
		::_identical( otnMat1.row(2), s3d::Vector<float>{0, 0, 1}.transpose() );

	//	otnMat1.col(1) = Vector<float, 3>{1, 0, 0}; // Compile Ban
	//	otnMat1.row(2) = Vector<float>{0, 0, 1}.transpose(); // Compile Ban

		s3d::Matrix<float, 2, 3> const Mat1
		{	1, 0, 0
		,	0, 1, 0
		};

		::_identical( otnMat1.block(0, 0, 2, 3), Mat1 );

	//	otnMat1.block(0, 0, 2, 3) = Mat1;	// Compile Ban
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _Skipped()
{
	{
		/**	Mathematically wrong unit vector but accepted as s3d::UnitVec .
		*	s3d::Skipped skips Vector normalization in s3d::UnitVec to avoid computing overhead .
		*	Carefully use s3d::Skipped Only when the data is guaranteed 
		*	to be normalized already .
		*/
		s3d::UnitVec<float, 2> NotUnit 
		=	s3d::Skipped< s3d::UnitVec<float, 2> >(s3d::Vector<float>{1, 2});

		::_identical(NotUnit, s3d::Vector<float>{1, 2});
	}
	{
		/**	Mathematically wrong orthonormal matrix but accepted as s3d::OrthogonalMat .
		*	s3d::Skipped skips Matrix orthonormalization in s3d::OrthogonalMat 
		*	to avoid computing overhead .
		*	Carefully use s3d::Skipped Only when the data is guaranteed 
		*	to be orthonormalized already .
		*/		
		s3d::OrthogonalMat<float, 2> NotOtn 
		=	s3d::Skipped< s3d::OrthogonalMat<float, 2> >(s3d::Matrix<float>::Ones(2, 2) );

		::_identical
		(	NotOtn
		,	s3d::Matrix<float, 2, 2>
			{	1, 1
			,	1, 1
			}
		);
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _invalid_when_divided_by_0()
{
	s3d::Matrix<float> Mat0;

	s3d::Matrix<float, 2, 2> const Mat1
	{	1, 2
	,	3, 4
	};

	s3d::Vector<float, 3> const Vec1{1, 3, 5};

	SGM_SPEC_ASSERT( s3d::is_valid(Mat0) && s3d::is_valid(Mat1) && s3d::is_valid(Vec1) );

	SGM_SPEC_ASSERT
	(	!(	s3d::is_valid(Mat1/0) || s3d::is_valid( Mat1/std::complex<float>(0) )
		||	s3d::is_valid(Vec1/0) || s3d::is_valid( Vec1/std::complex<float>(0) )
		||	s3d::is_valid(s3d::Matrix<float, 2, 2>::Zero().normalized())
		)
	);

	SGM_SPEC_ASSERT
	(	std::isnan(s3d::NaN< std::complex<float> >)
	&&	std::isnan( 1.f / std::complex<float>(0.f) )
	&&	std::isnan( std::complex<float>(1.f) / 0.f )
	&&	std::isnan( std::complex<float>(1.f) / std::complex<float>(0.f) )
	);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _invalid_Matrix()
{
	static_assert(std::is_convertible_v< s3d::NullMat_t, s3d::Matrix<float> >);

	s3d::Matrix<float, 2, 3> Mat1 = s3d::NULLMAT;
	s3d::Matrix<double> Mat2 = s3d::NULLMAT, Mat3;

	Mat3 = s3d::NULLMAT;

	SGM_SPEC_ASSERT(  !( s3d::is_valid(Mat1) || s3d::is_valid(Mat2) || s3d::is_valid(Mat3) )  );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void _as_Vector_iterable()
{
	{
		auto Mat 
		=	s3d::Matrix<float, s3d::DYNAMIC, 3>(4, s3d::FIXED_SIZE)
		=	{	1, 2, 3
			,	-4, -5, -6
			,	7, 8, 9
			,	-10, -11, -12
			};


		[&Mat](auto row_space)
		{
			auto itr = row_space.begin();
				
			::_identical(*itr++, s3d::RowVec<float, 3>{1, 2, 3});
			::_identical(*itr++, s3d::RowVec<float, 3>{-4, -5, -6});
			::_identical(*itr++, s3d::RowVec<float, 3>{7, 8, 9});
			::_identical(*itr++, s3d::RowVec<float, 3>{-10, -11, -12});
		}( s3d::as_row_space(Mat) );


		s3d::as_row_space(Mat).begin()[1] = s3d::RowVec<float, 3>{4, 5, 6};
		s3d::as_row_space(Mat).begin()[3] = s3d::RowVec<float, 3>{10, 11, 12};

		for( auto const& rv : s3d::as_row_space(Mat) )
			for(size_t i = 0;  i < 3;  ++i)
				SGM_SPEC_ASSERT( rv(i) > 0 );
	}
	{
		auto Mat 
		=	s3d::Matrix<float, s3d::DYNAMIC, 3>(4, s3d::FIXED_SIZE)
		=	{	1, 2, 3
			,	-4, -5, -6
			,	7, 8, 9
			,	-10, -11, -12
			};


		[](auto col_space)
		{
			auto itr = col_space.begin();

			::_identical(*itr++, s3d::ColVec<float, 4>{1, -4, 7, -10});
			::_identical(*itr++, s3d::ColVec<float, 4>{2, -5, 8, -11});
			::_identical(*itr++, s3d::ColVec<float, 4>{3, -6, 9, -12});
		}( s3d::as_col_space(Mat) );
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


SGM_SPECIFICATION_TEST(s3d::spec::Test_, Hamilton, /**/)
{	::_Construction_and_Resize
,	::_Substitution
,	::_Element
,	::_Partial_Access
,	::_inversion_and_transpose
,	::_Static_Matrix
,	::_Algebra
,	::_Vector_Computing
,	::_internal_Array
,	::_Column_and_Row_Space
,	::_UnitVector
,	::_OrthogonalMatrix
,	::_Skipped
,	::_invalid_when_divided_by_0
,	::_invalid_Matrix
,	::_as_Vector_iterable	
};