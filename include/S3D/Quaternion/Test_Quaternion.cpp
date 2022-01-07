/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Quaternion.hpp"
#include "../Euclid/Euclid.hpp"


using sgm::spec::is_True;
using s3d::Vector;
using s3d::UnitVec;


template<class...ARGS>
static void _identical(ARGS...args)
{
	is_True( s3d::spec::_Equivalent<s3d::spec::_Equiv_Quaternion_Tag>::calc(args...) );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void Construction()
{
	{	
		s3d::Quaternion<float> const
			q0,
			q1(0),
			q2(0, 0, 0, 0),
			q3(0, Vector<float, 3>::Zero()),
			q4 = q1,
			q5 = s3d::Quaternion<float>{},
			q6 = Vector<float, 3>{0, 0, 0};

		_identical(q0, q1, q2, q3, q4, q5, q6);
	}
	{
		s3d::UnitQuaternion<float> const
			uq0,
			uq1(1),
			uq2(1, 0, 0, 0),
			uq3(1, Vector<float, 3>::Zero()),
			uq4 = uq1,
			uq5 = s3d::UnitQuaternion<float>{},
			uq6 = s3d::Quaternion(1);

		_identical(uq0, uq1, uq2, uq3, uq4, uq5, uq6);

		s3d::UnitQuaternion<float> const uq7 = UnitVec<float, 3>::Axis<2>();

		static_assert( std::is_same_v<decltype(uq7), s3d::UnitQuaternion<float> const> );

		_identical( uq7, s3d::Quaternion(0, 0, 0, 1) );
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void Substitution()
{
	s3d::Quaternion<float> q1;

	q1 = 2;

	_identical( q1, s3d::Quaternion(2, 0, 0, 0) );

	q1 = s3d::Quaternion(0, 2, -1, 1);

	_identical( q1, s3d::Quaternion(0, 2, -1, 1) );

	s3d::Quaternion const q2(1, 1, 1, 1);

	q1 = q2;

	_identical(q1, q2);

	s3d::UnitQuaternion<float> const uq1;

	q1 = uq1;

	_identical( q1, s3d::Quaternion(1, 0, 0, 0) );

	s3d::UnitQuaternion<float> uq2;

	uq2 = s3d::Quaternion(0.f, 0.f, 0.f, .3f);

	_identical( uq2, s3d::Quaternion(0, 0, 0, 1) );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void Element()
{
	{
		s3d::Quaternion q1(4, 3, 2, 1);
	
		_identical(q1.w(), 4);  _identical(q1.x(), 3);  
		_identical(q1.y(), 2);  _identical(q1.z(), 1);
	
		q1.w() = 1,  q1.x() = 2,  q1.y() = 3, q1.z() = 4;
	
		_identical( q1, s3d::Quaternion(1, 2, 3, 4) );
		_identical( q1.v(), Vector<float>{2, 3, 4} );
	
		q1.v() = {-5, -6, -7};
	
		_identical( q1, s3d::Quaternion(1, -5, -6, -7) );
	}
	{
		s3d::UnitQuaternion uq1(1);

		_identical(uq1.w(), 1);  _identical(uq1.x(), uq1.y(), uq1.z(), 0);
		_identical(uq1.v(), Vector<float>{0, 0, 0});
	//	uq1.w() = 3;  uq1.x() = 3;  uq1.y() = 3,  uq1.z() = 3	// Error : read only 
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void Norm()
{
	{
		s3d::Quaternion q1(1, -1, -1, 1);

		float const 
			squard_norm = 1*1 + (-1)*(-1) + (-1)*(-1) + 1*1, 
			norm = std::sqrt(squard_norm);

		_identical( q1.sqr_norm(), squard_norm );
		_identical( q1.norm(), norm );

		s3d::Quaternion const qn1 = q1 / norm;

		_identical(q1.normalized(), qn1);  
		_identical( q1, s3d::Quaternion(1, -1, -1, 1) );

		q1.normalize();

		_identical(q1, qn1);
	}
	{
		s3d::UnitQuaternion uq1(1, -1, -1, 1);

		_identical(uq1, s3d::Quaternion(1, -1, -1, 1).normalized());
		_identical(uq1.norm(), uq1.sqr_norm(), 1);
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void Unary_Operation()
{
	s3d::Quaternion const q1(1, 2, 3, 4);

	_identical( q1.conjugate(), s3d::Quaternion(1, -2, -3, -4) );
	_identical( q1.inv(), q1.conjugate() / q1.sqr_norm() );
	_identical(+q1, q1);
	_identical( -q1, s3d::Quaternion(-1, -2,- 3, -4) );

	s3d::UnitQuaternion const uq1 = q1;

	_identical(uq1.inv(), uq1.conjugate(), q1.normalized().inv() );
	_identical(+uq1, uq1);
	_identical( -uq1, s3d::Quaternion(-1, -2, -3, -4).normalized() );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void Algebra()
{
	{
		s3d::Quaternion const q1(-1, -2, 3, 4), q2 = q1;

		_identical( q1 - q2, s3d::Quaternion(0) );
		_identical(q1 + q2, 2*q1);
		_identical( (q1 + q2)/2, q1 );
	}
	{
		s3d::Quaternion q1(2, 4, 6, -8);
		s3d::Quaternion const q2 = q1;

		q1 += q1;

		_identical(q1, 2*q2),  q1 = q2;

		q1 -= q1;

		_identical( q1, s3d::Quaternion(0) ),  q1 = q2;

		q1 /= 2;

		_identical(q1, q2/2),  q1 = q2;
	}
	{
		s3d::Quaternion const q1 = UnitVec<float, 3>::Axis<0>(); 

		float const theta = s3d::Pi<float>/2;

		s3d::UnitQuaternion const q2
		(	std::cos(theta/2)
		,	UnitVec<float, 3>::Axis<2>()*std::sin(theta/2) 
		);

		_identical( (q2*q1*q2.inv()).v(), UnitVec<float, 3>::Axis<1>() );
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


SGM_SPECIFICATION_TEST(s3d::spec::Test_, Quaternion, /**/)
{	::Construction
,	::Substitution
,	::Element
,	::Norm
,	::Unary_Operation
,	::Algebra
};