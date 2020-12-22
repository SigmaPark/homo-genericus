#pragma once


#ifndef _SGM_MXI_QUATERNION_
#define _SGM_MXI_QUATERNION_


#include "..\Matrix_interface\Matrix.hpp"

#ifndef _READ_WRITE_MEMFUNC
	#define _READ_WRITE_MEMFUNC(signature, ...)		\
		signature const __VA_ARGS__		\
		signature __VA_ARGS__
#else
	#error _READ_WRITE_MEMFUNC was already defined somewhere else.
#endif
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm::mxi
{
	
	template<class elem_t = float>
	class Quaternion;


	template<class>
	struct _is_Quaternion;

	template<class T>
	static bool constexpr is_Quaternion_v = _is_Quaternion< std::decay_t<T> >::value;


	template<class elem_t = float>
	class UnitQuaternion;


	template<class>
	struct _is_UnitQuaternion;

	template<class T>
	static bool constexpr is_UnitQuaternion_v = _is_UnitQuaternion< std::decay_t<T> >::value;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
class sgm::mxi::Quaternion
{
	using _raw_t = std::decay_t<T>;

	static_assert(std::is_arithmetic_v< _raw_t >, "Quaternion elememnt should be real.");


public:
	using elem_t = std::conditional_t< std::is_integral_v<_raw_t>, float, _raw_t >;

	Quaternion(elem_t w = 0, elem_t x = 0, elem_t y = 0, elem_t z = 0)
	:	_w(w), _v(Vector<elem_t, 3>{x, y, z})
	{}

	Quaternion(elem_t w, Vector<elem_t, 3> const& v) : _w(w), _v(v){}
	Quaternion(elem_t w, Vector<elem_t, 3>&& v) : _w(w), _v( std::move(v) ){}


	template
	<	class Q
	,	class 
		=	std::enable_if_t
			<	!std::is_same_v< Quaternion, std::decay_t<Q> > && is_Quaternion_v<Q>  
			>
	>
	Quaternion(Q&& q) : Quaternion( _cast<Q>(q) ){}

	
	template
	<	class Q, class = std::enable_if_t<  !std::is_same_v< Quaternion, std::decay_t<Q> >  >
	>
	auto operator=(Q&& q)-> Quaternion&{  return *this = _cast<Q>(q);  }


	auto w() const-> elem_t{  return _w;  }			auto w()-> elem_t&{  return _w;  }
	auto x() const-> elem_t{  return _v(0);  }		auto x()-> elem_t&{  return _v(0);  }
	auto y() const-> elem_t{  return _v(1);  }		auto y()-> elem_t&{  return _v(1);  }
	auto z() const-> elem_t{  return _v(2);  }		auto z()-> elem_t&{  return _v(2);  }
	
	auto v() const-> Vector<elem_t, 3> const&{  return _v;  }
	auto v()-> Vector<elem_t, 3>&{  return _v;  }


	auto operator-() const{  return Quaternion(-w(), -v());  }

	auto sqrNorm() const{  return pow(w(), 2) + v().dot(v());  }
	auto norm() const{  return sqrt(sqrNorm());  }

	auto normalized() const{  return *this / norm();  }

	auto conjugated() const{  return Quaternion(w(), -v());  }
	auto inversed() const{  return conjugated() / sqrNorm();  }


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
		if constexpr(std::is_arithmetic_v< std::decay_t<Q> >)
			return Quaternion(w()*q_, v()*q_);
		else
		{
			auto const q = _cast<Q>(q_);			

			return
			Quaternion( w()*q.w() - v().dot(q.v()), w()*q.v() + q.w()*v() + v().cross(q.v()) );
		}

	}

	auto operator/(elem_t s) const
	{
		auto constexpr epsilon = elem_t(1e-7);

		assert( abs(s) > epsilon && L"ill condition : divided by 0\n");

		return Quaternion(w()/s, v()/s);
	}


	auto normalize()-> Quaternion&{  return *this = normalized();  }
	auto conjugate()-> Quaternion&{  return *this = conjugated();  }
	auto inverse()-> Quaternion&{  return *this = inversed();  }

	template<class Q>
	auto operator+=(Q&& q)-> Quaternion&{  return *this = *this + _cast<Q>(q);  }

	template<class Q>
	auto operator-=(Q&& q)-> Quaternion&{  return *this = *this - _cast<Q>(q);  }

	template<class Q>
	auto operator*=(Q&& q)-> Quaternion&{  return *this = *this * _cast<Q>(q);  }

	auto operator/=(elem_t s)-> Quaternion&{  return *this = *this / s;  }


private:
	elem_t _w;
	Vector<elem_t, 3> _v;


	template<class Q>
	static decltype(auto) _cast_Helper(Q&& q)
	{
		if constexpr(std::is_same_v< Quaternion, std::decay_t<Q> >)
			return std::forward<Q>(q);
		else if constexpr(is_Quaternion_v<Q>)
			return Quaternion(q.w(), q.x(), q.y(), q.z());
		else if constexpr(std::is_arithmetic_v< std::decay_t<Q> >)
			return Quaternion( static_cast<elem_t>(q) );
		else SGM_COMPILE_FAILED(No suitable method was found.);
	}


	template<class Q>
	static decltype(auto) _cast(std::remove_reference_t<Q>& q){  return _cast_Helper(q);  }

	template<class Q>
	static decltype(auto) _cast(std::remove_reference_t<Q>&& q)
	{
		return _cast_Helper( std::move(q) );
	}

};


template<  class S, class T, class = std::enable_if_t< std::is_arithmetic_v<S> >  >
static auto operator*(S s, sgm::mxi::Quaternion<T> const& q){  return q * s;  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::mxi::UnitQuaternion
{
	using Qtn_t = Quaternion<T>;

public:
	using elem_t = typename Qtn_t::elem_t;


	UnitQuaternion() : _qtn(1){}
	UnitQuaternion(UnitVector<elem_t, 3> const& u) : _qtn(0, u){}
	UnitQuaternion(UnitVector<elem_t, 3>&& u) : _qtn( 0, std::move(u) ){}

	template
	<	class...ARGS
	,	class 
		=	std::enable_if_t< sizeof...(ARGS) != 0 && std::is_constructible_v<Qtn_t, ARGS...> >
	>
	UnitQuaternion(ARGS&&...args) : _qtn(  Qtn_t( std::forward<ARGS>(args...) ).normalize()  ){}


	template
	<	class Q
	,	class = std::enable_if_t<  !std::is_same_v< UnitQuaternion, std::decay_t<Q> >  >
	>
	auto operator=(Q&& q)-> UnitQuaternion&
	{
		if constexpr(is_UnitQuaternion_v<Q>)
			_qtn = q.qtn();
		else
			_qtn = std::forward<Q>(q);
		
		_qtn.normalize();

		return *this;
	}


	auto qtn() const-> Qtn_t const&{  return _qtn;  }

	decltype(auto) w() const{  return _qtn.w();  }
	decltype(auto) v() const{  return _qtn.v();  }
	decltype(auto) x() const{  return _qtn.x();  }
	decltype(auto) y() const{  return _qtn.y();  }
	decltype(auto) z() const{  return _qtn.z();  }


	auto operator-() const
	{
		auto cpy = *this;

		cpy._qtn *= -1;

		return cpy;
	}

	
	auto conjugated() const
	{
		auto cpy = *this;

		cpy._qtn.conjugate();

		return cpy;
	}

	auto conjugate()-> UnitQuaternion&
	{
		_qtn.conjugate();

		return *this;
	}

	auto inversed() const{  return conjugated();  }
	decltype(auto) inverse(){  return conjugate();  }


	template<class Q> 
	decltype(auto) operator*(Q&& q) const{  return qtn() * std::forward<Q>(q);  }

	decltype(auto) operator/(elem_t s) const{  return qtn() / s;  }


private:
	Qtn_t _qtn;
};


template<  class S, class T, class = std::enable_if_t< std::is_arithmetic_v<S> >  >
static auto operator*(S s, sgm::mxi::UnitQuaternion<T> const& q){  return q * s;  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class>
struct sgm::mxi::_is_Quaternion : False_t{};

template<class T>
struct sgm::mxi::_is_Quaternion< sgm::mxi::Quaternion<T> > : True_t{};


template<class>
struct sgm::mxi::_is_UnitQuaternion : False_t{};

template<class T>
struct sgm::mxi::_is_UnitQuaternion< sgm::mxi::UnitQuaternion<T> > : True_t{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#undef _READ_WRITE_MEMFUNC

#endif