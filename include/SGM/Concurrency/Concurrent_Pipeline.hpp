/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_CONCURRENT_PIPELINE_
#define _SGM_CONCURRENT_PIPELINE_


#include <condition_variable>
#include <mutex>
#include <cassert>
#include <exception>
#include "SGM/Utility/Family.hpp"
#include "SGM/Utility/Finally.hpp"
#include "SGM/Wrapper/Nullable.hpp"
#include "Fork_and_Join.hpp"


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


template<class T>
class sgm::_Pipeline_Buffer
{
public:
	template<class Q>
	_Pipeline_Buffer(Q const& q);


	auto data() const noexcept-> T const&{  return _p0->second();  }
	auto is_filled() const noexcept-> bool{  return _p0->first();  }


	template<class Q>
	auto take(Q&& q) noexcept-> Enable_if_t< is_Pipeline_Data<Q>::value >;

	template<class Q>
	auto take(Q&& q) noexcept-> Enable_if_t< !is_Pipeline_Data<Q>::value >;


	auto give() noexcept-> T&;


private:
	sgm::Family<bool, T> _buf[2], *_p0, *_p1;
	std::mutex _mx;
	std::condition_variable _cv;


	template<class Q>
	auto _take(Q&& q) noexcept-> void;


	auto _wait_until_buffer_is(bool const filled) noexcept-> std::unique_lock<std::mutex>;
};


template<class T>  template<class Q>
sgm::_Pipeline_Buffer<T>::_Pipeline_Buffer(Q const& q)
:	_buf{ {false, T(q)}, {false, T(q)} }, _p0(&_buf[0]), _p1(&_buf[1]){}


template<class T>  template<class Q>
auto sgm::_Pipeline_Buffer<T>::_take(Q&& q) noexcept-> void
{
	auto const eol_notice = Finally( Memfunc(_cv, &std::condition_variable::notify_one) );
	auto const eol_unlock = _wait_until_buffer_is(false);

	*_p0 = {true, Forward<Q>(q)};
}

template<class T>  template<class Q>
auto sgm::_Pipeline_Buffer<T>::take(Q&& q) noexcept-> Enable_if_t< is_Pipeline_Data<Q>::value >
{
	if(q.state() != Pipeline_Data_State::RETRY)
		_take( Forward<Q>(q) );
}

template<class T>  template<class Q>
auto sgm::_Pipeline_Buffer<T>::take(Q&& q) noexcept
->	Enable_if_t< !is_Pipeline_Data<Q>::value >{  _take( Forward<Q>(q) );  }


template<class T>
auto sgm::_Pipeline_Buffer<T>::give() noexcept-> T&
{
	auto const eol_notice = Finally( Memfunc(_cv, &std::condition_variable::notify_one) );
	auto const eol_unlock = _wait_until_buffer_is(true);

	Swap(_p0, _p1);

	_p0->first() = false;

	return _p1->second();
}


template<class T>
auto sgm::_Pipeline_Buffer<T>::_wait_until_buffer_is(bool const filled) noexcept
->	std::unique_lock<std::mutex>
{
	std::unique_lock<std::mutex> qL(_mx);

	while(is_filled() != filled)
		_cv.wait(qL);

	return qL;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, class FN>
class sgm::Pipeline_Member
{
public:
	using value_type = T;


	_Pipeline_Buffer< Pipeline_Data<T> > buffer;


	Pipeline_Member(FN& fn) : buffer(Pipeline_Data_State::UNDEF), _fn(fn){}


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
	template<class FAM, class...THS>
	static auto calc(FAM&, THS const&...) noexcept-> void{}
};

template<>
struct sgm::_concurrent_pipeline_detail::_Loop<2> : Unconstructible
{
public:
	template
	<	class FAM, class...THS, size_t IDX = sizeof...(THS)
	,	int _MODE = IDX == std::tuple_size< Decay_t<FAM> >::value - 1 ? 3 : 2
	>
	static auto calc(FAM& memfam, THS const&...ths) noexcept-> void
	{
		_Loop<_MODE>::calc
		(	memfam, ths...
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


#endif // end of #ifndef _SGM_CONCURRENT_PIPELINE_