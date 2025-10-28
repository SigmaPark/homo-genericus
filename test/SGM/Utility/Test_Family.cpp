/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Utility/Family.hpp"
#include "Test_Family.hpp"


using sgm::h2u::Specimen;


static void intro()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Introduction")
	<<	L"sgm::Family is a generic tuple-like container that provides efficient storage and "
	<<	L"manipulation of heterogeneous types. It supports various reference types, move semantics, "
	<<	L"and compile-time type operations, making it ideal for template metaprogramming and "
	<<	L"generic programming scenarios."
	<<	empty_line;
}


static void Constructor()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Basic Construction and Copy")
	<<	L"Family can be constructed with mixed value and reference types, maintaining "
	<<	L"type safety while supporting copy operations."
	<<	newl;

BEGIN_CODE_BLOCK(constructor_ex)
	Specimen s1 = 1, s2 = 3;
	Specimen const cs1 = 5;

	sgm::Family<Specimen, Specimen&, Specimen const&> fam1(-1, s1, cs1);

	auto fam2 = fam1;

	SGM_H2U_ASSERT
	(	fam1 == sgm::Make_Family( Specimen(-1), s1, cs1 )
	&&	fam2 == sgm::Make_Family( Specimen(-1), s1, cs1 )
	);
END_CODE_BLOCK_AND_LOAD(constructor_ex)

	mdo << empty_line;
}


static void Move_Rvalue_Member()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Move Semantics with Rvalue Members")
	<<	L"Family properly handles move semantics for rvalue reference members, "
	<<	L"preserving the moved state of individual elements."
	<<	newl;

BEGIN_CODE_BLOCK(move_rvalue_ex)
	Specimen s1 = 1, s2 = 3;
	Specimen const cs1 = 5;

	sgm::Family<Specimen, Specimen&, Specimen const&, Specimen&&> fam1
	(	-1, s1, cs1, sgm::Move(s2)
	);

	auto fam2 = sgm::Move(fam1);

	SGM_H2U_ASSERT
	(	fam2 == sgm::Make_Family( Specimen(-1), s1, cs1, Specimen(3) )
	&&	std::get<0>(fam1) == Specimen::State::MOVE_AWAY
	&&	std::get<1>(fam1) == s1
	&&	std::get<2>(fam1) == cs1
	&&	std::get<3>(fam1) == 3
	);
END_CODE_BLOCK_AND_LOAD(move_rvalue_ex)

	mdo << empty_line;
}


static void Assignment()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Assignment Operations")
	<<	L"Family supports assignment operations between compatible types, "
	<<	L"properly copying values and references."
	<<	newl;

BEGIN_CODE_BLOCK(assignment_ex)
	Specimen s1 = 1, s2 = 3;
	Specimen const cs1 = 5;

	sgm::Family<Specimen, Specimen&, Specimen&> fam1( -1, s1, s2 ), fam2( -3, s2, s1 );

	fam2 = fam1;

	SGM_H2U_ASSERT( fam2 == sgm::Family<Specimen, Specimen, Specimen>{-1, 1, 1} );
END_CODE_BLOCK_AND_LOAD(assignment_ex)

	mdo << empty_line;
}


static void Move_Assignment()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Move Assignment")
	<<	L"Family supports move assignment, transferring ownership efficiently "
	<<	L"while maintaining proper state tracking for moved elements."
	<<	newl;

BEGIN_CODE_BLOCK(move_assignment_ex)
	Specimen s1 = 1, s2 = 3, s3 = 5, s4 = 7;
	Specimen const cs1 = 9;

	sgm::Family<Specimen, Specimen&, Specimen&&>
		fam1( -1, s1, sgm::Move(s2) ), fam2( -3, s4, sgm::Move(s3) );

	fam2 = sgm::Move(fam1);

	SGM_H2U_ASSERT
	(	fam2 == sgm::Family<Specimen, Specimen, Specimen>{-1, 1, 3}
	&&	std::get<0>(fam1) == Specimen::State::MOVE_AWAY
	&&	std::get<1>(fam1) == 1
	&&	std::get<2>(fam1) == Specimen::State::MOVE_AWAY
	);
END_CODE_BLOCK_AND_LOAD(move_assignment_ex)

	mdo << empty_line;
}


static void Get_and_Forward()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Element Access and Perfect Forwarding")
	<<	L"Family provides both get() and forward() methods for accessing elements, "
	<<	L"with proper type deduction based on const-ness and value category."
	<<	newl;

BEGIN_CODE_BLOCK(get_forward_ex)
	Specimen s1(5);

	sgm::Family<Specimen, Specimen&> fam1( Specimen(2), s1 );

	static_assert
	(	sgm::Boolean_And
		<	sgm::is_Same< decltype(fam1.template get<0>()), Specimen& >
		,	sgm::is_Same< decltype(fam1.template get<1>()), Specimen& >
		,	sgm::is_Same< decltype( sgm::immut(fam1).template get<0>() ), Specimen const& >
		,	sgm::is_Same< decltype( sgm::immut(fam1).template get<1>() ), Specimen& >
		,	sgm::is_Same< decltype( sgm::Move(fam1).template get<0>() ), Specimen&& >
		,	sgm::is_Same< decltype( sgm::Move(fam1).template get<1>() ), Specimen& >
		>::	value
	,	""
	);

	static_assert
	(	sgm::Boolean_And
		<	sgm::is_Same< decltype(fam1.template forward<0>()), Specimen&& >
		,	sgm::is_Same< decltype(fam1.template forward<1>()), Specimen& >
		,	sgm::is_Same< decltype( sgm::immut(fam1).template forward<0>() ), Specimen const&& >
		,	sgm::is_Same< decltype( sgm::immut(fam1).template forward<1>() ), Specimen& >
		,	sgm::is_Same< decltype( sgm::Move(fam1).template forward<0>() ), Specimen&& >
		,	sgm::is_Same< decltype( sgm::Move(fam1).template forward<1>() ), Specimen& >
		>::	value
	,	""
	);
END_CODE_BLOCK_AND_LOAD(get_forward_ex)

	mdo << empty_line;
}


static void Convenience_Member_Functions()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Convenience Member Functions")
	<<	L"Like std::pair's .first and .second, Family provides convenient member functions "
	<<	L".first(), .second(), .third(), and .fourth() for easy access to elements instead of "
	<<	L"the more verbose std::get<Index>(family) syntax. These functions are only compiled "
	<<	L"when the Family has the corresponding number of elements."
	<<	newl;

BEGIN_CODE_BLOCK(convenience_members_ex)
	// Demonstrate .first() and .second() with a 2-element Family
	sgm::Family<int, double> duo(42, 3.14);

	SGM_H2U_ASSERT(duo.first() == 42 && duo.second() == 3.14);

	// Modify values through convenience functions
	duo.first() = 100;
	duo.second() = 2.718;

	SGM_H2U_ASSERT(duo.first() == 100 && duo.second() == 2.718);

	// Demonstrate .first(), .second(), .third() with a 3-element Family
	sgm::Family<int, double, wchar_t> trio(1, 2.5, L'A');

	SGM_H2U_ASSERT(trio.first() == 1 && trio.second() == 2.5 && trio.third() == L'A');

	// Type safety and const-correctness
	static_assert
	(	sgm::is_Same<decltype(trio.first()), int&>::value
	&&	sgm::is_Same<decltype(trio.second()), double&>::value
	&&	sgm::is_Same<decltype(trio.third()), wchar_t&>::value
	&&	sgm::is_Same<decltype(sgm::immut(trio).first()), int const&>::value
	&&	sgm::is_Same<decltype(sgm::immut(trio).second()), double const&>::value
	&&	sgm::is_Same<decltype(sgm::immut(trio).third()), wchar_t const&>::value
	);

	// Demonstrate all four convenience functions with a 4-element Family
	sgm::Family<int, double, wchar_t, bool> quartet(10, 1.5, L'Z', true);

	SGM_H2U_ASSERT
	(	quartet.first() == 10
	&&	quartet.second() == 1.5
	&&	quartet.third() == L'Z'
	&&	quartet.fourth() == true
	);

	// Compare convenience syntax vs std::get syntax
	SGM_H2U_ASSERT
	(	quartet.first() == std::get<0>(quartet)
	&&	quartet.second() == std::get<1>(quartet)
	&&	quartet.third() == std::get<2>(quartet)
	&&	quartet.fourth() == std::get<3>(quartet)
	);
END_CODE_BLOCK_AND_LOAD(convenience_members_ex)

	mdo << empty_line;
}


static void Merge_Family_Test()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Merging Families")
	<<	L"Multiple Family instances can be merged into a single Family, "
	<<	L"preserving the original type signatures and reference semantics."
	<<	newl;

BEGIN_CODE_BLOCK(merge_family_ex)
	Specimen s1 = 1, s2 = 3;

	sgm::Family<Specimen&, Specimen> fam1(s2, 3);
	sgm::Family<Specimen, Specimen&&> fam2( -4, sgm::Move(s1) );


	auto fam3 = sgm::Merge_Families(fam1, fam2);

	static_assert
	(	sgm::is_Same
		<	decltype(fam3), sgm::Family<Specimen&, Specimen, Specimen, Specimen&&>
		>::	value
	,	""
	);

	SGM_H2U_ASSERT(fam3 == sgm::Family<Specimen, Specimen, Specimen, Specimen>{s2, 3, -4, 1});
END_CODE_BLOCK_AND_LOAD(merge_family_ex)

	mdo << empty_line;
}


static void Harden_Test()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Hardening References")
	<<	L"The Harden operation converts rvalue references to values, "
	<<	L"materializing temporary objects while preserving lvalue references."
	<<	newl;

BEGIN_CODE_BLOCK(harden_ex)
	Specimen s1 = 1, s2 = 3;
	sgm::Family<Specimen&, Specimen&&> fam1( s1, sgm::Move(s2) );

	auto fam2 = sgm::Harden(fam1);

	static_assert
	(	sgm::is_Same< decltype(fam2), sgm::Family<Specimen&, Specimen> >::value
	,	""
	);

	SGM_H2U_ASSERT
	(	std::get<0>(fam2) == s1
	&&	std::get<1>(fam2) == 3
	&&	s2 == Specimen::State::MOVE_AWAY
	);
END_CODE_BLOCK_AND_LOAD(harden_ex)

	mdo << empty_line;
}


static void As_it_is_Test()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"As-It-Is Forwarding")
	<<	L"The as_it_is marker preserves values unchanged during forwarding, "
	<<	L"allowing selective application of transformations in tuple assignments."
	<<	newl;

BEGIN_CODE_BLOCK(as_it_is_ex)
	Specimen s1 = 1, s2 = 3;

	sgm::Tie(s1, s2) = sgm::Forward_as_Family(5, sgm::as_it_is);

	SGM_H2U_ASSERT( s1 == Specimen(5) && s2 == 3 );
END_CODE_BLOCK_AND_LOAD(as_it_is_ex)

	mdo << empty_line;
}


static void Structured_Binding()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Structured Binding Support")
	<<	L"Family provides full support for C++17 structured bindings, "
	<<	L"enabling intuitive decomposition of Family instances into individual variables."
	<<	newl;

BEGIN_CODE_BLOCK(structured_binding_ex)
#if SGM_CXX_STANDARD >= 2017
	{
		auto&& [a, b, c] = sgm::Family<int, double, int*>( 3, 0.14, (int*)nullptr );

		static_assert
		(	sgm::is_Same<decltype(a), int>::value
		&&	sgm::is_Same<decltype(b), double>::value
		&&	sgm::is_Same<decltype(c), int*>::value
		);
	}
	{
		int x = 0,  *px = &x;
		double const y = 5.5;

		auto&& [t1, t2, t3, t4, t5, t6]
		=	sgm::Forward_as_Family(3, 0.14, x, y, &x, px);

		static_assert
		(	sgm::is_Same<decltype(t1), int&&>::value
		&&	sgm::is_Same<decltype(t2), double&&>::value
		&&	sgm::is_Same<decltype(t3), int&>::value
		&&	sgm::is_Same<decltype(t4), double const&>::value
		&&	sgm::is_Same<decltype(t5), int*&&>::value
		&&	sgm::is_Same<decltype(t6), int*&>::value
		);
	}
	{
		auto duo = sgm::Family<int, double>(3, 3.14);
		auto [a, b] = duo;

		static_assert
		(	sgm::is_Same<decltype(a), int>::value
		&&	sgm::is_Same<decltype(b), double>::value
		&&	sgm::is_Same<decltype(duo.first()), int&>::value
		&&	sgm::is_Same<decltype(duo.second()), double&>::value
		);
	}
	{
		auto trio = sgm::Family<int, double, wchar_t>(3, 3.14, ' ');
		auto [a, b, c] = trio;

		static_assert
		(	sgm::is_Same<decltype(a), int>::value
		&&	sgm::is_Same<decltype(b), double>::value
		&&	sgm::is_Same<decltype(c), wchar_t>::value
		&&	sgm::is_Same<decltype(trio.first()), int&>::value
		&&	sgm::is_Same<decltype(trio.second()), double&>::value
		&&	sgm::is_Same<decltype(trio.third()), wchar_t&>::value
		);
	}
#endif
END_CODE_BLOCK_AND_LOAD(structured_binding_ex)

	mdo << empty_line;
}


static void outro()
{
	using namespace sgm::h2u;

	mdo << empty_line;
}


SGM_HOW2USE_TESTS(sgm::h2u::Test_, Family, /**/)
{	::intro
,	::Constructor
,	::Move_Rvalue_Member
,	::Assignment
,	::Move_Assignment
,	::Get_and_Forward
,	::Convenience_Member_Functions
,	::Merge_Family_Test
,	::Harden_Test
,	::As_it_is_Test
,	::Structured_Binding
,	::outro
};