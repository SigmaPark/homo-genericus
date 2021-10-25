#include "V3D\Hamilton\Test_Hamilton.hpp"
#include "Sigma\Specification\Specification.hpp"
#include "Decomposition.hpp"


using sgm::spec::is_True;
using namespace v3d;


template<class...TYPES>
static void _identical(TYPES...types)
{
	is_True( test::_Equivalent<test::_Equiv_Hamilton_Tag>::calc(types...) );
}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


static void Least_Square_Solution()
{
	auto const A
	=	Matrix<float>(2, 3)
	=	{	-1.f, -.737f, .511f
		,	-.0827f, .0655f, -.562f
		};

	Vector<float> b{-.906f, .358f, .359f};

	Vector<float, 2> const
		x_svd = Least_Square_Problem::solution<Solving_Mode::SVD>(A.transpose(), b),
		x_qr = Least_Square_Problem::solution<Solving_Mode::QR>(A.transpose(), b);

	::_identical(x_svd, x_qr);
}


static void Eigen_Decomp()
{
	Matrix<float, 2, 2> const Mat1
	{	5, -2
	,	-2, 2
	};

	auto _1or6_f = [](auto const x){  ::_identical( (x - 1.f)*(x - 6.f), std::complex<float>(0) );  };

	auto _are_parallel_f
	=	[](auto const v1, auto const v2){  ::_identical(  abs( v1.normalized().dot(v2.normalized()) ), 1.f  );  };

	{
		Eigen_Decomposition ed(Mat1);

		std::complex<float> const val0 = ed.eigenval(0), val1 = ed.eigenval(1);

		_1or6_f(val0);  _1or6_f(val1);

		Vector< std::complex<float> > const vec0 = ed.eigenvec(0), vec1 = ed.eigenvec(1);

		if( are_almost_same< std::complex<float> >(val0, 1) )
		{
			_are_parallel_f(vec0, Vector<float>{1, 2});
			_are_parallel_f(vec1, Vector<float>{-2, 1});
		}
		else
		{
			_are_parallel_f(vec0, Vector<float>{-2, 1});
			_are_parallel_f(vec1, Vector<float>{1, 2});
		}
	}
	{
		Eigen_Decomposition ed(Mat1, sgm::FlagSet<flag::Real_Symmetric>{});

		float const val0 = ed.eigenval(0), val1 = ed.eigenval(1);

		_1or6_f(val0);  _1or6_f(val1);
		
		Vector<float> const vec0 = ed.eigenvec(0), vec1 = ed.eigenvec(1);

		if( are_almost_same<float>(val0, 1) )
		{
			_are_parallel_f(vec0, Vector<float>{1, 2});
			_are_parallel_f(vec1, Vector<float>{-2, 1});
		}
		else
		{
			_are_parallel_f(vec0, Vector<float>{-2, 1});
			_are_parallel_f(vec1, Vector<float>{1, 2});
		}
	}
}


static void Singular_Value_Decomp()
{
	DynamicMat<float> const A
	=	Matrix<float, 9, 3>
		{	-10.f, -1.f, -.05f
		,	0.f, -1.f, 0.f
		,	10.f, -1.f, .2f
		,	-10.f, 0.f, 0.f
		,	0.f, 0.f, -.1f
		,	10.f, 0.f, 0.f
		,	-10.f, 1.f, -.05f
		,	0.f, 1.f, 0.f
		,	10.f, 1.f, .05f
		};

	auto is_diag_f
	=	[](Matrix<float> const &D)
		{	
			for(size_t i = 0;  i < D.rows();  ++i)
				for(size_t j = 0;  j < D.cols();  ++j)
					is_True(  i == j || are_almost_same<float>( D(i, j), 0 )  );
		};

	auto orthonormal_columns_f
	=	[](Matrix<float> const &M, size_t const s)
		{
			assert(s > 1);

			for(size_t j = 1;  j < s;  ++j)
				::_identical(  0.f, M.col(j-1).dot( M.col(j) )  );
		};

	Singular_Value_Decomposition svd0(A, sgm::FlagSet<flag::Value_Only>{});

	float const
		sv1 = svd0.singularval(0),
		sv2 = svd0.singularval(1),
		sv3 = svd0.singularval(2);

	is_True(sv1 >= sv2 && sv2 >= sv3);

	{
		Singular_Value_Decomposition svd(A);

		Matrix<float> const U = svd.Umat(), V = svd.Vmat(), D = svd.diagmat();

		::_identical(U.rows(), A.rows());
		::_identical(U.cols(), D.rows(), D.cols(), V.rows(), V.cols(), A.cols());

		is_diag_f(D);
		orthonormal_columns_f(U, A.cols());
		orthonormal_columns_f(V, A.cols());

		::_identical(A, U*D*V.transpose());

		float const
			s1 = svd.singularval(0),
			s2 = svd.singularval(1),
			s3 = svd.singularval(2);

		::_identical(Vector<float>{s1, s2, s3}, Vector<float>{sv1, sv2, sv3});
	}
	{
		Singular_Value_Decomposition svd(A, sgm::FlagSet<flag::FullMat>{});
		
		Matrix<float> const U = svd.Umat(), V = svd.Vmat(), D = svd.diagmat();

		::_identical(U.rows(), U.cols(), D.rows(), A.rows());
		::_identical(D.cols(), V.rows(), V.cols(), A.cols());

		is_diag_f(D);
		orthonormal_columns_f(U, A.cols());
		orthonormal_columns_f(V, A.cols());

		::_identical(A, U*D*V.transpose());
	}
	{
		Singular_Value_Decomposition svd(  A, sgm::Flags( flag::AbsolutelyTrunc<float>(1.f) )  );

		Matrix<float> const U = svd.Umat(), V = svd.Vmat(), D = svd.diagmat();
		
		size_t const nof_sv = svd.nof_singularvals();

		is_True(nof_sv == 2);

		::_identical(U.rows(), A.rows());
		::_identical(U.cols(), D.rows(), D.cols(), V.cols(), nof_sv);
		::_identical(V.rows(), A.cols());

		is_diag_f(D);
		orthonormal_columns_f(U, nof_sv);
		orthonormal_columns_f(V, nof_sv);
	}
	{
		Singular_Value_Decomposition svd(  A, sgm::Flags( flag::RelativelyTrunc<float>(.03f) )  );

		Matrix<float> const U = svd.Umat(), V = svd.Vmat(), D = svd.diagmat();
		
		size_t const nof_sv = svd.nof_singularvals();

		is_True(nof_sv == 2);

		::_identical(U.rows(), A.rows());
		::_identical(U.cols(), D.rows(), D.cols(), V.cols(), nof_sv);
		::_identical(V.rows(), A.cols());

		is_diag_f(D);
		orthonormal_columns_f(U, nof_sv);
		orthonormal_columns_f(V, nof_sv);
	}
	{
		Singular_Value_Decomposition svd(A, sgm::FlagSet<flag::UMat_Only, flag::ThinMat>{});

		is_True(svd.Vmat().size() == 0);

		svd(A, sgm::FlagSet<flag::VMat_Only, flag::ThinMat>{});

		is_True(svd.Umat().size() == 0);
	}
}


#include "Test_Decomposition.hpp"
#include <iostream>


void test::Decomposition_Tutorial::test()
{
	try
	{
		::Least_Square_Solution();
		::Eigen_Decomp();
		::Singular_Value_Decomp();

		std::wcout << L"Decomposition Test Complete.\n";
	}
	catch(sgm::spec::Exception const &e)
	{
		std::wcerr << L"Decomposition Test Failed. : " << e.what() << std::endl;
	}
}
