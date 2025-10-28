/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Container/iterable.hpp"
#include "Test_iterable.hpp"

#include <vector>
#include <list>
#include <forward_list>


static void is_iterable_Test()
{
	static_assert
	(	(	sgm::is_iterable< std::vector<int> >::value
		&&	sgm::is_iterable< std::list<int> >::value
		&&	sgm::is_iterable< std::forward_list<int> >::value
		&&	sgm::is_iterable< std::initializer_list<int> >::value
		&&	sgm::is_iterable< int[10] >::value
		)
	,	""
	);
}


static void is_iterator_Test()
{
	using vec_itr_t = std::vector<int>::iterator;
	using list_itr_t = std::list<int>::iterator;
	using flist_itr_t = std::forward_list<int>::iterator;
	
	static_assert
	(	(	sgm::is_iterator<vec_itr_t>::value
		&&	sgm::is_iterator<list_itr_t>::value
		&&	sgm::is_iterator<flist_itr_t>::value
		&&	sgm::is_iterator<int*>::value
		)
	,	""
	);

	static_assert
	(	(	sgm::is_bidirectional_iterator<vec_itr_t>::value
		&&	sgm::is_bidirectional_iterator<list_itr_t>::value
		&&	!sgm::is_bidirectional_iterator<flist_itr_t>::value
		&&	sgm::is_bidirectional_iterator<int*>::value
		)
	,	""
	);

	static_assert
	(	(	sgm::is_random_access_iterator<vec_itr_t>::value
		&&	!sgm::is_random_access_iterator<list_itr_t>::value
		&&	!sgm::is_random_access_iterator<flist_itr_t>::value
		&&	sgm::is_random_access_iterator<int*>::value
		)
	,	""
	);
}


static void Reverse_iterator_Test()
{
	std::vector<int> numbers = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	auto ritr1 = numbers.rbegin();
	auto ritr2 = sgm::rBegin(numbers);

	for(;  ritr1 != numbers.rend();  ++ritr1,  ++ritr2)
		SGM_H2U_ASSERT(*ritr1 == *ritr2);
}


static void Size_Test()
{
	int arr[5] = {0, 1, 2, 3, 4};
	std::vector<int> vec{0, 1, 2, 3, 4};
	std::list<int> li{0, 1, 2, 3, 4};
	std::forward_list<int> fli{0, 1, 2, 3, 4};

	SGM_H2U_ASSERT
	(	sgm::Size(arr) == 5
	&&	sgm::Size(vec) == 5
	&&	sgm::Size(li) == 5
	&&	sgm::Size(fli) == 5
	);
}


static void iterator_Wrapper_Test()
{
	using fLitr_t = std::forward_list<int>::iterator;
	using bLitr_t = std::list<int>::iterator;

	static_assert
	(	(	!sgm::is_random_access_iterator<bLitr_t>::value
		&&	sgm::is_bidirectional_iterator<bLitr_t>::value
		&&	!sgm::is_random_access_iterator<fLitr_t>::value
		&&	!sgm::is_bidirectional_iterator<fLitr_t>::value
		&&	sgm::is_iterator<fLitr_t>::value
		)
	,	""
	);


	using reverse_bLitr_t = sgm::Reverse_iterator<bLitr_t>;

	static_assert
	(	(	!sgm::is_random_access_iterator<reverse_bLitr_t>::value
		&&	sgm::is_bidirectional_iterator<reverse_bLitr_t>::value
		)
	,	""
	);


	using move_fLitr_t = sgm::Move_iterator<fLitr_t>;
	using move_bLitr_t = sgm::Move_iterator<bLitr_t>;

	static_assert
	(	(	!sgm::is_random_access_iterator<move_bLitr_t>::value
		&&	sgm::is_bidirectional_iterator<move_bLitr_t>::value
		&&	!sgm::is_random_access_iterator<move_fLitr_t>::value
		&&	!sgm::is_bidirectional_iterator<move_fLitr_t>::value
		&&	sgm::is_iterator<move_fLitr_t>::value
		)
	,	""
	);
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_HOW2USE_TESTS(sgm::h2u::Test_, iterable, /**/)
{	::is_iterable_Test
,	::is_iterator_Test
,	::Reverse_iterator_Test
,	::Size_Test
,	::iterator_Wrapper_Test
};