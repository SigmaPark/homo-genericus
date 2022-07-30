/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _S3D_EUCLID_
#define _S3D_EUCLID_


#include "../Decomposition/Decomposition.hpp"
#include "SGM/Array/Array.hpp"
#include "SGM/Abbreviable/Maybe.hpp"


namespace s3d
{
	
	template<class T, size_t DIM>  
	class Oriented;


	template<class T, size_t DIM>  
	class Plane;
	
	template<class T, size_t DIM>  
	class Line;


	template<class S, class D>  
	static auto Projection(S&& src, D&& des);
	

	template<class S, class D>  
	static auto sqrDistance(S&& src, D&& des);
	
	template<class S, class D>  
	static auto Distance(S&& src, D&& des);
	

	template<class S, class D>  
	static auto intersection(S&& src, D&& des);


	struct Direction;


	template<class T>  
	static decltype(auto) Position(T&&);


	template<class T>  
	struct _Position_Helper;

}


namespace s3d::trait
{
	
	SGM_USER_DEFINED_TYPE_CHECK
	(	SGM_MACROPACK(class T, size_t D)
	,	Oriented, <T, D>
	);


	SGM_USER_DEFINED_TYPE_CHECK
	(	SGM_MACROPACK(class T, size_t D)
	,	Plane, <T, D> 
	);


	SGM_USER_DEFINED_TYPE_CHECK
	(	SGM_MACROPACK(class T, size_t D)
	,	Line, <T, D> 
	);


	SGM_HAS_MEMBER(DIMENSION);

	
	template<class ORN>
	struct Dimension;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class ORN>
struct s3d::trait::Dimension : Unconstructible
{
private:
	template<class Q>
	static size_t constexpr _calc()
	{	
		if constexpr(Has_Member_DIMENSION<Q>::value)
			return Q::DIMENSION;
		else if constexpr(is_StrictVec<Q>::value)
			return Q::STT_ROW_SIZE * Q::STT_COL_SIZE;
		else
			return Compile_Fails();
	}

public:
	static size_t constexpr value = _calc< Decay_t<ORN> >();
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, std::size_t DIM>
class s3d::Oriented
{
public:
	using value_type = T;
	static size_t constexpr DIMENSION = DIM;
	using position_t = Vector<T, DIM>;

	static_assert( trait::is_real<T>::value && DIM > 1 );
};


template<class T, std::size_t DIM>
class s3d::Plane : public Oriented<T, DIM>
{
public:
	Plane() : _position(Vector<T, DIM>::Zero()), _normal(){}

	template
	<	class VEC, class UVEC
	,	class
		=	Enable_if_t
			<	is_Convertible< VEC, Vector<T, DIM> >::value 
			&&	is_Convertible< UVEC, UnitVec<T, DIM> >::value
			>
	>
	Plane(VEC&& pos, UVEC&& nml) noexcept(Aleph_Check<VEC&&, UVEC&&>::value)
	:	_position( Forward<VEC>(pos) ), _normal( Forward<UVEC>(nml) ){}

	auto position() const-> Vector<T, DIM> const&{  return _position;  }
	auto position()-> Vector<T, DIM>&{  return _position;  }
	auto normal() const-> UnitVec<T, DIM> const&{  return _normal;  }
	auto normal()-> UnitVec<T, DIM>&{  return _normal;  }

	auto signed_dist_to(Vector<T, DIM> const& pos) const
	->	T{  return normal().dot(pos - position());  }


private:
	Vector<T, DIM> _position;
	UnitVec<T, DIM> _normal;
};


template<class T, std::size_t DIM>
class s3d::Line : public Oriented<T, DIM>
{
public:
	Line() : _position(Vector<T, DIM>::Zero()), _tangent(){}

	template
	<	class VEC, class UVEC
	,	class
		=	Enable_if_t
			<	is_Convertible< VEC, Vector<T, DIM> >::value 
			&&	is_Convertible< UVEC, UnitVec<T, DIM> >::value
			>
	>
	Line(VEC&& pos, UVEC&& tgt) noexcept(Aleph_Check<VEC&&, UVEC&&>::value)
	:	_position( Forward<VEC>(pos) ), _tangent( Forward<UVEC>(tgt) ){}

	auto position() const-> Vector<T, DIM> const&{  return _position;  }
	auto position()-> Vector<T, DIM>&{  return _position;  }
	auto tangent() const-> UnitVec<T, DIM> const&{  return _tangent;  }
	auto tangent()-> UnitVec<T, DIM>&{  return _tangent;  }


private:
	Vector<T, DIM> _position;
	UnitVec<T, DIM> _tangent;	
};


namespace s3d
{

	template
	<	class V, class U
	,	class 
		=	Enable_if_t< trait::is_FixedSizeMat<V>::value && trait::is_FixedSizeMat<U>::value >
	,	class S = typename Decay_t<V>::value_type
	,	size_t DIM = trait::Dimension<V>::value
	>
	Plane(V&&, U&&)-> Plane<S, DIM>;


	template
	<	class V, class U
	,	class 
		=	Enable_if_t< trait::is_FixedSizeMat<V>::value && trait::is_FixedSizeMat<U>::value >
	,	class S = typename Decay_t<V>::value_type
	,	size_t DIM = trait::Dimension<V>::value
	>
	Line(V&&, U&&)-> Line<S, DIM>;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct s3d::Direction : Unconstructible
{
public:
	template<class U1, class U2>
	static bool are_acute_angled(U1 const& u1, U2 const& u2){  return u1.dot(u2) > 0;  }


	template<class U1, class U2>
	static bool are_obtuse_angled(U1 const& u1, U2 const& u2){  return u1.dot(u2) < 0;  }
	

	template
	<	class U1, class U2, class...ARGS
	,	class 
		=	Enable_if_t
			<	trait::Has_Matrix_interface<U1>::value 
			&&	trait::Has_Matrix_interface<U2>::value 
			>
	>
	static bool are_parallel(U1 const& u1, U2 const& u2, ARGS const&...args)
	{
		if constexpr( sizeof...(ARGS) > 0 )
			return are_parallel(u1, u2) && are_parallel(u1, args...);
		else
		{
			assert( Has_Vector_interface(u1) && Has_Vector_interface(u2) );

			if( !is_valid(u1) || !is_valid(u2) )
				return false;

			using T = trait::value_t<U1>;
			T const den = u1.norm()*u2.norm();

			return 
			!Are_almost_same<T>(den, 0) && Are_almost_same<T>(  std::abs( u1.dot(u2) ), den  );
		}	
	}


	template
	<	class U1, class U2, class...ARGS
	,	class 
		=	Enable_if_t
			<	trait::Has_Matrix_interface<U1>::value 
			&&	trait::Has_Matrix_interface<U2>::value 
			>
	>
	static bool are_orthogonal(U1 const& u1, U2 const& u2, ARGS const&...args)
	{
		if constexpr( sizeof...(ARGS) > 0 )
			return are_orthogonal(u1, u2) && are_orthogonal(u1, args...);
		else
		{
			assert( Has_Vector_interface(u1) && Has_Vector_interface(u2) );

			if( !is_valid(u1) || !is_valid(u2) )
				return false;

			using T = trait::value_t<U1>;
			T const den = u1.norm()*u2.norm();

			return 
			!Are_almost_same<T>(den, 0) && Are_almost_same<T>( u1.dot(u2), 0 );
		}	
	}


	template
	<	class U1, class U2, class...ARGS
	,	class 
		=	Enable_if_t
			<	trait::Has_Matrix_interface<U1>::value 
			&&	trait::Has_Matrix_interface<U2>::value 
			>
	>
	static auto angle(U1 const& u1, U2 const& u2)-> Maybe< trait::value_t<U1> >
	{
		assert( Has_Vector_interface(u1) && Has_Vector_interface(u2) );

		using T = trait::value_t<U1>;

		if( Maybe<T> const nopt = NullMyb;  !is_valid(u1) || !is_valid(u2) )
			return nopt;
		else
		{
			T const den = u1.norm()*u2.norm();

			auto clamp_f 
			=	[](T const t, T const Lb, T const hb)
				{  
					return t < Lb ? Lb : t > hb ? hb : t;  
				};


			return 
			Are_almost_same<T>(den, 0) 
			?	nopt
			:	Maybe<T>(   std::acos(  clamp_f( u1.dot(u2) / den, T(-1), T(1) )  )   );
		}
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace s3d
{

	template<class T>  
	auto _Pi()-> T const&
	{
		static T const pi = 2*std::acos( T(0) );
	
		return pi;
	}
	
	
	template<class S, class D>
	auto Projection(S&& src, D&& des)
	{
		if constexpr(trait::is_StrictVec<S>::value && trait::is_Plane<D>::value)
			return src - des.normal()*des.signed_dist_to(src);
		else if constexpr(trait::is_StrictVec<S>::value && trait::is_Line<D>::value)
			return des.position() + des.tangent()*des.tangent().dot(src - des.position());
		else if constexpr(trait::is_Line<S>::value && trait::is_Plane<D>::value)
		{
			decltype(src.tangent()) const tgt = Projection(src.tangent().vec(), des);
			using line_t = Referenceless_t<S>;
	
			return 
			is_valid(tgt) 
			?	make_Maybe(  line_t( Projection(src.position(), des), tgt )  )
			:	NullMyb;
		}
	}
	
	
	template<class S, class D>
	auto sqrDistance(S&& src, D&& des)
	{
		if constexpr(trait::is_StrictVec<S>::value && trait::is_StrictVec<D>::value)
			return (  Position( Forward<D>(des) ) - Position( Forward<S>(src) )  ).sqr_norm();
		else if constexpr(trait::is_StrictVec<S>::value && trait::is_Plane<D>::value)
			return std::pow( des.signed_dist_to(src), 2 );
		else if constexpr(trait::is_StrictVec<S>::value && trait::is_Line<D>::value)
			return ( src - Projection(src, des) ).sqr_norm();
		//else if constexpr(trait::is_Line<S>::value && trait::is_Line_v<D>::value)
	}
	
	
	template<class S, class D>
	auto Distance(S&& src, D&& des)
	{
		if constexpr(trait::is_StrictVec<S>::value && trait::is_StrictVec<D>::value)
			return (des - src).norm();
		else if constexpr(trait::is_StrictVec<S>::value && trait::is_Plane<D>::value)
			return std::abs( des.signed_dist_to(src) );
		else if constexpr(trait::is_StrictVec<S>::value && trait::is_Line<D>::value)
			return ( src - Projection(src, des) ).norm();
		//else if constexpr(trait::is_Line<S>::value && trait::is_Line_v<D>::value)
	}
	
	
	template<class S, class D>
	auto intersection(S&& src, D&& des)
	{
		if constexpr(trait::is_Line<S>::value && trait::is_Plane<D>::value)
		{
			using pos_t = typename Referenceless_t<S>::position_t;
	
			if
			(	auto const den = des.normal().dot(src.tangent())
			;	!Are_almost_same< trait::value_t<pos_t>, 2 >(den, 0) 
			)
				return 
				make_Maybe<pos_t>
				(	src.position() 
				+	src.tangent() * des.normal().dot(des.position() - src.position())/den
				);
			else
				return Maybe<pos_t>{};
		}
	}


	template<class T>
	decltype(auto) Position(T&& t)
	{
		if constexpr
		(	trait::Has_Matrix_interface<T>::value 
		&&	!trait::is_StrictMat<T>::value && !trait::is_UnitVec<T>::value)
		{
			assert( Has_Vector_interface(t) );
	
			return Forward<T>(t);
		}
		else if constexpr(trait::is_Oriented<T>::value)
			return _Position_Helper<T&&>::cast(t.position());
	}

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
struct s3d::_Position_Helper : Unconstructible
{
private:
	template<class Q> 
	friend decltype(auto) s3d::Position(Q&&);


	template<class Q>
	static decltype(auto) cast(Q&& q) 
	{
		return Selective_t< is_Rvalue_Reference<T>::value, Referenceless_t<Q>&&, Q&& >(q);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _S3D_EUCLID_
