/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _S3D_QUATERNION_
#define _S3D_QUATERNION_


#include "../Hamilton/Hamilton.hpp"


namespace s3d
{
	
	template<class T>  
	class Quaternion;

	template<class T>  
	class UnitQuaternion;

}


namespace s3d::trait
{
	
	template<class T>  
	struct is_Quaternion;
	

	SGM_USER_DEFINED_TYPE_CHECK
	(	class T
	,	UnitQuaternion, <T>
	);

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
struct s3d::trait::is_Quaternion
{
private:
	template<class Q> /* Declaration Only */  
	static auto _calc(Quaternion<Q>) noexcept-> True_t;
	
	template<class Q> /* Declaration Only */  
	static auto _calc(UnitQuaternion<Q>) noexcept-> True_t;
	
	template<class...> /* Declaration Only */  
	static auto _calc(...) noexcept-> False_t;


public:
	static bool constexpr value = decltype( _calc(Mock<T>()) )::value;

	using type = Boolean<value>;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class s3d::Quaternion
{
public:
	using scalar_type = T;


	Quaternion(T const w = 0, T const x = 0, T const y = 0, T const z = 0) 
	:	_w(w), _v(Vector<T, 3>{x, y, z}){}
	
	Quaternion(T const w, Vector<T, 3> const& v) : _w(w), _v(v){}
	Quaternion(T const w, Vector<T, 3>&& v) noexcept : _w(w), _v( Move(v) ){}
	Quaternion(Vector<T, 3> const& v) : Quaternion(0, v){}
	Quaternion(Vector<T, 3>&& v) noexcept : Quaternion( 0, Move(v) ){}

	template
	<	class S, class = Enable_if_t< !is_Same<S, T>::value && trait::is_real<S>::value >  
	>
	Quaternion(Quaternion<S> const& q) : Quaternion( _cast_impl(q) ){}

	template
	<	class S, class = Enable_if_t< !is_Same<S, T>::value && trait::is_real<S>::value >  
	>
	Quaternion(Quaternion<S>&& q) : Quaternion(  _cast_impl( Move(q) )  ){}


	template<   class Q, class = Enable_if_t<  !is_Same< Quaternion, Decay_t<Q> >::value  >   >
	auto operator=(Q&& q)-> Quaternion&{  return *this = _cast<Q>(q);  }


	auto w() const-> T{  return _w;  }		auto w()-> T&{  return _w;  }
	auto x() const-> T{  return _v(0);  }	auto x()-> T&{  return _v(0);  }
	auto y() const-> T{  return _v(1);  }	auto y()-> T&{  return _v(1);  }
	auto z() const-> T{  return _v(2);  }	auto z()-> T&{  return _v(2);  }

	auto v() const-> Vector<T, 3> const&{  return _v;  }
	auto v()-> Vector<T, 3>&{  return _v;  }

	auto operator+() const-> Quaternion const&{  return *this;  }
	auto operator-() const{  return Quaternion(-w(), -v());  }

	auto sqr_norm() const{  return w()*w() + v().dot(v());  }
	auto norm() const{  return std::sqrt(sqr_norm());  }
	auto normalized() const{  return *this / norm();  }
	auto normalize()-> Quaternion&{  return *this = normalized();  }

	auto conjugate() const{  return Quaternion(w(), -v());  }
	auto inv() const{  return conjugate() / sqr_norm();  }

	template<class Q>
	auto operator+(Q&& q_) const
	{
		auto const q = _cast<Q>(q_);

		return Quaternion(w() + q.w(), v() + q.v());
	}

	template<class Q>
	auto operator-(Q&& q) const{  return *this + (-q);  }

	template<class Q>
	auto operator*(Q&& q_) const
	{
		if constexpr(trait::is_real<Q>::value)
			return Quaternion(w()*q_, v()*q_);
		else
		{
			auto const q = _cast<Q>(q_);

			return 
			Quaternion( w()*q.w() - v().dot(q.v()), w()*q.v() + q.w()*v() + v().cross(q.v()) );
		}
	}

	auto operator/(T const s) const
	{
		auto const r = w()/s;

		assert( !std::isnan(r) );

		return Quaternion(r, v()/s);
	}

	template<class Q>
	auto operator+=(Q&& q)-> Quaternion&{  return *this = *this + _cast<Q>(q);  }

	template<class Q>
	auto operator-=(Q&& q)-> Quaternion&{  return *this = *this - _cast<Q>(q);  }

	template<class Q>
	auto operator*=(Q&& q)-> Quaternion&{  return *this = *this * _cast<Q>(q);  }

	auto operator/=(T const s)-> Quaternion&{  return *this = *this / s;  }


private:
	T _w;
	Vector<T, 3> _v;


	template<class Q>
	static decltype(auto) _cast_impl(Q&& q)
	{
		if constexpr(is_Same< Quaternion, Decay_t<Q> >::value)
			return Forward<Q>(q);
		else if constexpr(trait::is_Quaternion<Q>::value)
			return Quaternion(q.w(), q.x(), q.y(), q.z());
		else if constexpr(trait::Has_Matrix_interface<Q>::value)
		{
			assert( Has_Vector_interface(q) && q.size() == 3 );

			return Quaternion(  Vector<T, 3>( Forward<Q>(q) )  );
		}
		else if constexpr(is_Convertible<Q, double>::value)
			return Quaternion( static_cast<T>(q) );
		else
			return (Quaternion)Compile_Fails(); // no suitable method was found.
	}

	template<class Q>
	static decltype(auto) _cast(Referenceless_t<Q>& q){  return _cast_impl(q);  }

	template<class Q>
	static decltype(auto) _cast(Referenceless_t<Q>&& q){  return _cast_impl( Move(q) );  }
};


namespace s3d
{
	
	template<class T>
	using int_to_float_t = Selective_t< std::numeric_limits<T>::is_integer, float, T >;


	template
	<	class T
	,	class 
		=	Enable_if_t< std::numeric_limits<T>::is_integer || std::numeric_limits<T>::is_iec559 >
	>
	Quaternion(T const = 0, T const = 0, T const = 0, T const = 0)
	->	Quaternion< int_to_float_t<T> >;

	template<class T>
	Quaternion(T const, Vector<T, 3> const&)-> Quaternion< int_to_float_t<T> >;

	template<class T>
	Quaternion(T const, Vector<T, 3>&&) noexcept-> Quaternion< int_to_float_t<T> >;

	template<  class T, class = Enable_if_t< trait::is_StrictVec<T>::value >  >
	Quaternion(T&&)-> Quaternion< typename Decay_t<T>::value_type >;

}


template<  class S, class T, class = sgm::Enable_if_t< sgm::is_Convertible<S, double>::value >  >
static auto operator*(S const s, s3d::Quaternion<T> const& q){  return q * s;  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class s3d::UnitQuaternion
{
private:
	using _Qtn = Quaternion<T>;

	template<class U>
	using _qtn_cast_t = Selective_t< is_Convertible<U, double>::value, T, U&& >;


public:
	using scalar_type = T;

	UnitQuaternion() : _qtn(1){}

	template
	<	class...ARGS
	,	class = Enable_if_t< Has_Operator_New<_Qtn, ARGS&&...>::value > 
	>
	UnitQuaternion(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	:	_qtn(  _Qtn( _qtn_cast_t<ARGS>(args)... ).normalized()  ){}


	template<  class U, class = Enable_if_t< trait::is_UnitVec<U>::value >  >
	UnitQuaternion(U&& u) : _qtn( 0, u(0), u(1), u(2) ){}

	template
	<	class Q, class = Enable_if_t<  !is_Same< UnitQuaternion, Decay_t<Q> >::value  >  
	>
	auto operator=(Q&& q) noexcept(is_Rvalue_Reference<Q&&>::value)-> UnitQuaternion&
	{
		if constexpr(trait::is_UnitQuaternion<Q>::value)
			_qtn = Move_if< is_Rvalue_Reference<Q&&>::value >(q.qtn());
		else
			_qtn = Forward<Q>(q),  _qtn.normalize();
		
		return *this;
	}

	auto qtn() const-> _Qtn const&{  return _qtn;  }
	operator _Qtn const&() const{  return qtn();  }

	decltype(auto) w() const{  return qtn().w();  }
	decltype(auto) v() const{  return qtn().v();  }
	decltype(auto) x() const{  return qtn().x();  }
	decltype(auto) y() const{  return qtn().y();  }
	decltype(auto) z() const{  return qtn().z();  }

	auto operator-() const{  return Skipped<UnitQuaternion>(-qtn());  }
	auto operator+() const-> UnitQuaternion const&{  return *this;  }


	template<class Q>  
	decltype(auto) operator*(Q&& q) const{  return qtn() * Forward<Q>(q);  }
	
	template<class Q>  
	decltype(auto) operator*=(Q&& q){  return *this = *this * Forward<Q>(q);  }

	decltype(auto) operator/(T const s) const{  return qtn() / s;  }
	decltype(auto) operator/=(T const s) const{  return *this = *this / s;  }

	auto inv() const{  return conjugate();  }
	auto conjugate() const{  return Skipped<UnitQuaternion>(qtn().conjugate());  }

	auto sqr_norm() const-> T{  return 1;  }
	auto norm() const-> T{  return 1;  }
	auto normalized() const{  return *this;  }
	auto normalize()-> UnitQuaternion&{  return *this;  }


	static auto Slerp(UnitQuaternion const& uq0, UnitQuaternion const& uq1, T const t)
	->	UnitQuaternion;


private:
	_Qtn _qtn;


	template<class res_t, class...ARGS, class>
	friend auto s3d::Skipped(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value);

	template<class...ARGS>
	UnitQuaternion(_ExemptionTag, ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value) 
	:	_qtn( Forward<ARGS>(args)... ){}
};


namespace s3d
{
	
	template
	<	class...ARGS
	,	class 
		=	Enable_if_t
			<	sizeof...(ARGS) != 1 
			||	!trait::is_StrictVec< First_t<ARGS...> >::value  
			>
	,	class S = typename decltype( Quaternion(Mock<ARGS&&>()...) )::scalar_type 
	>
	UnitQuaternion(ARGS&&...)-> UnitQuaternion<S>;

	template<  class U, class = Enable_if_t< trait::is_StrictVec<U>::value >  >
	UnitQuaternion(U&&)
	->	UnitQuaternion<  Decay_t< decltype( Mock<U>()(0) ) >  >;

}


template<  class S, class T, class = sgm::Enable_if_t< sgm::is_Convertible<S, double>::value >  >
static auto operator*(S const s, s3d::UnitQuaternion<T> const& q){  return q*s;  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
auto s3d::UnitQuaternion<T>::Slerp
(	UnitQuaternion const& uq0, UnitQuaternion const& uq1, T const t
)->	UnitQuaternion
{
	auto const cos_half_theta = (uq1*uq0.inv()).w();
	UnitQuaternion const iuq0_uq1 = uq0.inv()*(cos_half_theta > 0 ? uq1 : -uq1);

	if( Are_almost_same<T>(iuq0_uq1.w(), 1) )
		return uq0;

	UnitVec<T, 3> const v = iuq0_uq1.v();
	T const half_theta = std::acos(  std::clamp( iuq0_uq1.w(), T(-1), T(1) )  );
	T const ct = std::cos(t*half_theta),  st = std::sin(t*half_theta);

	return uq0 * UnitQuaternion(ct, st*v);
}


#endif //  end of #ifndef _S3D_QUATERNION_