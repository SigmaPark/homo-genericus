/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Abbreviable/Avatar.hpp"
#include "Test_Avatar.hpp"
#include "SGM/interface_Traits/interface_Traits.hpp"


using sgm::h2u::Specimen;


static void intro()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Introduction")
	<<	L"sgm::Avatar is a reference wrapper that provides safe, typed access to objects. "
	<<	L"It acts as a lightweight proxy that forwards operations to the referenced object "
	<<	L"while maintaining type safety and const-correctness."
	<<	empty_line;
}


static void Basic_Usage()
{
	using namespace sgm::h2u;

	mdo << Title(L"Basic Avatar Usage");

	mdo
	<<	Title(L"Creating References", 2)
	<<	L"Avatar can be created using sgm::Refer() for mutable references or sgm::CRefer() "
	<<	L"for const references to existing objects."
	<<	newl;

BEGIN_CODE_BLOCK(basic_reference_ex)
	Specimen s_basic(2);
	auto avt = sgm::Refer(s_basic);

	s_basic = 22;

	SGM_H2U_ASSERT(avt == s_basic && s_basic == 22);
END_CODE_BLOCK_AND_LOAD(basic_reference_ex)

	mdo
	<<	Title(L"Const References", 2)
	<<	L"Avatar maintains const-correctness, preventing modification of const-referenced objects."
	<<	newl;

BEGIN_CODE_BLOCK(const_reference_ex)
	Specimen s_const(2);
	Specimen const cs(4);

	auto cavt_const = sgm::Refer(cs);

	//cavt_const = 44;	// Compile Ban

	SGM_H2U_ASSERT(cavt_const == cs);
END_CODE_BLOCK_AND_LOAD(const_reference_ex)

	mdo
	<<	Title(L"Explicit Const References", 2)
	<<	L"The sgm::CRefer() function creates const references explicitly, providing compile-time "
	<<	L"guarantees about immutability."
	<<	newl;

BEGIN_CODE_BLOCK(explicit_const_ref_ex)
	Specimen s_explicit(2);

	auto cavt_explicit = sgm::CRefer(s_explicit);

	static_assert
	(	(	sgm::is_constAvatar<decltype(cavt_explicit)>::value
		&&	sgm::is_Same<decltype(cavt_explicit.v()), Specimen const&>::value
		)
	,	""
	);
END_CODE_BLOCK_AND_LOAD(explicit_const_ref_ex)

	mdo << empty_line;
}


static void Const_Avatar_Types()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Const Avatar Types")
	<<	L"constAvatar provides immutable access to objects, ensuring type safety "
	<<	L"and preventing accidental modifications."
	<<	newl;

BEGIN_CODE_BLOCK(const_avatar_types_ex)
	Specimen s_types(2);
	sgm::constAvatar<Specimen> cavt = sgm::Avatar<Specimen>(s_types);

	//cavt = Avatar<Specimen>();	// Compile Failed.

	static_assert( sgm::is_constAvatar<decltype(cavt)>::value, " cavt is not constant." );
END_CODE_BLOCK_AND_LOAD(const_avatar_types_ex)

	mdo << empty_line;
}


static void Type_Conversions()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Type Conversions")
	<<	L"Avatar supports safe type conversions following const-correctness rules. "
	<<	L"Mutable Avatars can convert to const Avatars, but not vice versa."
	<<	newl;

BEGIN_CODE_BLOCK(type_conversions_ex)
	static_assert
	(	(	std::is_convertible< sgm::Avatar<double>, sgm::Avatar<double const> >::value
		&&	!std::is_convertible< sgm::Avatar<double const>, sgm::Avatar<double> >::value
		&&	std::is_convertible< sgm::Avatar<double>, sgm::constAvatar<double> >::value
		&&	!std::is_convertible< sgm::constAvatar<double>, sgm::Avatar<double> >::value
		)
	,	""
	);
END_CODE_BLOCK_AND_LOAD(type_conversions_ex)

	mdo << empty_line;
}


static void Reference_Tracking()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Reference Tracking")
	<<	L"Avatar maintains a live reference to the original object, reflecting changes "
	<<	L"made to the referenced object in real-time."
	<<	newl;

BEGIN_CODE_BLOCK(reference_tracking_ex)
	Specimen x_track(2);
	sgm::constAvatar<Specimen> avt1(x_track);

	x_track = 4;

	SGM_H2U_ASSERT(avt1 == 4);
END_CODE_BLOCK_AND_LOAD(reference_tracking_ex)

	mdo << empty_line;
}


static void Nested_Avatars()
{
	using namespace sgm::h2u;
	using sgm::Avatar;
	using sgm::constAvatar;

	mdo
	<<	Title(L"Nested Avatars (Abbreviable)")
	<<	L"Avatar supports nested references with automatic const propagation. "
	<<	L"The abbreviable feature simplifies access to deeply nested Avatar types."
	<<	newl;

BEGIN_CODE_BLOCK(nested_avatars_ex)
	static_assert
	(	(	sgm::is_constAvatar<   Avatar<  Avatar< Avatar<Specimen> const >  >   >::value
		&&	sgm::is_constAvatar<   Avatar<  Avatar< Avatar<Specimen const> >  >   >::value
		&&	sgm::is_constAvatar<  constAvatar< Avatar<Specimen> >  >::value
		&&	sgm::is_constAvatar<   Avatar<  Avatar< Avatar<Specimen> > const  >   >::value
		&&	!sgm::is_constAvatar<  Avatar< Avatar<Specimen> > const  >::value
		)
	,	""
	);


	Specimen s_nested(2);

	constAvatar<Specimen> avt1 = s_nested;

	auto avt2 = sgm::Refer(avt1);

	SGM_H2U_ASSERT(avt1 == 2 && avt2 == 2);

	static_assert( sgm::is_Same<decltype(avt2.v()), Specimen const&>::value, "" );
END_CODE_BLOCK_AND_LOAD(nested_avatars_ex)

	mdo << empty_line;
}


static void Arithmetic_Operations()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Arithmetic Operations")
	<<	L"Avatar forwards arithmetic operations to the referenced objects, "
	<<	L"enabling transparent mathematical operations."
	<<	newl;

BEGIN_CODE_BLOCK(arithmetic_operations_ex)
	int x1 = 2, x2 = 4;
	Specimen s_arith(6);

	sgm::Avatar<int> avt1 = x1, avt2 = x2;
	sgm::Avatar<Specimen> avts = s_arith;

	auto y1 = avt1 + avt2;
	auto y2 = avt2 - avt1;
	auto b = avt1 > avt2;

	static_assert
	(	(	sgm::is_Same<decltype(y1), int>::value
		&&	sgm::is_Same<decltype(y2), int>::value
		&&	sgm::is_Same<decltype(b), bool>::value
		)
	,	""
	);

	SGM_H2U_ASSERT(y1 == 2+4 && y2 == 4-2 && b == false && avts.v().value() == 6);
END_CODE_BLOCK_AND_LOAD(arithmetic_operations_ex)

	mdo << empty_line;
}


static void Function_Parameters()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Function Parameters")
	<<	L"Avatar can be passed to functions expecting different parameter types, "
	<<	L"including value, reference, const reference, and move semantics."
	<<	newl;

BEGIN_CODE_BLOCK(function_parameters_ex)
	Specimen s1(2), s2(4), s3(6), s4(8), s5(10);
	sgm::Avatar<Specimen> avt1 = s1, avt2 = s2, avt3 = s3, avt4 = s4, avt5 = s5;

	[](Specimen t1, Specimen& t2, Specimen const& t3, Specimen&& t4, Specimen const&& t5)
	{
		SGM_H2U_ASSERT(t1 == 2 && t2 == 4 && t3 == 6 && t4 == 8 && t5 == 10);
	}( s1, s2, s3, sgm::Move(s4), sgm::Move(s5) );
END_CODE_BLOCK_AND_LOAD(function_parameters_ex)

	mdo << empty_line;
}


static void Address_Access()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Address Access")
	<<	L"Avatar provides direct access to the address of the referenced object, "
	<<	L"maintaining pointer equivalence."
	<<	newl;

BEGIN_CODE_BLOCK(address_access_ex)
	Specimen s_addr(2);

	auto avt = sgm::Refer(s_addr);

	static_assert( sgm::is_Same<decltype(&s_addr), decltype(&avt)>::value, "" );

	SGM_H2U_ASSERT(&s_addr == &avt);
END_CODE_BLOCK_AND_LOAD(address_access_ex)

	mdo << empty_line;
}


static void Move_Semantics()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Move Semantics")
	<<	L"Avatar supports move operations, allowing efficient transfer of the "
	<<	L"referenced object's contents to new locations."
	<<	newl;

BEGIN_CODE_BLOCK(move_semantics_ex)
	{
		Specimen s_move(2);
	
		auto avt = sgm::Refer(s_move);
	
		Specimen s2 = sgm::Move(avt);
	
		SGM_H2U_ASSERT(s2 == 2 && s_move == Specimen::State::MOVE_AWAY);
	}
END_CODE_BLOCK_AND_LOAD(move_semantics_ex)

	{
		Specimen s_move(2);

		auto avt = sgm::Refer(s_move);

		Specimen s = sgm::Move(avt);

		SGM_H2U_ASSERT(s == 2 && s_move == Specimen::State::MOVE_AWAY);
	}

	mdo << empty_line;
}


static void Void_Avatar_Casting()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Void Avatar and Type Casting")
	<<	L"Avatar<void> serves as the reference counterpart to void pointers, designed for scenarios where "
	<<	L"the reference target doesn't need to be changed but requires type flexibility. Just as void pointers "
	<<	L"enable type erasure with explicit casting, Avatar<void> provides the same capability while maintaining "
	<<	L"reference semantics and safety."
	<<	newl
	<<	LR"(
			// Traditional void pointer approach
			void* ptr = &some_object;
			auto& obj = static_cast<SomeType&>(*ptr);  // requires dereferencing

			// Avatar<void> approach
			sgm::Avatar<void> avatar = some_object;
			auto& obj = static_cast<SomeType&>(avatar);  // direct reference casting
		)"_code
	<<	L"This design allows Avatar<void> to bridge the gap between type safety and flexibility, "
	<<	L"providing a reference-like interface that supports explicit type casting when needed."
	<<	newl;

BEGIN_CODE_BLOCK(void_avatar_casting_ex)
	{
		Specimen s_void(2);
	
		sgm::Avatar<void> va = s_void;
	
		static_cast<Specimen&>(va).value() = 5;
	
		SGM_H2U_ASSERT(s_void == 5);
	}
END_CODE_BLOCK_AND_LOAD(void_avatar_casting_ex)

	mdo
	<<	Title(L"Const Void Avatar", 2)
	<<	L"constAvatar<void> corresponds to const void pointers, maintaining const-correctness while "
	<<	L"providing type-erased access. It ensures that the referenced object cannot be modified through "
	<<	L"the Avatar, even after explicit casting, preserving the immutability contract."
	<<	newl;

BEGIN_CODE_BLOCK(const_void_avatar_ex)
	{
		Specimen s_cvoid(2);
	
		sgm::constAvatar<void> cva = s_cvoid;
	
		Specimen const& rs = static_cast<Specimen const&>(cva);
	
		SGM_H2U_ASSERT(rs == 2);
	
		s_cvoid.value() = 5;
	
		SGM_H2U_ASSERT(rs == 5);
	}
END_CODE_BLOCK_AND_LOAD(const_void_avatar_ex)

	{
		Specimen s_va(2);

		sgm::Avatar<void> va = sgm::Refer(s_va);

		static_cast<Specimen&>(va).value() = 5;

		SGM_H2U_ASSERT(s_va == 5);
	}
	{
		Specimen s_cva(2);

		sgm::constAvatar<void> cva = sgm::CRefer(s_cva);

		Specimen const& rs = static_cast<Specimen const&>(cva);

		SGM_H2U_ASSERT(rs == 2);

		s_cva.value() = 5;

		SGM_H2U_ASSERT(rs == 5);
	}

	mdo << empty_line;
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_HOW2USE_TESTS(sgm::h2u::Test_, Avatar, /**/)
{	::intro
,	::Basic_Usage
,	::Const_Avatar_Types
,	::Type_Conversions
,	::Reference_Tracking
,	::Nested_Avatars
,	::Arithmetic_Operations
,	::Function_Parameters
,	::Address_Access
,	::Move_Semantics
,	::Void_Avatar_Casting
};