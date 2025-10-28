/*  SPDX-FileCopyrightText: (c) 2023 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Wrapper/Convoy.hpp"
#include "Test_Convoy.hpp"
#include "SGM/TMP/interface_Traits.hpp"


using sgm::h2u::Specimen;


static void intro()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Introduction")
	<<	L"sgm::Convoy is a move-only wrapper designed to address a fundamental limitation in C++ move semantics. "
	<<	L"Traditional C++ move operations require mutable rvalue references (Type&&), which implies that the object "
	<<	L"can be modified before ownership transfer. However, there are scenarios where we want to transfer ownership "
	<<	L"while maintaining read-only access until the transfer occurs."
	<<	newl
	<<	LR"(
			void foo(Type&& t)
			{
				// only read from t here
				process_readonly(t);

				// transfer ownership
				bar(std::move(t));
			}
		)"_code
	<<	L"Consider the above function where foo only reads from t before passing ownership to another function. "
	<<	L"While 'Type const&&' would express the read-only intent, it prevents the subsequent move operation from "
	<<	L"working properly. C++ move semantics inherently assume mutability."
	<<	newl
	<<	L"sgm::Convoy separates the concepts of mutability and ownership transfer. It enables read-only access "
	<<	L"combined with efficient ownership transfer, providing a clean solution for scenarios where immutable "
	<<	L"objects need to be moved through a processing pipeline without unnecessary copying."
	<<	empty_line;
}


static void Move_Only_Construction()
{
	using namespace sgm::h2u;

	mdo << Title(L"Move-Only Construction");

	mdo
	<<	Title(L"Copy Construction Disabled", 2)
	<<	L"Convoy explicitly disables copy construction from const references, "
	<<	L"enforcing move-only semantics at compile time."
	<<	newl;

BEGIN_CODE_BLOCK(copy_disabled_ex)
	static_assert
	(	!sgm::Has_Operator_New< sgm::Convoy<Specimen>, Specimen const& >::value
	,	""
	);
END_CODE_BLOCK_AND_LOAD(copy_disabled_ex)

	mdo
	<<	Title(L"Move Construction", 2)
	<<	L"Convoy takes ownership of objects through move semantics, leaving the "
	<<	L"original object in a moved-from state."
	<<	newl;

BEGIN_CODE_BLOCK(move_construction_ex)
	Specimen s{3};

	sgm::Convoy<Specimen> cvy = sgm::Move(s);

	SGM_H2U_ASSERT
	(	cvy == 3
	&&	s == Specimen::State::MOVE_AWAY
	);
END_CODE_BLOCK_AND_LOAD(move_construction_ex)

	mdo << empty_line;
}


static void outro()
{

}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_HOW2USE_TESTS(sgm::h2u::Test_, Convoy, /**/)
{	::intro
,	::Move_Only_Construction
,	::outro
};