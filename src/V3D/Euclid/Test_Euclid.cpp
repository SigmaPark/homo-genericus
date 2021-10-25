#include "Test_Euclid.hpp"
#include "Sigma\Specification\Specification.hpp"


using sgm::spec::is_True;
using namespace v3d;


template<class...TYPES>
static void _identical(TYPES...types)
{
	is_True( test::_Equivalent<test::_Equiv_Euclid_Tag>::calc(types...) );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


static void _01_Construction()
{
	{
		Plane<float, 3> P1, P2(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());
		auto P3 = P1;

		::_identical
		(	P1, P2, P3
		,	Plane<float, 3>(Vector<float, 3>{3, 3, 0}, -UnitVec<float, 3>::Axis<2>())
		);
	}
	{
		Line<float, 3> L1, L2(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());
		auto L3 = L1;

		::_identical
		(	L1, L2, L3
		,	Line<float, 3>(Vector<float, 3>{0, 0, -23}, -UnitVec<float, 3>::Axis<2>())
		);
	}
}


static void _02_Projection()
{
	Vector<float, 3> const v1{1, 2, 3};
	Plane<float, 3> const P1(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());
	Line<float, 3> const L1(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());

	::_identical( Projection(v1, P1), Vector<float, 3>{1, 2, 0} );
	::_identical( Projection(v1, L1), Vector<float, 3>{0, 0, 3} );

	Line<float, 3> const L2(Vector<float, 3>{1, 1, 1}, UnitVec<float, 3>{0, -1, -1});

	::_identical( *Projection(L2, P1), Line<float, 3>(Vector<float, 3>{1, 0, 0}, UnitVec<float, 3>::Axis<1>()) );
}


static void _03_Distance()
{
	Vector<float, 3> const v0{0, 0, 0}, v1{0, 1, 1};
	Plane<float, 3> const P1(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());
	Line<float, 3> const L1(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());

	::_identical( Distance(v0, v1), (v1 - v0).norm() );
	::_identical( Distance(v1, P1), 1 );
	::_identical( Distance(v1, L1), 1 );
}


static void _04_intersection()
{
	Line<float, 3> const L1(Vector<float, 3>{1, 1, 1}, UnitVec<float, 3>{0, -1, -1});	
	Plane<float, 3> const P1(Vector<float, 3>::Zero(), UnitVec<float, 3>::Axis<2>());

	::_identical( *intersection(L1, P1), Vector<float, 3>{1, 0, 0} );
}


static void _05_Direction()
{
	UnitVec<float, 3> u1{1, 0, 0}, u2{0, 1, 0};

	is_True
	(	Direction::are_parallel(u1, -u1, Vector<float, 3>{2, 2, 2} - Vector<float, 3>{0, 2, 2})
	&&	Direction::are_orthogonal( u1, u2, u1.cross(u2) )
	);

	::_identical( *Direction::angle(u1, Vector<float, 3>{1, 1, 0}), Pi<float>/4 );
	::_identical( *Direction::angle(u1, -u1), Pi<float> );
}


static void _06_Average()
{
	{
		auto const mean = Average(std::initializer_list<int>{});

		static_assert( std::is_same_v< std::decay_t<decltype(*mean)>, float > );

		is_True(!mean.has_value());
	}
	{
		auto const mean = Average(std::initializer_list<int>{1, 2, 3, 4});

		static_assert( std::is_same_v< std::decay_t<decltype(*mean)>, float > );

		::_identical(*mean, 2.5f);
	}
	{
		auto const mean = Average(std::initializer_list<float>{1, 2, 3, 4});

		static_assert( std::is_same_v< std::decay_t<decltype(*mean)>, float > );

		::_identical(*mean, 2.5f);
	}
	{
		auto const mean = Average(std::initializer_list<double>{1, 2, 3, 4});

		static_assert( std::is_same_v< std::decay_t<decltype(*mean)>, double > );

		::_identical(*mean, 2.5);		
	}
	{
		auto const mean
		=	Average
			(	std::initializer_list< Vector<float, 2> >
				{	Vector<float, 2>{1, 0}
				,	Vector<float, 2>{-1, 0}
				,	Vector<float, 2>{0, -1}
				,	Vector<float, 2>{0, 1}
				}
			);

		static_assert( std::is_same_v< std::decay_t<decltype(*mean)>, Vector<float, 2> > );

		::_identical(*mean, Vector<float, 2>::Zero());
	}
	{
		auto const mean
		=	Average
			(	std::initializer_list< UnitVec<float, 2> >
				{	UnitVec<float, 2>{1, 0}
				,	UnitVec<float, 2>{-1, 0}
				,	UnitVec<float, 2>{0, -1}
				,	UnitVec<float, 2>{0, 1}
				}
			);

		static_assert( std::is_same_v< std::decay_t<decltype(*mean)>, UnitVec<float, 2> > );

		is_True(!mean.has_value());
	}
}


static void _07_Position()
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

		Spatial<float, 0> S0{pos};
		Spatial<float, 1> const S1{pos, UnitVec<float, 3>::Axis<1>(), 0};
		Spatial<float, 2> S2{pos, UnitVec<float, 3>::Axis<0>(), 0};

		static_assert
		(	std::is_same_v< decltype( Position(S0) ), Vector<float, 3>& >
		&&	std::is_same_v< decltype( Position(S1) ), Vector<float, 3> const& >
		&&	std::is_same_v< decltype( Position(S2) ), Vector<float, 3>& >
		&&	std::is_same_v< decltype( Position(Spatial<float, 2>{}) ), Vector<float, 3>&& >
		);

		::_identical( pos, Position(S0), Position(S1), Position(S2), Vector<float>{1, 2, 3} );
	}
	{
		Vector<float, 3> const pos{1, 2, 3};

		Plane<float, 3> P1(pos, UnitVec<float, 3>::Axis<2>());
		Line<float, 3> L1(pos, UnitVec<float, 3>::Axis<2>());

		static_assert
		(	std::is_same_v< decltype( Position(P1) ), Vector<float, 3>& >
		&&	std::is_same_v< decltype( Position(L1) ), Vector<float, 3>& >
		&&	std::is_same_v< decltype( Position(Plane<float, 3>()) ), Vector<float, 3>&& >
		&&	std::is_same_v< decltype( Position(Line<float, 3>()) ), Vector<float, 3>&& >
		);

		::_identical(pos, Position(P1), Position(L1), Vector<float>{1, 2, 3});
	}
}


static void _08_Fitting()
{
	std::vector< Vector<float, 3> > positions
	{	Vector<float, 3>{-10.f, -1.f, -.05f}
	,	Vector<float, 3>{0.f, -1.f, 0.f}	
	,	Vector<float, 3>{10.f, -1.f, .2f}	 
	,	Vector<float, 3>{-10.f, 0.f, 0.f}	 
	,	Vector<float, 3>{0.f, 0.f, -.1f}	 
	,	Vector<float, 3>{10.f, 0.f, 0.f}	 
	,	Vector<float, 3>{ -10.f, 1.f, -.05f}	 
	,	Vector<float, 3>{0.f, 1.f, 0.f}	 
	,	Vector<float, 3>{10.f, 1.f, .05f}	 
	};

	{
		auto x = Fitting< Plane<float, 3> >(positions);

		static_assert( std::is_same_v< decltype(x), Plane<float, 3> > );

		auto const &pos = x.position();
		auto const &nml = x.normal();

		is_True
		(	(pos - Vector<float, 3>::Zero()).norm() < 0.1f
		&&	(	Direction::angle(nml, UnitVec<float, 3>::Axis<2>()) < Pi<float>/40
			||	Direction::angle(-nml, UnitVec<float, 3>::Axis<2>()) < Pi<float>/40
			)
		);
	}
	{
		auto x = Fitting< Line<float, 3> >(positions);

		static_assert( std::is_same_v< decltype(x), Line<float, 3> > );

		auto const &pos = x.position();
		auto const &tgt = x.tangent();

		is_True
		(	(pos - Vector<float, 3>::Zero()).norm() < 0.1f
		&&	(	Direction::angle(tgt, UnitVec<float, 3>::Axis<0>()) < Pi<float>/40
			||	Direction::angle(-tgt, UnitVec<float, 3>::Axis<0>()) < Pi<float>/40
			)
		);
	}
	{
		auto x = Fitting< Spatial<float, 2> >(positions);		
		
		static_assert( std::is_same_v< decltype(x), Spatial<float, 2> > );

		auto const &pos = x.position;
		auto const &nml = x.normal,  &t1 = x.tangent1,  &t2 = x.tangent2;

		is_True
		(	(pos - Vector<float, 3>::Zero()).norm() < 0.1f
		&&	(	Direction::angle(nml, UnitVec<float, 3>::Axis<2>()) < Pi<float>/40
			||	Direction::angle(-nml, UnitVec<float, 3>::Axis<2>()) < Pi<float>/40
			)
		&&	(	Direction::angle(t1, UnitVec<float, 3>::Axis<1>()) < Pi<float>/40
			||	Direction::angle(-t1, UnitVec<float, 3>::Axis<1>()) < Pi<float>/40
			)
		&&	(	Direction::angle(t2, UnitVec<float, 3>::Axis<0>()) < Pi<float>/40
			||	Direction::angle(-t2, UnitVec<float, 3>::Axis<0>()) < Pi<float>/40
			)
		);
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


void test::EuclidTutorial::test()
{
	try
	{
		::_01_Construction();
		::_02_Projection();
		::_03_Distance();
		::_04_intersection();
		::_05_Direction();
		::_06_Average();
		::_07_Position();
		::_08_Fitting();

		std::wcout << L"Euclid Test Complete.\n";
	}
	catch(sgm::spec::Exception const &e)
	{
		std::wcerr << L"Euclid Test Failed. : " << e.what() << std::endl;
	}
}