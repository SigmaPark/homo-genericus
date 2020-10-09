#pragma once

#ifndef _SGM_CONCURRENCY_
#define _SGM_CONCURRENCY_

#include <future>
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	namespace par
	{

		struct _Parallel_Helper;

		struct Nof_HW_Core{  enum : unsigned{DYNAMIC = 0xffffffffUL};  };

		template<unsigned NOF_TASK = Nof_HW_Core::DYNAMIC>
		struct Parallel;

	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


struct sgm::par::_Parallel_Helper
{
	template<class T> using type = T;

protected:
	struct _Ranger;

	_Parallel_Helper() = default;
};


struct sgm::par::_Parallel_Helper::_Ranger
{
	_Ranger(size_t const idx_begin, size_t const idx_end, unsigned const nof_task)
	:	_idx_begin(idx_begin)
	,	_nof_task( static_cast<size_t>(nof_task) )
	,	_total_size
		(	[idx_begin, idx_end]()-> decltype(_total_size)
			{
				assert(idx_begin <= idx_end && L"invalid index range.\n");

				return idx_end - idx_begin;
			}()
		)
	,	_loop_q(_total_size / _nof_task)
	,	_loop_r(_total_size % _nof_task)
	{}


	auto operator=(_Ranger const&)-> _Ranger& = delete;

	
	auto operator()(unsigned const task_id) const-> std::initializer_list<size_t>
	{
		auto const 
			d = static_cast<size_t>(task_id),
			begin0 = d * _loop_q + (d < _loop_r ? d : _loop_r),
			end0 = begin0 + _loop_q + (d < _loop_r ? 1 : 0);

		return {begin0 + _idx_begin, end0 + _idx_begin};
	}


private:
	size_t const _idx_begin, _nof_task, _total_size, _loop_q, _loop_r;	
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned NOF_TASK>
struct sgm::par::Parallel : _Parallel_Helper
{
private:
	template<unsigned N>
	struct Static_Par
	{
		template<class F>
		static void For(F&& f)
		{
			auto fut = std::async(f, N - 1);
	
			Static_Par<N - 1>::For(f);
	
			fut.get();
		}
	};

	template<>
	struct Static_Par<0>
	{
		template<class F> static void For(F&&){}
	};


public:
	enum : unsigned{NUMBER_OF_TASK = NOF_TASK};


	auto number_of_task() const-> unsigned{  return NUMBER_OF_TASK;  }


	template<class F>
	void operator()(size_t const idx_begin, size_t const idx_end, F&& func) const
	{
		Static_Par<NOF_TASK>::For
		(	[&func, idx_begin, idx_end](unsigned const d)
			{
				auto const range = _Ranger(idx_begin, idx_end, NOF_TASK)(d);

				func(range.begin()[0], range.begin()[1], d);
			}
		);				
	}

	template<class F>
	void operator()(size_t const nof_iteration, F&& func) const
	{
		(*this)( 0, nof_iteration, std::forward<F>(func) );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<>
struct sgm::par::Parallel<sgm::par::Nof_HW_Core::DYNAMIC> : _Parallel_Helper
{
private:
	template<class F>
	static void Dynamic_Par_For(F&& f, unsigned d)
	{
		auto fut = std::async(f, --d);
	
		if(d > 0)
			Dynamic_Par_For(f, d);
	
		fut.get();
	}


	unsigned const _nof_task;


public:
	enum : unsigned{NUMBER_OF_TASK = Nof_HW_Core::DYNAMIC};


	struct Unknown_nof_HW_Core;


	Parallel(unsigned const nof_task) : _nof_task(nof_task){}
	Parallel(bool const throw_when_core_detection_fails = false);


	auto operator=(Parallel const&)-> Parallel& = delete;


	auto number_of_task() const-> unsigned{  return _nof_task;  }


	template<class F>
	void operator()(size_t const idx_begin, size_t const idx_end, F&& func) const
	{
		Dynamic_Par_For
		(	[&func, idx_begin, idx_end, this](unsigned const d)
			{
				auto const range = _Ranger(idx_begin, idx_end, _nof_task)(d);

				func(range.begin()[0], range.begin()[1], d);
			}
		,	_nof_task
		);				
	}

	template<class F>
	void operator()(size_t const nof_iteration, F&& func) const
	{
		(*this)( 0, nof_iteration, std::forward<F>(func) );
	}
};


struct sgm::par::Parallel<sgm::par::Nof_HW_Core::DYNAMIC>::Unknown_nof_HW_Core 
:	public std::exception
{
	auto what() const-> char const* override
	{
		return "Failed to find out how many hardware cores are equipped.\n";
	}


private:
	friend struct Parallel;

	Unknown_nof_HW_Core() = default;
};


sgm::par::Parallel<sgm::par::Nof_HW_Core::DYNAMIC>::Parallel(bool const throw_when_fails)
:	_nof_task
	(	[throw_when_fails](unsigned const nof_core)->	decltype(_nof_task)
		{
			return 
			nof_core != 0
			?	nof_core
			:	
			throw_when_fails
			?	Nof_HW_Core::DYNAMIC
			:	1;
		}(std::thread::hardware_concurrency())
	)
{
	if(_nof_task == Nof_HW_Core::DYNAMIC)
		throw Unknown_nof_HW_Core();
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<>
struct sgm::par::Parallel<1> : _Parallel_Helper
{
	enum : unsigned{NUMBER_OF_TASK = 1};


	auto number_of_task() const-> unsigned{  return NUMBER_OF_TASK;  }


	template<class F>
	void operator()(size_t const idx_begin, size_t const idx_end, F&& func) const
	{
		func(idx_begin, idx_end, NUMBER_OF_TASK - 1);
	}

	template<class F>
	void operator()(size_t const nof_iteration, F&& func) const
	{
		(*this)( 0, nof_iteration, std::forward<F>(func) );
	}			
};


#endif // end of #ifndef _SGM_CONCURRENCY_