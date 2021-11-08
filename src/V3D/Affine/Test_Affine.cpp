#include "Test_Affine.hpp"
#include "Sigma\Specification\Specification.hpp"


using sgm::spec::is_True;
using namespace v3d;


template<class...TYPES>
static void _identical(TYPES...types)
{
	is_True( test::_Equivalent<test::_Equiv_Affine_Tag>::calc(types...) );
}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


static void _01_Construction()
{
	{
		Affine_Transform<float, 3> const
			atr1,
			atr2(Matrix<float, 3, 3>::identity(), Vector<float, 3>::Zero()),
			atr3 = atr1,
			atr4 = Affine_Transform<float, 3>{},
			atr5 = Scalable_Body_Transform<float, 3>{};

		::_identical
		(	atr1, atr2, atr3, atr4, atr5
		,	Affine_Transform<float, 3>(Matrix<float, 3, 3>::identity(), Vector<float, 3>::Zero())
		);
	}
	{
		Scalable_Body_Transform<float, 3> const
			sbtr1,
			sbtr2(Matrix<float, 3, 3>::identity(), Vector<float, 3>::Zero(), 1),
			sbtr3 = sbtr1,
			sbtr4 = Scalable_Body_Transform<float, 3>{};

		::_identical
		(	sbtr1, sbtr2, sbtr3, sbtr4
		,	Scalable_Body_Transform<float, 3>(Matrix<float, 3, 3>::identity(), Vector<float, 3>::Zero(), 1)
		);
	}
	{
		Rigid_Body_Transform<float, 3> const
			rbtr1,
			rbtr2(OrthogonalMat<float, 3>::identity(), Vector<float, 3>::Zero()),
			rbtr3 = rbtr1,
			rbtr4 = Rigid_Body_Transform<float, 3>{};

		::_identical
		(	rbtr1, rbtr2, rbtr3, rbtr4
		,	Rigid_Body_Transform<float, 3>(OrthogonalMat<float, 3>::identity(), Vector<float, 3>::Zero())
		);
	}
}


static void _02_Composition_and_Transfer()
{
	{
		Affine_Transform<float, 2> const
			atr1
			(	Matrix<float, 2, 2>
				{	0, -1
				,	1, 0
				}
			,	Vector<float, 2>{1, 0}
			),
			atr2
			(	2*Matrix<float, 2, 2>::identity()
			,	Vector<float, 2>{0, -1}
			);

		::_identical
		(	Vector<float>{1, 0} >> atr1 >> atr2
		,	Vector<float>{2, 1}
		);

		Vector<float, 2> Vec1{1, 0};

		Vec1 >>= atr1 >> atr2;

		::_identical(Vec1, Vector<float>{2, 1});
	}
	{
		Scalable_Body_Transform<float, 2> const
			sbtr1
			(	OrthogonalMat<float, 2>
				{	0, -1
				,	1, 0
				}
			,	Vector<float, 2>{1, 0}
			,	1
			),
			sbtr2
			(	OrthogonalMat<float, 2>::identity()
			,	Vector<float, 2>{0, -1}
			,	2
			);

		::_identical
		(	Vector<float>{1, 0} >> sbtr1 >> sbtr2
		,	Vector<float>{2, 1}
		);

		Vector<float, 2> Vec1{1, 0};

		Vec1 >>= sbtr1 >> sbtr2;

		::_identical(Vec1, Vector<float>{2, 1});
	}
	{
		Rigid_Body_Transform<float, 2> const
			rbtr1 = Afn<float, 2>.rotate(Pi<float>/2).translate(1, 0),
			rbtr2 = Afn<float, 2>.translate(0, -1);

		::_identical
		(	Vector<float>{1, 0} >> rbtr1.scale(2) >> rbtr2
		,	Vector<float>{2, 1}
		);

		Vector<float, 2> Vec1{1, 0};

		Vec1
		>>=	Afn<float, 2>
			.	rotate(Pi<float>/2)
			.	translate(1, 0)
			.	scale(2)
			.	translate(0, -1);

		::_identical(Vec1, Vector<float>{2, 1});
	}
}


static void _03_Composition_and_Transfer_2()
{
	{
		Vector<float, 3> const v1{1, 0, 0};

		::_identical
		(	v1
			>>	Afn<float, 3>
				.	translate(Vector<float>{0, 2, 0})
				.	rotate(UnitVec<float, 3>{1, 1, 1}, Pi<float>*2.f/3.f)
			>>	Afn<float, 3>.translate(2, 0, 0).inv()
		,	Vector<float, 3>{-2, 1, 2}
		);
	}
	{
		std::vector< Vector<float, 3> > iterable
		{	Vector<float, 3>{1, 0, 0}
		,	Vector<float, 3>{0, 2, 0}
		,	Vector<float, 3>{0, 0, 3}
		};

		iterable >>= Afn<float, 3>.rotate(UnitVec<float, 3>{1, 1, 1}, Pi<float>*2.f/3.f);

		::_identical( iterable.at(0), Vector<float, 3>{0, 1, 0} );
		::_identical( iterable.at(1), Vector<float, 3>{0, 0, 2} );
		::_identical( iterable.at(2), Vector<float, 3>{3, 0, 0} );
	}
}


static void _04_Reflection()
{
	{
		Vector<float, 3> const v1{1, 1, 1};

		::_identical( v1 >> Afn<float, 3>.reflect(UnitVec<float, 3>{1, 1, 1}), -v1 );

		::_identical
		(	v1 >> Afn<float, 3>.reflect( Plane<float, 3>(Vector<float>{1, 1, 1}/2, UnitVec<float, 3>{1, 1, 1}) )
		,	Vector<float, 3>::Zero()
		);
	}
}


static void _05_Euler_Angles()
{
	float const alpha = Pi<float>/5.f,  beta = Pi<float>/7.f,  gamma = Pi<float>/8.f;

	float const
		ca = std::cos(alpha),  cb = std::cos(beta),  cg = std::cos(gamma),
		sa = std::sin(alpha),  sb = std::sin(beta),  sg = std::sin(gamma);

	Matrix<float, 3, 3> const
		Rx
		{	1.f, 0.f, 0.f
		,	0.f, ca, -sa
		,	0.f, sa, ca
		},
		Ry
		{	cb, 0.f, sb
		,	0.f, 1.f, 0.f
		,	-sb, 0.f, cb
		},
		Rz
		{	cg, -sg, 0.f
		,	sg, cg, 0.f
		,	0.f, 0.f, 1.f
		},
		A
		{	cg*cb, -sg*ca + cg*sb*sa, sg*sa + cg*sb*ca
		,	sg*cb, cg*ca + sg*sb*sa, -cg*sa + sg*sb*ca
		,	-sb, cb*sa, cb*ca
		},
		B = Rotation<float, 3>(alpha, beta, gamma).cortho_mat();

		::_identical(Rz*Ry*Rx, A, B);
}


SGM_SPECIFICATION_TESTS(test::, Affine, Tutorial)
{	::_01_Construction
,	::_02_Composition_and_Transfer
,	::_03_Composition_and_Transfer_2
,	::_04_Reflection
,	::_05_Euler_Angles
};