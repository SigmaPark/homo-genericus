/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Concurrency.hpp"
#include "Test_Concurrency.hpp"
#include "Poiya_Problem.hpp"


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
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace pipeline_test
{
	
	template<int N>
	struct Type;


	template<class INPUT, class OUTPUT>
	struct Functor;


	class Test_Process;
	class Serial_Process;
	class Async_Process;

	static void Time_per_Unit_Task();

}


template<int N>
struct pipeline_test::Type
{
	static int constexpr value = N;

	std::deque<int> log = {};


	Type() = default;

	template<int _N>
	Type(Type<_N> const& t) : log(t.log){}

	template<int _N>
	Type(Type<_N>&& t) : log( std::move(t.log) ){}
};


template<class INPUT, class OUTPUT>
struct pipeline_test::Functor
{
	static auto constexpr unit_kvalue_v 
#ifdef NDEBUG
	=	std::size_t(10'000);
#else
	=	std::size_t(100);
#endif

	std::size_t amount_of_task = 0;


	auto operator()(INPUT t) const-> OUTPUT
	{
		for(auto d = amount_of_task;  d-->0;)
			sgm::spec::Poiya_Problem(unit_kvalue_v);

		t.log.emplace_back(std::decay_t<INPUT>::value);

		return t;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class pipeline_test::Test_Process
{
public:
	Test_Process(std::deque<std::size_t> const& amount_of_tasks)
	{
		auto itr = amount_of_tasks.begin();

		_f01.amount_of_task = *itr++;
		_f12.amount_of_task = *itr++;
		_f23.amount_of_task = *itr++;
		_f34.amount_of_task = *itr++;
		_f45.amount_of_task = *itr++;
	}

protected:
	Functor< Type<0>, Type<1> > _f01;
	Functor< Type<1>, Type<2> > _f12;
	Functor< Type<2>, Type<3> > _f23;
	Functor< Type<3>, Type<4> > _f34;
	Functor< Type<4>, Type<5> > _f45;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class pipeline_test::Serial_Process : public Test_Process 
{
public:
	template<class Q>
	Serial_Process(std::deque<Q> const& Li) : Test_Process(Li){}


	auto test(std::size_t nof_cycle) const-> std::chrono::milliseconds
	{
		using namespace std::chrono;

		auto const starting_time_point = system_clock::now();

		while(nof_cycle-->0)
			_f45
			(	_f34
				(	_f23
					(	_f12
						(	_f01({})
						)
					)
				)
			);

		auto const total_computing_time 
		=	duration_cast<milliseconds>(system_clock::now() - starting_time_point);

		return total_computing_time;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class pipeline_test::Async_Process : public Test_Process
{
public:
	template<class Q>
	Async_Process(std::deque<Q> const& Li) : Test_Process(Li){}


	auto test(std::size_t nof_cycle) const-> std::chrono::milliseconds
	{
		using namespace std::chrono;

		auto supplier_f
		=	[&nof_cycle]()-> sgm::Pipeline_Data< Type<0> >
			{
				if(nof_cycle != 0)
					return --nof_cycle,  Type<0>{};
				else
					return sgm::Pipeline_Data_State::STOP;
			};

		auto consumer_f
		=	[answer = std::deque<int>{0, 1, 2, 3, 4}](Type<5> const& t)
			{
				if(t.log.size() != answer.size())
				{
					std::cout << "\nWrong Log Size!\n";

					return;
				}

				auto aitr = answer.begin();
				auto titr = t.log.begin();

				while(aitr != answer.end())
					if(*aitr++ != *titr++)
					{
						std::cout << "\nWrong Answer!\n";

						return;
					}
			};

		auto const starting_time_point = system_clock::now();

		sgm::Concurrent_Pipeline::run(supplier_f, _f01, _f12, _f23, _f34, _f45, consumer_f);

		auto const total_computing_time 
		=	duration_cast<milliseconds>(system_clock::now() - starting_time_point);

		return total_computing_time;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


void pipeline_test::Time_per_Unit_Task()
{
	using namespace std::chrono;

	int const N = 100;

	pipeline_test::Functor< pipeline_test::Type<0>, pipeline_test::Type<1> > func{1};


	auto const starting_time_point = system_clock::now();

	for( unsigned d = N;  d-->0; )
		func({});

	auto const total_computing_time 
	=	duration_cast<milliseconds>(system_clock::now() - starting_time_point);

	std::cout 
	<<	"\tTime_per_Unit_Task = " 
	<<	static_cast<double>(total_computing_time.count())/N 
	<<	'\n';
}


static void Pipeline_Test()
{
	using namespace std::chrono;
	using std::cout;

	cout << "Pipeline Test Begins\n";
	
	cout << "\tnof hardware cores = " << std::thread::hardware_concurrency() << '\n';

	pipeline_test::Time_per_Unit_Task();


	std::deque<std::size_t> const amount_of_tasks{2, 2, 2, 2, 2};
	std::size_t const nof_cycle = 100;

	{
		pipeline_test::Serial_Process const serial_proc(amount_of_tasks);

		auto const serial_test_time = serial_proc.test(nof_cycle);

		cout << "\tserial_test_time = " << serial_test_time.count() << '\n';
	}
	{
		pipeline_test::Async_Process const async_proc(amount_of_tasks);

		auto const async_test_time = async_proc.test(nof_cycle);

		cout << "\tasync_test_time = " << async_test_time.count() << '\n';
	}

	cout << "Pipeline Test Ends\n";
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Concurrency, /**/)
{	::Parallel_Sum_with_Static_Fork_n_Join
,	::Parallel_Sum_with_Dynamic_Fork_n_Join
,	::Pipeline_Test
};