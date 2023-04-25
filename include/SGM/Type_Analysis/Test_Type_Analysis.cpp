/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Type_Analysis.hpp"
#include "Test_Type_Analysis.hpp"
#include <type_traits>


static void Unconstructible_Test()
{
	class AA{};
	class BB : sgm::Unconstructible{};

	static_assert
	(	std::is_constructible<AA>::value && !std::is_constructible<BB>::value
	,	""
	);
}


static void Boolean_Test()
{
	static_assert
	(	(	sgm::True_t::value 
		&&	!sgm::False_t::value
		&&	sgm::Boolean_And< sgm::True_t, sgm::Boolean<0==0> >::value
		&&	sgm::Boolean_Or< sgm::Boolean<0==1>, sgm::False_t, sgm::True_t >::value
		&&	sgm::Boolean_Not< sgm::False_t >::value
		)
	,	""
	);
}


static void Qualifiers_Test()
{
	static_assert
	(	(	sgm::is_Same<int, int>::value && !sgm::is_Same<int, int&>::value
		&&	sgm::is_Same<int, sgm::Referenceless_t<int&> >::value
		&&	sgm::is_Same<int, sgm::Referenceless_t<int&&> >::value
		&&	sgm::is_Same<int, sgm::Alephless_t<int&&> >::value
		&&	!sgm::is_Same<int, sgm::Alephless_t<int&> >::value
		&&	sgm::is_Same<int, sgm::Pointerless_t<int*> >::value
		&&	sgm::is_Same<int, sgm::Constless_t<int const> >::value
		&&	sgm::is_Same<int, sgm::Volatileless_t<int volatile> >::value
		&&	sgm::is_Same<int, sgm::Decay_t<int volatile const&> >::value
		)
	,	""
	);
}


static void is_Something_Test()
{
	static_assert
	(	(	sgm::is_Void<void>::value
		&&	sgm::is_None<sgm::None>::value && sgm::is_None<sgm::None&>::value
		&&	sgm::is_Const<int const>::value && !sgm::is_Const<int const&>::value
		&&	sgm::is_Volatile<int volatile>::value
		&&	sgm::is_Pointer<int*>::value && !sgm::is_Pointer<int*&>::value
		&&	sgm::is_Lvalue_Reference<int&>::value 
		&&	sgm::is_Lvalue_Reference<int const&>::value
		&&	!sgm::is_Lvalue_Reference<int&&>::value
		&&	!sgm::is_Lvalue_Reference<int const&&>::value
		&&	!sgm::is_Rvalue_Reference<int&>::value 
		&&	!sgm::is_Rvalue_Reference<int const&>::value
		&&	sgm::is_Rvalue_Reference<int&&>::value
		&&	sgm::is_Rvalue_Reference<int const&&>::value
		&&	sgm::is_immutable<int const&>::value && sgm::is_immutable<int const*>::value
		&&	sgm::is_Primitive_Array<int[5]>::value
		)
	,	""
	);

	class BB{};
	class CC : public BB{};
	union UU{};

	static_assert
	(	(	sgm::is_Class_or_Union<CC>::value && sgm::is_Class_or_Union<UU>::value
		&&	std::is_class<CC>::value && !std::is_class<UU>::value
		&&	sgm::is_inherited<BB, CC>::value
		)
	,	""
	);
}


struct _Casting_Test_Helper
{
	template<class T> /* Declaration Only */
	static auto calc(T&& t)-> decltype( sgm::Forward<T>(t) );

	template<class IN, class OUT>
	using type = sgm::is_Same< decltype( calc(sgm::Mock<IN>()) ), OUT  >;
};


static void Casting_Test()
{
	static_assert
	(	(	sgm::is_Same< decltype( sgm::Move(sgm::Mock<int&>()) ), int&& >::value
		&&	sgm::is_Same< decltype( sgm::Move(sgm::Mock<int&&>()) ), int&& >::value
		&&	sgm::is_Same
			<	decltype( sgm::Move(sgm::Mock<int const&>()) ), int const&& 
			>::	value
		&&	sgm::is_Same
			<	decltype( sgm::Move(sgm::Mock<int const&&>()) ), int const&& 
			>::	value
		)
	,	""
	);

	static_assert
	(	(	::_Casting_Test_Helper::template type<int, int&&>::value
		&&	::_Casting_Test_Helper::template type<int&, int&>::value
		&&	::_Casting_Test_Helper::template type<int const&, int const&>::value
		&&	::_Casting_Test_Helper::template type<int&&, int&&>::value
		&&	::_Casting_Test_Helper::template type<int const&&, int const&&>::value
		)
	,	""
	);

	static_assert
	(	(	sgm::is_Same
			<	decltype( sgm::Move_if<true>(sgm::Mock<int&>()) ), int&& 
			>::	value
		&&	sgm::is_Same
			<	decltype( sgm::Move_if<false>(sgm::Mock<int&>()) ), int& 
			>::	value
		)
	,	""
	);

	class AA{};
	class BB{};

	static_assert
	(	sgm::is_Same<	sgm::Qualify_Like_t<AA const&, BB>, BB const& >::value
	,	""
	);
}


static void Parameter_Pack_Test()
{
	static_assert
	(	(	sgm::is_Same< sgm::Nth_t<2, int, char const*, double**, void>, double** >::value
		&&	sgm::is_Same< sgm::First_t<int, char const*, double**, void>, int >::value
		&&	sgm::is_Same< sgm::Last_t<int, char const*, double**, void>, void >::value
		&&	sgm::is_Same< decltype( sgm::Nth_Param<0>(1, 2.f, 3.0) ), int&& >::value
		&&	sgm::is_Same< decltype( sgm::Nth_Param<1>(1, 2.f, 3.0) ), float&& >::value
		&&	sgm::is_Same< decltype( sgm::Nth_Param<2>(1, 2.f, 3.0) ), double&& >::value
		)
	,	""
	);
}


static void Type_Check_Test()
{
	static_assert
	(	(	sgm::Check_if_All<std::is_integral, int, long long, unsigned char, short>::value
		&&	sgm::Check_if_Any<std::is_floating_point, int, long long, float, short>::value
		)
	,	""
	);

	static_assert
	(	(	sgm::is_Same
			<	typename
				sgm::Satisfying_Among
				<	std::is_floating_point, int, long long, float, short
				>::	type
			,	float
			>::	value
		&&	sgm::is_None
			<	typename
				sgm::Satisfying_Among<std::is_floating_point, int, long long, char, short>::type
			>::	value
		)
	,	""
	);


	class AA{};
	class BB{};
	class CC{};
	class CC_1 : public CC{};

	static_assert
	(	(	sgm::Has_Type<CC>::template among<AA, BB, CC>::value
		&&	sgm::Has_Type<CC>::template among<AA, BB, CC_1>::value
		)
	,	""
	);
}


static void Omni_Convertible_Test()
{
	static_cast<void>(sgm::Omni_Convertible{});

	class AA{};

	struct BB
	{
		static auto func()-> BB{  return sgm::Omni_Convertible{};  }
	};

	static_assert
	(	(	sgm::is_Convertible<sgm::Omni_Convertible, double>::value
		&&	sgm::is_Convertible<sgm::Omni_Convertible, AA>::value
		&&	sgm::SFINAE_t< decltype(BB::func()) >::value
		)
	,	""
	);
}


static void invocation_Result_Test()
{
	enum class AA{};

	auto Lambda_f = [](AA, int const* p)-> int{  return *p;  };

	class Foo
	{
	public:
		auto operator()(AA, int const* p)-> int{  return *p;  }
	};


	static_assert
	(	(	sgm::is_Same
			<	sgm::invocation_Result_t<decltype(Lambda_f), AA, int const*> 
			,	int
			>::	value
		&&	sgm::is_Same
			<	sgm::invocation_Result_t<Foo, AA, int const*> 
			,	int
			>::	value
		)
	,	""
	);
}


static void Member_Function_Test()
{
	struct Foo
	{
		int x;

		auto get_x() noexcept(false)-> int{  return x;  }
		auto nx_get_x() noexcept-> int{  return x;  }
	};


	Foo foo{5};

	auto const foo_xf = sgm::Memfunc(foo, &Foo::get_x);
	auto const foo_nx_xf = sgm::Memfunc(foo, &Foo::nx_get_x);

	sgm::spec::is_True(foo_xf() == 5 && foo_nx_xf() == 5);

	static_assert
	(	!noexcept(foo_xf()) && noexcept(foo_nx_xf())
	,	""
	);
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Type_Analysis, /**/)
{	::Unconstructible_Test
,	::Boolean_Test
,	::Qualifiers_Test
,	::is_Something_Test
,	::Casting_Test
,	::Parameter_Pack_Test
,	::Type_Check_Test
,	::Omni_Convertible_Test
,	::invocation_Result_Test
,	::Member_Function_Test
};