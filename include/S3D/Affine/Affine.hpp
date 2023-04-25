/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _S3D_AFFINE_
#define _S3D_AFFINE_


#include "../Euclid/Euclid.hpp"
#include "../Quaternion/Quaternion.hpp"
#include "SGM/Boomerang/Boomerang.hpp"


namespace s3d
{

	template<class A>  
	class _Affine_interface;

	template<class T, size_t DIM>
	class Affine_Transform;
	
	template<class T, size_t DIM>
	class Scalable_Body_Transform;
	
	template<class T, size_t DIM>
	class Rigid_Body_Transform;
	
	template<class T, size_t DIM>
	class Rotation;

	template<class T, size_t DIM>  
	inline auto const Afn = Rigid_Body_Transform<T, DIM>();

}


namespace s3d::trait
{
	
	template<class A>  
	struct is_AffineTr;
	
	SGM_USER_DEFINED_TYPE_CHECK
	(	SGM_MACROPACK(class T, size_t D)
	,	Affine_Transform, <T, D>
	);

	SGM_USER_DEFINED_TYPE_CHECK
	(	SGM_MACROPACK(class T, size_t D)
	,	Scalable_Body_Transform, <T, D>
	);

	SGM_USER_DEFINED_TYPE_CHECK
	(	SGM_MACROPACK(class T, size_t D)
	,	Rigid_Body_Transform, <T, D>
	);

	SGM_USER_DEFINED_TYPE_CHECK
	(	SGM_MACROPACK(class T, size_t D)
	,	Rotation, <T, D>
	);

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class A>
struct s3d::trait::is_AffineTr
{
private:
	template<class...> /* Declaration Only */ 
	static auto _calc(...) noexcept-> False_t;

	template<class T> /* Declaration Only */ 
	static auto _calc(_Affine_interface<T>) noexcept-> True_t;

public:
	using type = decltype( _calc(Mock<A>()) );
	
	static bool constexpr value = type::value;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template
<	class LHS, class A, class = sgm::Enable_if_t< !s3d::trait::is_AffineTr<LHS>::value >  
>
static auto operator>>(LHS const& Lhs, s3d::_Affine_interface<A> const& affine)
{
	return affine.transfer(Lhs);  
}


template
<	class LHS, class A, class = sgm::Enable_if_t< !s3d::trait::is_AffineTr<LHS>::value >  
>
static decltype(auto) operator>>=(LHS& Lhs, s3d::_Affine_interface<A> const& affine)
{
	if constexpr(sgm::is_iterable<LHS>::value)
		for(auto& t : Lhs)
			t >>= affine;
	else
		Lhs = Lhs >> affine;  

	return Lhs;
}


template<class A>
class s3d::_Affine_interface
{
private:
	class _CRTPian : public A{  friend class _Affine_interface;  };

	template
	<	class P
	,	class PRES = Selective_t< is_immutable<P>::value, _CRTPian const, _CRTPian >*  
	>
	static auto _crtp(P p){  return static_cast<PRES>(p);  }


	template<class LHS, class _A, class>
	friend auto ::operator>>(LHS const& Lhs, s3d::_Affine_interface<_A> const& affine);
	

	template<class Q>  
	auto transfer(Q const& q) const{  return _crtp(this)->_transfer(q);  }


public:
	template<class Q>  
	auto operator>>(Q const& q) const{  return _crtp(this)->_compose(q);  }
	

	template<class Q>  
	decltype(auto) operator>>=(Q const& q){  return *this = *this >> q;  }


	auto inv() const{  return _crtp(this)->_inv();  }


	template<class...ARGS>
	auto translate(ARGS const&...args) const
	{
		if constexpr( sizeof...(ARGS) == 1 )
			return _crtp(this)-> _translate( Nth_Param<0>(args...) );
		else if constexpr(is_Convertible< First_t<ARGS const&...>, double >::value)
		{
			using _T = Decay_t<decltype( Mock<A>().vec()(0) )>;

			return translate( Vector<_T, sizeof...(ARGS)>{static_cast<_T>(args)...} );
		}
	}


	template<class...ARGS>  
	auto rotate(ARGS const&...args) const{  return _crtp(this)->_rotate(args...);  }
	
	template<class VEC, class...ARGS>  
	auto rotate_at(VEC const& origin, ARGS const&...args) const
	{
		return translate(-origin).rotate(args...).translate(origin);
	}


	template<class Q>  
	auto reflect(Q const& q) const
	{
		if constexpr(trait::is_Plane<Q>::value)
			return translate(-q.position()).reflect(q.normal()).translate(q.position());
		else
			return _crtp(this)->_reflect(q);  
	}

	template<class Q>  
	auto scale(Q const& q) const{  return _crtp(this)->_scale(q);  }
	
	template<class VEC, class Q>  
	auto scale_at(VEC const& origin, Q const& q) const
	{
		return translate(-origin).scale(q).translate(origin);  
	}


	decltype(auto) mat() const{  return _crtp(this)->_mat();  }
	decltype(auto) vec() const{  return _crtp(this)->_vec();  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, size_t DIM>
class s3d::Affine_Transform : public _Affine_interface< Affine_Transform<T, DIM> >
{
private:
	static_assert(trait::is_complexible<T>::value);

	using _parent_t = _Affine_interface<Affine_Transform>;

public:
	template
	<	class M, class V
	,	class
		=	Enable_if_t
			<	Has_Operator_New< Matrix<T>, M&& >::value
			&&	Has_Operator_New< Matrix<T>, V&& >::value
			>
	>
	Affine_Transform(M&& mat, V&& vec) noexcept(Aleph_Check<M&&, V&&>::value)
	:	_mat_part( Forward<M>(mat) ), _vec_part( Forward<V>(vec) ){}

	Affine_Transform() 
	:	_mat_part(Matrix<T, DIM, DIM>::identity()), _vec_part(Vector<T, DIM>::Zero()){}

	template
	<	class A
	,	class 
		=	Enable_if_t
			<	trait::is_AffineTr<A>::value
			&&	!is_Same< Decay_t<A>, Affine_Transform >::value  
			>
	>
	Affine_Transform(A const& affine) : _mat_part(affine.mat()), _vec_part(affine.vec()){}


	template
	<	class A
	,	class 
		=	Enable_if_t
			<	trait::is_AffineTr<A>::value
			&&	!is_Same< Decay_t<A>, Affine_Transform >::value  
			>
	>
	auto operator=(A&& affine) noexcept(is_Rvalue_Reference<A&&>::value)-> Affine_Transform&
	{
		bool constexpr move_affine_v = is_Rvalue_Reference<A&&>::value;

		_mat_part = Move_if<move_affine_v>(affine.mat());
		_vec_part = Move_if<move_affine_v>(affine.vec());

		return *this;
	}


	decltype(auto) mat() const{  return _parent_t::mat();  }
	auto mat()-> Matrix<T, DIM, DIM>&{  return _mat_part;  }

	decltype(auto) vec() const{  return _parent_t::vec();  }
	auto vec()-> Vector<T, DIM>&{  return _vec_part;  }


protected:
	template<class Q>
	auto _compose(Q const& q) const
	->	Affine_Transform{  return{q.mat()*mat(), q.mat()*vec() + q.vec()};  }


	template<class Q>
	auto _transfer(Q const& q) const
	{
		if constexpr(trait::is_UnitVec<Q>::value)
			return UnitVec<T, DIM>(mat()*q);
		else
			return Vector<T, DIM>(mat()*q + vec());
	}


	auto _inv() const-> Affine_Transform
	{
		auto const im = mat().inv();

		return{im, -im*vec()};
	}


	template<class Q>
	auto _translate(Q const& q) const-> Affine_Transform{  return{mat(), vec() + q};  }


	template<class...ARGS>
	auto _rotate(ARGS const&...args) const-> Affine_Transform
	{
		return _compose(  Rigid_Body_Transform( Rotation<T, DIM>(args...) )  );  
	}


	template<class Q>
	auto _reflect(Q const& q) const-> Affine_Transform
	{
		if constexpr(trait::is_UnitVec<Q>::value)
			return
			_compose
			(	Affine_Transform( Matrix<T, DIM, DIM>::identity() - T(2)*q.dyadic(q)
			,	Vector<T, DIM>::Zero() )
			);
		else 
			return Compile_Fails(); // no method to reflect with .
	}


	template<class Q>
	auto _scale(Q const &q) const-> Affine_Transform{  return{q*mat(), vec()};  }


	auto _mat() const-> Matrix<T, DIM, DIM> const&{  return _mat_part;  }
	auto _vec() const-> Vector<T, DIM> const&{  return _vec_part;  }


private:
	Matrix<T, DIM, DIM> _mat_part;
	Vector<T, DIM> _vec_part;
};


namespace s3d
{

	template
	<	class M, class V
	,	class 
		=	Enable_if_t< trait::is_FixedSizeMat<M>::value && trait::is_FixedSizeMat<V>::value >
	,	class _S = typename Decay_t<V>::value_type
	,	size_t _DIM = trait::Dimension<V>::value
	>
	Affine_Transform(M&&, V&&)-> Affine_Transform<_S, _DIM>;

	template
	<	class A
	,	class = Enable_if_t< trait::is_AffineTr<A>::valule >
	,	class _V = Decay_t< decltype(Mock<A>().vec()) >
	,	class _S = typename Decay_t<_V>::value_type
	,	size_t _DIM = trait::Dimension<_V>::value
	>
	Affine_Transform(A const&)-> Affine_Transform<_S, _DIM>;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, size_t DIM>
class s3d::Scalable_Body_Transform : public _Affine_interface< Scalable_Body_Transform<T, DIM> >
{
private:
	static_assert(trait::is_complexible<T>::value);

	using _parent_t = _Affine_interface<Scalable_Body_Transform>;

public:
	using _parent_t::mat;


	template
	<	class M, class V, class S
	,	class
		=	Enable_if_t
			<	Has_Operator_New< OrthogonalMat<T, DIM>, M&& >::value 
			&&	Has_Operator_New< Matrix<T>, V&& >::value
			&&	is_Convertible<S, double>::value
			>
	>
	Scalable_Body_Transform(M&& mat, V&& vec, S const scalar) 
	noexcept(Aleph_Check<M&&, V&&>::value)
	:	_otmat_part( Forward<M>(mat) ), _vec_part( Forward<V>(vec) )
	,	_scalar( static_cast<T>(scalar) )
	{}

	Scalable_Body_Transform() 
	:	_otmat_part(), _vec_part(Vector<T, DIM>::Zero()), _scalar( T(1) ){}

	Scalable_Body_Transform(Rigid_Body_Transform<T, DIM> const& rbTr)
	:	_otmat_part(rbTr.mat()), _vec_part(rbTr.vec()), _scalar( T(1) ){}

	Scalable_Body_Transform(Rotation<T, DIM> const& rot)
	:	_otmat_part(rot.ortho_mat()), _vec_part(Vector<T, DIM>::Zero()), _scalar( T(1) ){}


	template
	<   class Q
	,	class 
		=	Enable_if_t
			<	is_Convertible<Q&&, Scalable_Body_Transform>::value
			&&	!is_Same< Decay_t<Q>, Scalable_Body_Transform >::value
			>
	>
	auto operator=(Q&& q) noexcept(is_Rvalue_Reference<Q&&>::value)-> Scalable_Body_Transform&
	{  
		return *this = Scalable_Body_Transform( Forward<Q>(q) );
	}


	operator Affine_Transform<T, DIM>() const{  return{mat(), vec()};  }


	auto vec()-> Vector<T, DIM>&{  return _vec_part;  }
	decltype(auto) vec() const{  return _parent_t::vec();  }

	auto scalar() const{  return _scalar;  }
	auto scalar()-> T&{  return _scalar;  }

	auto ortho_mat() const-> OrthogonalMat<T, DIM> const&{  return _otmat_part;  }
	auto ortho_mat()-> OrthogonalMat<T, DIM>&{  return _otmat_part;  }	


protected:
	template<class Q>
	auto _compose(Q const& q) const
	{
		if constexpr(trait::is_Affine_Transform<Q>::value)
			return Affine_Transform(q.mat()*mat(), q.mat()*vec() + q.vec());
		else if constexpr(trait::is_Scalable_Body_Transform<Q>::value)
			return 
			Scalable_Body_Transform
			(	q.ortho_mat()*ortho_mat(), q.mat()*vec() + q.vec(), q.scalar()*scalar()
			);
		else if constexpr(trait::is_Rigid_Body_Transform<Q>::value)
			return 
			Scalable_Body_Transform
			(	q.ortho_mat()*ortho_mat(), q.mat()*vec() + q.vec(), scalar()
			);
		else if constexpr(trait::is_Rotation<Q>::value)
			return Scalable_Body_Transform(q.ortho_mat()*ortho_mat(), q.mat()*vec(), scalar());
		else 
			return (Scalable_Body_Transform)Compile_Fails(); // no method for composition .
	}


	template<class Q>
	auto _transfer(Q const& q) const
	{
		if constexpr(trait::is_UnitVec<Q>::value)
			return UnitVec<T, DIM>(ortho_mat()*q);
		else
			return Vector<T, DIM>(mat()*q + vec());
	}


	auto _inv() const-> Scalable_Body_Transform
	{
		auto const im = ortho_mat().transpose();
		T const s = T(1)/scalar();

		return{im, -s*im*vec(), s};
	}


	template<class Q>
	auto _translate(Q const& q) const
	->	Scalable_Body_Transform{  return{ortho_mat(), vec() + q, scalar()};  }


	template<class...ARGS>
	auto _rotate(ARGS const&...args) const-> Scalable_Body_Transform
	{
		return _compose(  Rigid_Body_Transform( Rotation<T, DIM>(args...) )  );  
	}


	template<class Q>
	auto _reflect(Q const& q) const-> Scalable_Body_Transform
	{	
		if constexpr(trait::is_UnitVec<Q>::value)
			return
			_compose
			(	Scalable_Body_Transform
				(	Matrix<T, DIM, DIM>::identity() - T(2)*q.dyadic(q)
				,	Vector<T, DIM>::Zero()
				,	1
				)
			);
		else 
			return Compile_Fails(); // no method to reflect with .
	}


	template<class Q>
	auto _scale(Q const& q) const
	->	Scalable_Body_Transform{  return{ortho_mat(), q*vec(), q*scalar()};  }


	auto _mat() const-> Matrix<T, DIM, DIM>{  return scalar()*ortho_mat();  }
	auto _vec() const-> Vector<T, DIM> const&{  return _vec_part;  }


private:
	OrthogonalMat<T, DIM> _otmat_part;
	Vector<T, DIM> _vec_part;
	T _scalar;
};


namespace s3d
{
	
	template
	<	class M, class V, class S
	,	class
		=	Enable_if_t
			<	trait::is_FixedSizeMat<M>::value && trait::is_FixedSizeMat<V>::value 
			&&	trait::is_real<S>::value
			>
	,	class _scalar_type = typename Decay_t<V>::value_type
	,	size_t _DIM = trait::Dimension<V>::value
	>
	Scalable_Body_Transform(M&&, V&&, S const)-> Scalable_Body_Transform<_scalar_type, _DIM>;

	template<class T, size_t DIM>
	Scalable_Body_Transform(Rigid_Body_Transform<T, DIM> const&)-> Scalable_Body_Transform<T, DIM>;

	template<class T, size_t DIM>
	Scalable_Body_Transform(Rotation<T, DIM> const&)-> Scalable_Body_Transform<T, DIM>;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, size_t DIM>
class s3d::Rigid_Body_Transform : public _Affine_interface< Rigid_Body_Transform<T, DIM> >
{
private:
	static_assert(trait::is_real<T>::value);

	using _parent_t = _Affine_interface<Rigid_Body_Transform>;

public:
	using _parent_t::mat;


	template
	<	class R, class V
	,	class
		=	Enable_if_t
			<	Has_Operator_New< Rotation<T, DIM>, R&& >::value 
			&&	Has_Operator_New< Matrix<T>, V&& >::value
			>
	>
	Rigid_Body_Transform(R&& r, V&& v) noexcept(Aleph_Check<R&&, V&&>::value)
	:	_rotation_part( Forward<R>(r) ), _vec_part( Forward<V>(v) ){}

	template
	<	class R
	,	class 
		=	Enable_if_t
			<	Has_Operator_New< Rotation<T, DIM>, R&& >::value 
			&&	!trait::is_Rigid_Body_Transform<R>::value 
			>   
	>
	Rigid_Body_Transform(R&& r)
	:	_rotation_part( Forward<R>(r) ), _vec_part(Vector<T, DIM>::Zero()){}

	Rigid_Body_Transform() : _rotation_part(), _vec_part(Vector<T, DIM>::Zero()){}


	template
	<   class Q
	,	class 
		=	Enable_if_t
			<	is_Convertible<Q&&, Rigid_Body_Transform>::value
			&&	!is_Same< Decay_t<Q>, Rigid_Body_Transform >::value
			>
	>
	auto operator=(Q&& q) noexcept(is_Rvalue_Reference<Q&&>::value)
	->	Rigid_Body_Transform&{  return *this = Rigid_Body_Transform( Forward<Q>(q) );  }


	operator Affine_Transform<T, DIM>() const{  return{mat(), vec()};  }
	operator Scalable_Body_Transform<T, DIM>() const{  return{ortho_mat(), vec(), T(1)};  }


	auto vec()-> Vector<T, DIM>&{  return _vec_part;  }
	decltype(auto) vec() const{  return _parent_t::vec();  }

	decltype(auto) ortho_mat() const{  return _rotation_part.ortho_mat();  }
	decltype(auto) ortho_mat(){  return _rotation_part.ortho_mat();  }

	auto rotator() const-> Rotation<T, DIM> const&{  return _rotation_part;  }
	auto rotator()-> Rotation<T, DIM>&{  return _rotation_part;  }


protected:
	template<class Q>
	auto _compose(Q const& q) const
	{
		if constexpr(trait::is_Affine_Transform<Q>::value)
			return Affine_Transform(q.mat()*mat(), q.mat()*vec() + q.vec());
		else if constexpr(trait::is_Scalable_Body_Transform<Q>::value)
			return 
			Scalable_Body_Transform
			(	q.ortho_mat()*ortho_mat(), q.mat()*vec() + q.vec(), q.scalar()
			);
		else if constexpr(trait::is_Rigid_Body_Transform<Q>::value)
			return Rigid_Body_Transform(rotator().rotate(q.rotator()), q.mat()*vec() + q.vec());
		else if constexpr(trait::is_Rotation<Q>::value)
			return Rigid_Body_Transform(rotator().rotate(q), q.mat()*vec());
		else 
			return (Rigid_Body_Transform)Compile_Fails(); // no method for composition .
	}


	template<class Q>
	auto _transfer(Q const& q) const
	{
		if constexpr(trait::is_UnitVec<Q>::value)
			return UnitVec<T, DIM>( rotator()(q) );
		else
			return Vector<T, DIM>( rotator()(q) + vec() );
	}


	auto _inv() const-> Rigid_Body_Transform
	{
		auto const ir = rotator().inv();

		return {ir, -ir(vec())};
	}


	template<class Q>
	auto _translate(Q const& q) const-> Rigid_Body_Transform{  return{ortho_mat(), vec() + q};  }


	template<class...ARGS>
	auto _rotate(ARGS const&...args) const-> Rigid_Body_Transform
	{
		return _compose(  Rigid_Body_Transform( Rotation<T, DIM>(args...) )  );  
	}


	template<class Q>
	auto _reflect(Q const& q) const-> Scalable_Body_Transform<T, DIM>
	{
		return static_cast< Scalable_Body_Transform<T, DIM> >(*this).reflect(q);
	}


	template<class Q>
	auto _scale(Q const& q) const
	->	Scalable_Body_Transform<T, DIM>{  return{mat(), q*vec(), q};  }
	

	auto _mat() const-> Matrix<T, DIM, DIM>{  return ortho_mat();  }
	auto _vec() const-> Vector<T, DIM> const&{  return _vec_part;  }


private:
	Rotation<T, DIM> _rotation_part;
	Vector<T, DIM> _vec_part;
};


namespace s3d
{
	
	template
	<	class R, class V
	,	class
		=	Enable_if_t
			<	(	trait::is_Rotation<R>::value 
				||	trait::is_Quaternion<R>::value 
				||	trait::Has_Matrix_interface<R>::value
				)
			&&	trait::is_FixedSizeMat<V>::value
			>
	,	class S = typename Decay_t<V>::value_type
	,	size_t _DIM = trait::Dimension<V>::value
	>
	Rigid_Body_Transform(R&&, V&&)-> Rigid_Body_Transform<S, _DIM>;

	template
	<	class R, class = Enable_if_t< trait::is_Rotation<R>::value >
	,	class S = typename Decay_t<R>::scalar_type
	,	size_t _DIM = Decay_t< decltype(Mock<R>().cortho_mat()) >::COL_SIZE
	>
	Rigid_Body_Transform(R&&)-> Rigid_Body_Transform<S, _DIM>;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class s3d::Rotation<T, 2>
{
public:
	static_assert(trait::is_real<T>::value);

	using scalar_type = T;


	Rotation( T const angle = T(0) ) : _angle(angle){}
	Rotation(OrthogonalMat<T, 2> const& m) : _angle( _from_OrthogonalMat(m) ){}


	template
	<	class Q
	,	class
		=	Enable_if_t
			<	Has_Operator_New<Rotation, Q&&>::value
			&&	!is_Same< Decay_t<Q>, Rotation >::value
			>
	>
	auto operator=(Q&& q)-> Rotation&{  return *this = Rotation( Forward<Q>(q) );  }


	auto inv() const-> Rotation{  return -angle();  }


	auto cortho_mat() const-> OrthogonalMat<T, 2>{  return _to_OrthogonalMat(_angle);  }
	decltype(auto) ortho_mat() const{  return cortho_mat();  }

	decltype(auto) ortho_mat()
	{
		return
		throw_Boomerang
		(	_to_OrthogonalMat(_angle)
		,	[&a = _angle](OrthogonalMat<T, 2> const& m){  a = _from_OrthogonalMat(m);  }
		);
	}


	auto angle() const-> T{  return _angle;  }


	auto operator()(Vector<T, 2> const& v) const-> Vector<T, 2>{  return ortho_mat()*v;  }
	auto operator()(UnitVec<T, 2> const& u) const-> UnitVec<T, 2>{  return (*this)(u.vec());  }


	template<class...ARGS>
	auto rotate(ARGS&&...args) const
	->	Rotation<T, 2>{  return Rotation<T, 2>( Forward<ARGS>(args)... ).angle() + angle();  }


private:
	T _angle;


	static auto _to_OrthogonalMat(T const angle)-> OrthogonalMat<T, 2>
	{
		T const c = std::cos(angle), s = std::sin(angle);

		return
		{	c, -s
		,	s, c
		};
	}

	static auto _from_OrthogonalMat(OrthogonalMat<T, 2> const& m)-> T
	{
		auto const sign = T( m(1, 0) >= 0 ? 1 : -1 );

		return sign*std::acos( m(0, 0) );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class s3d::Rotation<T, 3>
{
public:
	static_assert(trait::is_real<T>::value);


private:
	using _UQtn_t = UnitQuaternion<T>;
	using _OrthoMat_t = OrthogonalMat<T, 3>;


public:
	using scalar_type = T;
	

	Rotation() : _uqtn(){}
	
	Rotation(T const alpha, T const beta, T const gamma) 
	:	_uqtn( _from_Euler_angles(alpha, beta, gamma) ){}
	
	Rotation(UnitVec<T, 3> const &u, T const theta) : _uqtn( _from_Spin(u, theta) ){}

	Rotation(_UQtn_t const& uq) : _uqtn(uq){}
	Rotation(_UQtn_t&& uq) : _uqtn( Move(uq) ){}

	template
	<	class MAT
	,	class = Enable_if_t< trait::Has_Matrix_interface<MAT>::value >
	>
	Rotation(MAT&& m) : Rotation(  static_cast<_OrthoMat_t>( Forward<MAT>(m) )  ){}

	Rotation(_OrthoMat_t const& otm) : _uqtn( _from_OrthogonalMat(otm) ){}
	Rotation(_OrthoMat_t&& otm) : _uqtn(  _from_OrthogonalMat( Move(otm) )  ){}


	template
	<	class Q
	,	class 
		=	Enable_if_t
			<	is_Convertible<Q&&, Rotation>::value
			&&	!is_Same< Decay_t<Q>, Rotation >::value
			>
	>
	auto operator=(Q&& q) noexcept(is_Rvalue_Reference<Q&&>::value)
	->	Rotation&{  return *this = Rotation( Forward<Q>(q) );  }


	auto inv() const-> Rotation{  return cunit_qtn().inv();  }

	auto cunit_qtn() const-> _UQtn_t const&{  return _uqtn;  }
	decltype(auto) unit_qtn() const{  return cunit_qtn();  }
	auto unit_qtn()-> _UQtn_t&{  return _uqtn;  }


	auto cortho_mat() const-> _OrthoMat_t{  return _to_OrthogonamMat(_uqtn);  }
	decltype(auto) ortho_mat() const{  return cortho_mat();  }

	decltype(auto) ortho_mat()
	{
		return 
		throw_Boomerang
		(	_to_OrthogonamMat(_uqtn)
		,	[&q = _uqtn](_OrthoMat_t const& m){  q = _from_OrthogonalMat(m);  }
		);
	}


	auto cspin_vec() const-> Vector<T, 3>{  return _to_Spin(_uqtn);  }
	decltype(auto) spin_vec() const{  return cspin_vec();  }

	decltype(auto) spin_vec()
	{
		return
		throw_Boomerang
		(	_to_Spin(_uqtn)
		,	[&q = _uqtn](Vector<T, 3> const& v)
			{  
				T const norm = v.norm();

				q = _from_Spin( Skipped< UnitVec<T, 3> >(v/norm), norm );
			} 
		);
	}


	auto operator()(Vector<T, 3> const& v) const
	->	Vector<T, 3>{  return ( _uqtn*Quaternion<T>(0, v)*_uqtn.inv() ).v();  }

	auto operator()(UnitVec<T, 3> const &u) const-> UnitVec<T, 3>{  return (*this)(u.vec());  }


	template<class...ARGS>
	auto rotate(ARGS&&...args) const
	{
		_UQtn_t const q = Rotation<T, 3>( Forward<ARGS>(args)... ).cunit_qtn();

		return Rotation(q*_uqtn);
	}


private:
	_UQtn_t _uqtn;


	static auto _from_Spin(UnitVec<T, 3> const& u, T const theta)-> _UQtn_t
	{
		T const h = T(.5)*theta;

		return {std::cos(h), std::sin(h)*u};
	}

	static auto _to_Spin(_UQtn_t const& q)-> Vector<T, 3>
	{
		auto clamp_f 
		=	[](T const t, T const low, T const high)
			{  
				return t < low ? low : t > high ? high : t;  
			};

		T const st = clamp_f( q.v().norm(), T(-1), T(1) );

		return T(2)*std::asin(st)*q.v()/st;		
	}


	/**	Rotating order
	*	rotate along x-axis by alpha radian first,
	*	rotate along y-axis be beta radian next, 
	*	and rotate by gamma radian along z-axis.
	*	In other words, by rotation matrix multiplications, Rz(gamma) * Ry(beta) * Rx(alpha) 
	*	for column vector .
	*/
	static auto _from_Euler_angles(T const alpha, T const beta, T const gamma)-> _UQtn_t
	{
		T const
			ha = T(.5)*alpha, hb = T(.5)*beta, hg = T(.5)*gamma,
			ca = std::cos(ha), cb = std::cos(hb), cg = std::cos(hg),
			sa = std::sin(ha), sb = std::sin(hb), sg = std::sin(hg);

		return
		{	ca*cb*cg + sa*sb*sg
		,	sa*cb*cg - ca*sb*sg
		,	ca*sb*cg + sa*cb*sg
		,	ca*cb*sg - sa*sb*cg
		};
	}


	static auto _from_OrthogonalMat(_OrthoMat_t const& m)-> _UQtn_t
	{
		auto sqrt_plus1_f 
		=	[](T const t)-> T{  return static_cast<T>(  std::sqrt( t + T(1) )  );  };

		if( T const tr = m(0, 0) + m(1, 1) + m(2, 2);  tr > std::numeric_limits<T>::epsilon() )
		{
			auto const s = sqrt_plus1_f(tr);

			return
			{	s
			,	( m(2, 1) - m(1, 2) ) / s
			,	( m(0, 2) - m(2, 0) ) / s
			,	( m(1, 0) - m(0, 1) ) / s
			};
		}
		else if( m(0, 0) > m(1, 1) && m(0, 0) > m(2, 2) )
		{
			auto const s = sqrt_plus1_f( m(0, 0) - m(1, 1) - m(2, 2) );

			return
			{	( m(2, 1) - m(1, 2) ) / s
			,	s
			,	( m(0, 1) + m(1, 0) ) / s
			,	( m(2, 0) + m(0, 2) ) / s
			};
		}
		else if( m(1, 1) > m(2, 2) )
		{
			auto const s = sqrt_plus1_f( m(1, 1) - m(0, 0) - m(2, 2) );

			return
			{	( m(0, 2) - m(2, 0) ) / s
			,	( m(0, 1) + m(1, 0) ) / s
			,	s
			,	( m(1, 2) + m(2, 1) ) / s
			};
		}
		else
		{
			auto const s = sqrt_plus1_f( m(2, 2) - m(0, 0) - m(1, 1) );

			return
			{	( m(1, 0) - m(0, 1) ) / s
			,	( m(2, 0) + m(0, 2) ) / s
			,	( m(1, 2) + m(2, 1) ) / s
			,	s
			};
		}
	}

	static auto _to_OrthogonamMat(_UQtn_t const& q)-> _OrthoMat_t
	{
		T const& w = q.w();
		Vector<T, 3> const& v = q.v();

		return 
		(w*w - v.sqr_norm())*OrthogonalMat<T, 3>::identity() + 2*w*v.skew() + 2*v.dyadic(v);
	}
};


namespace s3d
{
	
	template<class T, size_t D>
	Rotation(OrthogonalMat<T, D> const&)-> Rotation<T, D>;

	template<class T, size_t D>
	Rotation(OrthogonalMat<T, D>&&)-> Rotation<T, D>;

	template<class T>
	Rotation(UnitQuaternion<T> const&)-> Rotation<T, 3>;

	template<class T>
	Rotation(UnitQuaternion<T>&&)-> Rotation<T, 3>;

	template<  class UVEC, class T, class = Enable_if_t< trait::is_UnitVec<UVEC>::value >  >
	Rotation(UVEC&&, T const)-> Rotation< typename Decay_t<UVEC>::value_type, 3 >;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _S3D_AFFINE_