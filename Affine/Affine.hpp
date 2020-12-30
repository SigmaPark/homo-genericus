#pragma once


#ifndef _SGM_MXI_AFFINE_
#define _SGM_MXI_AFFINE_

#include "..\Matrix_interface\Matrix.hpp"


namespace sgm::mxi
{
	
	template<class elem_t, unsigned DIMENSION>
	struct AffineTransform;

	SGM_USER_DEFINED_TYPE_CHECK
	(	AffineTransform
	,	SGM_MACROPACK(class T, unsigned DIMENSION)
	,	SGM_MACROPACK(T, DIMENSION)
	);


	template<class elem_t, unsigned DIMENSION>
	struct RigidBodyTransform;

	SGM_USER_DEFINED_TYPE_CHECK
	(	RigidBodyTransform
	,	SGM_MACROPACK(class T, unsigned DIMENSION)
	,	SGM_MACROPACK(T, DIMENSION)
	);


	template<class elem_t, unsigned DIMENSION>
	struct ScalableBodyTransform;

	SGM_USER_DEFINED_TYPE_CHECK
	(	ScalableBodyTransform
	,	SGM_MACROPACK(class T, unsigned DIMENSION)
	,	SGM_MACROPACK(T, DIMENSION)
	);

	
	struct Affine;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, unsigned D>
struct sgm::mxi::AffineTransform
{
	using elem_t = std::decay_t<T>;
	static auto constexpr DIMENSION = D;


	Matrix<T, D, D> mat = Matrix<T, D, D>::identity();
	Vector<T, D> vec = Vector<T, D>::zero();


	template<class Q>
	auto operator()(Q const& q) const
	{
		if constexpr(MxTraits::is_mxiVector_v<Q>)
			return decltype(vec)(mat*q + vec);
		else if constexpr(MxTraits::is_mxiMatrix_v<Q>)
		{
			assert( q.is_SqrMat() && (q.cols() == D || q.cols() == D + 1) );

			return
			(*this)
			(	q.cols() == D
				?	AffineTransform{Matrix<T, D< D>(q), Vector<T, D>::zero()}
				:	AffineTransform
					{	Matrix<T, D, D>( q.block(0, 0, D, D) )
					,	Vector<T, D>( q.block(0, D, D, 1) )
					}
			);
		}
		else if constexpr(is_AffineTransform_v<Q>)
			return AffineTransform{q.mat*mat, q.mat*vec + q.vec};
	}


	auto inversed() const-> AffineTransform
	{
		decltype(mat) const im = mat.inversed();

		return{im, -im*vec};
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#



//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::mxi::Affine : No_Making
{

	template<class T, unsigned D>
	static auto inverse(AffineTransform<T, D> const& aft){  return aft.inversed();  }

	template<class T, unsigned D>
	static auto identity()-> AffineTransform<T, D> const&
	{
		static AffineTransform<T, D> const res;
		
		return res;
	}

};


#endif