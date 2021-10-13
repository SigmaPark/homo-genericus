#pragma once

#ifndef _V3D_QUATERNION_
#define _V3D_QUATERNION_

#include "..\Hamilton\Hamilton.hpp"

namespace v3d
{
	
	template<class T>  class Quaternion;

	template<class T>  class UnitQuaternion;
	template<class T>  using UnitQtn = UnitQuaternion<T>;

}


namespace v3d::trait
{
	
	template<class T>  struct is_Quaternion;
	template<class T>  static bool constexpr is_Quaternion_v = is_Quaternion<T>::value;

	SGM_USER_DEFINED_TYPE_CHECK14(_, UnitQuaternion, class T, T);

}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


template<class T>
struct v3d::trait::is_Quaternion
{
private:
	template<class Q> /* Declaration Only */  static sgm::True_t _calc(Quaternion<Q>) noexcept;
	template<class Q> /* Declaration Only */  static sgm::True_t _calc(UnitQuaternion<Q>) noexcept;
	template<class...> /* Declaration Only */  static sgm::False_t _calc(...) noexcept;

public:
	static bool constexpr value = decltype( _calc(std::declval<T>()) )::value;

	using type = sgm::Boolean_type<value>;
};


template<class T>
class v3d::Quaternion
{
private:
	static_assert(trait::is_real_v<T>);

public:
	using scalar_type = T;

	Quaternion(T const w = 0, T const x = 0, T const y = 0, T const z = 0) : _w(w), _v(Vector<T, 3>{x, y, z}){}
	Quaternion(T const w, Vector<T, 3> const &v) : _w(w), _v(v){}
	Quaternion(T const w, Vector<T, 3> &&v) noexcept : _w(w), _v( std::move(v) ){}
	Quaternion(Vector<T, 3> const &v) : Quaternion(0, v){}
	Quaternion(Vector<T, 3> &&v) noexcept : Quaternion( 0, std::move(v) ){}


	template
	<	class Q
	,	class = std::enable_if_t<  !std::is_same_v< Quaternion, std::decay_t<Q> > && trait::is_Quaternion_v<Q>  >
	>
	Quaternion(Q &&q) : Quaternion( _cast<Q>(q) ){}

	template<   class Q, class = std::enable_if_t<  !std::is_same_v< Quaternion, std::decay_t<Q> >  >   >
	auto operator=(Q &&q)-> Quaternion&{  return *this = _cast<Q>(q);  }


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
	auto operator+(Q &&q_) const
	{
		auto const q = _cast<Q>(q_);

		return Quaternion(w() + q.w(), v() + q.v());
	}

	template<class Q>
	auto operator-(Q &&q) const{  return *this + (-q);  }

	template<class Q>
	auto operator*(Q &&q_) const
	{
		if constexpr(trait::is_real_v<Q>)
			return Quaternion(w()*q_, v()*q_);
		else
		{
			auto const q = _cast<Q>(q_);

			return Quaternion( w()*q.w() - v().dot(q.v()), w()*q.v() + q.w()*v() + v().cross(q.v()) );
		}
	}

	auto operator/(T const s) const
	{
		auto const r = w()/s;

		assert( !std::isnan(r) );

		return Quaternion(r, v()/s);
	}

	template<class Q>
	auto operator+=(Q &&q)-> Quaternion&{  return *this = *this + _cast<Q>(q);  }

	template<class Q>
	auto operator-=(Q &&q)-> Quaternion&{  return *this = *this - _cast<Q>(q);  }

	template<class Q>
	auto operator*=(Q &&q)-> Quaternion&{  return *this = *this * _cast<Q>(q);  }

	auto operator/=(T const s)-> Quaternion&{  return *this = *this / s;  }


private:
	T _w;
	Vector<T, 3> _v;


	template<class Q>
	static decltype(auto) _cast_impl(Q &&q)
	{
		if constexpr(std::is_same_v< Quaternion, std::decay_t<Q> >)
			return std::forward<Q>(q);
		else if constexpr(trait::is_Quaternion_v<Q>)
			return Quaternion(q.w(), q.x(), q.y(), q.z());
		else if constexpr(trait::is_v3dMat_v<Q>)
		{
			assert( Has_Vector_interface(q) && q.size() == 3 );

			return Quaternion(  Vector<T, 3>( std::forward<Q>(q) )  );
		}
		else if constexpr(std::is_convertible_v<Q, double>)
			return Quaternion( static_cast<T>(q) );
		else SGM_COMPILE_FAILED(no suitable method was found.);
	}

	template<class Q>
	static decltype(auto) _cast(std::remove_reference_t<Q> &q){  return _cast_impl(q);  }

	template<class Q>
	static decltype(auto) _cast(std::remove_reference_t<Q> &&q){  return _cast_impl( std::move(q) );  }
};


template<  class S, class T, class = std::enable_if_t< std::is_convertible_v<S, double> >  >
static auto operator*(S const s, v3d::Quaternion<T> const &q){  return q * s;  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class T>
class v3d::UnitQuaternion
{
private:
	using _Qtn = Quaternion<T>;

	template<class U>
	using _qtn_cast_t = std::conditional_t< std::is_convertible_v<U, double>, T, U&& >;

public:
	using scalar_type = T;

	UnitQuaternion() : _qtn(1){}

	template
	<	class...ARGS
	,	class = std::enable_if_t< std::is_constructible_v<_Qtn, ARGS&&...> > 
	>
	UnitQuaternion(ARGS&&...args) noexcept(sgm::Check_All<std::is_rvalue_reference>::for_any<ARGS&&...>::value)
	:	_qtn(  _Qtn( _qtn_cast_t<ARGS>(args)... ).normalized()  ){}


	template<  class U, class = std::enable_if_t< trait::is_UnitVec_v<U> >  >
	UnitQuaternion(U &&u) : _qtn( 0, u(0), u(1), u(2) ){}

	template<   class Q, class = std::enable_if_t<  !std::is_same_v< UnitQuaternion, std::decay<Q> >  >   >
	auto operator=(Q &&q) noexcept(std::is_rvalue_reference_v<Q&&>)-> UnitQuaternion&
	{
		if constexpr(trait::is_UnitQuaternion_v<Q>)
			_qtn = sgm::Move_if< std::is_rvalue_reference_v<Q&&> >::cast(q.qtn());
		else
			_qtn = std::forward<Q>(q),  _qtn.normalize();
		
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


	template<class Q>  decltype(auto) operator*(Q &&q) const{  return qtn() * std::forward<Q>(q);  }
	template<class Q>  decltype(auto) operator*=(Q &&q){  return *this = *this * std::forward<Q>(q);  }

	decltype(auto) operator/(T const s) const{  return qtn() / s;  }
	decltype(auto) operator/=(T const s) const{  return *this = *this / s;  }

	auto inv() const{  return conjugate();  }
	auto conjugate() const{  return Skipped<UnitQuaternion>(qtn().conjugate());  }

	auto sqr_norm() const-> T{  return 1;  }
	auto norm() const-> T{  return 1;  }
	auto normalized() const{  return *this;  }
	auto normalize()-> UnitQuaternion&{  return *this;  }


private:
	_Qtn _qtn;


	template<class res_t, class...ARGS, class>
	friend auto v3d::Skipped(ARGS&&...args) 
	noexcept(sgm::Check_All<std::is_rvalue_reference>::for_any<ARGS&&...>::value);

	template<class...ARGS>
	UnitQuaternion(_ExemptionTag, ARGS&&...args)
	noexcept(sgm::Check_All<std::is_rvalue_reference>::for_any<ARGS&&...>::value) 
	:	_qtn( std::forward<ARGS>(args)... ){}
};


template<  class S, class T, class = std::enable_if_t< std::is_convertible_v<S, double> >  >
static auto operator*(S const s, v3d::UnitQuaternion<T> const &q){  return q*s;  }


#endif //  end of #ifndef _V3D_QUATERNION_