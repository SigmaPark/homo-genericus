/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Nullable.hpp"
#include "Test_Nullable.hpp"


using sgm::spec::is_True;
using sgm::spec::Specimen;


static void intro()
{
}


static void Null_type()
{
	sgm::Null_t constexpr null{};

	static_assert
	(	(	sgm::is_Nullable<decltype(null)>::value
		&&	sgm::is_constNullable<decltype(null)>::value
		)
	,	""
	);

	is_True(!null.has_value());
}


static void Constructions()
{
	{
		auto constexpr n = sgm::Nullable<double>{};

		is_True(!n.has_value());
	}
	{
		sgm::Nullable<Specimen>
			nb1,
			nb2( Specimen(2) ),
			nb3(4),
			nb4 = sgm::Null_t{};

		is_True
		(	!nb1.has_value()
		&&	nb2.has_value() && nb2 == Specimen(2)
		&&	nb3.has_value() && nb3 == Specimen(4)
		&&	!nb4.has_value()
		);
	}
	{
		auto nb1 = sgm::make_Nullable( Specimen(2) );
		sgm::Nullable<Specimen> nb2 = nb1;

		static_assert
		(	sgm::is_Same< decltype(nb1), sgm::Nullable<Specimen> >::value
		,	""
		);

		is_True
		(	nb1 == Specimen(2)
		&&	nb1 == nb2
		&&	nb2 == Specimen::State::COPY_CONSTRUCTION
		);
	}
	{
		auto nb1 = sgm::make_Nullable( Specimen(2) );
		sgm::Nullable<Specimen> nb2 = sgm::Move(nb1);

		static_assert
		(	sgm::is_Same< decltype(nb1), sgm::Nullable<Specimen> >::value
		,	""
		);

		is_True
		(	nb1 == Specimen(2)
		&&	nb1 == nb2
		&&	nb2 == Specimen::State::MOVE_CONSTRUCTION
		);		
	}
}


static void Substitutions()
{
	{
		sgm::Nullable<Specimen> nb(2);

		is_True( nb.has_value() && nb == Specimen(2) );

		nb = sgm::Null_t{};

		is_True(!nb.has_value());
	}
	{
		sgm::Nullable<Specimen> nb(2);

		is_True( nb == Specimen(2) );

		nb = 4;

		is_True( nb == Specimen(4) );
	}
	{
		sgm::Nullable<Specimen> nb1(2);

		is_True( nb1 == Specimen(2) );

		sgm::Nullable<int> nb2(4);

		nb1 = nb2;

		is_True( nb1 == Specimen(4) );		
	}
	{
		sgm::Nullable<Specimen> nb1(2);

		is_True( nb1 == Specimen(2) );

		sgm::Nullable<int> nb2(4);

		nb1 = sgm::Move(nb2);

		is_True( nb1 == Specimen(4) );		
	}
	{
		sgm::Nullable<Specimen> nb1(2);

		is_True( nb1 == Specimen(2) );

		sgm::Nullable<Specimen> nb2(4);

		nb1 = nb2;

		is_True( nb1 == Specimen(4) && nb1 == Specimen::State::COPY_ASSIGNMENT );		
	}
	{
		sgm::Nullable<Specimen> nb1(2);

		is_True( nb1 == Specimen(2) );

		sgm::Nullable<Specimen> nb2(4);

		nb1 = sgm::Move(nb2);

		is_True( nb1 == Specimen(4) && nb1 == Specimen::State::MOVE_ASSIGNMENT );		
	}
	{
		sgm::Nullable<Specimen> nb(2);

		is_True( nb.has_value() && nb == Specimen(2) );

		nb = sgm::Null_t{};

		is_True(!nb.has_value());
	}
	{
		sgm::Nullable<Specimen> nb = sgm::Null_t{};

		is_True(!nb.has_value());

		nb = 4;

		is_True( nb == Specimen(4) );
	}
	{
		sgm::Nullable<Specimen> nb1 = sgm::Null_t{};

		is_True(!nb1.has_value());

		sgm::Nullable<int> nb2(4);

		nb1 = nb2;

		is_True( nb1 == Specimen(4) );		
	}
	{
		sgm::Nullable<Specimen> nb1 = sgm::Null_t{};

		is_True(!nb1.has_value());

		sgm::Nullable<int> nb2(4);

		nb1 = sgm::Move(nb2);

		is_True( nb1 == Specimen(4) );		
	}
	{
		sgm::Nullable<Specimen> nb1 = sgm::Null_t{};

		is_True(!nb1.has_value());

		sgm::Nullable<Specimen> nb2(4);

		nb1 = nb2;

		is_True( nb1 == Specimen(4) && nb1 == Specimen::State::COPY_CONSTRUCTION );		
	}
	{
		sgm::Nullable<Specimen> nb1 = sgm::Null_t{};

		is_True(!nb1.has_value());

		sgm::Nullable<Specimen> nb2(4);

		nb1 = sgm::Move(nb2);

		is_True( nb1 == Specimen(4) && nb1 == Specimen::State::MOVE_CONSTRUCTION );		
	}
}


static void Get()
{
	{
		//	sgm::Null_t{}.v();	// compile error : deleted function
	}
	{
		sgm::Nullable<Specimen> nb1 = sgm::Null_t{}, nb2( Specimen(2) );

		is_True
		(	!nb1.has_value()
		//&&	nb1.v()		// fragment error in run time
		&&	nb2.has_value() && nb2 == nb2.v() && nb2.v() == Specimen(2)
		);
	}
}


static void Get_or()
{
	{
		sgm::Null_t constexpr null{};
		bool exception_occured = false;

		try
		{
			null.v_or();
		}
		catch(sgm::Bad_Access_to_Nullable const&)
		{
			exception_occured = true;
		}

		is_True(exception_occured);
	}
	{
		sgm::Null_t constexpr null{};
		Specimen s(2);
		bool exception_occured = false;

		try
		{
			s = null.v_or( Specimen(4) );
		}
		catch(sgm::Bad_Access_to_Nullable const&)
		{
			exception_occured = true;
		}

		is_True( !exception_occured && s == Specimen(4) );
	}
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

		is_True(exception_occured && !nb.has_value());
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

		is_True( !exception_occured && s == Specimen(4) );
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

		is_True(!exception_occured && nb.has_value());
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

		is_True( !exception_occured && nb.has_value() && s == Specimen(2) );
	}
}


static void Operations()
{
	class AA
	{
	public:
		AA(Specimen const s) : _s(s){}

		auto get_spec()-> Specimen&{  return _s;  }
		auto get_spec() const-> Specimen const&{  return _s;  }

		auto operator+(AA const& aa) const-> AA{  return {_s.value() + aa._s.value()};  }

		auto operator*() const-> Specimen const&{  return get_spec();  }
		auto operator*()-> Specimen&{  return get_spec();  }

	private:
		Specimen _s;
	};


	{
		sgm::Nullable<AA> nb(  AA( Specimen(2) )  );

		AA const aa2 = nb + AA( Specimen(10) );

		is_True( *nb == Specimen(2) && aa2.get_spec() == Specimen(12) );

		*nb = Specimen(4);

		is_True( *nb == Specimen(4) && nb.v().get_spec() == *nb );
	}
	{
		sgm::constNullable<AA> nb(  AA( Specimen(2) )  );

		AA const aa2 = nb + AA( Specimen(10) );

		is_True( *nb == Specimen(2) && aa2.get_spec() == Specimen(12) );

		// *nb = Specimen(4);	// compile error
	}
}


static void invoked()
{
	{
		sgm::Nullable<Specimen> nb( Specimen(2) );
		auto invoke_f = [](Specimen& s){  s = Specimen(4);  };

		invoke_f(nb);

		is_True( nb.v() == Specimen(4) );
	}
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
}


static void Abbreviable_Nullable()
{
	sgm::Nullable< sgm::Nullable<Specimen> > nnb;
	sgm::Nullable<  sgm::constNullable< sgm::Nullable<Specimen> >  > ncnnb;

	static_assert
	(	(	sgm::is_Same<decltype(nnb.v()), Specimen&>::value
		&&	sgm::is_Same<decltype(ncnnb.v()), Specimen const&>::value
		)
	,	""
	);
}


static void outro()
{
	
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Nullable, /**/)
{	::intro
,	::Null_type
,	::Constructions
,	::Substitutions
,	::Get
,	::Get_or
,	::Operations
,	::invoked
,	::Abbreviable_Nullable
,	::outro
};