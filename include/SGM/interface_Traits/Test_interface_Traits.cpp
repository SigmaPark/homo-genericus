/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "interface_Traits.hpp"
#include "Test_interface_Traits.hpp"


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


static void Test01()
{
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
}


namespace sgm
{
	namespace _test_Assignable_detail
	{

		struct Foo
		{
			spec::Specimen s = {10};
		};
		
		struct Bar1
		{
			spec::Specimen s = {10};

			auto operator=(Bar1 const&)-> Bar1& = delete;
		};

		struct Bar2
		{
			spec::Specimen s = {10};

			auto operator=(Bar2 const&)-> Bar2& = delete;

			auto operator=(Bar2&& bar)-> Bar2&
			{
				return s = Move(bar.s),  *this;
			}
		};

	}
}


static void Test02()
{
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
}


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


static void Test03()
{
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
}


namespace sgm
{
	namespace _test_Has_Operator_Delete_detail
	{
		
		struct Foo{};
		struct Bar{  ~Bar() = delete;  };

	}
}


static void Test04()
{
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
}


namespace sgm
{
	namespace _test_is_invocable_detail
	{

		auto Lambda_f = [](spec::Specimen, int const* p)-> int{  return *p;  };

		class Foo
		{
		public:
			auto operator()(spec::Specimen, int const* p)-> int{  return *p;  }
		};

	}
}


static void Test05()
{
	using sgm::_test_is_invocable_detail::Foo;
	using Lambda_t = decltype(sgm::_test_is_invocable_detail::Lambda_f);

	static_assert
	(	(	sgm::Has_Operator_invocation<Foo, sgm::spec::Specimen, int const*>::value
		&&	sgm::Has_Operator_invocation<Lambda_t, sgm::spec::Specimen, int const*>::value
		&&	!sgm::Has_Operator_invocation<Foo>::value
		&&	!sgm::Has_Operator_invocation<Lambda_t>::value
		&&	!sgm::Has_Operator_invocation<Lambda_t, double**>::value
		)
	,	""
	);
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, interface_Traits, /**/)
{	::Test01
,	::Test02
,	::Test03
,	::Test04
,	::Test05
};