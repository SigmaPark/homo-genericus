/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Concurrency/Concurrency.hpp"
#include "Test_Concurrency.hpp"
#include "Poiya_Problem.hpp"

#include <chrono>
#include <type_traits>


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

	SGM_SPEC_ASSERT(answer == 5050);
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

	SGM_SPEC_ASSERT(answer == 5050);	
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

	static auto Time_per_Unit_Task()-> double;


	static auto constexpr unit_kvalue_v 
#ifdef NDEBUG
	=	std::size_t(2'000);
#else
	=	std::size_t(100);
#endif

	static std::size_t constexpr nof_Functors_in_Short_Cycle_v = 3;
	static int constexpr Stride_for_Full_Cycle_v = 3;

}


template<int N>
struct pipeline_test::Type
{
	int tag;


	Type(int tag = 0) : tag(tag){}

	template<int _N>
	Type(Type<_N> const& t) : tag(t.tag){}

	template<int _N>
	Type(Type<_N>&& t) : tag( std::move(t.tag) ){}
};


template<class INPUT, class OUTPUT>
struct pipeline_test::Functor
{
	std::size_t amount_of_task = 0;


	auto operator()(INPUT t) const-> OUTPUT
	{
		for(auto d = amount_of_task;  d-->0;)
			sgm::spec::Poiya_Problem(unit_kvalue_v);

		return t;
	}
};

template<>
struct pipeline_test::Functor
<	pipeline_test::Type<pipeline_test::nof_Functors_in_Short_Cycle_v-1>
,	pipeline_test::Type<pipeline_test::nof_Functors_in_Short_Cycle_v>
>
{
private:
	using _input_t = Type<nof_Functors_in_Short_Cycle_v - 1>;
	using _output_t = sgm::Pipeline_Data< Type<nof_Functors_in_Short_Cycle_v> >;


public:
	std::size_t amount_of_task = 0;


	auto operator()(_input_t t) const-> _output_t
	{
		for(auto d = amount_of_task;  d-->0;)
			sgm::spec::Poiya_Problem(unit_kvalue_v);

		if(t.tag % Stride_for_Full_Cycle_v == 0)
			return t;
		else
			return sgm::Pipeline_retry_cue_v;
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

		_result_count = 0;
	}

protected:
	Functor< Type<0>, Type<1> > _f01;
	Functor< Type<1>, Type<2> > _f12;
	Functor< Type<2>, Type<3> > _f23;
	Functor< Type<3>, Type<4> > _f34;
	Functor< Type<4>, Type<5> > _f45;


	auto _check_result(Type<5> const t) noexcept-> void
	{
		if(t.tag != Stride_for_Full_Cycle_v*_result_count)
			std::wcout
			<<	"Pipeline Test Failed : Test_Process::_check_result gets wrong tag number. \n"
			<<	"t.tag == " << t.tag << " , _result_count = " << _result_count << '\n';

		++_result_count;
	}


private:
	int _result_count;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class pipeline_test::Serial_Process : public Test_Process 
{
public:
	template<class Q>
	Serial_Process(std::deque<Q> const& Li) : Test_Process(Li){}


	auto test(std::size_t const nof_cycle)-> std::chrono::milliseconds
	{
		using namespace std::chrono;
		using std::move;

		auto const starting_time_point = system_clock::now();
	
		for(int i = 0;  i < nof_cycle;  ++i)
		{
			auto r1 = _f01( Type<0>(i) );

			if( _is_valid(r1) )
			{
				auto r2 = _f12( move(r1) );

				if( _is_valid(r2) )
				{
					auto r3 = _f23( move(r2) );

					if( _is_valid(r3) )
					{
						auto r4 = _f34( move(r3) );

						if( _is_valid(r4) )	
							this->_check_result(  _f45( move(r4) )  );
					}
				}
			}
		}

		auto const total_computing_time 
		=	duration_cast<milliseconds>(system_clock::now() - starting_time_point);

		return total_computing_time;
	}


private:
	template<class Q>
	static auto _is_valid(sgm::Pipeline_Data<Q> const& pd) noexcept
	->	bool{  return pd.is_valid();  }

	template<class Q>
	static auto _is_valid(Q const&) noexcept
	-> std::enable_if_t< !sgm::is_Pipeline_Data<Q>::value, bool >{  return true;  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class pipeline_test::Async_Process : public Test_Process
{
public:
	template<class Q>
	Async_Process(std::deque<Q> const& Li) : Test_Process(Li){}


	auto test(std::size_t const nof_cycle)-> std::chrono::milliseconds
	{
		using namespace std::chrono;

		int cur_cycle = 0;

		auto supplier_f
		=	[&cur_cycle, &nof_cycle]()-> sgm::Pipeline_Data< Type<0> >
			{
				if(cur_cycle != nof_cycle)
					return Type<0>(cur_cycle++);
				else
					return sgm::Pipeline_stop_cue_v;
			};

		auto consumer_f = [this](Type<5> const& t){  this->_check_result(t);  };

		auto const starting_time_point = system_clock::now();

		sgm::Concurrent_Pipeline::run(supplier_f, _f01, _f12, _f23, _f34, _f45, consumer_f);

		auto const total_computing_time 
		=	duration_cast<milliseconds>(system_clock::now() - starting_time_point);

		return total_computing_time;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#

#if 0
template<class PREC>
static auto Unit_task_time_check(std::size_t const reps)-> double
{
	using namespace std::chrono;

	auto const starting_time_point = system_clock::now();

	for(auto d = reps;  d-->0;)
		sgm::spec::Poiya_Problem(pipeline_test::unit_kvalue_v);

	auto const total_computing_time 
	=	duration_cast<PREC>(system_clock::now() - starting_time_point);

	return static_cast<double>(total_computing_time.count())/reps;
}


auto pipeline_test::Time_per_Unit_Task()-> double
{
	using namespace std::chrono;

	std::wcout << "\tMeasuring unit task time. Wait for 10 seconds...\n";

	double constexpr measuring_time_by_millisec_v = 10 * 1e3;

	auto const coarse_result_by_millisec = ::Unit_task_time_check<nanoseconds>(10) * 1e-6;

	std::size_t const fine_test_reps 
	=	static_cast<std::size_t>(measuring_time_by_millisec_v/ coarse_result_by_millisec);
	
	auto const res = ::Unit_task_time_check<milliseconds>(fine_test_reps);

	std::wcout << "\tTime_per_Unit_Task = " << res << '\n';

	return res;
}
#endif

static void Pipeline_Test()
{
#if 0
	using namespace std::chrono;
	using std::wcout;

	wcout << "\nPipeline Test Begins\n";
	
	wcout << "\tnof hardware cores = " << std::thread::hardware_concurrency() << '\n';

	auto const unit_task_time_by_millisec = pipeline_test::Time_per_Unit_Task();


	std::deque<std::size_t> const amount_of_tasks{14, 20, 23, 26, 17};
	std::size_t const nof_cycle = 100;

	auto accumulated_task_f
	=	[&amount_of_tasks](std::size_t const t)-> std::size_t
		{
			std::size_t res = 0;

			for(std::size_t i = 0;  i < t;  ++i)
				res += amount_of_tasks[i];

			return res;
		};

	auto const nof_full_task = accumulated_task_f(amount_of_tasks.size());

	{
		std::size_t const
			nof_full_cycle = (nof_cycle-1)/pipeline_test::Stride_for_Full_Cycle_v + 1,
			nof_short_cycle = nof_cycle - nof_full_cycle;

		auto const nof_short_task 
		=	accumulated_task_f(pipeline_test::nof_Functors_in_Short_Cycle_v);

		wcout 
		<<	"\tExpected time for serial test = "
		<<	(	nof_full_cycle*nof_full_task
			+	nof_short_cycle*nof_short_task
			) * unit_task_time_by_millisec
		<<	" millisec\n";
	}
	{
		auto const max_task
		=	[&amount_of_tasks]()-> std::size_t
			{
				std::size_t res = 0;

				for(auto const t : amount_of_tasks)
					if(res < t)
						res = t;

				return res;
			}();

		wcout
		<<	"\tideal time for concurrent test = "
		<<	( nof_full_task + max_task*(nof_cycle-1) ) * unit_task_time_by_millisec
		<<	" millisec\n";
	}

	wcout << '\n';

	{
		pipeline_test::Serial_Process serial_proc(amount_of_tasks);

		auto const serial_test_time = serial_proc.test(nof_cycle);

		wcout << "\tserial_test_time = " << serial_test_time.count() << '\n';
	}
	{
		pipeline_test::Async_Process async_proc(amount_of_tasks);

		auto const async_test_time = async_proc.test(nof_cycle);

		wcout << "\tasync_test_time = " << async_test_time.count() << '\n';
	}

	wcout << "Pipeline Test Ends\n";
#endif
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Concurrency, /**/)
{	::Parallel_Sum_with_Static_Fork_n_Join
,	::Parallel_Sum_with_Dynamic_Fork_n_Join
,	::Pipeline_Test
};