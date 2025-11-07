/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/FP/Countable.hpp"
#include "SGM/FP/High_Templar.hpp"
#include "Test_High_Templar.hpp"
#include <forward_list>


using sgm::h2u::Specimen;
using sgm::h2u::Are_Equivalent_Ranges;


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
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void Countable_Test()
{
	SGM_H2U_ASSERT
	(	Are_Equivalent_Ranges
		(	sgm::Countable<int>(5)
		,	std::initializer_list<int>{0, 1, 2, 3, 4}
		)
	&&	Are_Equivalent_Ranges
		(	sgm::Countable<int>(5, 1)
		,	std::initializer_list<int>{1, 2, 3, 4, 5}
		)
	);

	{
		auto const seq = sgm::Countable<int>(5);
		auto const rseq_arr = sgm::Array<int>(seq.rbegin(), seq.rend());
	
		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	rseq_arr
			,	std::initializer_list<int>{4, 3, 2, 1, 0}
			)
		);
	}
}


static void Morph_Test()
{
	auto triple_f = [](int x)-> Specimen{  return 3*x;  };
	
	std::initializer_list<Specimen> const answer
	{	Specimen(0), Specimen(3), Specimen(6), Specimen(9), Specimen(12)
	};

	
	SGM_H2U_ASSERT
	(	Are_Equivalent_Ranges
		(	sgm::Morph( sgm::Countable<int>(5), triple_f ), answer  
		)
	);
	
	{
		auto itr = answer.begin();

		for(  auto&& x : sgm::Morph( sgm::Countable<int>(5), triple_f )  )
			SGM_H2U_ASSERT(x == *itr++);
	}
	{
		auto arr = sgm::Morph( sgm::Countable<int>(5), triple_f ).eval();

		static_assert
		(	sgm::is_Same
			<	decltype(arr), sgm::Array<Specimen, sgm::arrSize::DYNAMIC> 
			>::	value
		,	""
		);

		SGM_H2U_ASSERT( Are_Equivalent_Ranges(arr, answer) );
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

		SGM_H2U_ASSERT( Are_Equivalent_Ranges(arr, answer) );
		SGM_H2U_ASSERT(count == 5);
	}
	{
		auto arr 
		=	sgm::Morph( sgm::Countable<int>(5), triple_f ).eval< sgm::Fork_and_Join_Flag<5> >();

		static_assert
		(	sgm::is_Same< decltype(arr), sgm::Array<Specimen, sgm::arrSize::DYNAMIC> >::value
		,	""
		);

		SGM_H2U_ASSERT( Are_Equivalent_Ranges(arr, answer) );
	}
	{
		std::forward_list<int> fLi{0, 1, 2, 3, 4};

		auto arr = sgm::Morph(fLi, triple_f).eval();

		SGM_H2U_ASSERT( Are_Equivalent_Ranges(arr, answer) );
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

		SGM_H2U_ASSERT( Are_Equivalent_Ranges(arr, answer) );
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

	SGM_H2U_ASSERT
	(	Are_Equivalent_Ranges
		(	aa.val
		,	sgm::Array<int, 5>{8, 6, 4, 2, 0}
		)
	);
}


static void Filter_Test()
{
	auto is_odd_f = [](Specimen const& s)-> bool{  return s.value() % 2 == 1;  };
	auto make_h2u_f = [](int const& x)-> Specimen{  return x;  };

	sgm::Array<Specimen> const inputs
	=	sgm::Morph( sgm::Countable<int>(10), make_h2u_f );

	std::initializer_list<Specimen> const answer
	{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9)
	};

	
	SGM_H2U_ASSERT(  Are_Equivalent_Ranges( sgm::Filter(inputs, is_odd_f), answer )  );
	
	{
		auto itr = answer.begin();

		for( auto&& x : sgm::Filter(inputs, is_odd_f) )
			SGM_H2U_ASSERT(x == *itr++);
	}
	{
		auto arr = sgm::Filter(inputs, is_odd_f).eval();

		static_assert
		(	sgm::is_Same< decltype(arr), sgm::Array<Specimen, sgm::arrSize::DYNAMIC> >::value
		,	""
		);

		SGM_H2U_ASSERT( Are_Equivalent_Ranges(arr, answer) );
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

		SGM_H2U_ASSERT( Are_Equivalent_Ranges(arr, answer) );
		SGM_H2U_ASSERT(count == 5);		
	}
}


static void Fold_Test()
{
	auto const seq = sgm::Countable<int>(100, 1);
	auto add_f = [](int x1, int x2)-> int{  return x1 + x2;  };
	int const answer = 5050;
	
	SGM_H2U_ASSERT
	(	sgm::h2u::Are_All_Equivalent_to
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
		)
	);
}


static void Plait_Test()
{
	using sgm::Array;
	using sgm::Family;
	using sgm::h2u::Specimen;

	Array<int> arr_int{1, 3, 5, 7, 9};
	Array<Specimen> arr_h2u{Specimen(0), Specimen(2), Specimen(4), Specimen(6), Specimen(8)};
	std::initializer_list<wchar_t> iL{'a', 'b', 'c', 'd', 'e'};

	auto answer
	=	Array< Family<int, Specimen, wchar_t> >
		{	Family<int, Specimen, wchar_t>(1, Specimen(0), 'a')
		,	Family<int, Specimen, wchar_t>(3, Specimen(2), 'b')
		,	Family<int, Specimen, wchar_t>(5, Specimen(4), 'c')
		,	Family<int, Specimen, wchar_t>(7, Specimen(6), 'd')
		,	Family<int, Specimen, wchar_t>(9, Specimen(8), 'e')
		};

	SGM_H2U_ASSERT(  Are_Equivalent_Ranges( sgm::Plait(arr_int, arr_h2u, iL), answer )  );

	{
		int i2 = 1;
		Specimen s2{0};
		wchar_t c2 = 'a';
		

		for( auto&& [i, s, c] : sgm::Plait(arr_int, arr_h2u, iL) )
		{
			SGM_H2U_ASSERT(i == i2 && s == s2 && c == c2);

			i2 += 2,  s2.value() += 2,  c2++;
		}
	}
		
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


//========//========//========//========//=======#//========//========//========//========//=======#


SGM_HOW2USE_TESTS(sgm::h2u::Test_, High_Templar, /**/)
{	::Countable_Test
,	::Morph_Test
,	::Function_Sequence_Test
,	::Filter_Test
,	::Fold_Test
,	::Plait_Test
};