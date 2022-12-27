/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Countable.hpp"
#include "High_Templar.hpp"
#include "Test_High_Templar.hpp"


using sgm::spec::Specimen;


static void Countable_Test()
{
	sgm::spec::Are_Equivalent_Ranges
	(	sgm::Countable<int>(5)
	,	std::initializer_list<int>{0, 1, 2, 3, 4}
	);

	sgm::spec::Are_Equivalent_Ranges
	(	sgm::Countable<int>(5, 1)
	,	std::initializer_list<int>{1, 2, 3, 4, 5}
	);

	{
		auto const seq = sgm::Countable<int>(5);
		auto const rseq_arr = sgm::Array<int>(seq.rbegin(), seq.rend());
	
		sgm::spec::Are_Equivalent_Ranges
		(	rseq_arr
		,	std::initializer_list<int>{4, 3, 2, 1, 0}
		);
	}

}


class Testing_Allocator : public sgm::Allocator<Specimen>
{
public:
	Testing_Allocator(int& count) : Allocator<Specimen>(), _pcount(&count){}

public:
	using value_type = Specimen;
	using pointer = value_type*;
	using const_pointer = value_type const*;
	using void_pointer = void*;
	using const_void_pointer = void const*;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	void construct(Specimen* p, Specimen val)
	{
		++*_pcount,  Allocator<Specimen>::construct(p, val);  
	}

	void destroy(Specimen* p){  --*_pcount;  Allocator<Specimen>::destroy(p);  }

private:
	int* _pcount;
};


static void Morph_Test()
{
	auto triple_f = [](int x)-> Specimen{  return 3*x;  };
	
	std::initializer_list<Specimen> const answer
	{	Specimen(0), Specimen(3), Specimen(6), Specimen(9), Specimen(12)
	};

	{
		sgm::spec::Are_Equivalent_Ranges
		(	sgm::Morph( sgm::Countable<int>(5), triple_f ), answer  
		);
	}
	{
		auto itr = answer.begin();

		for(  auto&& x : sgm::Morph( sgm::Countable<int>(5), triple_f )  )
			sgm::spec::is_True(x == *itr++);
	}
	{
		auto arr = sgm::Morph( sgm::Countable<int>(5), triple_f ).eval();

		static_assert
		(	sgm::is_Same
			<	decltype(arr), sgm::Array<Specimen, sgm::arrSize::DYNAMIC> 
			>::	value
		,	""
		);

		sgm::spec::Are_Equivalent_Ranges(arr, answer);
	}
	{
		int count = 0;

		auto arr 
		=	sgm::Morph( sgm::Countable<int>(5), triple_f ).eval( ::Testing_Allocator(count) );

		static_assert
		(	sgm::is_Same
			<	decltype(arr), sgm::Array<Specimen, sgm::arrSize::DYNAMIC, ::Testing_Allocator>
			>::	value
		,	""
		);

		sgm::spec::Are_Equivalent_Ranges(arr, answer);
		sgm::spec::is_True(count == 5);
	}
	{
		auto arr 
		=	sgm::Morph( sgm::Countable<int>(5), triple_f ).eval< sgm::Fork_and_Join_Flag<5> >();

		static_assert
		(	sgm::is_Same< decltype(arr), sgm::Array<Specimen, sgm::arrSize::DYNAMIC> >::value
		,	""
		);

		sgm::spec::Are_Equivalent_Ranges(arr, answer);
	}
#if SGM_CXX_STANDARD >= 2017 && defined(_MSC_VER)
	{
		auto arr 
		=	sgm::Morph( sgm::Countable<int>(5), triple_f )
			.	eval<std::execution::parallel_policy>();

		static_assert
		(	sgm::is_Same< decltype(arr), sgm::Array<Specimen, sgm::arrSize::DYNAMIC> >::value
		,	""
		);

		sgm::spec::Are_Equivalent_Ranges(arr, answer);
	}
#endif
}


namespace sgm
{
	namespace _function_sequence_test_detail
	{

		struct AA
		{
			int val[5] = {4, 3, 2, 1, 0};
		};
		
		
		template<class HOST>
		struct _iter_functor
		{
			_iter_functor(HOST *p) : _p(p){}
		
			decltype(auto) operator()(size_t const i){  return _p->val[i];  }
		
		private:
			HOST *_p;
		};		

	}
}


static void Function_Sequence_Test()
{
	using namespace sgm::_function_sequence_test_detail;

	AA aa;
	auto fseq = sgm::Morph( sgm::Countable(5), _iter_functor(&aa) );

	for(auto &x : fseq)
		x *= 2;

	sgm::spec::Are_Equivalent_Ranges
	(	aa.val
	,	sgm::Array<int, 5>{8, 6, 4, 2, 0}
	);
}


static void Filter_Test()
{
	auto is_odd_f = [](Specimen s)-> bool{  return s.value() % 2 == 1;  };
	auto make_spec_f = [](int x)-> Specimen{  return x;  };

	sgm::Array<Specimen> const inputs
	=	sgm::Morph( sgm::Countable<int>(10), make_spec_f );

	std::initializer_list<Specimen> const answer
	{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9)
	};

	{
		sgm::spec::Are_Equivalent_Ranges( sgm::Filter(inputs, is_odd_f), answer );
	}
	{
		auto itr = answer.begin();

		for( auto&& x : sgm::Filter(inputs, is_odd_f) )
			sgm::spec::is_True(x == *itr++);
	}
	{
		auto arr = sgm::Filter(inputs, is_odd_f).eval();

		static_assert
		(	sgm::is_Same< decltype(arr), sgm::Array<Specimen, sgm::arrSize::DYNAMIC> >::value
		,	""
		);

		sgm::spec::Are_Equivalent_Ranges(arr, answer);
	}
	{
		int count = 0;

		auto arr = sgm::Filter(inputs, is_odd_f).eval( ::Testing_Allocator(count) );

		static_assert
		(	sgm::is_Same
			<	decltype(arr), sgm::Array<Specimen, sgm::arrSize::DYNAMIC, ::Testing_Allocator>
			>::	value
		,	""
		);

		sgm::spec::Are_Equivalent_Ranges(arr, answer);
		sgm::spec::is_True(count == 5);		
	}
}


static void Fold_Test()
{
	auto const seq = sgm::Countable<int>(100, 1);
	auto add_f = [](int x1, int x2)-> int{  return x1 + x2;  };
	int const answer = 5050;

	{
		sgm::spec::Are_All_Equivalent_to
		(	std::initializer_list<int>
			{	sgm::Fold(seq, add_f)
			,	sgm::Fold(seq, add_f, 0)
			,	sgm::Fold<sgm::Fork_and_Join_Flag<>>(seq, add_f)
			,	sgm::Fold<sgm::Fork_and_Join_Flag<>>(seq, add_f, 0)
			,	sgm::rFold(seq, add_f)
			,	sgm::rFold(seq, add_f, 0)
			,	sgm::rFold<sgm::Fork_and_Join_Flag<>>(seq, add_f)
			,	sgm::rFold<sgm::Fork_and_Join_Flag<>>(seq, add_f, 0)
			}
		,	answer
		);
	}
}


static void Plait_Test()
{
	using sgm::Array;
	using sgm::Family;
	using sgm::spec::Specimen;

	Array<int> arr_int{1, 3, 5, 7, 9};
	Array<Specimen> arr_spec{Specimen(0), Specimen(2), Specimen(4), Specimen(6), Specimen(8)};
	std::initializer_list<char> iL{'a', 'b', 'c', 'd', 'e'};

	auto answer
	=	Array< Family<int, Specimen, char> >
		{	Family<int, Specimen, char>(1, Specimen(0), 'a')
		,	Family<int, Specimen, char>(3, Specimen(2), 'b')
		,	Family<int, Specimen, char>(5, Specimen(4), 'c')
		,	Family<int, Specimen, char>(7, Specimen(6), 'd')
		,	Family<int, Specimen, char>(9, Specimen(8), 'e')
		};

	sgm::spec::Are_Equivalent_Ranges( sgm::Plait(arr_int, arr_spec, iL), answer );

	{
		int i2 = 1;
		Specimen s2{0};
		char c2 = 'a';
		

		for( auto&& [i, s, c] : sgm::Plait(arr_int, arr_spec, iL) )
			sgm::spec::is_True(i == i2 && s == s2 && c == c2),
			i2 += 2,  s2.value() += 2,  c2++;
	}
		
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, High_Templar, /**/)
{	::Countable_Test
,	::Morph_Test
,	::Function_Sequence_Test
,	::Filter_Test
,	::Fold_Test
,	::Plait_Test
};