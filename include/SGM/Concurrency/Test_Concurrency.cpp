/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Concurrency.hpp"
#include "Test_Concurrency.hpp"


using sgm::spec::is_True;


static void Parallel_Sum_with_Static_Fork_n_Join()
{
	size_t constexpr NTASK = 4;

	int results[NTASK] = {0,};

	auto sum_f
	=	[&results](size_t const bidx, size_t const eidx, unsigned const task_id)
		{
			auto& res = results[task_id];
			auto const last_n = static_cast<int>(eidx);

			for(auto n = static_cast<int>(bidx);  n < last_n;  res += ++n);
		};

	sgm::Fork_and_Join<4>()(100, sum_f);

	int const answer 
	=	[&results]
		{
			int res = 0;

			for(auto const x : results)
				res += x;
				
			return res;
		}();

	is_True(answer == 5050);
}


static void Parallel_Sum_with_Dynamic_Fork_n_Join()
{
	size_t constexpr NTASK = 4;

	int results[NTASK] = {0,};

	auto sum_f
	=	[&results](size_t const bidx, size_t const eidx, unsigned const task_id)
		{
			auto& res = results[task_id];
			auto const last_n = static_cast<int>(eidx);

			for(auto n = static_cast<int>(bidx);  n < last_n;  res += ++n);
		};

	unsigned const tasks = 4;

	sgm::Fork_and_Join<>{tasks}(100, sum_f);

	int const answer 
	=	[&results]
		{
			int res = 0;

			for(auto const x : results)
				res += x;
				
			return res;
		}();

	is_True(answer == 5050);	
}


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Concurrency, /**/)
{	::Parallel_Sum_with_Static_Fork_n_Join
,	::Parallel_Sum_with_Dynamic_Fork_n_Join
};