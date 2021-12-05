#pragma once
#ifndef _SGM_STOPWATCH_
#define _SGM_STOPWATCH_

#include <chrono>


namespace sgm
{
	template<class clock_t = std::chrono::high_resolution_clock>  class StopWatch;
}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


template<class clock_t>
class sgm::StopWatch
{
private:
	using _time_point_t = std::chrono::time_point<clock_t>;

	_time_point_t _starting_tp;

public:
	StopWatch(){  start();  }

	auto start()-> _time_point_t{  return _starting_tp = clock_t::now();  }

	template<class unit_t = std::chrono::milliseconds>
	auto stop() const
	->	unsigned long long{  return std::chrono::duration_cast<unit_t>(clock_t::now() - _starting_tp).count();  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#endif// end of #ifndef _SGM_STOPWATCH_