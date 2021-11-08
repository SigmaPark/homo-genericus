#include "Test_Quaternion.hpp"
#include "Sigma\Specification\Specification.hpp"


using sgm::spec::is_True;
using namespace v3d;


template<class...ARGS>
static void _identical(ARGS...args)
{
	test::_Equivalent<test::_Equiv_Quaternion_Tag>::calc(args...);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


static void _01_Construction()
{
	{	
		Quaternion<float> const
			q0,
			q1(0),
			q2(0, 0, 0, 0),
			q3(0, Vector<float, 3>::Zero()),
			q4 = q1,
			q5 = Quaternion<float>{},
			q6 = Vector<float, 3>{0, 0, 0};

		_identical(q0, q1, q2, q3, q4, q5, q6);
	}
	{
		UnitQuaternion<float> const
			uq0,
			uq1(1),
			uq2(1, 0, 0, 0),
			uq3(1, Vector<float, 3>::Zero()),
			uq4 = uq1,
			uq5 = UnitQuaternion<float>{},
			uq6 = Quaternion<float>(1);

		_identical(uq0, uq1, uq2, uq3, uq4, uq5, uq6);

		UnitQuaternion<float> const uq7 = UnitVec<float, 3>::Axis<2>();

		_identical( uq7, Quaternion<float>(0, 0, 0, 1) );
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


static void _02_Substitution()
{
	Quaternion<float> q1;

	q1 = 2;

	_identical( q1, Quaternion<float>(2, 0, 0, 0) );

	q1 = Quaternion<float>(0, 2, -1, 1);

	_identical( q1, Quaternion<float>(0, 2, -1, 1) );

	Quaternion<float> const q2(1, 1, 1, 1);

	q1 = q2;

	_identical(q1, q2);

	UnitQuaternion<float> const uq1;

	q1 = uq1;

	_identical( q1, Quaternion<float>(1, 0, 0, 0) );

	UnitQuaternion<float> uq2;

	uq2 = Quaternion<float>(0, 0, 0, .3f);

	_identical( uq2, Quaternion<float>(0, 0, 0, 1) );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


static void _03_Element()
{
	{
		Quaternion<float> q1(4,3,2,1);
	
		_identical(q1.w(), 4);  _identical(q1.x(), 3);  _identical(q1.y(), 2);  _identical(q1.z(), 1);
	
		q1.w() = 1,  q1.x() = 2,  q1.y() = 3, q1.z() = 4;
	
		_identical( q1, Quaternion<float>(1,2,3,4) );
		_identical( q1.v(), Vector<float>{2, 3, 4} );
	
		q1.v() = {-5, -6, -7};
	
		_identical( q1, Quaternion<float>(1, -5, -6, -7) );
	}
	{
		UnitQuaternion<float> uq1(1);

		_identical(uq1.w(), 1);  _identical(uq1.x(), uq1.y(), uq1.z(), 0);
		_identical(uq1.v(), Vector<float>{0, 0, 0});
	//	uq1.w() = 3;  uq1.x() = 3;  uq1.y() = 3,  uq1.z() = 3	// Error : read only 
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


static void _04_Norm()
{
	{
		Quaternion<float> q1(1, -1, -1, 1);

		float const squard_norm = 1*1 + (-1)*(-1) + (-1)*(-1) * 1*1, norm = std::sqrt(squard_norm);

		_identical( q1.sqr_norm(), squard_norm );
		_identical( q1.norm(), norm );

		Quaternion<float> const qn1 = q1 / norm;

		_identical(q1.normalized(), qn1);  
		_identical( q1, Quaternion<float>(1, -1, -1, 1) );

		q1.normalize();

		_identical(q1, qn1);
	}
	{
		UnitQuaternion<float> uq1(1, -1, -1, 1);

		_identical(uq1, Quaternion<float>(1, -1, -1, 1).normalized());
		_identical(uq1.norm(), uq1.sqr_norm(), 1);
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


static void _05_Unary_Operation()
{
	Quaternion<float> const q1(1,2,3,4);

	_identical( q1.conjugate(), Quaternion<float>(1, -2, -3, -4) );
	_identical( q1.inv(), q1.conjugate() / q1.sqr_norm() );
	_identical(+q1, q1);
	_identical( -q1, Quaternion<float>(-1, -2,- 3, -4) );

	UnitQuaternion<float> const uq1 = q1;

	_identical(uq1.inv(), uq1.conjugate(), q1.normalized().inv() );
	_identical(+uq1, uq1);
	_identical( -uq1, Quaternion<float>(-1,-2,-3,-4).normalized() );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#include "..\Euclid\Euclid.hpp"


static void _06_Algebra()
{
	{
		Quaternion<float> const q1(-1, -2, 3, 4), q2 = q1;

		_identical( q1 - q2, Quaternion<float>(0) );
		_identical(q1 + q2, 2*q1);
		_identical( (q1 + q2)/2, q1 );
	}
	{
		Quaternion<float> q1(2, 4, 6, -8);
		Quaternion<float> const q2 = q1;

		q1 += q1;

		_identical(q1, 2*q2),  q1 = q2;

		q1 -= q1;

		_identical( q1, Quaternion<float>(0) ),  q1 = q2;

		q1 /= 2;

		_identical(q1, q2/2),  q1 = q2;
	}
	{
		Quaternion<float> const q1 = UnitVec<float, 3>::Axis<0>(); 

		float const theta = v3d::Pi<float>/2;

		UnitQuaternion<float> const q2( std::cos(theta/2), UnitVec<float, 3>::Axis<2>()*std::sin(theta/2) );

		_identical( (q2*q1*q2.inv()).v(), UnitVec<float, 3>::Axis<1>() );
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


SGM_SPECIFICATION_TESTS(test::, Quaternion, Tutorial)
{	::_01_Construction
,	::_02_Substitution
,	::_03_Element
,	::_04_Norm
,	::_05_Unary_Operation
,	::_06_Algebra
};