/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_CONCURRENCY_
#define _SGM_CONCURRENCY_


#include <future>
#include <limits>
#include <condition_variable>
#include <mutex>
#include <cassert>
#include "../Family/Family.hpp"
#include "../Abbreviable/Nullable.hpp"
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

	class Task_Guard;

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
		Task_Guard const tg(func, IDX-1);

		_Static_Nof_Loop<IDX-1>::calc(func);
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
	static auto _Dynamic_Nof_Loop_calc(FUNC&& func, unsigned task_id)-> void
	{
		Task_Guard const tg(func, --task_id);

		if(task_id > 0)
			_Dynamic_Nof_Loop_calc(func, task_id);
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


namespace sgm
{

	struct Concurrent_Pipeline;

	template<class T>
	class Pipeline_Data;


	SGM_USER_DEFINED_TYPE_CHECK
	(	class T
	,	Pipeline_Data, <T>
	);


	enum class Pipeline_Data_State{UNDEF, VALID, STOP, RETRY};

	static auto constexpr 
		Pipeline_stop_cue_v = Pipeline_Data_State::STOP,
		Pipeline_retry_cue_v = Pipeline_Data_State::RETRY;

}


namespace sgm
{

	template<class T, class FN>
	class Pipeline_Member;


	template<class T>
	class _Pipeline_Buffer;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
class sgm::Pipeline_Data
{
private:
	using _state_t = Pipeline_Data_State;


public:
	using value_type = T;


	template
	<	class...ARGS
	,	class = Enable_if_t<sizeof...(ARGS) != 1>
	>
	Pipeline_Data(ARGS&&...args);

	template
	<	class Q
	,	class 
		=	Enable_if_t
			<	!Has_Same_Origin<Q, Null_t>::value
			&&	!Has_Same_Origin<Q, _state_t>::value
			&&	!Has_Same_Origin<Q, Pipeline_Data>::value
			>
	>
	Pipeline_Data(Q&& q);

	Pipeline_Data(_state_t const s);

	Pipeline_Data(Pipeline_Data const&) = default;
	Pipeline_Data(Pipeline_Data&&) noexcept = default;


	template
	<	class Q
	,	class 
		=	Enable_if_t
			<	!Has_Same_Origin<Q, Null_t>::value
			&&	!Has_Same_Origin<Q, _state_t>::value
			&&	!Has_Same_Origin<Q, Pipeline_Data>::value
			>
	>
	auto operator=(Q&& q)-> Pipeline_Data&;

	auto operator=(_state_t const s)-> Pipeline_Data&;

	auto operator=(Pipeline_Data const&)-> Pipeline_Data& = default;
	auto operator=(Pipeline_Data&&) noexcept-> Pipeline_Data& = default;

	operator T&() noexcept{  return _nb;  }
	operator T const&() const noexcept{  return _nb;  }


	auto is_valid() const noexcept-> bool{  return _nb.has_value();  }
	auto state() const noexcept-> _state_t{  return _state;  }


private:
	Nullable<T> _nb;
	_state_t _state;
};


template<class T>  template<class...ARGS, class>
sgm::Pipeline_Data<T>::Pipeline_Data(ARGS&&...args) 
:	_nb( Forward<ARGS>(args)... ), _state(_state_t::VALID){}

template<class T>  template<class Q, class>
sgm::Pipeline_Data<T>::Pipeline_Data(Q&& q) : _nb( Forward<Q>(q) ), _state(_state_t::VALID){}

template<class T>
sgm::Pipeline_Data<T>::Pipeline_Data(_state_t const s) 
:	_nb(Null_t{}), _state(s){  assert(s != _state_t::VALID);  }


template<class T>  template<class Q, class>
auto sgm::Pipeline_Data<T>::operator=(Q&& q)-> Pipeline_Data&
{
	_nb = Forward<Q>(q);
	_state = _state_t::VALID;

	return *this;
}

template<class T>
auto sgm::Pipeline_Data<T>::operator=(_state_t const s)-> Pipeline_Data&
{
	assert(s != _state_t::VALID);

	_nb = Null_t{};
	_state = s;

	return *this;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{
	namespace _pipeline_guard_detail
	{

		class _Notice_Guard;

	}
}


class sgm::_pipeline_guard_detail::_Notice_Guard
{
public:
	_Notice_Guard(std::condition_variable& cv) : _cv(cv){}
	~_Notice_Guard(){  _cv.notify_one();  }


private:
	std::condition_variable& _cv;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{
	namespace _concurrent_pipeline_detail
	{

		template< class T, bool = is_Pipeline_Data< Decay_t<T> >::value >
		struct _PL_Data;

		template<class T>
		struct _PL_Data<T, false> : As_type_itself<T>{};

		template<class T>
		struct _PL_Data<T, true> : As_type_itself< typename Decay_t<T>::value_type >{};


		template<int>
		struct _Pipe;

		template<int>
		struct _Loop;

	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


//#define _USE_SGM_CIRCULAR_QUEUE
#ifdef _USE_SGM_CIRCULAR_QUEUE
#include "../Queue/Queue.hpp"


template<class T>
class sgm::_Pipeline_Buffer
{
public:
	_Pipeline_Buffer();

	auto data() const noexcept-> T const&{  return _cirq.front();  }
	auto is_full() const noexcept-> bool{  return _cirq.is_full();  }
	auto is_empty() const noexcept-> bool{  return _cirq.is_empty();  }

	template
	<	class Q
	,	class = Enable_if_t< !Has_Same_Origin<Q, Pipeline_Data_State>::value >  
	>
	auto take(Q&& q) noexcept-> void;

	template<class...>
	auto take(Pipeline_Data_State const s) noexcept-> void;

	auto give() noexcept-> T&;


private:
	sgm::Circular_Queue<T> _cirq;
	std::mutex _mx;
	std::condition_variable _cv;


	auto _wait_if_full() noexcept
	->	sgm::Duo< std::unique_lock<std::mutex>, _pipeline_guard_detail::_Notice_Guard >;

	auto _wait_if_empty() noexcept
	->	sgm::Duo< std::unique_lock<std::mutex>, _pipeline_guard_detail::_Notice_Guard >;
};


template<class T>
sgm::_Pipeline_Buffer<T>::_Pipeline_Buffer() : _cirq(1 + 1){}


template<class T>  template<class Q, class>
auto sgm::_Pipeline_Buffer<T>::take(Q&& q) noexcept-> void
{
	auto const guards = _wait_if_full();

	_cirq.push( Forward<Q>(q) );
}

template<class T>  template<class...>
auto sgm::_Pipeline_Buffer<T>::take(Pipeline_Data_State const s) noexcept-> void
{
	using PL_data_t = Pipeline_Data< typename _concurrent_pipeline_detail::_PL_Data<T>::type >;
	
	if(s != Pipeline_retry_cue_v)
		take( PL_data_t(s) );
}


template<class T>
auto sgm::_Pipeline_Buffer<T>::give() noexcept-> T&
{
	auto const guards = _wait_if_empty();

	while(_cirq.size() > 1)
		_cirq.pop();

	return _cirq.front();
}


template<class T>
auto sgm::_Pipeline_Buffer<T>::_wait_if_full() noexcept
->	sgm::Duo< std::unique_lock<std::mutex>, _pipeline_guard_detail::_Notice_Guard >
{
	std::unique_lock<std::mutex> qL(_mx);

	while(is_full())
		_cv.wait(qL);

	return {Move(qL), _cv};
}

template<class T>
auto sgm::_Pipeline_Buffer<T>::_wait_if_empty() noexcept
->	sgm::Duo< std::unique_lock<std::mutex>, _pipeline_guard_detail::_Notice_Guard >
{
	std::unique_lock<std::mutex> qL(_mx);

	while(is_empty())
		_cv.wait(qL);

	return {Move(qL), _cv};
}
#else


template<class T>
class sgm::_Pipeline_Buffer
{
public:
	template<class Q>
	_Pipeline_Buffer(Q const& q);


	auto data() const noexcept-> T const&{  return _p0->second();  }
	auto is_filled() const noexcept-> bool{  return _p0->first();  }


	template
	<	class Q
	,	class = Enable_if_t< !Has_Same_Origin<Q, Pipeline_Data_State>::value >  
	>
	auto take(Q&& q) noexcept-> void;

	template<class...>
	auto take(Pipeline_Data_State const s) noexcept-> void;

	auto give() noexcept-> T&;


private:
	sgm::Duo<bool, T> _buf[2], *_p0, *_p1;
	std::mutex _mx;
	std::condition_variable _cv;


	auto _wait_until_buffer_is(bool const filled) noexcept
	->	sgm::Duo< std::unique_lock<std::mutex>, _pipeline_guard_detail::_Notice_Guard >;
};


template<class T>  template<class Q>
sgm::_Pipeline_Buffer<T>::_Pipeline_Buffer(Q const& q)
:	_buf{ {false, T(q)}, {false, T(q)} }, _p0(&_buf[0]), _p1(&_buf[1]){}


template<class T>  template<class Q, class>
auto sgm::_Pipeline_Buffer<T>::take(Q&& q) noexcept-> void
{
	auto const guards = _wait_until_buffer_is(false);

	*_p0 = {true, Forward<Q>(q)};
}

template<class T>  template<class...>
auto sgm::_Pipeline_Buffer<T>::take(Pipeline_Data_State const s) noexcept-> void
{
	using PL_data_t = Pipeline_Data< typename _concurrent_pipeline_detail::_PL_Data<T>::type >;
	
	if(s != Pipeline_retry_cue_v)
		take( PL_data_t(s) );
}


template<class T>
auto sgm::_Pipeline_Buffer<T>::give() noexcept-> T&
{
	auto const guards = _wait_until_buffer_is(true);

	Swap(_p0, _p1);

	_p0->first() = false;

	return _p1->second();
}


template<class T>
auto sgm::_Pipeline_Buffer<T>::_wait_until_buffer_is(bool const filled) noexcept
->	sgm::Duo< std::unique_lock<std::mutex>, _pipeline_guard_detail::_Notice_Guard >
{
	std::unique_lock<std::mutex> qL(_mx);

	while(is_filled() != filled)
		_cv.wait(qL);

	return {Move(qL), _cv};
}
#endif
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, class FN>
class sgm::Pipeline_Member
{
public:
	using value_type = T;


	_Pipeline_Buffer< Pipeline_Data<T> > buffer;

#ifdef _USE_SGM_CIRCULAR_QUEUE
	Pipeline_Member(FN& fn) : buffer(), _fn(fn){}
#else
	Pipeline_Member(FN& fn) : buffer(Pipeline_Data_State::UNDEF), _fn(fn){}
#endif

	template<class...ARGS>
	auto operator()(ARGS&...args) noexcept
	->	invocation_Result_t<FN, ARGS&...>{  return _fn(args...);  }
	

private:
	FN& _fn;
};


template<class FN>
class sgm::Pipeline_Member<sgm::None, FN>
{
public:
	using value_type = None;


	Pipeline_Member(FN& fn) : _fn(fn){}

	template<class...ARGS>
	auto operator()(ARGS&...args) noexcept-> void{  _fn(args...);  }


private:
	FN& _fn;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<>
struct sgm::_concurrent_pipeline_detail::_Loop<3> : Unconstructible
{
	template<class FAM, class...TGS>
	static auto calc(FAM&, TGS const&...) noexcept-> void{}
};

template<>
struct sgm::_concurrent_pipeline_detail::_Loop<2> : Unconstructible
{
public:
	template
	<	class FAM, class...TGS, size_t IDX = sizeof...(TGS) 
	,	int _MODE = IDX == std::tuple_size< Decay_t<FAM> >::value - 1 ? 3 : 2
	>
	static auto calc(FAM& memfam, TGS const&...tgs) noexcept-> void
	{
		_Loop<_MODE>::calc
		(	memfam, tgs...
		,	_loop(memfam.template get<IDX-1>(), memfam.template get<IDX>())
		);
	}


private:
	template<class T1, class FN1, class FN2>
	static auto _loop(Pipeline_Member<T1, FN1>& mem1, Pipeline_Member<None, FN2>& mem2) noexcept
	->	Task_Guard
	{
		return
		[&mem1, &mem2]() noexcept
		{
			while(true)
			{
				auto& res = mem1.buffer.give();

				if(res.is_valid())
					mem2(res);
				else
					break;
			}
		};
	}

	template
	<	class T1, class FN1, class T2, class FN2, class = Enable_if_t< !is_None<T2>::value >
	>
	static auto _loop(Pipeline_Member<T1, FN1>& mem1, Pipeline_Member<T2, FN2>& mem2) noexcept
	->	Task_Guard
	{
		return 
		[&mem1, &mem2]() noexcept
		{
			while(true)
			{
				auto& res = mem1.buffer.give();

				if(res.is_valid())
					mem2.buffer.take( mem2(res) );
				else
					break;
			}

			mem2.buffer.take(Pipeline_stop_cue_v);
		};
	}
};

template<>
struct sgm::_concurrent_pipeline_detail::_Loop<1> : Unconstructible
{
public:
	template<class FAM>
	static auto calc(FAM&& memfam) noexcept-> void
	{
		_Loop<2>::calc( memfam, _loop(memfam.template get<0>()) );
	}


private:
	template<class T, class FN>
	static auto _loop(Pipeline_Member<T, FN>& mem) noexcept-> Task_Guard
	{
		return
		[&mem]() noexcept
		{
			while(mem.buffer.data().state() != Pipeline_Data_State::STOP)
				mem.buffer.take(mem());
		};
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<>
struct sgm::_concurrent_pipeline_detail::_Pipe<4> : Unconstructible
{
	template<class...MEMS>
	static auto calc(MEMS&&...mems) noexcept-> void
	{
		_Loop<1>::calc( Forward_as_Family(mems...) );
	}
};

template<>
struct sgm::_concurrent_pipeline_detail::_Pipe<2> : Unconstructible
{
	template
	<	class FAM, class...MEMS, size_t IDX = sizeof...(MEMS)
	,	class _FN = std::tuple_element_t< IDX, Decay_t<FAM> >
	>
	static auto calc(FAM& fnfam, MEMS&&...mems) noexcept-> void
	{
		_Pipe<4>::calc( mems..., Pipeline_Member<None, _FN>(fnfam.template get<IDX>()) );
	}
};

template<>
struct sgm::_concurrent_pipeline_detail::_Pipe<3> : Unconstructible
{
	template
	<	class FAM, class...MEMS, size_t IDX = sizeof...(MEMS)
	,	class _FAM = Decay_t<FAM>, class _FN = std::tuple_element_t<IDX, _FAM> 
	,	int _MODE = IDX == std::tuple_size<_FAM>::value - 2 ? 2 : 3
	,	class _INPUT = typename Decay_t< Last_t<MEMS&&...> >::value_type
	,	class _OUTPUT = typename _PL_Data< invocation_Result_t<_FN, _INPUT> >::type
	>
	static auto calc(FAM& fnfam, MEMS&&...mems) noexcept-> void
	{
		_Pipe<_MODE>
		::	calc( fnfam, mems..., Pipeline_Member<_OUTPUT, _FN>(fnfam.template get<IDX>()) );
	}
};

template<>
struct sgm::_concurrent_pipeline_detail::_Pipe<1> : Unconstructible
{
	template
	<	class FAM
	,	class _FAM = Decay_t<FAM>
	,	class _FN = std::tuple_element_t<0, _FAM> 
	,	int _MODE = std::tuple_size<_FAM>::value == 2 ? 2 : 3
	,	class _OUTPUT = typename _PL_Data< invocation_Result_t<_FN> >::type
	>
	static auto calc(FAM&& fnfam) noexcept-> void
	{
		_Pipe<_MODE>::calc( fnfam, Pipeline_Member<_OUTPUT, _FN>(fnfam.template get<0>()) );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::Concurrent_Pipeline : Unconstructible
{
	template< class...FS, class = Guaranteed_t<( sizeof...(FS) >= 2 )> >
	static auto run(FS&&...fs) noexcept-> void 
	{
		_concurrent_pipeline_detail::_Pipe<1>::calc( Forward_as_Family(fs...) );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_CONCURRENCY_