#pragma once

#ifndef _V3D_EUCLID_
#define _V3D_EUCLID_


#include "..\Hamilton\Hamilton.hpp"


namespace v3d
{
	
	template<class T, size_t DIM>  class Oriented;

	template<class T, size_t DIM = 3>  class Plane;
	template<class T, size_t DIM = 3>  class Line;


	template<class S, class D>  static auto Projection(S &&src, D &&des);
	template<class S, class D>  static auto sqrDistance(S &&src, D &&des);
	template<class S, class D>  static auto Distance(S &&src, D &&des);
	template<class S, class D>  static auto intersection(S &&src, D &&des);

	struct Direction;

	template<class CON>  static auto Average(CON &&con);
	template<class CON, class W>  static auto Average(CON &&con, W &&weight);


	template<class T>  inline auto _Pi()-> T const&;
	template<class T>  inline T const &Pi = _Pi<T>();


	template<class T>  struct _Position_Helper;
	template<class T>  static decltype(auto) Position(T&&);


	template<class res_t, class MAT, class POS>  class _Fitter;

	template
	<	class res_t, class RS, class CS
	,	class _V = std::decay_t< decltype( Position(std::declval<res_t>()) ) >
	,	size_t _R = std::is_convertible_v<RS, FixedSize> ? trait::Dimension_v<_V> : DYNAMIC
	,	size_t _C = std::is_convertible_v<CS, FixedSize> ? trait::Dimension_v<_V> : DYNAMIC
	,	class _M = Matrix< trait::value_t<_V>, _R, _C, _V::STORING_ORDER >
	>
	static auto Fitting(RS const r, CS const c)-> _Fitter<res_t, _M, _V>;

	template<class res_t, class CON>  static auto Fitting(CON &&con)-> res_t;


	template<class T>  class Paraboloid;
	template<class T>  class Helix;

	template<class T, size_t ORDER>  struct Spatial;

	struct _Average_Helper;

}


namespace v3d::trait
{
	
	template<class>  struct is_Oriented;
	template<class T>  static bool constexpr is_Oriented_v = is_Oriented<T>::value;

	SGM_USER_DEFINED_TYPE_CHECK14( _, Plane, SGM_MACROPACK(class T, size_t DIM), SGM_MACROPACK(T, DIM) );
	SGM_USER_DEFINED_TYPE_CHECK14( _, Line, SGM_MACROPACK(class T, size_t DIM), SGM_MACROPACK(T, DIM) );

	SGM_HAS_MEMBER(DIMENSION);

	template<class ORN>
	static size_t constexpr Dimension_v
	=	[]() constexpr
		{
			if constexpr(Has_Member_DIMENSION<ORN>::value)
				return ORN::DIMENSION;
			else if constexpr(trait::is_v3dVec_v<ORN>)
			{
				using _M = std::decay_t<ORN>;

				return _M::ROW_SIZE * _M::COL_SIZE;
			}
			else SGM_COMPILE_FAILED(no suitable method to define v3d::trait::Dimension_v .);
		}();

	template<class>  struct is_Spatial;
	template<class T>  static bool constexpr is_Spatial_v = is_Spatial<T>::value;
}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


template<class ORN>
struct v3d::trait::is_Oriented : sgm::No_Making
{
private:
	template<class...> /* Declaration Only */ static sgm::False_t _calc(...);
	template<class T, size_t DIM> /* Declaration Only */ static sgm::True_t _calc(Oriented<T, DIM>);

public:
	static bool constexpr value = decltype( _calc(std::declval<ORN>()) )::value;

	using type = sgm::Boolean_type<value>;
};


template<class S>
struct v3d::trait::is_Spatial : sgm::No_Making
{
private:
	template<class...> /* Declaration Only */ static sgm::False_t _calc(...);
	template<class T, size_t ORDER> /* Declaration Only */ static sgm::True_t _calc(Spatial<T, ORDER>);

public:
	static bool constexpr value = decltype( _calc(std::declval<S>()) )::value;

	using type = sgm::Boolean_type<value>;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class T, size_t DIM>
class v3d::Oriented
{
public:
	using value_type = T;
	static size_t constexpr DIMENSION = DIM;
	using position_t = Vector<T, DIM>;

	static_assert( trait::is_real_v<T> && DIM > 1 );
};


template<class T, size_t DIM>
class v3d::Plane : public v3d::Oriented<T, DIM>
{
public:
	Plane() : _position(Vector<T, DIM>::Zero()), _normal(){}

	template
	<	class VEC, class UVEC
	,	class
		=	std::enable_if_t
			<	std::is_convertible_v< VEC, Vector<T, DIM> > && std::is_convertible_v< UVEC, UnitVec<T, DIM> >
			>
	>
	Plane(VEC &&pos, UVEC &&nml) noexcept(std::is_rvalue_reference_v<VEC&&> || std::is_rvalue_reference_v<UVEC&&>)
	:	_position( std::forward<VEC>(pos) ), _normal( std::forward<UVEC>(nml) ){}

	auto position() const-> Vector<T, DIM> const&{  return _position;  }
	auto position()-> Vector<T, DIM>&{  return _position;  }
	auto normal() const-> UnitVec<T, DIM> const&{  return _normal;  }
	auto normal()-> UnitVec<T, DIM>&{  return _normal;  }

	auto signed_dist_to(Vector<T, DIM> const &pos) const-> T{  return normal().dot(pos - position());  }


private:
	Vector<T, DIM> _position;
	UnitVec<T, DIM> _normal;
};


template<class T, size_t DIM>
class v3d::Line : public v3d::Oriented<T, DIM>
{
public:
	Line() : _position(Vector<T, DIM>::Zero()), _tangent(){}

	template
	<	class VEC, class UVEC
	,	class
		=	std::enable_if_t
			<	std::is_convertible_v< VEC, Vector<T, DIM> > && std::is_convertible_v< UVEC, UnitVec<T, DIM> >
			>
	>
	Line(VEC &&pos, UVEC &&tgt) noexcept(std::is_rvalue_reference_v<VEC&&> || std::is_rvalue_reference_v<UVEC&&>)
	:	_position( std::forward<VEC>(pos) ), _tangent( std::forward<UVEC>(tgt) ){}

	auto position() const-> Vector<T, DIM> const&{  return _position;  }
	auto position()-> Vector<T, DIM>&{  return _position;  }
	auto tangent() const-> UnitVec<T, DIM> const&{  return _tangent;  }
	auto tangent()-> UnitVec<T, DIM>&{  return _tangent;  }


private:
	Vector<T, DIM> _position;
	UnitVec<T, DIM> _tangent;	
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#include <cmath>


template<class T>  
auto v3d::_Pi()-> T const&
{
	static T const pi = acos( T(0) )*2;

	return pi;
}


#include <optional>


struct v3d::Direction : sgm::No_Making
{
public:
	template<class U1, class U2>
	static bool are_acute_angled(U1 const &u1, U2 const &u2){  return u1.dot(u2) > 0;  }

	template<class U1, class U2>
	static bool are_obtuse_angled(U1 const &u1, U2 const &u2){  return u1.dot(u2) < 0;  }
	
	template
	<	class U1, class U2, class...ARGS
	,	class = std::enable_if_t< trait::has_Matrix_interface_v<U1> && trait::has_Matrix_interface_v<U2> >
	>
	static bool are_parallel(U1 const &u1, U2 const &u2, ARGS const&...args)
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

			return !are_almost_same<T>(den, 0) && are_almost_same<T>(  abs( u1.dot(u2) ), den  );
		}	
	}


	template
	<	class U1, class U2, class...ARGS
	,	class = std::enable_if_t< trait::has_Matrix_interface_v<U1> && trait::has_Matrix_interface_v<U2> >
	>
	static bool are_orthogonal(U1 const &u1, U2 const &u2, ARGS const&...args)
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

			return !are_almost_same<T>(den, 0) && are_almost_same<T>( u1.dot(u2), 0 );
		}	
	}


	template
	<	class U1, class U2, class...ARGS
	,	class = std::enable_if_t< trait::has_Matrix_interface_v<U1> && trait::has_Matrix_interface_v<U2> >
	>
	static auto angle(U1 const &u1, U2 const &u2)-> std::optional< trait::value_t<U1> >
	{
		assert( Has_Vector_interface(u1) && Has_Vector_interface(u2) );

		using T = trait::value_t<U1>;

		if( std::optional<T> const nopt = std::nullopt;  !is_valid(u1) || !is_valid(u2) )
			return nopt;
		else
		{
			T const den = u1.norm()*u2.norm();

			auto clamp_f = [](T const t, T const Lb, T const hb){  return t < Lb ? Lb : t > hb ? hb : t;  };

			return 
			are_almost_same<T>(den, 0) ? nopt
			:	/* otherwise */ std::optional<T>(   acos(  clamp_f( u1.dot(u2) / den, T(-1), T(1) )  )   );
		}
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class S, class D>
auto v3d::Projection(S &&src, D &&des)
{
	if constexpr(trait::is_v3dVec_v<S> && trait::is_Plane_v<D>)
		return src - des.normal()*des.signed_dist_to(src);
	else if constexpr(trait::is_v3dVec_v<S> && trait::is_Line_v<D>)
		return des.position() + des.tangent()*des.tangent().dot(src - des.position());
	else if constexpr(trait::is_Line_v<S> && trait::is_Plane_v<D>)
	{
		decltype(src.tangent()) const tgt = Projection(src.tangent().vec(), des);

		return 
		is_valid(tgt) 
		?	std::make_optional< std::remove_reference_t<S> >( Projection(src.position(), des), tgt )
		:	std::nullopt;
	}
	else SGM_COMPILE_FAILED(no suitable method for v3d::Projection .);
}


template<class S, class D>
auto v3d::sqrDistance(S &&src, D &&des)
{
	if constexpr(trait::is_v3dVec_v<S> && trait::is_v3dVec_v<D>)
		return (  Position( std::forward<D>(des) ) - Position( std::forward<S>(src) )  ).sqr_norm();
	else if constexpr(trait::is_v3dVec_v<S> && trait::is_Plane_v<D>)
		return std::pow( des.signed_dist_to(src), 2 );
	else if constexpr(trait::is_v3dVec_v<S> && trait::is_Line_v<D>)
		return ( src - Projection(src, des) ).sqr_norm();
	//else if constexpr(trait::is_Line_v<S> && trait::is_Line_v<D>)		// Least Square Problem
	else SGM_COMPILE_FAILED(no suitable method for v3d::Distance .);	
}


template<class S, class D>
auto v3d::Distance(S &&src, D &&des)
{
	if constexpr(trait::is_v3dVec_v<S> && trait::is_v3dVec_v<D>)
		return (des - src).norm();
	else if constexpr(trait::is_v3dVec_v<S> && trait::is_Plane_v<D>)
		return abs( des.signed_dist_to(src) );
	else if constexpr(trait::is_v3dVec_v<S> && trait::is_Line_v<D>)
		return ( src - Projection(src, des) ).norm();
	//else if constexpr(trait::is_Line_v<S> && trait::is_Line_v<D>)		// Least Square Problem
	else SGM_COMPILE_FAILED(no suitable method for v3d::Distance .);
}


template<class S, class D>
auto v3d::intersection(S &&src, D &&des)
{
	if constexpr(trait::is_Line_v<S> && trait::is_Plane_v<D>)
	{
		using pos_t = typename std::remove_reference_t<S>::position_t;

		if
		(	auto const den = des.normal().dot(src.tangent())
		;	!are_almost_same< trait::value_t<pos_t>, 2 >(den, 0) 
		)
			return 
			std::make_optional<pos_t>
			(	src.position() + src.tangent()*des.normal().dot(des.position() - src.position())/den
			);
		else
			return std::optional<pos_t>{};
	}
	else SGM_COMPILE_FAILED(no suitable method for v3d::intersection .);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class T>  
struct v3d::Spatial<T, 0>
{
	using scalar_type = T;

	Vector<T, 3> position = {};  

	auto x() const{  return position(0);  }    auto x()-> scalar_type&{  return position(0);  }
	auto y() const{  return position(1);  }    auto y()-> scalar_type&{  return position(1);  }
	auto z() const{  return position(2);  }    auto z()-> scalar_type&{  return position(2);  }
};


template<class T>  
struct v3d::Spatial<T, 1> : Spatial<T, 0>
{
	UnitVec<T, 3> normal = UnitVec<T, 3>::Axis<2>();
	T kappa0 = -1;
	
	auto nx() const{  return normal(0);  }
	auto ny() const{  return normal(1);  }
	auto nz() const{  return normal(2);  }
};


template<class T>  
struct v3d::Spatial<T, 2> : Spatial<T, 1>
{
	UnitVec<T, 3> tangent1 = UnitVec<T, 3>::Axis<1>(),  tangent2 = UnitVec<T, 3>::Axis<0>();
	T kappa1 = -1, kappa2 = -1;

	auto t1x() const{  return tangent1(0);  }    auto t2x() const{  return tangent2(0);  }
	auto t1y() const{  return tangent1(1);  }    auto t2y() const{  return tangent2(1);  }
	auto t1z() const{  return tangent1(2);  }    auto t2z() const{  return tangent2(2);  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


struct v3d::_Average_Helper : sgm::No_Making
{
private:
	template<class CON>  friend auto v3d::Average(CON &&con);
	template<class CON, class W>  friend auto v3d::Average(CON &&con, W &&weight);

	template
	<	class T
	,	int
		=	!std::is_same_v< T, std::decay_t<T> > ? 0
		:	trait::is_v3dMat_v<T> ? 1
		:	std::is_integral_v<T> || std::is_same_v<T, float> ? 2
		:	/* otherwise */ 3
	>
	struct _denominator;

	template<class T>  struct _denominator<T, 0> : _denominator< std::decay_t<T> >{};
	template<class T>  struct _denominator<T, 1>{  using type = trait::value_t<T>;  };
	template<class T>  struct _denominator<T, 2>{  using type = float;  };
	template<class T>  struct _denominator<T, 3>{  using type = double;  };


	template
	<	class T
	,	int
		=	!std::is_same_v< T, std::decay_t<T> > ? 0
		:	trait::is_v3dMat_v<T> ? 1
		:	/* otherwise */ 2
	>
	struct _summand;

	template<class T>  struct _summand<T, 0> : _summand< std::decay_t<T> >{};
	template<class T>  struct _summand<T, 1>{  using type = std::decay_t< decltype( Eval(std::declval<T>()) ) >;  };
	template<class T>  struct _summand<T, 2> : _denominator<T>{};


	template<class T>
	static auto calc(T &&res)-> std::optional<T>
	{
		if constexpr(trait::is_v3dMat_v<T>)
			return is_valid(res) ? std::make_optional<T>( std::move(res) ) : std::optional<T>{};
		else
			return std::make_optional<T>( std::move(res) );
	}
};


template<class CON>  
auto v3d::Average(CON &&con)
{
	static_assert(sgm::is_iterable<CON>::value);

	using deref_t = std::decay_t< sgm::Deref_t<CON> >;
	using den_t = typename _Average_Helper::_denominator<deref_t>::type;
	using res_t = std::conditional_t< std::is_integral_v<deref_t>, float, deref_t >;
	using summand_t =	typename _Average_Helper::_summand<deref_t>::type;


	auto const nof_elem = static_cast<den_t>( sgm::Size(con) );
	
	if(nof_elem == 0)
		return std::optional<res_t>{};

	auto &&elem0 = static_cast<summand_t const&>( *sgm::Begin(con) );
	summand_t sum = elem0 - elem0;

	for(auto const &t : con)
		sum += static_cast<summand_t const&>(t);

	return _Average_Helper::calc<res_t>(sum / nof_elem);
}


template<class CON, class W> 
auto v3d::Average(CON &&con, W &&weight)
{
	static_assert(sgm::is_iterable<CON>::value);
	using deref_t = std::decay_t< sgm::Deref_t<CON> >;
	using res_t = std::conditional_t< std::is_integral_v<deref_t>, float, deref_t >;
	using summand_t =	typename _Average_Helper::_summand<deref_t>::type;

	if( auto const nof_elem = sgm::Size(con);  nof_elem == 0 )
		return std::optional<res_t>{};

	using sgm::Begin, sgm::End;

	auto &&elem0 = static_cast<summand_t const&>( *Begin(con) );
	summand_t sum = elem0 - elem0;

	auto den
	=	[]
		{
			if constexpr(std::is_invocable_v<W, deref_t>)
				return std::invoke_result_t<W, deref_t>(0);
			else
				return typename _Average_Helper::_denominator<deref_t>::type(0);
		}();

	if constexpr(std::is_invocable_v<W, deref_t>)
		for(auto const &t : con)
		{
			auto const w = weight(t);

			sum += static_cast<summand_t const&>(w*t);
			den += w;
		}
	else if constexpr(sgm::is_iterable<W>::value)	
		for( auto[sitr, witr] = std::pair( Begin(con), Begin(weight) );	 sitr != End(con);  sitr++,  witr++ )
		{
			sum += static_cast<summand_t const&>(*witr * *sitr);
			den += *witr;
		}
	else
		return std::optional<res_t>{};

	return _Average_Helper::calc<res_t>(sum / den);	
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class T>
struct v3d::_Position_Helper : sgm::No_Making
{
private:
	template<class T>  friend decltype(auto) v3d::Position(T&&);


	template<class Q>
	static decltype(auto) cast(Q &&q) 
	{
		return std::conditional_t< std::is_rvalue_reference_v<T>, std::remove_reference_t<Q>&&, Q&& >(q);
	}
};


template<class T>
decltype(auto) v3d::Position(T &&t)
{
	if constexpr(trait::has_Matrix_interface_v<T> && !trait::is_StrictMat_v<T> && !trait::is_UnitVec_v<T>)
	{
		assert( Has_Vector_interface(t) );

		return std::forward<T>(t);
	}
	else if constexpr(trait::is_Spatial_v<T>)
		return _Position_Helper<T&&>::cast(t.position);
	else if constexpr(trait::is_Oriented_v<T>)
		return _Position_Helper<T&&>::cast(t.position());
	else SGM_COMPILE_FAILED(no suitable method to compute its position .);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#include "..\Decomposition\Decomposition.hpp"
#include <array>


template<class res_t, class CON>
auto v3d::Fitting(CON &&con)-> res_t
{
	using POS = std::decay_t<decltype(  Position( *sgm::Begin(con) )  )>;

	if constexpr(trait::is_FixedSizeMat_v<POS>)
	{
		if constexpr(POS::COL_SIZE == 1)
			return Fitting<res_t>( FIXED_SIZE, sgm::Size(con) )( std::forward<CON>(con) );
		else
			return Fitting<res_t>( sgm::Size(con), FIXED_SIZE )( std::forward<CON>(con) );
	}
	else
		return Fitting<res_t>(0, 0)( std::forward<CON>(con) );
}


template<class res_t, class RS, class CS, class _V, size_t _R, size_t _C, class _M>
auto v3d::Fitting(RS const r, CS const c)-> _Fitter<res_t, _M, _V>{  return _M(r, c);  }


template<class res_t, class MAT, class POS>
class v3d::_Fitter
{
	template<class>  struct _order_of_spatial : std::integral_constant<size_t, 0>{};

	template<class T, size_t ORDER>
	struct _order_of_spatial< Spatial<T, ORDER> > : std::integral_constant<size_t, ORDER>{};

	template<class S>  static size_t constexpr order_of_spatial = _order_of_spatial< std::decay_t<S> >::value;


public:
	_Fitter(MAT &&m) : _A( std::move(m) ), _positions( std::max(_A.rows(), _A.cols()) ){}


	template<class CON>
	auto operator()(CON &&con)-> res_t
	{
		auto const centroid = _centroid_and_shifted_Vectors( std::forward<CON>(con) );
		Eigen_Decomposition const pd(_AA(), sgm::FlagSet<flag::Real_Symmetric>{});
		auto const [min_idx, max_idx] = _minmax_ev_indices(pd);

		size_t constexpr DIM = trait::Dimension_v<decltype(centroid)>;
		using uvec_t = UnitVec< trait::value_t<POS>, DIM >;

		if constexpr(trait::is_Plane_v<res_t> || trait::is_Spatial_v<res_t> && order_of_spatial<res_t> == 1)
		{
			auto const normal = Skipped<uvec_t>( pd.eigenvec(min_idx) );

			if constexpr(trait::is_Plane_v<res_t>)
				return {centroid, normal};
			else
				return {centroid, normal, pd.eigenval(min_idx)};
		}
		else if constexpr(trait::is_Line_v<res_t>)
		{
			auto const tangent = Skipped<uvec_t>( pd.eigenvec(max_idx) );

			return {centroid, tangent};
		}
		else if constexpr(trait::is_Spatial_v<res_t> && order_of_spatial<res_t> == 2)
		{
			static_assert(DIM == 3);

			size_t const
				&i0 = min_idx, &i2 = max_idx,
				i1
				=	i0 != 0 && i2 != 0 ? 0
				:	i0 != 1 && i2 != 1 ? 1
				:	/* otherwise */ 2;

			uvec_t
				normal = Skipped<uvec_t>( pd.eigenvec(i0) ),
				tangent1 = Skipped<uvec_t>( pd.eigenvec(i1) ),
				tangent2 = Skipped<uvec_t>( pd.eigenvec(i2) );

			return
			{	centroid, normal, pd.eigenval(i0)
			,	tangent1, tangent2, pd.eigenval(i1), pd.eigenval(i2)
			};
		}
		else SGM_COMPILE_FAILED(no suitable method to Fitting with .);
	}


private:
	MAT _A;
	std::vector<POS> _positions;


	template<class CON>
	auto _centroid_and_shifted_Vectors(CON &&con)
	{
		static_assert(sgm::is_iterable<CON>::value);

		size_t const n = sgm::Size(con);

		assert(n != 0);

		_positions.resize(n);

		std::transform
		(	sgm::Begin(con), sgm::End(con), _positions.begin(), [](auto const &t){  return Position(t);  }
		);

		assert(_positions.front().size() != 0 && _positions.front().size() <= n);

		auto const centroid = *Average(_positions);

		for(auto &pos : _positions)
			pos -= centroid;

		return centroid;
	}


	auto _AA()
	{
		if constexpr(trait::is_DynamicSizeMat_v<MAT>)
		{
			size_t const n = _positions.size();
			bool const are_col_vecs = _positions.begin()->cols() == 1;
			size_t const d = _positions.front().size();
			auto const [rows, cols] = are_col_vecs ? std::array<size_t, 2>{d, n} : std::array<size_t, 2>{n, d};
			DynamicMat< trait::value_t<POS> > AA(d, d);

			_A.resize(rows, cols);

			if(are_col_vecs)
				return 
				std::copy( _positions.cbegin(), _positions.cend(), as_col_space(_A) ),  AA = _A*_A.transpose();
			else
				return
				std::copy( _positions.cbegin(), _positions.cend(), as_row_space(_A) ),  AA = _A.transpose()*_A;
		}
		else
		{
			_A = _positions;

			if constexpr(POS::COL_SIZE == 1)
				return Eval(_A*_A.transpose());
			else
				return Eval(_A.transpose()*_A);
		}
	}


	template<class T, size_t R, size_t C, Storing_Order STOR>
	static auto _minmax_ev_indices(Eigen_Decomposition<T, R, C, STOR, true> const &ed)-> std::array<size_t, 2>
	{
		std::pair<size_t, T> Minimum{0, ed.eigenval(0)},  Maximum = Minimum;

		for(size_t idx = 0;  idx < ed.size();  ++idx)
		{
			auto const &val = ed.eigenval(idx);

			if(val < Minimum.second)
				Minimum = {idx, val};

			if(val > Maximum.second)
				Maximum = {idx, val};
		}

		return {Minimum.first, Maximum.first};
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------

#endif // end of #ifndef _V3D_EUCLID_