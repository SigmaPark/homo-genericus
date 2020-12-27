#pragma once

#ifndef _SGM_MXI_GEOMETRY_
#define _SGM_MXI_GEOMETRY_

#include "..\Matrix_interface\Matrix.hpp"

namespace sgm::mxi
{
	
	template<class T, unsigned DIMENSION = 3>
	struct Plane;

	template<class T, unsigned DIMENSION = 3>
	struct Line;


	template<class>
	struct _is_Plane;

	template<class>
	struct _is_Line;

	template<class T>
	static bool constexpr is_Plane_v = _is_Plane< std::decay_t<T> >::value;

	template<class T>
	static bool constexpr is_Line_v = _is_Line< std::decay_t<T> >::value;


	class Projection;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, unsigned DIM>
struct sgm::mxi::Plane
{
	using elem_t = std::decay_t<T>;
	static unsigned constexpr DIMENSION = DIM;

	static_assert(std::is_floating_point_v<elem_t> && DIMENSION > 1);
	

	UnitVector<elem_t, DIMENSION> normal;
	Vector<elem_t, DIMENSION> point;


	template<class VEC>
	auto signedDist_to(VEC&& vec) const-> elem_t
	{
		return normal.dot(vec - point);  
	}


	template<class VEC>
	auto distance_to(VEC&& vec) const-> elem_t
	{
		return abs(  signedDist_to( std::forward<VEC>(vec) )  );  
	}


	template<class VEC>
	auto projected_from(VEC&& vec) const-> Vector<elem_t, DIMENSION>
	{
		return vec - normal * signedDist_to(vec);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, unsigned DIM>
struct sgm::mxi::Line
{
	using elem_t = std::decay_t<T>;
	static unsigned constexpr DIMENSION = DIM;

	static_assert(std::is_floating_point_v<elem_t> && DIMENSION > 1);


	UnitVector<elem_t, DIMENSION> tangent;
	Vector<elem_t, DIMENSION> point;


	template<class VEC>
	auto distance_to(VEC&& vec) const-> elem_t
	{
		return ( vec - projected_from(vec) ).norm();
	}


	template<class VEC>
	auto projected_from(VEC&& vec) const-> Vector<elem_t, DIMENSION>
	{
		return point + tangent*tangent.dot(vec - point);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class>
struct sgm::mxi::_is_Plane : False_t{};

template<class T, unsigned D>
struct sgm::mxi::_is_Plane< sgm::mxi::Plane<T, D> > : True_t{};


template<class>
struct sgm::mxi::_is_Line : False_t{};

template<class T, unsigned D>
struct sgm::mxi::_is_Line< sgm::mxi::Line<T, D> > : True_t{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::mxi::Projection
{
	template<class ORN, class VEC>
	static decltype(auto) distance(ORN&& orn, VEC&& vec)
	{
		static_assert( (is_Plane_v<ORN> || is_Line_v<ORN>) && MxTraits::is_mxiVector_v<VEC> );

		return orn.distance_to( std::forward<VEC>(vec) );
	}

	template<class ORN, class VEC>
	static decltype(auto) position(ORN&& orn, VEC&& vec)
	{
		static_assert( (is_Plane_v<ORN> || is_Line_v<ORN>) && MxTraits::is_mxiVector_v<VEC> );
		
		return orn.projected_from( std::forward<VEC>(vec) );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif