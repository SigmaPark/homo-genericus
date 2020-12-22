#include "Quaternion.hpp"
#include "..\Specification\Specification.hpp"
#include "..\idiom\idiom.hpp"


using sgm::mxi::Quaternion;
using sgm::spec::is_True;
using sgm::R_Comp;


template<unsigned>
static void Test();


template<class Q1, class Q2>
static bool Equal_qtn(Q1 q1, Q2 q2)
{
	static auto constexpr eps = (typename Q1::elem_t)1e-7;

	return 
	(	R_Comp(q1.w(), q2.w(), eps) && R_Comp(q1.x(), q2.x(), eps)
	&&	R_Comp(q1.y(), q2.y(), eps) && R_Comp(q1.z(), q2.z(), eps)
	);
}


template<>
void Test<1>()
{
	Quaternion<float> q1, q2(1,3,4,9), q3(-2, sgm::mxi::Vector<float>{0, -1, -2});
	Quaternion<double> const cq1(q1);


	is_True
	(	::Equal_qtn( q1, Quaternion(0, 0, 0, 0) )
	&&	::Equal_qtn( q2, Quaternion(1, 3, 4, 9) )
	&&	::Equal_qtn( q3, Quaternion(-2, 0, -1, -2) )
	&&	::Equal_qtn(q1, cq1)
	);
}


template<>
void Test<2>()
{
	Quaternion q1, q2(4, 2, 3, 0);

	q1 = q2;

	is_True( ::Equal_qtn(q1, q2) );

	q1 = 52;

	is_True(  ::Equal_qtn( q1, Quaternion(52) )  );

	q1.w() = -2;
}


template<>
void Test<3>()
{
	Quaternion q1(3, 4, -2, 5);

	is_True
	(	::Equal_qtn( -q1, Quaternion(-q1.w(), -q1.v()) )
	&&	::Equal_qtn( q1*q1.inversed(), Quaternion(1) )
	);
}


template<>
void Test<4>()
{
	Quaternion q1, q2(-1, -2, 3, 4);

	q1 = q2;

	is_True
	(	::Equal_qtn( q1 - q2, Quaternion(0) )
	&&	::Equal_qtn( q1 + q2, 2*q1)
	&&	::Equal_qtn( (q1 + q2)/2, q1 )
	);
}


#include "Test_Quaternion.hpp"
#include <iostream>


void Test_sgm_Quaternion::test()
{
	try
	{
		::Test<1>();
		::Test<2>();
		::Test<3>();
		::Test<4>();


		std::wcout << L"Quaternion Test Complete.\n";
	}
	catch(...)
	{
		std::wcerr << L"Quaternion Test Failed.\n";

		throw;
	}
}