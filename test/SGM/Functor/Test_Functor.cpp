/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Functor/Functor.hpp"
#include "Test_Functor.hpp"
#include <cmath>


using sgm::h2u::Specimen;


struct Test : sgm::Unconstructible
{
	static void Lambda();
	static void Functor();
	static void Blank();
	static void Composition();
	static void Permutation();
	static void Basel_Problem();
};
//========//========//========//========//=======#//========//========//========//========//=======#


void Test::Lambda()
{
	int x = 5;
	Specimen s{20};

	SGM_H2U_ASSERT
	(	SGM_LAMBDA( 10*(_0 + _1) - _2.value() )(x, 3, s) 
	==	10*(x + 3) - s.value()
	);
}


void Test::Functor()
{
	{
		int x = 5;
		Specimen s{20};

		sgm::fp::Functor ftr = SGM_LAMBDA( 10*(_0 + _1) - _2.value() );

		SGM_H2U_ASSERT( ftr(x, 3, s) == 10*(x + 3) - s.value() );
	}
	{
		int x = 5;
		Specimen s{20};

		sgm::fp::Functor ftr = [x, &s](int y)-> int{  return 10*(x + y) - s.value();  };

		SGM_H2U_ASSERT( ftr(3) == 10*(x + 3) - s.value() );		
	}
}


void Test::Blank()
{
	using namespace sgm::fp;

	sgm::fp::Functor ftr = SGM_LAMBDA(100*_0 + 10*_1 + _2);

	SGM_H2U_ASSERT
	(	ftr(_, 3, 5)(1) == 135
	&&	ftr(1, _, _)(3, 5) == 135
	&&	ftr(_, _, 5)(1, 3) == 135
	&&	ftr(_, 3, _)(1, 5) == 135
	);
}


void Test::Composition()
{
	{
		sgm::fp::Functor ftr1 = SGM_LAMBDA(10*_0 + _1);
		sgm::fp::Functor ftr2 = SGM_LAMBDA(-10*_0);
		auto composed_ftr = ftr2*ftr1;

		SGM_H2U_ASSERT( composed_ftr(4, 2) == -10*(10*4 + 2) );
	}
	{
		sgm::Array<Specimen> arr{Specimen(1), Specimen(2), Specimen(3)};
		sgm::fp::Functor func = SGM_LAMBDA(2 * _0) * SGM_LAMBDA(_0.value());

		auto const res = sgm::fp::Morph_f(arr, func);

		SGM_H2U_ASSERT
		(	sgm::h2u::Are_Equivalent_Ranges
			(	res
			,	sgm::Array<int, 3>{2, 4, 6}
			)
		);
	}
	{
		sgm::Array<Specimen> arr{Specimen(1), Specimen(2), Specimen(3)};

		auto const res
		=	sgm::fp::Morph_f
			(	arr
			,	( SGM_LAMBDA(2*_0) * SGM_LAMBDA(_0.value()) ).eval()
			);

		SGM_H2U_ASSERT
		(	sgm::h2u::Are_Equivalent_Ranges
			(	res
			,	sgm::Array<int, 3>{2, 4, 6}
			)
		);
	}
}


void Test::Permutation()
{
	int const x = 5;
	double y = 3.2;

	auto&& [a0, a1, a2] = sgm::fp::Permute<1, 2, 0>(x, y, L'a');
	
	static_assert
	(	sgm::is_Same<decltype(a0), double&>::value 
	&&	sgm::is_Same<decltype(a1), wchar_t&&>::value
	&&	sgm::is_Same<decltype(a2), int const&>::value
	);
}


void Test::Basel_Problem()
{
	using namespace sgm::fp;

	std::size_t constexpr N = 10000;

	auto ftr
	=	(	Fold_f( _, SGM_LAMBDA(_0 + _1) )
		*	Morph_f(  _, ( SGM_LAMBDA(1.0/_0) * SGM_LAMBDA(_0*_0) ).eval()  )
		*	integers
		) .	eval();

	auto const res = ftr(N, 1);

	double const 
		pi = std::acos(0)*2,
		answer = pi*pi/6;

	SGM_H2U_ASSERT( std::abs(res - answer) < 1e-4 );
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_HOW2USE_TESTS(sgm::fp::h2u::Test_, Functor, /**/)
{	::Test::Lambda
,	::Test::Functor
,	::Test::Blank
,	::Test::Composition
,	::Test::Permutation
,	::Test::Basel_Problem
};