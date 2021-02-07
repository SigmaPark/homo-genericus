#pragma once


#ifndef _SGM_MXI_AFFINE_
#define _SGM_MXI_AFFINE_

#include "..\Matrix_interface\Matrix.hpp"


namespace sgm::mxi
{
	
	template<class elem_t, unsigned DIMENSION>
	class AffineTransform;

	SGM_USER_DEFINED_TYPE_CHECK14
	(	_, AffineTransform
	,	SGM_MACROPACK(class T, unsigned DIMENSION)
	,	SGM_MACROPACK(T, DIMENSION)
	);


	template<class elem_t, unsigned DIMENSION>
	class Rotation;

	SGM_USER_DEFINED_TYPE_CHECK14
	(	_,	Rotation
	,	SGM_MACROPACK(class T, unsigned DIMENSION)
	,	SGM_MACROPACK(T, DIMENSION)
	);


	template<class elem_t, unsigned DIMENSION>
	class RigidBodyTransform;

	SGM_USER_DEFINED_TYPE_CHECK14
	(	_, RigidBodyTransform
	,	SGM_MACROPACK(class T, unsigned DIMENSION)
	,	SGM_MACROPACK(T, DIMENSION)
	);


	template<class elem_t, unsigned DIMENSION>
	class ScalableBodyTransform;

	SGM_USER_DEFINED_TYPE_CHECK14
	(	_, ScalableBodyTransform
	,	SGM_MACROPACK(class T, unsigned DIMENSION)
	,	SGM_MACROPACK(T, DIMENSION)
	);

	
	//struct Affine;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, unsigned D>
class sgm::mxi::Rotation
{
private:
	using _basis_t = OrthonormalMatrix<T, D>;

public:
	using elem_t = Decay_t<T>;
	static auto constexpr DIMENSION = D;

	
	Rotation(_basis_t const& basis) : _basis(basis){}
	Rotation(_basis_t&& basis) noexcept : _basis( Move(basis) ){}

	Rotation() : Rotation(identity()){}


	template
	<	class Q
	,	class
		=	Enable_if_t
			<	is_Same_v< Decay_t<Q>, _basis_t > && !is_Same_v< Decay_t<Q>, Rotation >
			>
	>
	auto operator=(Q&& basis)-> Rotation&
	{
		_basis = Forward<Q>(basis);

		return *this;
	}


	auto basis() const-> _basis_t const&{  return _basis;  }


	template<class...ARGS>
	auto rotated(ARGS&&...args) const-> Rotation
	{
		if constexpr
		(	sizeof...(ARGS) == 1 && is_Same_v<  Decay_t< First_t<ARGS...> >, Rotation  >
		)
			return Rotation(  Nth_elem<0>( Forward<ARGS>(args)... ).basis()*basis()  );
		else 
			return rotated(  Rotation( Forward<ARGS>(args)... )  );
	}

	template<class...ARGS>
	auto rotate(ARGS&&...args)-> Rotation&
	{
		return *this = rotated( Forward<ARGS>(args)... );
	}


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
	using elem_t = Decay_t<T>;
	static auto constexpr DIMENSION = 2;


	Rotation( T const angle = T(0) ) : _angle(angle){}
	Rotation(OrthonormalMatrix<T, 2> const& onx) : _angle(  acos( onx(0, 0) )  ){}


	template<   class Q, class = Enable_if_t<  !is_Same_v< Decay_t<Q>, Rotation >  >   >
	auto operator=(Q&& q)-> Rotation&
	{
		if constexpr(is_Convertible_v< Decay_t<Q>, double >)
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
		regard_normalized
		(	Matrix<T, 2>
			{	c, -s
			,	s, c
			}
		);
	}


	template<class...ARGS>
	auto rotated(ARGS&&...args) const-> Rotation
	{
		if constexpr
		(	sizeof...(ARGS) == 1 && is_Same_v<  Decay_t< First_t<ARGS...> >, Rotation  >
		)
			return Rotation(  Nth_elem<0>( Forward<ARGS>(args)... ).basis()*basis()  );
		else 
			return rotated(  Rotation( Forward<ARGS>(args)... )  );
	}

	template<class...ARGS>
	auto rotate(ARGS&&...args)-> Rotation&
	{
		return *this = rotated( Forward<ARGS>(args)... );
	}


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
	using elem_t = Decay_t<T>;
	static auto constexpr DIMENSION = 3;


	Rotation(_uqtn_t const& uqtn) : _uqtn(uqtn){}
	Rotation(_uqtn_t&& uqtn) noexcept : _uqtn( Move(uqtn) ){}

	Rotation() : Rotation(identity()){}

	Rotation(_basis_t const& basis) : Rotation( _Basis_to_Uqtn(basis) ){};
	
	Rotation(T const alpha, T const beta, T const gamma)
	:	Rotation( _Euler_to_Uqtn(alpha, beta, gamma) )
	{}

	Rotation(T const theta, UnitVector<T, 3> const& axis)
	:	Rotation( _Spin_to_Uqtn(theta, axis) )
	{}


	auto uqtn() const-> _uqtn_t const&{  return _uqtn;  }

	auto basis() const-> _basis_t
	{
		T const
			&w = _uqtn.w(), &x = _uqtn.x(), &y = _uqtn.y(), &z = _uqtn.z(),
			ww = w*w, xx = x*x, yy = y*y, zz = z*z,
			xy = x*y, yz = y*z, zx = z*x, wx = w*x, wy = w*y, wz = w*z;

		return
		regard_normalized
		(	Matrix<T, 3, 3>
			{	ww + xx - yy - zz, T(2)*(xy - wz), T(2)*(zx + wy)
			,	T(2)*(xy + wz), ww - xx + yy - zz, T(2)*(yz - wx)
			,	T(2)*(zx - wy), T(2)*(yz + wx), ww - xx - yy + zz
			}
		);
	}


	template<class...ARGS>
	auto rotated(ARGS&&...args) const-> Rotation
	{
		if constexpr
		(	sizeof...(ARGS) == 1 && is_Same_v<  Decay_t< First_t<ARGS...> >, Rotation  >
		)
			return Rotation(  Nth_elem<0>( Forward<ARGS>(args)... ).uqtn()*uqtn()  );
		else 
			return rotated(  Rotation( Forward<ARGS>(args)... )  );
	}

	template<class...ARGS>
	auto rotate(ARGS&&...args)-> Rotation&
	{
		return *this = rotated( Forward<ARGS>(args)... );
	}


	static auto identity()-> Rotation const&
	{
		static Rotation const res = _uqtn_t();

		return res;
	}


private:
	_uqtn_t _uqtn;


	static auto _Euler_to_Uqtn(T const alpha, T const beta, T const gamma)-> _uqtn_t
	{
		T const
			ha = T(.5)*alpha, hb = T(.5)*beta, hg = T(.5)*gamma,
			ca = cos(ha), cb = cos(hb), cg = cos(hg),
			sa = sin(ha), sb = sin(hb), sg = sin(hg);

		return
		regard_normalized
		(	Quaternion<T>
			{	ca*cb*cg + sa*sb*sg
			,	ca*cb*sg - sa*sb*cg
			,	ca*sb*cg + sa*cb*sg
			,	sa*cb*cg - ca*sb*sg
			}
		);
	}


	static auto _Basis_to_Uqtn(_basis_t const& m)-> _uqtn_t
	{
		T const _2w = sqrt( T(1) + m(0, 0) + m(1, 1) + m(2, 2) );

		Vector<T, 3> const _2v
		=	Vector<T, 3>{m(2, 1) - m(1, 2), m(0, 2) - m(2, 0), m(1, 0) - m(0, 1)} / _2w;

		return regard_normalized( T(.5)*Quaternion<T>{_2w, _2v} );
	}


	static auto _Spin_to_Uqtn(T const th, UnitVector<T, 3> const& u)-> _uqtn_t
	{
		T const hth = T(.5)*th;

		return regard_normalized( Quaternion<T>{cos(hth), Vector<T, 3>( sin(hth)*u )} );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, unsigned D>
class sgm::mxi::AffineTransform
{
public:
	using elem_t = Decay_t<T>;
	static auto constexpr DIMENSION = D;


	template
	<	class MAT = Matrix<T, D, D>, class VEC = Vector<T, D> 
	,	class = Enable_if_t< MxTraits::is_mxiMatrix_v<MAT> && MxTraits::is_mxiVector_v<VEC> >
	>
	AffineTransform(MAT&& mat = MAT::identity(), VEC&& vec = VEC::zero())
	noexcept( is_RvalueReference_v<MAT&&> && is_RvalueReference_v<VEC&&> )
	:	_mat( Forward<MAT>(mat) ), _vec( Forward<VEC>(vec) )
	{}


	auto partM() const-> Matrix<T, D, D> const&{  return _mat;  }
	auto partM()-> Matrix<T, D, D>&{  return _mat;  }
	auto partV() const-> Vector<T, D> const&{  return _vec;  }
	auto partV()-> Vector<T, D>&{  return _vec;  }


	template<class Q>
	auto affine(Q const& q) const
	{
		if constexpr(MxTraits::is_mxiMatrix_v<Q>)
		{
			assert( q.is_SqrMat() && (q.cols() == D || q.cols() == D + 1) );

			return
			affine
			(	q.cols() == D
				?	AffineTransform( Matrix<T, D, D>(q) )
				:	AffineTransform
					(	Matrix<T, D, D>( q.block(0, 0, D, D) )
					,	Vector<T, D>( q.block(0, D, D, 1) )
					)
			);
		}
		else if constexpr(is_AffineTransform_v<Q>)
			return AffineTransform(q.partM()*partM(), q.partM()*partV() + q.partV());
		else if constexpr(is_Convertible_v<Q, AffineTransform>)
			return affine( static_cast<AffineTransform>(q) );
	}

	template
	<	class MAT, class VEC
	,	class = Enable_if_t< MxTraits::is_mxiMatrix_v<MAT> && MxTraits::is_mxiVector_v<VEC> >
	>
	auto affine(MAT&& mat, VEC&& vec) const
	{
		return affine(  AffineTransform( Forward<MAT>(mat), Forward<VEC>(vec) )  );
	}


	auto inversed() const-> AffineTransform
	{
		Matrix<T, D, D> const im = partM().inversed();

		return{im, -im*partV()};
	}


	auto transfer(Vector<T, D> const& q) const{  return Vector<T, D>(partM()*q + partV());  }


	static auto identity()-> AffineTransform const&
	{
		static AffineTransform const res;

		return res;
	}


private:
	Matrix<T, D, D> _mat;
	Vector<T, D> _vec;	
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, unsigned D>
class sgm::mxi::RigidBodyTransform
{
public:
	using elem_t = Decay_t<T>;
	static auto constexpr DIMENSION = D;	


	template< class ROT = Rotation<T, D>, class VEC = Vector<T, D> >
	RigidBodyTransform(ROT&& rot = {}, VEC&& vec = VEC::zero()) 
	noexcept( is_RvalueReference_v<ROT&&> && is_RvalueReference_v<VEC&&> )
	:	_rot( Forward<ROT>(rot) ), _vec( Forward<VEC>(vec) )
	{}


	operator AffineTransform<T, D>() const{  return {_rot.basis().mat(), _vec};  }


	auto partR() const-> Rotation<T, D> const&{  return _rot;  }
	auto partR()-> Rotation<T, D>&{  return _rot;  }
	auto partV() const-> Vector<T, D> const&{  return _vec;  }
	auto partV()-> Vector<T, D>&{  return _vec;  }


	
private:
	Rotation<T, D> _rot;
	Vector<T, D> _vec;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


//struct sgm::mxi::Affine : No_Making
//{
//
//	template<class T, unsigned D>
//	static auto inverse(AffineTransform<T, D> const& aft){  return aft.inversed();  }
//
//	template<class T, unsigned D>
//	static auto identity()-> AffineTransform<T, D> const&
//	{
//		static AffineTransform<T, D> const res;
//		
//		return res;
//	}
//
//};


#endif



#if 0 

tr1(tr2).rotate(angle, uvec).translate()

#endif 