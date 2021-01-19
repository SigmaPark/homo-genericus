#include "Affine.hpp"
#include "..\Specification\Specification.hpp"


using namespace sgm;

using 
	mxi::Vector, mxi::Matrix,
	mxi::AffineTransform, mxi::Affine,
	spec::is_True;


template<class T1, class T2>
static bool is_same_pos(T1&& t1, T2&& t2)
{
	bool res = true;

	for(unsigned i = 0;  res && i < 3;  ++i)
		res = R_Comp( t1(i), t2(i), .000001f );

	return res;
}


template<unsigned>
void Test();


template<>
void Test<1>()
{
	AffineTransform<float, 3> aft
	(	Matrix<float, 3, 3>
		{	0, -1, 0
		,	1, 0, 0
		,	0, 0, 1
		} 
	,	Vector<float>{0, 1, 0}
	);

	is_True
	(	is_same_pos(	aft.transfer(Vector<float>{1, 0, 0}), Vector<float>{0, 2, 0}  )
	&&	is_same_pos
		(	aft.inversed().transfer(Vector<float>{0, 2, 0}), Vector<float>{1, 0, 0}  
		)
	&&	is_same_pos
		(	Affine::inverse(aft).transfer(Vector<float>{0, 2, 0}), Vector<float>{1, 0, 0}  
		)
	);

	AffineTransform<float, 3> aft2;

	is_True
	(	is_same_pos(  aft2.transfer(Vector<float>{1, 0, 0}), Vector<float>{1, 0, 0}  )
	&&	is_same_pos(  aft2.transfer(Vector<float>{1, 1, -1}), Vector<float>{1, 1, -1}  )
	&&	is_same_pos(  aft2(aft).transfer(Vector<float>{1, 0, 0}), Vector<float>{0, 2, 0}  )
	);
}


#include "Test_Affine.hpp"
#include <iostream>


void Test_sgm_Affine::test()
{
	try
	{
		::Test<1>();

		std::wcout << L"Affine Test Complete.\n";
	}
	catch(...)
	{
		std::wcerr << L"Affine Test Failed.\n";

		throw;
	}
}