#include "Matrix_interface.hpp"
#include <cassert>
#include "..\Reform\ReformEigen.h"

template<class T, int R, int C, int STRG>
struct Matrix_impl : Eigen::Map< Eigen::Matrix<T, R, C, STRG> >{};


class Tutorial : sgm::No_Making
{
public:

	template<unsigned> static void Case();

	template<> static void Case<1>()
	{
		using namespace sgm::mxi;

		Matrix<float, 2, 3> const mat1
		{	1, 2, 3
		,	4, 5, 6
		};

		assert(mat1.data()[1] == 4 && mat1.data()[2] == 2);

		Matrix<float> const mat2(std::vector<float>{1, 2, 3, 4}, 2, 2);

		assert(mat2.data()[1] == 3 && mat2.data()[2] == 2);

		
	}

	template<> static void Case<2>()
	{
		using namespace sgm;

		std::vector<float> vec
		{	1, 3, 5
		,	7, 9, 11
		,	13, 15, 17
		};

		auto mat 
		=	Eigen::Map< Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> >
			(	vec.data(), 3, 3
			);

		assert
		(	mat(0, 1) == 3
		&&	mat(1, 0) == 7
		&&	mat.row(0)(0) == Eigen::Vector3f(1, 3, 5)(0)
		&&	mat.row(0)(1) == Eigen::Vector3f(1, 3, 5)(1)
		&&	mat.row(0)(2) == Eigen::Vector3f(1, 3, 5)(2)
		);

		//vec.~vector();

		//assert
		//(	mat(0, 1) == 3
		//&&	mat(1, 0) == 7
		//&&	mat.row(0)(0) == Eigen::Vector3f(1, 3, 5)(0)
		//&&	mat.row(0)(1) == Eigen::Vector3f(1, 3, 5)(1)
		//&&	mat.row(0)(2) == Eigen::Vector3f(1, 3, 5)(2)
		//);

	}

	template<> static void Case<3>()
	{
	
	}


};


int main()
{
	Tutorial::Case<1>();
	Tutorial::Case<2>();
	Tutorial::Case<3>();

	return 0;
}