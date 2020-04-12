#include "Matrix_interface.hpp"

#include "..\Reform\ReformEigen.h"
#include <vector>
#include <array>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm::mxi
{

	template<class T, int R, int C, int STRG>
	struct Matrix_impl : No_Making
	{
	private:
		template<class PDATA, class...SIZES>
		struct impl
		{
		private:
			using Mat_t 
			=	std::conditional_t
				<	is_DynamicMatSize_v<R, C>
				,	Eigen::Matrix<T, MatSize::DYNAMIC, MatSize::DYNAMIC, STRG>
				,	Eigen::Matrix<T, R, C, STRG>
				>;

			Eigen::Map
			<	std::conditional_t< is_immutable<PDATA>::value, Mat_t const, Mat_t >  
			>
			_core;


		public:
			impl(PDATA pdata, SIZES...sizes) : _core(pdata, sizes...){}

			auto operator()() &&-> decltype(_core) const&{  return _core;  }

			decltype(auto) element(int i, int j) &&{  return _core(i, j);  } 

			decltype(auto) block(int i, int j, int r, int c) &&
			{  
				return _core.block(i, j, r, c);  
			}

			decltype(auto) inverse() &&{  return _core.inverse();  }

			decltype(auto) transpose() &&{  return _core.adjoint();  }

		};


	public:
		template<class PDATA> static decltype(auto) calc
		(	PDATA pdata, [[maybe_unused]]size_t r, [[maybe_unused]]size_t c
		)
		{
			static_assert(std::is_pointer_v<PDATA>, "PDATA is not pointer type.");

			if constexpr(is_DynamicMatSize_v<R, C>)
				return impl(pdata, r, c);
			else
				return impl(pdata);
		}
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	template<class T, int SIZE>
	struct Vector_impl : No_Making
	{
	private:
		template<class PDATA, class..._SIZES>
		struct impl
		{
		private:
			using Vec_t = Eigen::Matrix<T, SIZE, 1>;

			Eigen::Map
			<	std::conditional_t< is_immutable<PDATA>::value, Vec_t const, Vec_t >
			>
			_core;


		public:
			impl(PDATA pdata, _SIZES..._sizes) : _core(pdata, _sizes...){}

			auto operator()() &&-> decltype(_core) const&{  return _core;  }

			decltype(auto) element(int d) &&{  return _core(d);  }

			decltype(auto) head(int n) &&{  return _core.head(n);  }

			decltype(auto) tail(int n) &&{  return _core.tail(n);  }

			decltype(auto) norm() &&{  return _core.norm();  }

			decltype(auto) norm_sqr() &&{  return _core.squaredNorm();  }


			template<class Q>
			decltype(auto) dot(Q&& q) &&{  return _core.dot( std::forward<Q>(q) );  }


			template<class Q>
			decltype(auto) cross(Q&& q) &&
			{  
				using V3 = Eigen::Matrix<T, 1, 3>;

				return V3(_core).cross( static_cast<V3>(q) );  
			}

			decltype(auto) rowvec() &&{  return _core.adjoint();  }

			decltype(auto) colvec() &&{  return _core;  }
		};

	public:
		template<class PDATA> static decltype(auto) calc
		(	PDATA pdata, [[maybe_unused]]size_t s
		)
		{
			static_assert(std::is_pointer_v<PDATA>, "PDATA is not pointer type.");

			if constexpr(SIZE == MatSize::DYNAMIC)
				return impl(pdata, s, 1);
			else
				return impl(pdata);
		}

	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	template<class T, int R, int C>
	struct internalData : No_Making
	{
		using type 
		=	std::conditional_t
			<	is_DynamicMatSize_v<R, C>
			,	std::vector<T>, std::array<T, R*C>
			>;
	};

}

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#



class Tutorial : sgm::No_Making
{
private:
	template<class T>
	static bool _Equal(T t1, decltype(t1) t2)
	{
		if constexpr(std::is_integral_v<T>)
			return t1 == t2;
		else if constexpr(std::is_floating_point_v<T>)
			return abs(t1- t2) < T(1e-5); 
		else SGM_COMPILE_FAILED(undefined comparison.);
	}


public:

	template<unsigned> static void Case();

	template<> static void Case<1>()
	{
		using namespace sgm::mxi;

		Matrix<float, 2, 3> const mat1
		{	1, 2, 3
		,	4, 5, 6
		};

		assert(mat1.data()[1] == 4 && mat1.data()[2] == 2);

		assert
		(	_Equal( mat1(0, 2), 3 )
		&&	_Equal( mat1(1, 2), 6 )
		);

		Matrix<float> const mat2(std::vector<float>{1, 2, 3, 4}, 2, 2);

		assert( _Equal(mat2.data()[1], 3) && _Equal(mat2.data()[2], 2) );
	}

	template<> static void Case<2>()
	{
		using namespace sgm;

		std::vector<float> vec
		{	1, 3, 5
		,	7, 9, 11
		,	13, 15, 17
		};

		auto mat 
		=	Eigen::Map< Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> >
			(	vec.data(), 3, 3
			);

		assert
		(	mat(0, 1) == 3
		&&	mat(1, 0) == 7
		&&	mat.row(0)(0) == Eigen::Vector3f(1, 3, 5)(0)
		&&	mat.row(0)(1) == Eigen::Vector3f(1, 3, 5)(1)
		&&	mat.row(0)(2) == Eigen::Vector3f(1, 3, 5)(2)
		);

		mat = mat * mat;

		assert( _Equal(vec.front(), 87) );
	}

	template<> static void Case<3>()
	{
		using namespace sgm::mxi;

		Matrix_<float> M1;

		M1
		=	Matrix<float, 2, 2>
			{	1, 2
			,	3, 4
			};

		assert
		(	_Equal( M1(0, 0), 1 )
		&&	_Equal( M1(0, 1), 2 )
		&&	_Equal( M1(1, 0), 3 )
		&&	_Equal( M1(1, 1), 4 )
		);

		Matrix<float> r0 = M1.row(0);

		Eigen::Matrix<float, 2, 2> EM;
		
		EM	
		<<	1, 2,
			3, 4;

		assert
		(	_Equal( EM(0, 0), 1 )
		&&	_Equal( EM(0, 1), 2 )
		&&	_Equal( EM(1, 0), 3 )
		&&	_Equal( EM(1, 1), 4 )
		);

		Eigen::MatrixXf r1 = EM.block(0,0,2,1);

		assert( _Equal(r1(0, 0), 1) && _Equal(r1(1, 0), 3) );

		assert( _Equal(r0.rows(), 1) && _Equal(r0.cols(), 2) );

		assert( _Equal(r0(0, 0), 1) && _Equal(r0(0, 1), 2) );
	}

	template<> static void Case<4>()
	{
		using namespace sgm::mxi;

		Matrix<float, 2, 2> const M1
		{	.5, .0
		,	.0, .5
		};

		Matrix<float, 2, 2> const iM1 = M1.inverse();

		assert(  _Equal( iM1(0, 0), 2 ) && _Equal( iM1(1, 1), 2 )  );

		Matrix<float, 2, 3> const M2
		{	2, 4, 6
		,	1, 3, 5
		};

		Matrix<float> const tM = M2.transpose();

		assert(  _Equal( tM(0, 1), 1 ) && _Equal( tM(2, 0), 6 )  );
	}

	template<> static void Case<5>()
	{
		using namespace sgm::mxi;

		Matrix<float, 2, 2> const 
			M1
			{	1, 0
			,	0, 1
			},
			M2
			{	0, 2
			,	2, 1
			},
			M3
			{	4, 3
			,	2, 1
			};

		Matrix<float, 2, 2> const Mplus = M1 + M2 + M3;
		Matrix<float, 2, 2> const Mminus = -M1 - M2 - M3;

		assert(  _Equal( Mplus(1, 1), 3 ) && _Equal( Mminus(1, 1), (-3) )  );
	}


	template<> static void Case<6>()
	{
		using namespace sgm::mxi;

		Matrix<float> M1;

		M1 
		=	Matrix<float, 2, 2>
			{	1, 2
			,	3, 4
			};

		Matrix<float, 2, 2> const M2
		{	0, 1
		,	1, 0
		};

		Matrix<float, 2, 2> const M3 = 3.0*(M1*M2);

		assert(  _Equal( M3(0, 0), 2*3 ) && _Equal( M3(1, 1), 3*3 )  );
	}


	template<> static void Case<7>()
	{
		using namespace sgm::mxi;

		Vector<float, 3> V1{3, 2, 1};

		assert(  _Equal( V1(0), 3 ) && _Equal( V1(2), 1 )  );

		V1(2) = 4;

		assert(  _Equal( V1(2), 4 )  );

		Vector<float> V2 = V1.head(2), V3;

		V3 = V1.tail(2);

		assert(  _Equal( V2(0), 3 ) && _Equal( V3(0), 2 )  );
	}

	template<> static void Case<8>()
	{
		using namespace sgm::mxi;

		Vector<float> V1{1, 2, 3}, V2{1, 0, 0}, V3{0, 1, 0};

		assert
		(	_Equal<float>(V1.norm_sqr(), 14) 
		&&	_Equal<float>( V1.norm_sqr(), V1.dot(V1) )
		&&	_Equal<float>( V2.cross(V3)(2), 1 )
		);
	}

	template<> static void Case<9>()
	{
		using namespace sgm::mxi;

		Vector<float> const V1{1, 2, 3}, V2{4, 5, 6};

		Matrix<float, 3, 2> M3;

		M3.col(1) = V1.colvec();
		M3.col(0) = V2.colvec();

		assert
		(	_Equal<float>( M3(0, 1), 1 )
		&&	_Equal<float>( M3(1, 1), 2 ) 
		&&	_Equal<float>( M3(2, 1), 3 )
		);

		assert
		(	_Equal<float>( M3(0, 0), 4 )
		&&	_Equal<float>( M3(1, 0), 5 ) 
		&&	_Equal<float>( M3(2, 0), 6 )
		);

		M3.row(2) = V1.rowvec().head(2);

		assert(  _Equal<float>( M3(2, 1), 2 )  );

		//M3.col(0).head(2);	// OperatorGuard should have all member functions!!
	}


};


int main()
{
	Tutorial::Case<1>();
	Tutorial::Case<2>();
	Tutorial::Case<3>();
	Tutorial::Case<4>();
	Tutorial::Case<5>();
	Tutorial::Case<6>();
	Tutorial::Case<7>();
	Tutorial::Case<8>();
	Tutorial::Case<9>();

	return 0;
}

#if 0

#endif