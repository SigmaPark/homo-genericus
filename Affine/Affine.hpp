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
	class Rotation;

	SGM_USER_DEFINED_TYPE_CHECK
	(	Rotation
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


template<class T, unsigned D>
class sgm::mxi::Rotation
{
private:
	using _basis_t = OrthonormalMatrix<T, D>;

public:
	using elem_t = std::decay_t<T>;
	static auto constexpr DIMENSION = D;

	
	Rotation(_basis_t const& basis) : _basis(basis){}
	Rotation(_basis_t&& basis) : _basis( std::move(basis) ){}

	Rotation() : Rotation(identity()){}


	template
	<	class Q
	,	class
		=	std::enable_if_t
			<	std::is_same_v< std::decay_t<Q>, _basis_t >
			&&	!std::is_same_v< std::decay_t<Q>, Rotation >
			>
	>
	auto operator=(Q&& basis)-> Rotation&
	{
		_basis = std::forward<Q>(basis);

		return *this;
	}


	auto basis() const-> _basis_t const&{  return _basis;  }
	auto basis(index_t const idx) const-> UnitVector<T, D>{  return _basis.col(idx);  }


	static auto identity()-> Rotation const&
	{
		static Rotation const res = _basis_t::identity();

		return res;
	}


private:
	_basis_t _basis;
};


template<class T>
class sgm::mxi::Rotation<T, 2>
{
public:
	using elem_t = std::decay_t<T>;
	static auto constexpr DIMENSION = 2;


	Rotation( T const angle = T(0) ) : _angle(angle){}
	Rotation(OrthonormalMatrix<T, 2> const& onx) : _angle(  acos( onx(0, 0) )  ){}


	template
	<	class Q, class = std::enable_if_t<  !std::is_same_v< std::decay_t<Q>, Rotation >  > 
	>
	auto operator=(Q&& q)-> Rotation&
	{
		if constexpr(std::is_scalar_v< std::decay_t<Q> >)
			_angle = q;
		else if constexpr(MxTraits::is_OrthonormalMatrix_v<Q>)
			*this = Rotation(q);
		else SGM_COMPILE_FAILED(no suitable method was found);

		return *this;
	}


	auto basis() const-> OrthonormalMatrix<T, 2>
	{
		T const c = cos(_angle), s = sin(_angle);

		return 
		Matrix<T, 2>
		{	c, -s
		,	s, c
		};
	}

	auto basis(index_t const idx) const-> UnitVector<T, 2>{  return basis().col(idx);  }


	static auto identity()-> Rotation const&
	{
		static Rotation const res;

		return res;
	}


private:
	elem_t _angle;
};


#include "..\Quaternion\Quaternion.hpp"


template<class T>
class sgm::mxi::Rotation<T, 3>
{
private:
	using _uqtn_t = UnitQuaternion<T>;
	using _basis_t = OrthonormalMatrix<T, 3>;

public:
	using elem_t = std::decay_t<T>;
	static auto constexpr DIMENSION = 3;


	Rotation(_uqtn_t const& uqtn) : _uqtn(uqtn){}
	Rotation(_uqtn_t&& uqtn) : _uqtn( std::move(uqtn) ){}

	Rotation() : Rotation(identity()){}

	Rotation(_basis_t const& basis);
	
	Rotation(T const alpha, T const beta, T const gamma);
	Rotation(T const theta, UnitVector<T, 3> const& axis);



	static auto identity()-> Rotation const&
	{
		static Rotation const res = _uqtn_t();

		return res;
	}


private:
	_uqtn_t _uqtn;
};
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