/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Maybe.hpp"
#include "Test_Maybe.hpp"


using sgm::spec::is_True;
using sgm::spec::Specimen;


static void Constructions()
{
	{
		sgm::Maybe<Specimen> 
			ys1,
			ys2( Specimen(2) ),
			ys3(4),
			ys4 = sgm::NullMyb;

		is_True
		(	!ys1
		&&	ys2.has_value() && ys2.get() == Specimen(2)
		&&	ys3.has_value() && ys3.get() == Specimen(4)
		&&	!ys4
		);
	}
	{
		auto ys1 = sgm::make_Maybe( Specimen(2) );
		sgm::Maybe<Specimen> ys2 = ys1;

		static_assert
		(	sgm::is_Same< decltype(ys1), sgm::Maybe<Specimen> >::value
		,	""
		);

		is_True
		(	ys1.get() == Specimen(2)
		&&	ys1.get() == ys2.get()
		&&	ys2.get() == Specimen::State::COPY_CONSTRUCTION
		);
	}
	{
		auto ys1 = sgm::make_Maybe( Specimen(2) );
		sgm::Maybe<Specimen> ys2 = sgm::Move(ys1);

		static_assert
		(	sgm::is_Same< decltype(ys1), sgm::Maybe<Specimen> >::value
		,	""
		);

		is_True
		(	ys1.get() == Specimen(2)
		&&	ys1.get() == ys2.get()
		&&	ys2.get() == Specimen::State::MOVE_CONSTRUCTION
		);
	}
	{
		sgm::Maybe<int> ys1 = 2, ys2(4);
		sgm::Maybe<Specimen> ys3 = ys1,  ys4 = ys2;
		
		is_True
		(	ys1.get() == ys3->value()
		&&	ys2.get() == ys4->value()
		);
	}
}


static void Substitutions()
{
	{
		sgm::Maybe<Specimen> ys(2);

		is_True(ys.has_value() && ys->value() == 2);

		ys = sgm::NullMyb;

		is_True(!ys);
	}
	{
		sgm::Maybe<Specimen> ys(2);

		is_True(ys->value() == 2);

		ys = 4;

		is_True( *ys == Specimen(4) );
	}
	{
		sgm::Maybe<Specimen> ys1(2);

		is_True(ys1->value() == 2);

		sgm::Maybe<int> ys2(4);

		ys1 = ys2;

		is_True( *ys1 == Specimen(4) );	
	}
	{
		sgm::Maybe<Specimen> ys1(2);

		is_True(ys1->value() == 2);

		sgm::Maybe<int> ys2(4);

		ys1 = sgm::Move(ys2);

		is_True( *ys1 == Specimen(4) );	
	}
	{
		sgm::Maybe<Specimen> ys1(2);

		is_True(ys1->value() == 2);

		sgm::Maybe<Specimen> ys2(4);

		ys1 = ys2;

		is_True( *ys1 == Specimen(4) && *ys1 == Specimen::State::COPY_ASSIGNMENT );	
	}
	{
		sgm::Maybe<Specimen> ys1(2);

		is_True(ys1->value() == 2);

		sgm::Maybe<Specimen> ys2(4);

		ys1 = sgm::Move(ys2);

		is_True( *ys1 == Specimen(4) && *ys1 == Specimen::State::MOVE_ASSIGNMENT );	
	}
}


static void get_or()
{
	sgm::Maybe<Specimen> ys1 = sgm::NullMyb,  ys2(2);

	is_True
	(	!ys1 && ys1.get_or( Specimen(4) ) == Specimen(4)
	&&	ys2.has_value() && ys2.get_or( Specimen(4) ) == Specimen(2)
	);
}


static void Abbreviable_Maybe()
{
	sgm::Maybe< sgm::Maybe<Specimen> > yys;

	static_assert
	(	sgm::is_Same<decltype(yys.get()), Specimen&>::value
	,	""
	);
}


static void constMaybe()
{
	static_assert
	(	(	sgm::is_constMaybe< sgm::constMaybe<Specimen> >::value
		&&	sgm::is_constMaybe<  sgm::Maybe< sgm::Maybe<Specimen const> >  >::value
		&&	sgm::is_constMaybe<  sgm::Maybe< sgm::Maybe<Specimen> const >  >::value
		&&	!sgm::is_constMaybe<  sgm::Maybe< sgm::Maybe<Specimen> > const  >::value
		)
	,	""
	);

	sgm::constMaybe<Specimen> ys(5);

	is_True
	(	*ys == Specimen(5)
	&&	ys->value() == 5
	&&	ys.get() == Specimen(5)
	&&	ys.get_or( Specimen(7) ) == Specimen(5)
	);

	//ys = Specimen(4);	// Compile Ban.
	//ys->value() = Specimen(2); // Compile Ban.
	//*ys = Specimen(2); // Compile Ban.
	//ys.get() = Specimen(2); // Compile Ban.
	//ys.get_or( Specimen(2) ) = Specimen(6) // Compile Ban.
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Maybe, /**/)
{	::Constructions
,	::Substitutions
,	::get_or
,	::Abbreviable_Maybe
,	::constMaybe
};
