#include "Geometry.hpp"
#include "..\Specification\Specification.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


using namespace sgm;

using 
	mxi::Plane, mxi::Line, mxi::Vector, mxi::UnitVector,
	mxi::Projection, mxi::Direction,
	spec::is_True;


template<class T1, class T2>
static bool is_same_pos(T1&& t1, T2&& t2)
{
	bool res = true;

	for(unsigned i = 0;  res && i < 3;  ++i)
		res = R_Comp( t1(i), t2(i), .000001f );

	return res;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned>
static void Test();


template<>
void Test<1>()
{
	Plane<float> P1{ Vector<float, 3>::zero(), UnitVector<float, 3>{0, 0, 1} };
	auto constexpr eps = .000001f;
	Vector<float> const pos{ 1, 1, 1 };

	is_True
	(	R_Comp( P1.distance_to(pos), 1.f, eps )
	&&	R_Comp( P1.signedDist_to(-pos), -1.f, eps )
	&&	R_Comp( Projection::distance(P1, pos), 1.f, eps )
	);

	Plane<float> const P2 = P1;

	is_True
	(	R_Comp( P2.distance_to(pos), 1.f, eps )
	&&	R_Comp( P2.signedDist_to(-pos), -1.f, eps )
	&&	R_Comp( Projection::distance(P2, pos), 1.f, eps )
	);

	Vector<float> const projected_pos{ 1, 1, 0 };

	is_True
	(	is_same_pos( P1.projected_from(pos), projected_pos )
	&&	is_same_pos( P2.projected_from(pos), projected_pos )
	&&	is_same_pos( Projection::position(P1, pos), projected_pos )
	&&	is_same_pos( Projection::position(P2, pos), projected_pos )
	);
}


template<>
void Test<2>()
{
	Line<float> L1{ Vector<float, 3>::zero(), UnitVector<float, 3>{0, 0, 1} };
	auto constexpr eps = .000001f;
	Vector<float> const pos{ 1, 1, 1 };

	is_True
	(	R_Comp( L1.distance_to(pos), sqrt(2.f), eps )
	&&	R_Comp( Projection::distance(L1, pos), sqrt(2.f), eps )
	);

	Line<float> const L2 = L1;

	is_True
	(	R_Comp( L2.distance_to(pos), sqrt(2.f), eps )
	&&	R_Comp( Projection::distance(L2, pos), sqrt(2.f), eps )
	);

	Vector<float> const projected_pos{ 0, 0, 1 };

	is_True
	(	is_same_pos( L1.projected_from(pos), projected_pos )
	&&	is_same_pos( L2.projected_from(pos), projected_pos )
	&&	is_same_pos( Projection::position(L1, pos), projected_pos )
	&&	is_same_pos( Projection::position(L2, pos), projected_pos )
	);
}


template<>
void Test<3>()
{
	UnitVector<float, 3> const u1{1, 0, 0}, u2{0, 1, 0};

	is_True( Direction::is_perpendicular(u1, u2) && Direction::is_parallel(u1, u1) );
}


#include "Test_Geometry.hpp"
#include <iostream>


void Test_sgm_Geometry::test()
{
	try
	{
		::Test<1>();
		::Test<2>();
		::Test<3>();

		std::wcout << L"Geometry Test Complete.\n";
	}
	catch(...)
	{
		std::wcerr << L"Geometry Test Failed.\n";

		throw;
	}
}