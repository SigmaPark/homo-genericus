/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Wrapper/Nullable.hpp"
#include "Test_Nullable.hpp"
#include "SGM/TMP/interface_Traits.hpp"


using sgm::h2u::Specimen;


static void intro()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Introduction")
	<<	L"sgm::Nullable is a type-safe wrapper that represents values that may or may not exist. "
	<<	L"It provides null-safe operations and type conversions, similar to std::optional but with "
	<<	L"additional features like abbreviable syntax and custom exception handling."
	<<	empty_line;
}


static void Null_type()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Null Type")
	<<	L"The sgm::Null_t type represents the absence of a value. It can be checked for nullability "
	<<	L"traits and provides compile-time safety guarantees."
	<<	newl;

BEGIN_CODE_BLOCK(null_type_ex)
	sgm::Null_t constexpr null{};

	static_assert
	(	(	sgm::is_Nullable<decltype(null)>::value
		&&	sgm::is_constNullable<decltype(null)>::value
		)
	,	""
	);

	SGM_H2U_ASSERT(!null.has_value());
END_CODE_BLOCK_AND_LOAD(null_type_ex)

	mdo << empty_line;
}


static void Constructions()
{
	using namespace sgm::h2u;

	mdo << Title(L"Construction Methods");

	mdo
	<<	Title(L"Default Construction", 2)
	<<	L"A default-constructed Nullable starts in a null state, containing no value."
	<<	newl;

BEGIN_CODE_BLOCK(default_construction_ex)
	{
		auto const n = sgm::Nullable<double>{};

		SGM_H2U_ASSERT(!n.has_value());
	}
END_CODE_BLOCK_AND_LOAD(default_construction_ex)

	mdo
	<<	Title(L"Value Construction", 2)
	<<	L"Nullable can be constructed with initial values, from other values, or explicitly "
	<<	L"set to null using sgm::Null_t."
	<<	newl;

BEGIN_CODE_BLOCK(value_construction_ex)
	{
		sgm::Nullable<Specimen>
			nb1,
			nb2( Specimen(2) ),
			nb3(4),
			nb4 = sgm::Null_t{};

		SGM_H2U_ASSERT
		(	!nb1.has_value()
		&&	nb2.has_value() && nb2 == Specimen(2)
		&&	nb3.has_value() && nb3 == Specimen(4)
		&&	!nb4.has_value()
		);
	}
END_CODE_BLOCK_AND_LOAD(value_construction_ex)

	mdo
	<<	Title(L"Copy Construction", 2)
	<<	L"Copy construction preserves the contained value and properly tracks object lifecycle "
	<<	L"through the Specimen state system."
	<<	newl;

BEGIN_CODE_BLOCK(copy_construction_ex)
	{
		auto nb1 = sgm::make_Nullable( Specimen(2) );
		sgm::Nullable<Specimen> nb2 = nb1;

		static_assert
		(	sgm::is_Same< decltype(nb1), sgm::Nullable<Specimen> >::value
		,	""
		);

		SGM_H2U_ASSERT
		(	nb1 == Specimen(2)
		&&	nb1 == nb2
		&&	nb2 == Specimen::State::COPY_CONSTRUCTION
		);
	}
END_CODE_BLOCK_AND_LOAD(copy_construction_ex)

	mdo
	<<	Title(L"Move Construction", 2)
	<<	L"Move construction efficiently transfers ownership of the contained value without "
	<<	L"unnecessary copying."
	<<	newl;

BEGIN_CODE_BLOCK(move_construction_ex)
	{
		auto nb1 = sgm::make_Nullable( Specimen(2) );
		sgm::Nullable<Specimen> nb2 = sgm::Move(nb1);

		static_assert
		(	sgm::is_Same< decltype(nb1), sgm::Nullable<Specimen> >::value
		,	""
		);

		SGM_H2U_ASSERT
		(	nb1 == Specimen(2)
		&&	nb1 == nb2
		&&	nb2 == Specimen::State::MOVE_CONSTRUCTION
		);
	}
END_CODE_BLOCK_AND_LOAD(move_construction_ex)

	mdo << empty_line;
}


static void Conversions()
{
	using namespace sgm::h2u;

	mdo << Title(L"Type Conversions");

	mdo
	<<	Title(L"Implicit Conversion", 2)
	<<	L"Nullable values can be implicitly converted to their underlying type when they "
	<<	L"contain a valid value."
	<<	newl;

BEGIN_CODE_BLOCK(implicit_conversion_ex)
	{
		sgm::Nullable<Specimen> nb{2};

		Specimen s = nb;
		Specimen const cs = nb;

		SGM_H2U_ASSERT( s == Specimen(2) && cs == Specimen(2) );
	}
END_CODE_BLOCK_AND_LOAD(implicit_conversion_ex)

	mdo
	<<	Title(L"Reference Conversion", 2)
	<<	L"Nullable supports both value and reference conversions, maintaining proper "
	<<	L"const-correctness throughout."
	<<	newl;

BEGIN_CODE_BLOCK(reference_conversion_ex)
	{
		sgm::Nullable<double> nb{0.25};

		double d = nb,  &rd = nb;
		double const cd = nb, &crd = nb;

		SGM_H2U_ASSERT(d == 0.25 && cd == 0.25 && rd == 0.25 && crd == 0.25);
	}
END_CODE_BLOCK_AND_LOAD(reference_conversion_ex)

	mdo
	<<	Title(L"Move Conversion", 2)
	<<	L"The move() method explicitly transfers the contained value, leaving the Nullable "
	<<	L"in a moved-from state."
	<<	newl;

BEGIN_CODE_BLOCK(move_conversion_ex)
	{
		sgm::Nullable<Specimen> nb{2};

		Specimen s = nb.move();

		SGM_H2U_ASSERT( s == Specimen(2) && nb.v() == Specimen::State::MOVE_AWAY );
	}
END_CODE_BLOCK_AND_LOAD(move_conversion_ex)

	{
		sgm::constNullable<Specimen> nb{2};

		Specimen s1 = nb;
		Specimen const cs1 = nb;

		SGM_H2U_ASSERT(s1 == Specimen(2) && cs1 == Specimen(2));
	}
	{
		sgm::Nullable<Specimen> nb{2};

		Specimen const s = nb.move();

		SGM_H2U_ASSERT( s == Specimen(2) && nb.v() == Specimen::State::MOVE_AWAY );
	}
	{
		sgm::constNullable<Specimen> nb{2};

		Specimen s = nb.move();

		SGM_H2U_ASSERT( s == Specimen(2) && nb.v() != Specimen::State::MOVE_AWAY );
	}

	mdo << empty_line;
}


static void Substitutions()
{
	using namespace sgm::h2u;

	mdo << Title(L"Assignment Operations");

	mdo
	<<	Title(L"Null Assignment", 2)
	<<	L"A Nullable containing a value can be reset to null state using sgm::Null_t assignment."
	<<	newl;

BEGIN_CODE_BLOCK(null_assignment_ex)
	{
		sgm::Nullable<Specimen> nb(2);

		SGM_H2U_ASSERT( nb.has_value() && nb == Specimen(2) );

		nb = sgm::Null_t{};

		SGM_H2U_ASSERT(!nb.has_value());
	}
END_CODE_BLOCK_AND_LOAD(null_assignment_ex)

	mdo
	<<	Title(L"Value Assignment", 2)
	<<	L"Direct value assignment replaces the current contents with a new value, "
	<<	L"automatically converting types when possible."
	<<	newl;

BEGIN_CODE_BLOCK(value_assignment_ex)
	{
		sgm::Nullable<Specimen> nb(2);

		SGM_H2U_ASSERT( nb == Specimen(2) );

		nb = 4;

		SGM_H2U_ASSERT( nb == Specimen(4) );
	}
END_CODE_BLOCK_AND_LOAD(value_assignment_ex)

	{
		sgm::Nullable<Specimen> nb1(2);

		SGM_H2U_ASSERT( nb1 == Specimen(2) );

		sgm::Nullable<int> nb2(4);

		nb1 = nb2;

		SGM_H2U_ASSERT( nb1 == Specimen(4) );
	}
	{
		sgm::Nullable<Specimen> nb1(2);

		SGM_H2U_ASSERT( nb1 == Specimen(2) );

		sgm::Nullable<int> nb2(4);

		nb1 = sgm::Move(nb2);

		SGM_H2U_ASSERT( nb1 == Specimen(4) );
	}
	{
		sgm::Nullable<Specimen> nb1(2);

		SGM_H2U_ASSERT( nb1 == Specimen(2) );

		sgm::Nullable<Specimen> nb2(4);

		nb1 = nb2;

		SGM_H2U_ASSERT( nb1 == Specimen(4) && nb1 == Specimen::State::COPY_ASSIGNMENT );
	}
	{
		sgm::Nullable<Specimen> nb1(2);

		SGM_H2U_ASSERT( nb1 == Specimen(2) );

		sgm::Nullable<Specimen> nb2(4);

		nb1 = sgm::Move(nb2);

		SGM_H2U_ASSERT( nb1 == Specimen(4) && nb1 == Specimen::State::MOVE_ASSIGNMENT );
	}
	{
		sgm::Nullable<Specimen> nb(2);

		SGM_H2U_ASSERT( nb.has_value() && nb == Specimen(2) );

		nb = sgm::Null_t{};

		SGM_H2U_ASSERT(!nb.has_value());
	}
	{
		sgm::Nullable<Specimen> nb = sgm::Null_t{};

		SGM_H2U_ASSERT(!nb.has_value());

		nb = 4;

		SGM_H2U_ASSERT( nb == Specimen(4) );
	}
	{
		sgm::Nullable<Specimen> nb1 = sgm::Null_t{};

		SGM_H2U_ASSERT(!nb1.has_value());

		sgm::Nullable<int> nb2(4);

		nb1 = nb2;

		SGM_H2U_ASSERT( nb1 == Specimen(4) );
	}
	{
		sgm::Nullable<Specimen> nb1 = sgm::Null_t{};

		SGM_H2U_ASSERT(!nb1.has_value());

		sgm::Nullable<int> nb2(4);

		nb1 = sgm::Move(nb2);

		SGM_H2U_ASSERT( nb1 == Specimen(4) );
	}
	{
		sgm::Nullable<Specimen> nb1 = sgm::Null_t{};

		SGM_H2U_ASSERT(!nb1.has_value());

		sgm::Nullable<Specimen> nb2(4);

		nb1 = nb2;

		SGM_H2U_ASSERT( nb1 == Specimen(4) && nb1 == Specimen::State::COPY_CONSTRUCTION );
	}
	{
		sgm::Nullable<Specimen> nb1 = sgm::Null_t{};

		SGM_H2U_ASSERT(!nb1.has_value());

		sgm::Nullable<Specimen> nb2(4);

		nb1 = sgm::Move(nb2);

		SGM_H2U_ASSERT( nb1 == Specimen(4) && nb1 == Specimen::State::MOVE_CONSTRUCTION );
	}

	mdo << empty_line;
}


static void Get()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Value Access")
	<<	L"The v() method provides direct access to the contained value, but throws an exception "
	<<	L"if called on an empty Nullable."
	<<	newl;

BEGIN_CODE_BLOCK(value_access_ex)
	{
		sgm::Nullable<Specimen> nb1 = sgm::Null_t{}, nb2( Specimen(2) );

		SGM_H2U_ASSERT
		(	!nb1.has_value()
		//&&	nb1.v()		// fragment error in run time
		&&	nb2.has_value() && nb2 == nb2.v() && nb2.v() == Specimen(2)
		);
	}
END_CODE_BLOCK_AND_LOAD(value_access_ex)

	{
		//	sgm::Null_t{}.v();	// compile error : deleted function
	}

	mdo << empty_line;
}


static void Get_or()
{
	using namespace sgm::h2u;

	mdo << Title(L"Safe Value Access with Default");

	mdo
	<<	Title(L"Exception Handling", 2)
	<<	L"Calling v_or() without arguments on a null value throws Bad_Access_to_Nullable exception."
	<<	newl;

BEGIN_CODE_BLOCK(exception_handling_ex)
	{
		sgm::Null_t constexpr null_ex{};
		bool exception_occurred_ex = false;

		try
		{
			null_ex.v_or();
		}
		catch(sgm::Bad_Access_to_Nullable const&)
		{
			exception_occurred_ex = true;
		}

		SGM_H2U_ASSERT(exception_occurred_ex);
	}
END_CODE_BLOCK_AND_LOAD(exception_handling_ex)

	mdo
	<<	Title(L"Default Value Fallback", 2)
	<<	L"Providing a default value to v_or() returns that value when the Nullable is null, "
	<<	L"avoiding exceptions entirely."
	<<	newl;

BEGIN_CODE_BLOCK(default_value_ex)
	{
		sgm::Null_t constexpr null_def{};
		Specimen s_def(2);
		bool exception_occurred_def = false;

		try
		{
			s_def = null_def.v_or( Specimen(4) );
		}
		catch(sgm::Bad_Access_to_Nullable const&)
		{
			exception_occurred_def = true;
		}

		SGM_H2U_ASSERT( !exception_occurred_def && s_def == Specimen(4) );
	}
END_CODE_BLOCK_AND_LOAD(default_value_ex)

	{
		sgm::Nullable<Specimen> nb = sgm::Null_t{};
		bool exception_occured = false;

		try
		{
			nb.v_or();
		}
		catch(sgm::Bad_Access_to_Nullable const&)
		{
			exception_occured = true;
		}

		SGM_H2U_ASSERT(exception_occured && !nb.has_value());
	}
	{
		sgm::Nullable<Specimen> nb = sgm::Null_t{};
		Specimen s(2);
		bool exception_occured = false;

		try
		{
			s = nb.v_or( Specimen(4) );
		}
		catch(sgm::Bad_Access_to_Nullable const&)
		{
			exception_occured = true;
		}

		SGM_H2U_ASSERT( !exception_occured && s == Specimen(4) );
	}
	{
		sgm::Nullable<Specimen> nb( Specimen(2) );
		bool exception_occured = false;

		try
		{
			nb.v_or();
		}
		catch(sgm::Bad_Access_to_Nullable const&)
		{
			exception_occured = true;
		}

		SGM_H2U_ASSERT(!exception_occured && nb.has_value());
	}
	{
		sgm::Nullable<Specimen> nb( Specimen(2) );
		Specimen s(6);
		bool exception_occured = false;

		try
		{
			s = nb.v_or( Specimen(4) );
		}
		catch(sgm::Bad_Access_to_Nullable const&)
		{
			exception_occured = true;
		}

		SGM_H2U_ASSERT( !exception_occured && nb.has_value() && s == Specimen(2) );
	}

	mdo << empty_line;
}


static void Operations()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Operator Overloading")
	<<	L"Nullable types can participate in operations by forwarding calls to their contained values, "
	<<	L"enabling transparent usage in expressions."
	<<	newl;

BEGIN_CODE_BLOCK(operator_class_ex)
	class AA
	{
	public:
		AA(Specimen const s) : _s(s){}

		auto get_h2u()-> Specimen&{  return _s;  }
		auto get_h2u() const-> Specimen const&{  return _s;  }

		auto operator+(AA const& aa) const-> AA{  return {_s.value() + aa._s.value()};  }

		auto operator*() const-> Specimen const&{  return get_h2u();  }
		auto operator*()-> Specimen&{  return get_h2u();  }

	private:
		Specimen _s;
	};
END_CODE_BLOCK_AND_LOAD(operator_class_ex)

	mdo
	<<	L"The Nullable wrapper forwards operator calls to the contained object, allowing "
	<<	L"natural usage patterns with custom types."
	<<	newl;

BEGIN_CODE_BLOCK(operator_usage_ex)
	{
		sgm::Nullable<AA> nb(  AA( Specimen(2) )  );

		AA const aa2 = nb + AA( Specimen(10) );

		SGM_H2U_ASSERT( *nb == Specimen(2) && aa2.get_h2u() == Specimen(12) );

		*nb = Specimen(4);

		SGM_H2U_ASSERT( *nb == Specimen(4) && nb.v().get_h2u() == *nb );
	}
END_CODE_BLOCK_AND_LOAD(operator_usage_ex)

	{
		sgm::constNullable<AA> nb(  AA( Specimen(2) )  );

		AA const aa = nb + AA( Specimen(10) );

		SGM_H2U_ASSERT( *nb == Specimen(2) && aa.get_h2u() == Specimen(12) );

		// *nb = Specimen(4);	// compile error
	}

	mdo << empty_line;
}


static void invoked()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Function Invocation")
	<<	L"Nullable objects can be passed directly to functions expecting their underlying type, "
	<<	L"with automatic conversion and proper const-correctness enforcement."
	<<	newl;

BEGIN_CODE_BLOCK(function_invocation_ex)
	{
		sgm::Nullable<Specimen> nb( Specimen(2) );
		auto invoke_f = [](Specimen& s){  s = Specimen(4);  };

		invoke_f(nb);

		SGM_H2U_ASSERT( nb.v() == Specimen(4) );
	}
END_CODE_BLOCK_AND_LOAD(function_invocation_ex)

	{
		sgm::constNullable<Specimen> cnb( Specimen(2) );
		auto invoke_f = [](Specimen& s){  s = Specimen(4);  };
		auto cinvoke_f = [](Specimen const&){};

		static_assert
		(	(	!sgm::Has_Operator_invocation<decltype(invoke_f), decltype(cnb)>::value
			&&	sgm::Has_Operator_invocation<decltype(cinvoke_f), decltype(cnb)>::value
			)
		,	""
		);
	}

	mdo << empty_line;
}


static void Abbreviable_Nullable()
{
	using namespace sgm::h2u;

	mdo
	<<	Title(L"Nested Nullable (Abbreviable)")
	<<	L"The abbreviable feature automatically unwraps nested Nullable types, simplifying "
	<<	L"access to deeply nested optional values."
	<<	newl;

BEGIN_CODE_BLOCK(abbreviable_nullable_ex)
	sgm::Nullable< sgm::Nullable<Specimen> > nnb;
	sgm::Nullable<  sgm::constNullable< sgm::Nullable<Specimen> >  > ncnnb;

	static_assert
	(	(	sgm::is_Same<decltype(nnb.v()), Specimen&>::value
		&&	sgm::is_Same<decltype(ncnnb.v()), Specimen const&>::value
		)
	,	""
	);
END_CODE_BLOCK_AND_LOAD(abbreviable_nullable_ex)

	mdo << empty_line;
}


static void outro()
{

}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_HOW2USE_TESTS(sgm::h2u::Test_, Nullable, /**/)
{	::intro
,	::Null_type
,	::Constructions
,	::Conversions
,	::Substitutions
,	::Get
,	::Get_or
,	::Operations
,	::invoked
,	::Abbreviable_Nullable
,	::outro
};