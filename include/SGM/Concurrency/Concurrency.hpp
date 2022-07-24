/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_CONCURRENCY_
#define _SGM_CONCURRENCY_


#include <future>
#include <limits>
#include "../Type_Analysis/Type_Analysis.hpp"
#include "../Exception/Exception.hpp"


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


	struct _Fork_and_Join_Helper;

}
//========//========//========//========//=======#//========//========//========//========//=======#


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
	static void calc(FUNC&& func)
	{
		auto fut = std::async(func, IDX - 1);

		_Static_Nof_Loop<IDX - 1>::calc(func);

		fut.get();
	}
};


template<>
struct sgm::_Fork_and_Join_Helper::_Static_Nof_Loop<0> : Unconstructible
{
	template<class FUNC>
	static void calc(FUNC&&){}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned NOF_TASK>
struct sgm::Fork_and_Join : private _Fork_and_Join_Helper
{
	static unsigned constexpr NUMBER_OF_TASK = NOF_TASK;

	auto constexpr number_of_task() const-> unsigned{  return NUMBER_OF_TASK;  }


	template<class FUNC>
	void operator()(size_t const begin_idx, size_t const end_idx, FUNC&& func) const
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
	void operator()(size_t const loops, FUNC&& func) const
	{
		(*this)( 0, loops, Forward<FUNC>(func) );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<>
struct sgm::Fork_and_Join<1> : private _Fork_and_Join_Helper
{
	static unsigned constexpr NUMBER_OF_TASK = 1;

	auto constexpr number_of_task() const-> unsigned{  return 1;  }


	template<class FUNC>
	void operator()(size_t const begin_idx, size_t const end_idx, FUNC&& func) const
	{
		func( begin_idx, end_idx, unsigned(0) );
	}

	template<class FUNC>
	void operator()(size_t const loops, FUNC&& func) const
	{
		(*this)( 0, loops, Forward<FUNC>(func) );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::FnJ_Fail_to_get_Nof_Core : public Exception
{
public:
	auto what() const
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
	static void _Dynamic_Nof_Loop_calc(FUNC&& func, unsigned task_id)
	{
		auto fut = std::async(func, --task_id);

		if(task_id > 0)
			_Dynamic_Nof_Loop_calc(func, task_id);

		fut.get();
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
	void operator()(size_t const begin_idx, size_t const end_idx, FUNC&& func) const
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
	void operator()(size_t const loops, FUNC&& func) const
	{
		(*this)( 0, loops, Forward<FUNC>(func) );
	}


private:
	unsigned const _nof_task;
};


#endif // end of #ifndef _SGM_CONCURRENCY_