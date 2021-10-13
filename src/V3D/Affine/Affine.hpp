#pragma once


#ifndef _V3D_AFFINE_
#define _V3D_AFFINE_


#include "..\Euclid\Euclid.hpp"


namespace v3d
{

	template<class A>  class _Affine_interface;

	template<class T, size_t DIM = 3>  class Affine_Transform;
	template<class T, size_t DIM = 3>  class Scalable_Body_Transform;
	template<class T, size_t DIM = 3>  class Rigid_Body_Transform;
	template<class T, size_t DIM = 3>  class Rotation;

	template<class T, size_t DIM = 3>  inline auto const Afn = Rigid_Body_Transform<T, DIM>();

}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


namespace v3d::trait
{
	
	template<class A>  struct is_AffineTr;
	template<class A>  static bool constexpr is_AffineTr_v = is_AffineTr<A>::value;


#ifndef _V3D_AFFINE_INTERFACE_TYPE_CHECK
	#define _V3D_AFFINE_INTERFACE_TYPE_CHECK(TITLE)	\
		SGM_USER_DEFINED_TYPE_CHECK14( _, TITLE, SGM_MACROPACK(class T, size_t DIM), SGM_MACROPACK(T, DIM) )

	_V3D_AFFINE_INTERFACE_TYPE_CHECK(Affine_Transform);
	_V3D_AFFINE_INTERFACE_TYPE_CHECK(Scalable_Body_Transform);
	_V3D_AFFINE_INTERFACE_TYPE_CHECK(Rigid_Body_Transform);
	_V3D_AFFINE_INTERFACE_TYPE_CHECK(Rotation);

	#undef _V3D_AFFINE_INTERFACE_TYPE_CHECK
#else
	#error _V3D_AFFINE_INTERFACE_TYPE_CHECK was already defined somewhere else.
#endif

}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


template<class A>
struct v3d::trait::is_AffineTr
{
private:
	template<class...> /* Declaration Only */ static sgm::False_t _calc(...) noexcept;
	template<class T> /* Declaration Only */ static sgm::True_t _calc(_Affine_interface<T>) noexcept;

public:
	static bool constexpr value = decltype( _calc(std::declval<A>()) )::value;

	using type = sgm::Boolean_type<value>;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<  class LHS, class A, class = std::enable_if_t< !v3d::trait::is_AffineTr_v<LHS> >  >
static auto operator>>(LHS const &Lhs, v3d::_Affine_interface<A> const &affine){  return affine.transfer(Lhs);  }


template<  class LHS, class A, class = std::enable_if_t< !v3d::trait::is_AffineTr_v<LHS> >  >
static decltype(auto) operator>>=(LHS &Lhs, v3d::_Affine_interface<A> const &affine)
{
	if constexpr(sgm::is_iterable<LHS>::value)
		for(auto &t : Lhs)
			t >>= affine;
	else
		Lhs = Lhs >> affine;  

	return Lhs;
}


template<class A>
class v3d::_Affine_interface
{
private:
	class _CRTPian : public A{  friend class _Affine_interface;  };

	template
	<	class P
	,	class pres_t = std::conditional_t< sgm::is_immutable<P>::value, _CRTPian const, _CRTPian >*  
	>
	static auto _crtp(P p){  return static_cast<pres_t>(p);  }


	template<class LHS, class A, class>
	friend auto ::operator>>(LHS const &Lhs, v3d::_Affine_interface<A> const &affine);
	
	template<class Q>  auto transfer(Q const &q) const{  return _crtp(this)->_transfer(q);  }


public:
	template<class Q>  auto operator>>(Q const &q) const{  return _crtp(this)->_compose(q);  }
	template<class Q>  decltype(auto) operator>>=(Q const &q){  return *this = *this >> q;  }

	auto inv() const{  return _crtp(this)->_inv();  }

	template<class...ARGS>
	auto translate(ARGS const&...args) const
	{
		if constexpr( sizeof...(ARGS) == 1 )
			return _crtp(this)-> _translate( sgm::Nth_elem<0>(args...) );
		else if constexpr(std::is_convertible_v< sgm::First_t<ARGS const&...>, double >)
		{
			using _T = std::decay_t<decltype( std::declval<A>().vec()(0) )>;

			return translate( Vector<_T, sizeof...(ARGS)>{static_cast<_T>(args)...} );
		}
		else SGM_COMPILE_FAILED(no suitable method for translate .);
	}

	template<class...ARGS>  auto rotate(ARGS const&...args) const{  return _crtp(this)->_rotate(args...);  }
	
	template<class VEC, class...ARGS>  
	auto rotate_at(VEC const &origin, ARGS const&...args) const
	{
		return translate(-origin).rotate(args...).translate(origin);
	}

	template<class Q>  
	auto reflect(Q const &q) const
	{
		if constexpr(trait::is_Plane_v<Q>)
			return translate(-q.position()).reflect(q.normal()).translate(q.position());
		else
			return _crtp(this)->_reflect(q);  
	}

	template<class Q>  auto scale(Q const &q) const{  return _crtp(this)->_scale(q);  }
	
	template<class VEC, class Q>  
	auto scale_at(VEC const &origin, Q const &q) const{  return translate(-origin).scale(q).translate(origin);  }


	decltype(auto) mat() const{  return _crtp(this)->_mat();  }
	decltype(auto) vec() const{  return _crtp(this)->_vec();  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class T, size_t DIM>
class v3d::Affine_Transform : public _Affine_interface< Affine_Transform<T, DIM> >
{
private:
	static_assert(trait::is_complexible_v<T>);

	using _parent_t = _Affine_interface<Affine_Transform>;

public:
	template
	<	class M, class V
	,	class
		=	std::enable_if_t
			<	std::is_constructible_v< Matrix<T>, M&& > && std::is_constructible_v< Matrix<T>, V&& >
			>
	>
	Affine_Transform(M &&mat, V &&vec) noexcept(std::is_rvalue_reference_v<M&&> || std::is_rvalue_reference_v<V&&>)
	:	_mat_part( std::forward<M>(mat) ), _vec_part( std::forward<V>(vec) ){}

	Affine_Transform() : _mat_part(Matrix<T, DIM, DIM>::identity()), _vec_part(Vector<T, DIM>::Zero()){}

	template
	<	class A
	,	class 
		=	std::enable_if_t<  trait::is_AffineTr_v<A> && !std::is_same_v< std::decay_t<A>, Affine_Transform >  >
	>
	Affine_Transform(A const &affine) : _mat_part(affine.mat()), _vec_part(affine.vec()){}


	template
	<	class A
	,	class 
		=	std::enable_if_t<  trait::is_AffineTr_v<A> && !std::is_same_v< std::decay_t<A>, Affine_Transform >  >
	>
	auto operator=(A &&affine) noexcept(std::is_rvalue_reference_v<A&&>)-> Affine_Transform&
	{
		bool constexpr move_affine = std::is_rvalue_reference_v<A&&>;

		_mat_part = sgm::Move_if<move_affine>::cast(affine.mat());
		_vec_part = sgm::Move_if<move_affine>::cast(affine.vec());

		return *this;
	}


	auto mat()-> Matrix<T, DIM, DIM>&{  return _mat_part;  }
	auto vec()-> Vector<T, DIM>&{  return _vec_part;  }
	decltype(auto) mat() const{  return _parent_t::mat();  }
	decltype(auto) vec() const{  return _parent_t::vec();  }


protected:
	template<class Q>
	auto _compose(Q const &q) const-> Affine_Transform
	{
		return{q.mat()*mat(), q.mat()*vec() + q.vec()};
	}

	template<class Q>
	auto _transfer(Q const &q) const
	{
		if constexpr(trait::is_UnitVec_v<Q>)
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
	auto _translate(Q const &q) const-> Affine_Transform{  return{mat(), vec() + q};  }

	template<class...ARGS>
	auto _rotate(ARGS const&...args) const
	->	Affine_Transform{  return _compose(  Rigid_Body_Transform<T, DIM>( Rotation<T, DIM>(args...) )  );  }

	template<class Q>
	auto _reflect(Q const &q) const-> Affine_Transform
	{
		if constexpr(trait::is_UnitVec_v<Q>)
			return
			_compose
			(	Affine_Transform( Matrix<T, DIM, DIM>::identity() - T(2)*q.dyadic(q), Vector<T, DIM>::Zero() )
			);
		else SGM_COMPILE_FAILED(no method to reflect with .);
	}

	template<class Q>
	auto _scale(Q const &q) const-> Affine_Transform{  return{q*mat(), vec()};  }

	auto _mat() const-> Matrix<T, DIM, DIM> const&{  return _mat_part;  }
	auto _vec() const-> Vector<T, DIM> const&{  return _vec_part;  }


private:
	Matrix<T, DIM, DIM> _mat_part;
	Vector<T, DIM> _vec_part;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class T, size_t DIM>
class v3d::Scalable_Body_Transform : public _Affine_interface< Scalable_Body_Transform<T, DIM> >
{
private:
	static_assert(trait::is_complexible_v<T>);

	using _parent_t = _Affine_interface<Scalable_Body_Transform>;

public:
	using _parent_t::mat;


	template
	<	class M, class V, class S
	,	class
		=	std::enable_if_t
			<	std::is_constructible_v< OrthogonalMat<T, DIM>, M&& > && std::is_constructible_v< Matrix<T>, V&& >
			&&	std::is_convertible_v<S, double>
			>
	>
	Scalable_Body_Transform(M &&mat, V &&vec, S const scalar) 
	noexcept(std::is_rvalue_reference_v<M&&> || std::is_rvalue_reference_v<V&&>)
	:	_otmat_part( std::forward<M>(mat) ), _vec_part( std::forward<V>(vec) ), _scalar( static_cast<T>(scalar) ){}

	Scalable_Body_Transform() : _otmat_part(), _vec_part(Vector<T, DIM>::Zero()), _scalar( T(1) ){}

	Scalable_Body_Transform(Rigid_Body_Transform<T, DIM> const &rbTr)
	:	_otmat_part(rbTr.mat()), _vec_part(rbTr.vec()), _scalar( T(1) ){}

	Scalable_Body_Transform(Rotation<T, DIM> const &rot)
	:	_otmat_part(rot.ortho_mat()), _vec_part(Vector<T, DIM>::Zero()), _scalar( T(1) ){}


	template
	<   class Q
	,	class 
		=	std::enable_if_t
			<	std::is_convertible_v<Q&&, Scalable_Body_Transform>
			&&	!std::is_same_v< std::decay_t<Q>, Scalable_Body_Transform >  
			>
	>
	auto operator=(Q &&q) noexcept(std::is_rvalue_reference_v<Q&&>)-> Scalable_Body_Transform&
	{  
		return *this = Scalable_Body_Transform( std::forward<Q>(q) );
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
	auto _compose(Q const &q) const
	{
		if constexpr(trait::is_Affine_Transform_v<Q>)
			return Affine_Transform<T, DIM>(q.mat()*mat(), q.mat()*vec() + q.vec());
		else if constexpr(trait::is_Scalable_Body_Transform_v<Q>)
			return Scalable_Body_Transform(q.ortho_mat()*ortho_mat(), q.mat()*vec() + q.vec(), q.scalar()*scalar());
		else if constexpr(trait::is_Rigid_Body_Transform_v<Q>)
			return Scalable_Body_Transform(q.ortho_mat()*ortho_mat(), q.mat()*vec() + q.vec(), scalar());
		else if constexpr(trait::is_Rotation_v<Q>)
			return Scalable_Body_Transform(q.ortho_mat()*ortho_mat(), q.mat()*vec(), scalar());
		else SGM_COMPILE_FAILED(no method for composition .);
	}

	template<class Q>
	auto _transfer(Q const &q) const
	{
		if constexpr(trait::is_UnitVec_v<Q>)
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
	auto _translate(Q const &q) const-> Scalable_Body_Transform{  return{ortho_mat(), vec() + q, scalar()};  }

	template<class...ARGS>
	auto _rotate(ARGS const&...args) const->	Scalable_Body_Transform
	{
		return _compose(  Rigid_Body_Transform<T, DIM>( Rotation<T, DIM>(args...) )  );  
	}

	template<class Q>
	auto _reflect(Q const &q) const-> Scalable_Body_Transform
	{	
		if constexpr(trait::is_UnitVec_v<Q>)
			return
			_compose
			(	Scalable_Body_Transform
				(	Matrix<T, DIM, DIM>::identity() - T(2)*q.dyadic(q), Vector<T, DIM>::Zero(), 1
				)
			);
		else SGM_COMPILE_FAILED(no method to reflect with .);
	}

	template<class Q>
	auto _scale(Q const &q) const-> Scalable_Body_Transform{  return{ortho_mat(), q*vec(), q*scalar()};  }

	auto _mat() const-> Matrix<T, DIM, DIM>{  return scalar()*ortho_mat();  }
	auto _vec() const-> Vector<T, DIM> const&{  return _vec_part;  }


private:
	OrthogonalMat<T, DIM> _otmat_part;
	Vector<T, DIM> _vec_part;
	T _scalar;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class T, size_t DIM>
class v3d::Rigid_Body_Transform : public _Affine_interface< Rigid_Body_Transform<T, DIM> >
{
private:
	static_assert(trait::is_real_v<T>);

	using _parent_t = _Affine_interface<Rigid_Body_Transform>;

public:
	using _parent_t::mat;


	template
	<	class R, class V
	,	class
		=	std::enable_if_t
			<	std::is_constructible_v< Rotation<T, DIM>, R&& > && std::is_constructible_v< Matrix<T>, V&& >
			>
	>
	Rigid_Body_Transform(R &&r, V &&v) noexcept(std::is_rvalue_reference_v<R&&> || std::is_rvalue_reference_v<V&&>)
	:	_rotation_part( std::forward<R>(r) ), _vec_part( std::forward<V>(v) ){}

	template
	<	class R
	,	class 
		=	std::enable_if_t
			<	std::is_constructible_v< Rotation<T, DIM>, R&& > && !trait::is_Rigid_Body_Transform_v<R> 
			>   
	>
	Rigid_Body_Transform(R &&r)
	:	_rotation_part( std::forward<R>(r) ), _vec_part(Vector<T, DIM>::Zero()){}

	Rigid_Body_Transform() : _rotation_part(), _vec_part(Vector<T, DIM>::Zero()){}


	template
	<   class Q
	,	class 
		=	std::enable_if_t
			<	std::is_convertible_v<Q&&, Rigid_Body_Transform>
			&&	!std::is_same_v< std::decay_t<Q>, Rigid_Body_Transform >  
			>
	>
	auto operator=(Q &&q) noexcept(std::is_rvalue_reference_v<Q&&>)-> Rigid_Body_Transform&
	{  
		return *this = Rigid_Body_Transform( std::forward<Q>(q) );
	}


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
	auto _compose(Q const &q) const
	{
		if constexpr(trait::is_Affine_Transform_v<Q>)
			return Affine_Transform<T, DIM>(q.mat()*mat(), q.mat()*vec() + q.vec());
		else if constexpr(trait::is_Scalable_Body_Transform_v<Q>)
			return Scalable_Body_Transform<T, DIM>(q.ortho_mat()*ortho_mat(), q.mat()*vec() + q.vec(), q.scalar());
		else if constexpr(trait::is_Rigid_Body_Transform_v<Q>)
			return Rigid_Body_Transform(rotator().rotate(q.rotator()), q.mat()*vec() + q.vec());
		else if constexpr(trait::is_Rotation_v<Q>)
			return Rigid_Body_Transform(rotator().rotate(q), q.mat()*vec());
		else SGM_COMPILE_FAILED(no method for composition .);		
	}

	template<class Q>
	auto _transfer(Q const &q) const
	{
		if constexpr(trait::is_UnitVec_v<Q>)
			return UnitVec<T, DIM>( rotator()(q) );
		else
			return Vector<T, DIM>( rotator()(q) + vec() );
	}

	auto _inv() const-> Rigid_Body_Transform
	{
		auto const im = ortho_mat().transpose();

		return{im, -im*vec()};
	}

	template<class Q>
	auto _translate(Q const &q) const-> Rigid_Body_Transform{  return{ortho_mat(), vec() + q};  }

	template<class...ARGS>
	auto _rotate(ARGS const&...args) const
	->	Rigid_Body_Transform{  return _compose(  Rigid_Body_Transform( Rotation<T, DIM>(args...) )  );  }

	template<class Q>
	auto _reflect(Q const &q) const-> Scalable_Body_Transform<T, DIM>
	{
		return static_cast< Scalable_Body_Transform<T, DIM> >(*this).reflect(q);
	}

	template<class Q>
	auto _scale(Q const &q) const-> Scalable_Body_Transform<T, DIM>{  return{mat(), q*vec(), q};  }
	
	auto _mat() const-> Matrix<T, DIM, DIM>{  return ortho_mat();  }
	auto _vec() const-> Vector<T, DIM> const&{  return _vec_part;  }


private:
	Rotation<T, DIM> _rotation_part;
	Vector<T, DIM> _vec_part;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#include "Sigma\Trader\Trader.hpp"


template<class T>
class v3d::Rotation<T, 2>
{
private:
	static_assert(trait::is_real_v<T>);

public:
	using scalar_type = T;

	Rotation( T const angle = T(0) ) : _angle(angle){}
	Rotation(OrthogonalMat<T, 2> const &m) : _angle( _from_OrthogonalMat(m) ){}

	template
	<	class Q
	,	class
		=	std::enable_if_t
			<	std::is_constructible_v<Rotation, Q&&> && !std::is_same_v< std::decay_t<Q>, Rotation > 
			>
	>
	auto operator=(Q &&q)-> Rotation&{  return *this = Rotation( std::forward<Q>(q) );  }

	auto cortho_mat() const-> OrthogonalMat<T, 2>{  return _to_OrthogonalMat(_angle);  }
	decltype(auto) ortho_mat() const{  return cortho_mat();  }

	decltype(auto) ortho_mat()
	{
		return
		sgm::make_Trader
		(	_to_OrthogonalMat(_angle), [&a = _angle](OrthogonalMat<T, 2> const &m){  a = _from_OrthogonalMat(m);  }
		);
	}


	auto angle() const-> T{  return _angle;  }

	auto operator()(Vector<T, 2> const &v) const-> Vector<T, 2>{  return ortho_mat()*v;  }

	template<class...ARGS>
	auto rotate(ARGS&&...args) const-> Rotation<T, 2>
	{
		return Rotation<T, 2>( std::forward<ARGS>(args)... )._angle + _angle;
	}

private:
	T _angle;

	static auto _to_OrthogonalMat(T const angle)
	{
		T const c = std::cos(angle), s = std::sin(angle);

		return
		Skipped< OrthogonalMat<T, 2> >
		(	Matrix<T, 2, 2>
			{	c, -s
			,	s, c
			}
		);
	}

	static auto _from_OrthogonalMat(OrthogonalMat<T, 2> const &m)-> T
	{
		auto const sign = T( m(1, 0) >= 0 ? 1 : -1 );

		return sign*std::acos( m(0, 0) );
	}

};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#include "..\Quaternion\Quaternion.hpp"


template<class T>
class v3d::Rotation<T, 3>
{
private:
	static_assert(trait::is_real_v<T>);

	using _UQtn_t = UnitQuaternion<T>;
	using _OrthoMat_t = OrthogonalMat<T, 3>;

public:
	using scalar_type = T;
	
	Rotation() : _uqtn(){}
	Rotation(T const alpha, T const beta, T const gamma) : _uqtn( _from_Euler_angles(alpha, beta, gamma) ){}
	Rotation(UnitVec<T, 3> const &u, T const theta) : _uqtn( _from_Spin(u, theta) ){}
	
	template<   class Q, class = std::enable_if_t<  !std::is_same_v< std::decay_t<Q>, Rotation >  >   >
	Rotation(Q &&q) noexcept(std::is_rvalue_reference_v<Q&&>)
	:	_uqtn
		(	[&q]
			{
				if constexpr(trait::is_Quaternion_v<Q>)
					return std::forward<Q>(q);
				else if constexpr(trait::is_v3dMat_v<Q>)
					return _from_OrthogonalMat(q);
				else if constexpr(trait::is_TempMat_v<Q>)
					return _from_OrthogonalMat(  Eval( std::forward<Q>(q) )  );
				else SGM_COMPILE_FAILED(no method to construct v3d::Rotation .);
			}()
		)
	{}


	template
	<	class Q
	,	class 
		=	std::enable_if_t
			<	std::is_convertible_v<Q&&, Rotation>  
			&&	!std::is_same_v< std::decay_t<Q>, Rotation >
			>
	>
	auto operator=(Q &&q) noexcept(std::is_rvalue_reference_v<Q&&>)-> Rotation&
	{
		return *this = Rotation( std::forward<Q>(q) );
	}


	auto cunit_qtn() const-> _UQtn_t const&{  return _uqtn;  }
	decltype(auto) unit_qtn() const{  return cunit_qtn();  }
	auto unit_qtn()-> _UQtn_t&{  return _uqtn;  }

	auto cortho_mat() const-> _OrthoMat_t{  return _to_OrthogonamMat(_uqtn);  }
	decltype(auto) ortho_mat() const{  return cortho_mat();  }

	decltype(auto) ortho_mat()
	{
		return 
		sgm::make_Trader
		(	_to_OrthogonamMat(_uqtn), [&q = _uqtn](_OrthoMat_t const &m){  q = _from_OrthogonalMat(m);  }
		);
	}

	auto cspin_vec() const-> Vector<T, 3>{  return _to_Spin(_uqtn);  }
	decltype(auto) spin_vec() const{  return cspin_vec();  }

	decltype(auto) spin_vec()
	{
		return
		sgm::make_Trader
		(	_to_Spin(_uqtn)
		,	[&q = _uqtn](Vector<T, 3> const &v)
			{  
				T const norm = v.norm();

				q = _from_Spin( Skipped< UnitVec<T, 3> >(v/norm), norm );
			} 
		);
	}


	auto operator()(Vector<T, 3> const &v) const-> Vector<T, 3>
	{
		return ( _uqtn*Quaternion<T>(0, v)*_uqtn.inv() ).v();
	}


	template<class...ARGS>
	auto rotate(ARGS&&...args) const
	{
		_UQtn_t const q = Rotation<T, 3>( std::forward<ARGS>(args)... ).unit_qtn();

		return Rotation(q*_uqtn);
	}


private:
	_UQtn_t _uqtn;


	static auto _from_Spin(UnitVec<T, 3> const &u, T const theta)
	{
		T const h = T(.5)*theta;

		return Skipped<_UQtn_t>( std::cos(h), std::sin(h)*u );
	}

	static auto _to_Spin(_UQtn_t const &q)-> Vector<T, 3>
	{
		auto clamp_f = [](T const t, T const low, T const high){  return t < low ? low : t > high ? high : t;  };
		T const st = clamp_f( q.v().norm(), T(-1), T(1) );

		return T(2)*std::asin(st)*q.v()/st;		
	}


	/**	Rotating order
	*	rotate along x-axis by alpha radian first,
	*	rotate along y-axis be beta radian next, 
	*	and rotate by gamma radian along z-axis.
	*	In other words, by rotation matrix multiplications, Rz(gamma) * Ry(beta) * Rx(alpha) for column vector .
	*/
	static auto _from_Euler_angles(T const alpha, T const beta, T const gamma)
	{
		T const
			ha = T(.5)*alpha, hb = T(.5)*beta, hg = T(.5)*gamma,
			ca = std::cos(ha), cb = std::cos(hb), cg = std::cos(hg),
			sa = std::sin(ha), sb = std::sin(hb), sg = std::sin(hg);

		return
		Skipped<_UQtn_t>
		(	ca*cb*cg + sa*sb*sg
		,	sa*cb*cg - ca*sb*sg
		,	ca*sb*cg + sa*cb*sg
		,	ca*cb*sg - sa*sb*cg
		);
	}

	static auto _from_OrthogonalMat(_OrthoMat_t const &m)
	{
		T const _2w = std::sqrt( T(1) + m(0, 0) + m(1, 1) + m(2, 2) );
		Vector<T, 3> const _2v = Vector<T, 3>{m(2, 1) - m(1, 2), m(0, 2) - m(2, 0), m(1, 0) - m(0, 1)} / _2w;

		return Skipped<_UQtn_t>( T(.5)*Quaternion<T>(_2w, _2v) );
	}

	static auto _to_OrthogonamMat(_UQtn_t const &q)
	{
		T const &w = q.w();
		Vector<T, 3> const &v = q.v();

		return
		Skipped<_OrthoMat_t>( (w*w - v.sqr_norm())*OrthogonalMat<T, 3>::identity() + 2*w*v.skew() + 2*v.dyadic(v) );
	}

};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#endif // end of #ifndef _V3D_AFFINE_