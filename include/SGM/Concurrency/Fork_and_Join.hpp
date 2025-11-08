/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_FORK_AND_JOIN_
#define _SGM_FORK_AND_JOIN_


#include <future>
#include <limits>
#include <thread>
#include "SGM/Utility/Family.hpp"


namespace sgm
{

	struct Nof_Hardware_Core
	{
		static unsigned constexpr DYNAMIC = std::numeric_limits<unsigned>::max();

		enum class When_Fails : unsigned{SEQUANCIAL = 1, THROW = DYNAMIC};
	};


	template<unsigned NOF_TASK = Nof_Hardware_Core::DYNAMIC>
	struct Fork_and_Join;


	class FnJ_Fail_to_get_Nof_Core;

	class Task_Guard;
	class Thread_Guard;

	struct _Fork_and_Join_Helper;

}
//========//========//========//========//=======#//========//========//========//========//=======#


class sgm::Task_Guard
{
public:
	template<class FN, class...ARGS>
	Task_Guard(FN&& fn, ARGS&&...args)
	:	_fut(  std::async( Forward<FN>(fn), Forward<ARGS>(args)... )  ){}

	~Task_Guard(){  _fut.get();  }


private:
	std::future<void> _fut;
};


class sgm::Thread_Guard
{
public:
	template<class FN, class...ARGS>
	Thread_Guard(FN&& fn, ARGS&&...args) : _th( Forward<FN>(fn), Forward<ARGS>(args)... ){}

	~Thread_Guard(){  _th.join();  }


private:
	std::thread _th;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::_Fork_and_Join_Helper
{
protected:
	_Fork_and_Join_Helper() = default;


	template<unsigned IDX>
	struct _Static_Nof_Loop;


	struct _Range{  size_t begin_idx, end_idx;  };


	static auto Create_Partial_Range
	(	_Range const whole_range, size_t const nof_task, size_t const task_id
	)->	_Range
	{
		size_t const
			whole_len = whole_range.end_idx - whole_range.begin_idx,
			q = whole_len/nof_task, r = whole_len%nof_task;

		_Range const offset_range
		=	task_id < r
			?	_Range{task_id*(q+1), (task_id+1)*(q+1)}
			:	_Range{task_id*q + r, (task_id+1)*q + r};

		return
		{	whole_range.begin_idx + offset_range.begin_idx
		,	whole_range.begin_idx + offset_range.end_idx
		};
	}
};


template<unsigned IDX>
struct sgm::_Fork_and_Join_Helper::_Static_Nof_Loop : Unconstructible
{
	template<class FUNC>
	static auto calc(FUNC&& func)-> void
	{
		Task_Guard(func, IDX - 1),  _Static_Nof_Loop<IDX-1>::calc(func);
	}
};


template<>
struct sgm::_Fork_and_Join_Helper::_Static_Nof_Loop<0> : Unconstructible
{
	template<class FUNC>
	static auto calc(FUNC&&) noexcept-> void{}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned NOF_TASK>
struct sgm::Fork_and_Join : private _Fork_and_Join_Helper
{
	static unsigned constexpr NUMBER_OF_TASK = NOF_TASK;

	auto constexpr number_of_task() const-> unsigned{  return NUMBER_OF_TASK;  }


	template<class FUNC>
	auto operator()(size_t const begin_idx, size_t const end_idx, FUNC&& func) const-> void
	{
		_Fork_and_Join_Helper::_Static_Nof_Loop<NOF_TASK>::calc
		(	[&func, begin_idx, end_idx](size_t const task_id)
			{
				auto const rg
				=	_Fork_and_Join_Helper::Create_Partial_Range
					(	_Range{begin_idx, end_idx}, NOF_TASK, task_id
					);

				func( rg.begin_idx, rg.end_idx, static_cast<unsigned>(task_id) );
			}
		);
	}

	template<class FUNC>
	auto operator()(size_t const loops, FUNC&& func) const
	->	void{  (*this)( 0, loops, Forward<FUNC>(func) );  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<>
struct sgm::Fork_and_Join<1> : private _Fork_and_Join_Helper
{
	static unsigned constexpr NUMBER_OF_TASK = 1;

	auto constexpr number_of_task() const-> unsigned{  return 1;  }


	template<class FUNC>
	auto operator()(size_t const begin_idx, size_t const end_idx, FUNC&& func) const
	->	void{  func( begin_idx, end_idx, unsigned(0) );  }

	template<class FUNC>
	auto operator()(size_t const loops, FUNC&& func) const
	->	void{  (*this)( 0, loops, Forward<FUNC>(func) );  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::FnJ_Fail_to_get_Nof_Core : public std::exception
{
public:
	auto what() const noexcept
	->	char const* override{  return "Failed to get the number of hardware cores.";  }

private:
	friend struct sgm::Fork_and_Join<sgm::Nof_Hardware_Core::DYNAMIC>;

	FnJ_Fail_to_get_Nof_Core() = default;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<>
struct sgm::Fork_and_Join<sgm::Nof_Hardware_Core::DYNAMIC> : private _Fork_and_Join_Helper
{
private:
	template<class FUNC>
	static auto _Dynamic_Nof_Loop_calc(FUNC&& func, unsigned task_id)-> void
	{
		if(task_id-- == 0)
			return;

		Task_Guard(func, task_id),  _Dynamic_Nof_Loop_calc(func, task_id);
	}

public:
	static unsigned constexpr NUMBER_OF_TASK = Nof_Hardware_Core::DYNAMIC;


	Fork_and_Join(unsigned const nof_task) : _nof_task(nof_task){}

	Fork_and_Join
	(	Nof_Hardware_Core::When_Fails const wf_flag = Nof_Hardware_Core::When_Fails::SEQUANCIAL
	)
	:	_nof_task
		(	[wf_flag](unsigned const nof_hw_core)-> unsigned
			{
				return
				(	nof_hw_core != 0 ? nof_hw_core
				:	wf_flag == Nof_Hardware_Core::When_Fails::SEQUANCIAL ? unsigned(1)
				:	/* otherwise */ NUMBER_OF_TASK
				);
			}(std::thread::hardware_concurrency())
		)
	{
		if(_nof_task == NUMBER_OF_TASK)
			throw FnJ_Fail_to_get_Nof_Core{};
	}


	auto number_of_task() const-> unsigned{  return _nof_task;  }


	template<class FUNC>
	auto operator()(size_t const begin_idx, size_t const end_idx, FUNC&& func) const-> void
	{
		auto const ntask = _nof_task;

		_Dynamic_Nof_Loop_calc
		(	[&func, begin_idx, end_idx, ntask](size_t const task_id)
			{
				auto const rg
				=	_Fork_and_Join_Helper::Create_Partial_Range
					(	_Range{begin_idx, end_idx}, ntask, task_id
					);

				func( rg.begin_idx, rg.end_idx, static_cast<unsigned>(task_id) );
			}
		,	ntask
		);
	}

	template<class FUNC>
	auto operator()(size_t const loops, FUNC&& func) const
	->	void{  (*this)( 0, loops, Forward<FUNC>(func) );  }


private:
	unsigned const _nof_task;
};
//========//========//========//========//=======#//========//========//========//========//=======#


#endif // end of #ifndef _SGM_FORK_AND_JOIN_