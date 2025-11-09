/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Wrapper/Pinweight.hpp"
#include "Test_Pinweight.hpp"
#include "SGM/TMP/interface_Traits.hpp"


using h2u::Specimen;


static void intro()
{
	using namespace h2u;

	mdo
	<<	Title(L"Introduction")
	<<	L"sgm::Pinweight is a smart pointer that provides shared ownership with copy-on-write semantics. "
	<<	L"It shares similarities with `std::shared_ptr` in implementing shared ownership through reference counting, "
	<<	L"but has several important differences."
	<<	newl
	<<	L"First, while `std::shared_ptr` provides a pointer-like interface (`->`, `*` operators), "
	<<	L"Pinweight offers a reference-like interface, enabling more intuitive and natural object access."
	<<	newl
	<<	L"Second, functionally unlike `std::shared_ptr`, it operates with Copy-on-Write semantics. "
	<<	L"Multiple Pinweight instances share the same object until one of them needs modification, "
	<<	L"at which point a private copy is created. This lazy copying strategy significantly improves "
	<<	L"memory efficiency and provides performance benefits, especially in scenarios with frequent "
	<<	L"read operations and infrequent modifications."
	<<	newl
	<<	L"The detailed explanation of Copy-on-Write mechanics and specific usage examples will be "
	<<	L"covered in the \"Copy-on-Write Semantics\" section below."
	<<	empty_line;
}


static void Basic_Usage()
{
	using namespace h2u;

	mdo
	<<	Title(L"Basic Pinweight Usage")
	<<	L"Pinweight can be constructed and assigned values, maintaining a reference count "
	<<	L"of shared instances."
	<<	newl;

BEGIN_CODE_BLOCK(basic_usage_ex)
	sgm::Pinweight<Specimen> pw;

	pw = Specimen(3);

	H2U_ASSERT(pw == 3 && pw.share_count() == 1);
END_CODE_BLOCK_AND_LOAD(basic_usage_ex)

	mdo << empty_line;
}


static void Construction_and_Assignment()
{
	using namespace h2u;

	mdo
	<<	Title(L"Construction and Assignment")
	<<	L"Pinweight supports various construction methods and assignment operations "
	<<	L"while maintaining independence from original objects."
	<<	newl;

BEGIN_CODE_BLOCK(construction_assignment_ex)
	Specimen const cs(3);
	sgm::Pinweight<Specimen> pw1(5), pw2(cs);

	H2U_ASSERT(pw1 == 5 && pw2 == 3);

	pw1 = 7, pw2 = 9;

	H2U_ASSERT(pw1 == 7 && pw2 == 9 && cs == 3);
	H2U_ASSERT(pw1.share_count() == 1 && pw2.share_count() == 1);
END_CODE_BLOCK_AND_LOAD(construction_assignment_ex)

	mdo << empty_line;
}


static void Shared_Ownership()
{
	using namespace h2u;

	mdo
	<<	Title(L"Shared Ownership")
	<<	L"Multiple Pinweight instances can share the same underlying object, "
	<<	L"tracking the number of shared references."
	<<	newl;

BEGIN_CODE_BLOCK(shared_ownership_ex)
	sgm::Pinweight<Specimen> pw1(3), pw2 = pw1;

	H2U_ASSERT(pw1 == 3 && pw2 == 3);
	H2U_ASSERT( pw1.share_with(pw2) && pw1.share_count() == 2 && pw2.share_count() == 2 );

	pw2 = Specimen(3);

	H2U_ASSERT(pw1 == 3 && pw2 == 3);
	H2U_ASSERT( !pw1.share_with(pw2) && pw1.share_count() == 1 && pw2.share_count() == 1 );
END_CODE_BLOCK_AND_LOAD(shared_ownership_ex)

	mdo << empty_line;
}


static void Copy_Construction()
{
	using namespace h2u;

	mdo
	<<	Title(L"Copy Construction")
	<<	L"Copy construction creates shared references, allowing multiple Pinweight "
	<<	L"instances to access the same underlying object."
	<<	newl;

BEGIN_CODE_BLOCK(copy_construction_ex)
	sgm::Pinweight<Specimen> pw1(3), pw2 = sgm::Pinweight<Specimen>(pw1);

	H2U_ASSERT( pw1.share_with(pw2) );

	sgm::Pinweight<Specimen> const pw3 = pw1;
	sgm::Pinweight<Specimen const> cpw = pw3;

	H2U_ASSERT( pw1.share_with(pw3) && pw2.share_with(pw3) );
	H2U_ASSERT(pw1.share_count() == 4);
END_CODE_BLOCK_AND_LOAD(copy_construction_ex)

	mdo << empty_line;
}


static void Const_Pinweight()
{
	using namespace h2u;

	mdo
	<<	Title(L"Const Pinweight")
	<<	L"constPinweight provides immutable access to shared objects, preventing "
	<<	L"modifications while allowing read operations."
	<<	newl;

BEGIN_CODE_BLOCK(const_pinweight_ex)
	sgm::constPinweight<Specimen> cpw1(3);

	H2U_ASSERT(cpw1.v().value() == 3);
END_CODE_BLOCK_AND_LOAD(const_pinweight_ex)

	mdo << empty_line;
}


template<class CPW>
struct _constPinweight_Test_Helper
{
private:
	template<class _CPW>
	static auto _calc(int)-> sgm::SFINAE_t< decltype(sgm::Mock<_CPW>().v() = 7) >;

	template<class...>
	static auto _calc(...)-> sgm::False_t;

public:
	static bool constexpr cannot_assign_to_constPinweight_value
	=	!decltype( _calc<CPW>(0) )::value;
};


static void Nested_Pinweight()
{
	using namespace h2u;

	mdo
	<<	Title(L"Nested Pinweight (Abbreviable)")
	<<	L"Pinweight supports nesting and abbreviable access, automatically unwrapping "
	<<	L"nested types while maintaining const-correctness."
	<<	newl;

BEGIN_CODE_BLOCK(nested_pinweight_ex)
	sgm::Pinweight<  sgm::Pinweight< sgm::Pinweight<Specimen> >  > ppw(3);
	sgm::constPinweight<Specimen> cpw(5);


	static_assert
	(	(	sgm::is_Same< decltype(ppw.v()), Specimen const& >::value
		&&	sgm::is_Same< decltype(cpw.v()), Specimen const& >::value
		&&	::_constPinweight_Test_Helper
			<	sgm::constPinweight<Specimen>
			>::	cannot_assign_to_constPinweight_value
		)
	,	""
	);
END_CODE_BLOCK_AND_LOAD(nested_pinweight_ex)

	mdo << empty_line;
}


struct Foo
{
	int x;
	Foo(int x_) : x(x_){}

	void set(int a){  x = a;  }
	auto value() const-> int{  return x;  }
	auto value()-> int&{  return x;  }
};


static void Copy_On_Write()
{
	using namespace h2u;

	mdo
	<<	Title(L"Copy-on-Write Semantics")
	<<	L"Pinweight implements copy-on-write: shared instances become independent "
	<<	L"when one needs to be modified using mut()."
	<<	newl;

BEGIN_CODE_BLOCK(copy_on_write_ex)
	auto pw1 = sgm::Share( ::Foo(1) );
	sgm::Pinweight<::Foo> pw2 = pw1;
	sgm::constPinweight<::Foo> pw3 = pw1;

	static_assert
	(	(	sgm::is_constPinweight<decltype(pw3)>::value
		&&	sgm::is_constPinweight< sgm::Pinweight<::Foo const> >::value
		&&	!sgm::is_constPinweight< sgm::Pinweight<::Foo> const >::value
		&&	sgm::is_constPinweight<  sgm::Pinweight< sgm::constPinweight<::Foo> >  >::value
		&&	sgm::is_constPinweight<  sgm::Pinweight< sgm::Pinweight<::Foo> const >  >::value
		)
	,	""
	);

	H2U_ASSERT( pw1.share_with(pw2) );

	pw1.mut().set(3);

	H2U_ASSERT
	(	!pw1.share_with(pw2)
	&&	pw1.v().value() == 3
	&&	pw3.v().value() == 1
	);
END_CODE_BLOCK_AND_LOAD(copy_on_write_ex)

	mdo << empty_line;
}


static void Arithmetic_Operations()
{
	using namespace h2u;

	mdo
	<<	Title(L"Arithmetic Operations")
	<<	L"Pinweight supports arithmetic operations by forwarding calls to the "
	<<	L"contained objects, while mut() triggers copy-on-write when modification is needed."
	<<	newl;

BEGIN_CODE_BLOCK(arithmetic_operations_ex)
	sgm::constPinweight<int> pwi1 = 3, pwi2 = 5;

	auto y1 = pwi1 + pwi2;
	auto y2 = pwi2 - pwi1;
	auto b = pwi1 < pwi2;

	static_assert
	(	(	sgm::is_Same<decltype(y1), int>::value
		&&	sgm::is_Same<decltype(y2), int>::value
		&&	sgm::is_Same<decltype(b), bool>::value
		)
	,	""
	);

	H2U_ASSERT(y1 == 8 && y2 == 2 && b == true);

	sgm::Pinweight<int> pw3 = pwi2;

	H2U_ASSERT( pw3.share_with(pwi2) );

	pw3.mut()++;

	H2U_ASSERT( pwi2 == 5 && pw3 == 5+1 && !pw3.share_with(pwi2) );
END_CODE_BLOCK_AND_LOAD(arithmetic_operations_ex)

	mdo << empty_line;
}


static void Custom_Memory_Management()
{
	using namespace h2u;

	mdo
	<<	Title(L"Custom Memory Management")
	<<	L"Pinweight supports custom memory allocation by providing buffer space "
	<<	L"for object storage, enabling stack-based memory management."
	<<	newl;

BEGIN_CODE_BLOCK(custom_memory_ex)
	enum class Byte : wchar_t{};
	using Pinweight_bytes = Byte[sgm::Pinweight<Specimen>::data_size_value];

	Pinweight_bytes buf[2];

	for(int i = 0;  i < 10;  ++i)
	{
		auto pw1 = sgm::Share( Specimen(i), buf[0] );
		sgm::constPinweight<Specimen> pw2(i, buf[1]);

		[[maybe_unused]] auto pw3 = pw1;
		sgm::Pinweight<Specimen> pw4(8);

		H2U_ASSERT
		(	pw1 == i && pw2 == i
		&&	&pw1.v() == reinterpret_cast<Specimen const*>(&buf[0])
		&&	&pw2.v() == reinterpret_cast<Specimen const*>(&buf[1])
		);
	}

	H2U_ASSERT
	(	reinterpret_cast<Specimen*>(buf[0])->state() == Specimen::State::DESTRUCTION
	&&	reinterpret_cast<Specimen*>(buf[1])->state() == Specimen::State::DESTRUCTION
	);
END_CODE_BLOCK_AND_LOAD(custom_memory_ex)

	mdo << empty_line;
}


static void outro()
{

}
//========//========//========//========//=======#//========//========//========//========//=======#


H2U_HOW2USE_TESTS(sgm::test::Test_, Pinweight, /**/)
{	::intro
,	::Basic_Usage
,	::Construction_and_Assignment
,	::Shared_Ownership
,	::Copy_Construction
,	::Const_Pinweight
,	::Nested_Pinweight
,	::Copy_On_Write
,	::Arithmetic_Operations
,	::Custom_Memory_Management
,	::outro
};