#pragma once

#ifndef _SGM_CONCURRENCY_
#define _SGM_CONCURRENCY_

#include <future>
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	namespace par
	{


		struct _Parallel_Helper
		{
			template<class T> using type = T;

			struct Nof_HW_Core
			{  
				enum : unsigned{DYNAMIC = unsigned(-1)};  
			};


		protected:
			struct _Ranger
			{
				struct Range{  size_t begin, end;  };


				_Ranger(size_t const idx_begin, size_t const idx_end, unsigned const nof_task)
				:	_idx_begin(idx_begin), _idx_end(idx_end)
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
		
		
				Range operator()(unsigned const task_id) const
				{
					auto const 
						d = static_cast<size_t>(task_id),
						begin0 = d * _loop_q + (d < _loop_r ? d : _loop_r),
						end0 = begin0 + _loop_q + (d < _loop_r ? 1 : 0);
		
					return {begin0 + _idx_begin, end0 + _idx_begin};
				}
		
		
			private:
				size_t const _idx_begin, _idx_end, _nof_task, _total_size, _loop_q, _loop_r;
			};


			_Parallel_Helper() = default;
		};
		//========//========//========//========//=======#//========//========//========//========


		template<unsigned NOF_TASK = _Parallel_Helper::Nof_HW_Core::DYNAMIC>
		struct Parallel : _Parallel_Helper
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

						func(range.begin, range.end, d);
					}
				);				
			}

			template<class F>
			void operator()(size_t const nof_iteration, F&& func) const
			{
				(*this)( 0, nof_iteration, std::forward<F>(func) );
			}
		};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template<>
		struct Parallel<_Parallel_Helper::Nof_HW_Core::DYNAMIC> : _Parallel_Helper
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
			enum : unsigned{NUMBER_OF_TASK = _Parallel_Helper::Nof_HW_Core::DYNAMIC};


			Parallel(unsigned const nof_task) : _nof_task(nof_task){}


			Parallel(bool const throw_when_core_detection_fails = false)
			:	_nof_task
				(	[throw_when_core_detection_fails](unsigned const nof_core)
					->	decltype(_nof_task)
					{
						return 
						nof_core != 0
						?	nof_core
						:	
						throw_when_core_detection_fails
						?	_Parallel_Helper::Nof_HW_Core::DYNAMIC
						:	1;
					}(std::thread::hardware_concurrency())
				)
			{
				if(_nof_task == _Parallel_Helper::Nof_HW_Core::DYNAMIC)
					throw false;
			}


			auto operator=(Parallel const&)-> Parallel& = delete;


			auto number_of_task() const-> unsigned{  return _nof_task;  }


			template<class F>
			void operator()(size_t const idx_begin, size_t const idx_end, F&& func) const
			{
				Dynamic_Par_For
				(	[&func, idx_begin, idx_end, this](unsigned const d)
					{
						auto const range = _Ranger(idx_begin, idx_end, _nof_task)(d);

						func(range.begin, range.end, d);
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
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template<>
		struct Parallel<1> : _Parallel_Helper
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
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


	} // end of namespace par
} // end of namespace sgm


#endif // end of #ifndef _SGM_CONCURRENCY_