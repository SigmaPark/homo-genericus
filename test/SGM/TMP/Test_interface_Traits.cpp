/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/TMP/interface_Traits.hpp"
#include "Test_interface_Traits.hpp"


using h2u::Specimen;


static void intro()
{
	using namespace h2u;

	mdo
	<<	Title(L"Introduction")
	<<	L"sgm::interface_Traits provides compile-time type inspection utilities that enable "
	<<	L"detection of class members, operators, and capabilities through SFINAE-based "
	<<	L"metaprogramming techniques. This library allows developers to write generic code "
	<<	L"that adapts behavior based on the interface provided by types, making it essential "
	<<	L"for template metaprogramming and concept-like programming in pre-C++20 codebases."
	<<	empty_line;
}


BEGIN_CODE_BLOCK(member_detection_ex_decl)
namespace sgm
{
	namespace _test_Foo_detail
	{

		class Foo
		{
		public:
			auto member_func()-> Foo{  return {};  }
			static auto static_member_func()-> Foo{  return {};  }

			static int constexpr value = 10;

			using type = double;


			auto operator+(Foo)-> Foo{  return {};  }
			auto operator-(Foo)-> Foo{  return {};  }
		};


		SGM_HAS_MEMFUNC(member_func);
		SGM_HAS_MEMFUNC(static_member_func);

		SGM_HAS_MEMBER(value);

		SGM_HAS_NESTED_TYPE(type);

	}
}
END_CODE_BLOCK(member_detection_ex_decl)


static void Member_Detection()
{
	using namespace h2u;

	mdo
	<<	Title(L"Member Detection")
	<<	L"The interface_Traits library provides macros and templates to detect the presence "
	<<	L"of various class members at compile time. This includes member functions, static "
	<<	L"member functions, member variables, nested types, and operators."
	<<	newl
	<<	Load_code_block(L"member_detection_ex_decl")
	<<	newl
	<<	L"Using these test classes, we can verify the trait detection results:"
	<<	newl;

BEGIN_CODE_BLOCK(member_detection_ex)
	using sgm::_test_Foo_detail::Foo;

	static_assert
	(	(	sgm::_test_Foo_detail::Has_MemFunc_member_func<Foo>::value
		&&	sgm::_test_Foo_detail::Has_MemFunc_static_member_func<Foo>::value
		&&	sgm::_test_Foo_detail::Has_Member_value<Foo>::value
		&&	sgm::_test_Foo_detail::Has_NestedType_type<Foo>::value
		&&	sgm::Has_Operator_Plus<Foo>::value
		&&	sgm::Has_Operator_Minus<Foo>::value
		&&	!sgm::Has_Operator_Multiply<Foo>::value
		&&	!sgm::Has_Operator_Divide<Foo>::value
		)
	,	""
	);
END_CODE_BLOCK_AND_LOAD(member_detection_ex)

	mdo << empty_line;
}


BEGIN_CODE_BLOCK(assignment_operators_ex_decl)
namespace sgm
{
	namespace _test_Assignable_detail
	{

		struct Foo
		{
			h2u::Specimen s = {10};
		};

		struct Bar1
		{
			h2u::Specimen s = {10};

			auto operator=(Bar1 const&)-> Bar1& = delete;
		};

		struct Bar2
		{
			h2u::Specimen s = {10};

			auto operator=(Bar2 const&)-> Bar2& = delete;

			auto operator=(Bar2&& bar)-> Bar2&
			{
				return s = Move(bar.s),  *this;
			}
		};

	}
}
END_CODE_BLOCK(assignment_operators_ex_decl)


static void Assignment_Operators()
{
	using namespace h2u;

	mdo
	<<	Title(L"Assignment Operator Detection")
	<<	L"The library can detect whether a class supports copy assignment and move "
	<<	L"assignment operators. This is particularly useful for generic programming "
	<<	L"where different assignment semantics need to be handled appropriately."
	<<	newl
	<<	Load_code_block(L"assignment_operators_ex_decl")
	<<	newl
	<<	L"Using these test classes, we can verify the assignment operator detection:"
	<<	newl;

BEGIN_CODE_BLOCK(assignment_operators_ex)
	using sgm::_test_Assignable_detail::Foo;
	using sgm::_test_Assignable_detail::Bar1;
	using sgm::_test_Assignable_detail::Bar2;

	static_assert
	(	(	sgm::Has_Operator_Copy_Assignment<Foo>::value
		&&	!sgm::Has_Operator_Copy_Assignment<Bar1>::value
		&&	!sgm::Has_Operator_Copy_Assignment<Bar2>::value
		&&	sgm::Has_Operator_Move_Assignment<Foo>::value
		&&	!sgm::Has_Operator_Move_Assignment<Bar1>::value
		&&	sgm::Has_Operator_Move_Assignment<Bar2>::value
		)
	,	""
	);
END_CODE_BLOCK_AND_LOAD(assignment_operators_ex)

	mdo << empty_line;
}


BEGIN_CODE_BLOCK(constructor_detection_ex_decl)
namespace sgm
{
	namespace _test_Has_Operator_New_detail
	{

		struct Foo : Unconstructible{};

		struct Bar1
		{
			Bar1() = default;
			Bar1(int, double*){}
			Bar1(Bar1 const&) = delete;
		};

		struct Bar2
		{
			Bar2() = default;
			Bar2(Bar2 const&) = delete;
			Bar2(Bar2&&) = default;
		};

	}
}
END_CODE_BLOCK(constructor_detection_ex_decl)


static void Constructor_Detection()
{
	using namespace h2u;

	mdo
	<<	Title(L"Constructor Detection")
	<<	L"The Has_Operator_New trait can detect whether a class can be constructed "
	<<	L"with specific argument types. This is essential for generic code that needs "
	<<	L"to conditionally instantiate objects based on available constructors."
	<<	newl
	<<	Load_code_block(L"constructor_detection_ex_decl")
	<<	newl
	<<	L"Using these test classes, we can verify the constructor detection capabilities:"
	<<	newl;

BEGIN_CODE_BLOCK(constructor_detection_ex)
	using sgm::_test_Has_Operator_New_detail::Foo;
	using sgm::_test_Has_Operator_New_detail::Bar1;
	using sgm::_test_Has_Operator_New_detail::Bar2;

	static_assert
	(	(	!sgm::Has_Operator_New<Foo>::value
		&&	sgm::Has_Operator_New<Bar1>::value
		&&	sgm::Has_Operator_New<Bar1, int, double*>::value
		&&	sgm::Has_Operator_New<Bar2>::value
		)
	,	""
	);
END_CODE_BLOCK_AND_LOAD(constructor_detection_ex)

	mdo << empty_line;
}


BEGIN_CODE_BLOCK(destructor_detection_ex_decl)
namespace sgm
{
	namespace _test_Has_Operator_Delete_detail
	{

		struct Foo{};
		struct Bar{  ~Bar() = delete;  };

	}
}
END_CODE_BLOCK(destructor_detection_ex_decl)


static void Destructor_Detection()
{
	using namespace h2u;

	mdo
	<<	Title(L"Destructor Detection")
	<<	L"The Has_Operator_Delete trait detects whether a class can be safely destroyed. "
	<<	L"This is particularly important when working with dynamically allocated objects "
	<<	L"and ensuring proper resource cleanup in generic contexts."
	<<	newl
	<<	Load_code_block(L"destructor_detection_ex_decl")
	<<	newl
	<<	L"Using these test classes, we can verify the destructor detection results:"
	<<	newl;

BEGIN_CODE_BLOCK(destructor_detection_ex)
	using sgm::_test_Has_Operator_Delete_detail::Foo;
	using sgm::_test_Has_Operator_Delete_detail::Bar;

	static_assert
	(	(	sgm::Has_Operator_Delete<Foo>::value
		&&	!sgm::Has_Operator_Delete<Bar>::value
		&&	sgm::Has_Operator_Delete<Foo*>::value
		&&	sgm::Has_Operator_Delete<Bar*>::value
		)
	,	""
	);
END_CODE_BLOCK_AND_LOAD(destructor_detection_ex)

	mdo << empty_line;
}


BEGIN_CODE_BLOCK(invocation_detection_ex_decl)
namespace sgm
{
	namespace _test_is_invocable_detail
	{

		auto Lambda_f = [](h2u::Specimen, int const* p)-> int{  return *p;  };

		class Foo
		{
		public:
			auto operator()(h2u::Specimen, int const* p)-> int{  return *p;  }
		};

	}
}
END_CODE_BLOCK(invocation_detection_ex_decl)


static void Invocation_Detection()
{
	using namespace h2u;

	mdo
	<<	Title(L"Function Call Operator Detection")
	<<	L"The Has_Operator_invocation trait can detect whether an object can be called "
	<<	L"as a function with specific argument types. This works with function objects, "
	<<	L"lambdas, and any class that overloads the function call operator."
	<<	newl
	<<	Load_code_block(L"invocation_detection_ex_decl")
	<<	newl
	<<	L"Using these test classes, we can verify the function call operator detection:"
	<<	newl;

BEGIN_CODE_BLOCK(invocation_detection_ex)
	using sgm::_test_is_invocable_detail::Foo;
	using Lambda_t = decltype(sgm::_test_is_invocable_detail::Lambda_f);

	static_assert
	(	(	sgm::Has_Operator_invocation<Foo, Specimen, int const*>::value
		&&	sgm::Has_Operator_invocation<Lambda_t, Specimen, int const*>::value
		&&	!sgm::Has_Operator_invocation<Foo>::value
		&&	!sgm::Has_Operator_invocation<Lambda_t>::value
		&&	!sgm::Has_Operator_invocation<Lambda_t, double**>::value
		)
	,	""
	);
END_CODE_BLOCK_AND_LOAD(invocation_detection_ex)

	mdo << empty_line;
}
//========//========//========//========//=======#//========//========//========//========//=======#


H2U_HOW2USE_TESTS(sgm::test::Test_, interface_Traits, /**/)
{	::intro
,	::Member_Detection
,	::Assignment_Operators
,	::Constructor_Detection
,	::Destructor_Detection
,	::Invocation_Detection
};