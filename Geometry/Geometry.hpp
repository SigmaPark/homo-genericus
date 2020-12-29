#pragma once

#ifndef _SGM_MXI_GEOMETRY_
#define _SGM_MXI_GEOMETRY_

#include "..\Matrix_interface\Matrix.hpp"

namespace sgm::mxi
{

	template<class T, unsigned DIMENSION = 3>
	struct Oriented;

	template<class T, unsigned DIMENSION = 3>
	struct Plane;

	template<class T, unsigned DIMENSION = 3>
	struct Line;


	template<class>
	struct _is_Oriented;

	template<class T>
	static bool constexpr is_Oriented_v = _is_Oriented<T>::value;

	SGM_USER_DEFINED_TYPE_CHECK
	(	Plane, SGM_MACROPACK(class T, unsigned DIMENSION), SGM_MACROPACK(T, DIMENSION)
	);

	SGM_USER_DEFINED_TYPE_CHECK
	(	Line, SGM_MACROPACK(class T, unsigned DIMENSION), SGM_MACROPACK(T, DIMENSION)
	);


	struct Projection;

	struct Direction;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, unsigned DIM>
struct sgm::mxi::Oriented
{
	using elem_t = std::decay_t<T>;
	static unsigned constexpr DIMENSION = DIM;

	static_assert(std::is_floating_point_v<elem_t> && DIMENSION > 1);

	using Point_t = Vector<elem_t, DIMENSION>;


	Point_t point;
	UnitVector<elem_t, DIMENSION> direction;
};


template<class T, unsigned DIM>
struct sgm::mxi::Plane : Oriented<T, DIM>
{
	using typename Oriented<T, DIM>::elem_t;
	using Oriented<T, DIM>::DIMENSION;
	using typename Oriented<T, DIM>::Point_t;

	using Oriented<T, DIM>::point;
	decltype(Oriented<T, DIM>::direction)& normal = Oriented<T, DIM>::direction;


	auto signedDist_to(Point_t const& vec) const-> elem_t
	{
		return normal.dot(vec - point);
	}


	auto distance_to(Point_t const& vec) const-> elem_t
	{
		return abs( signedDist_to(vec) );  
	}


	auto projected_from(Point_t const& vec) const-> Point_t
	{
		return vec - normal * signedDist_to(vec);
	}
};


template<class T, unsigned DIM>
struct sgm::mxi::Line : Oriented<T, DIM>
{
	using typename Oriented<T, DIM>::elem_t;
	using Oriented<T, DIM>::DIMENSION;
	using typename Oriented<T, DIM>::Point_t;

	using Oriented<T, DIM>::point;
	decltype(Oriented<T, DIM>::direction)& tangent = Oriented<T, DIM>::direction;


	auto distance_to(Point_t const& vec) const-> elem_t
	{
		return ( vec - projected_from(vec) ).norm();
	}


	auto projected_from(Point_t const& vec) const-> Point_t
	{
		return point + tangent*tangent.dot(vec - point);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class ORN>
struct sgm::mxi::_is_Oriented : No_Making
{
private:
	template<class>
	static False_t _calc(...);	//	Declaration only.

	template<class T, unsigned DIM>
	static True_t _calc(Oriented<T, DIM>);	//	Declaration only.

public:
	static bool constexpr value = decltype( _calc(Declval<ORN>()) )::value;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::mxi::Projection
{
	template<  class ORN, class = Guaranteed_t< is_Oriented_v<ORN> >  >
	static decltype(auto) distance(ORN const& orn, typename ORN::Point_t const& vec)
	{
		return orn.distance_to(vec);
	}

	template<  class ORN, class = Guaranteed_t< is_Oriented_v<ORN> >  >
	static decltype(auto) position(ORN const& orn, typename ORN::Point_t const& vec)
	{
		return orn.projected_from(vec);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::mxi::Direction : No_Making
{
private:
	template<class U1, class U2>
	static decltype(auto) _dot(U1 const& u1, U2 const& u2)
	{
		static_assert(MxTraits::is_UnitVector_v<U1> && MxTraits::is_UnitVector_v<U2>);

		return u1.dot(u2.vec());
	}


public:
	template<  class U1, class U2, class real_t = std::decay_t< decltype(Declval<U1>()(0)) >  >
	static bool is_perpendicular
	(	U1 const& u1, U2 const& u2
	,	real_t const epsilon = 1000 * std::numeric_limits<real_t>::epsilon()
	)
	{
		return R_Comp<real_t>( _dot(u1, u2), 0, epsilon );
	}


	template<  class U1, class U2, class real_t = std::decay_t< decltype(Declval<U1>()(0)) >  >
	static bool is_parallel
	(	U1 const& u1, U2 const& u2
	,	real_t const epsilon = 1000 * std::numeric_limits<real_t>::epsilon()
	)
	{
		return R_Comp<real_t>(  abs( _dot(u1, u2) ), 1, epsilon  );
	}


	template<class U1, class U2>
	static auto angle(U1 const& u1, U2 const& u2)
	{
		auto const val = _dot(u1, u2);

		assert( abs(val) <= 1 );

		return acos(val);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif