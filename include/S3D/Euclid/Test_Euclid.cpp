/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Euclid.hpp"
#include "SGM/Mathexpr/Mathexpr.hpp"


using s3d::Vector;
using s3d::UnitVec;

auto constexpr Pi = sgm::Mathexpr::pi<float>();


template<class...TYPES>
static void _identical(TYPES...types)
{
	SGM_SPEC_ASSERT( s3d::spec::_Equivalent<s3d::spec::_Equiv_Euclid_Tag>::calc(types...) );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void Construction()
{
	{
		s3d::Plane<float, 3> P1;
		s3d::Plane P2(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());
		auto P3 = P1;

		::_identical
		(	P1, P2, P3
		,	s3d::Plane(Vector<float, 3>{3, 3, 0}, -UnitVec<float, 3>::Axis<2>())
		);
	}
	{
		s3d::Line<float, 3> L1;
		s3d::Line L2(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());
		auto L3 = L1;

		::_identical
		(	L1, L2, L3
		,	s3d::Line(Vector<float, 3>{0, 0, -23}, -UnitVec<float, 3>::Axis<2>())
		);
	}
}


static void Projection()
{
	Vector<float, 3> const v1{1, 2, 3};
	s3d::Plane const P1(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());
	s3d::Line const L1(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());

	::_identical( Projection(v1, P1), Vector<float, 3>{1, 2, 0} );
	::_identical( Projection(v1, L1), Vector<float, 3>{0, 0, 3} );

	s3d::Line const L2(Vector<float, 3>{1, 1, 1}, UnitVec<float, 3>{0, -1, -1});

	::_identical
	(	Projection(L2, P1).v()
	,	s3d::Line(Vector<float, 3>{1, 0, 0}, UnitVec<float, 3>::Axis<1>()) 
	);
}


static void Distance()
{
	Vector<float, 3> const v0{0, 0, 0}, v1{0, 1, 1};
	s3d::Plane const P1(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());
	s3d::Line const L1(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());

	::_identical( s3d::Distance(v0, v1), (v1 - v0).norm() );
	::_identical( s3d::Distance(v1, P1), 1 );
	::_identical( s3d::Distance(v1, L1), 1 );
}


static void intersection()
{
	s3d::Line const L1(Vector<float, 3>{1, 1, 1}, UnitVec<float, 3>{0, -1, -1});	
	s3d::Plane const P1(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());

	::_identical( intersection(L1, P1).v(), Vector<float, 3>{1, 0, 0} );
}


static void Direction()
{
	UnitVec<float, 3> u1{1, 0, 0}, u2{0, 1, 0};

	SGM_SPEC_ASSERT
	(	s3d::Direction::are_parallel
		(	u1, -u1, Vector<float, 3>{2, 2, 2} - Vector<float, 3>{0, 2, 2}
		)
	&&	s3d::Direction::are_orthogonal( u1, u2, u1.cross(u2) )
	);

	::_identical( s3d::Direction::angle(u1, Vector<float, 3>{1, 1, 0}).v(), ::Pi/4 );
	::_identical( s3d::Direction::angle(u1, -u1).v(), ::Pi );
}


static void Position()
{
	{
		Vector<float, 3> const Vec1{1, 1, 2};

		static_assert
		(	std::is_same_v< decltype( Position(Vec1) ), Vector<float, 3> const& >
		&&	std::is_same_v< decltype( Position(Vector<float, 3>{}) ), Vector<float, 3>&& >
		);
		
		::_identical( Vec1, Position(Vec1) );
	}
	{
		Vector<float, 3> const pos{1, 2, 3};

		s3d::Plane P1(pos, UnitVec<float, 3>::Axis<2>());
		s3d::Line L1(pos, UnitVec<float, 3>::Axis<2>());

		static_assert
		(	std::is_same_v< decltype( Position(P1) ), Vector<float, 3>& >
		&&	std::is_same_v< decltype( Position(L1) ), Vector<float, 3>& >
		&&	std::is_same_v< decltype( Position(s3d::Plane<float, 3>()) ), Vector<float, 3>&& >
		&&	std::is_same_v< decltype( Position(s3d::Line<float, 3>()) ), Vector<float, 3>&& >
		);

		::_identical(pos, Position(P1), Position(L1), Vector<float>{1, 2, 3});
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


SGM_SPECIFICATION_TEST(s3d::spec::Test_, Euclid, /**/)
{	::Construction
,	::Projection
,	::Distance
,	::intersection
,	::Direction
,	::Position
};
