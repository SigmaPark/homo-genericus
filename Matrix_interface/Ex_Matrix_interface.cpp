#include "Matrix_interface.hpp"
#include "..\Reform\ReformEigen.h"

#include <vector>
#include <array>

namespace sgm::mxi
{
	
	template<class T, int R, int C, int STRG>
	struct Matrix_impl : No_Making
	{
		template<class PDATA> static decltype(auto) calc
		(	PDATA pdata, [[maybe_unused]]size_t r, [[maybe_unused]]size_t c
		)
		{
			if constexpr(R != MatSize::DYNAMIC && C != MatSize::DYNAMIC)
				return
				Eigen::Map
				<	std::conditional_t
					<	is_immutable<PDATA>::value
					,	Eigen::Matrix<T, R, C, STRG> const
					,	Eigen::Matrix<T, R, C, STRG>
					>
				>
				(pdata);
			else
				return
				Eigen::Map
				<	std::conditional_t
					<	is_immutable<PDATA>::value
					,	Eigen::Matrix<T, -1, -1, STRG> const
					,	Eigen::Matrix<T, -1, -1, STRG>
					>
				>
				(pdata, r, c);
		}
	};


	template<class T, int R, int C>
	struct internalData
	{
		using type 
		=	std::conditional_t
			<	R == MatSize::DYNAMIC || C == MatSize::DYNAMIC
			,	std::vector<T>, std::array<T, R*C>
			>;
	};

}
//========//========//========//========//=======#//========//========//========//========//=======#


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

		mat = mat * mat;

		assert(vec.front() == 87);
	}

	template<> static void Case<3>()
	{
		using namespace sgm::mxi;

		Matrix_<float> const M1
		(	std::vector<float>
			{	1, 2
			,	3, 4
			}
		,	2, 2
		);

		Matrix<float> r0 = M1.row(0);

		assert(r0.rows() == 2 && r0.cols() == 1);
	}


};


int main()
{
	Tutorial::Case<1>();
	Tutorial::Case<2>();
	Tutorial::Case<3>();

	return 0;
}

