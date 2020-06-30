#pragma once

#ifndef _SGM_HIGH_TEMPLAR11_
#define _SGM_HIGH_TEMPLAR11_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

#include "..\interface_Traits\interface_Traits.hpp"
#include "..\indexable\indexable.hpp"

#include <future>
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

/**	Flag System Design and Implement
*	using <thread> library and replace openMP
*/

namespace sgm
{


	struct No_Deco{  template<class T> using type = T;  };

	struct Parallel_Proc
	{
		template<class T> using type = T;

		struct Nof_HW_Core{  enum : unsigned{DYNAMIC = unsigned(-1)};  };
	};


	template<unsigned NOF_THREAD = Parallel_Proc::Nof_HW_Core::DYNAMIC>
	struct Parallel : Parallel_Proc
	{
		enum : unsigned{NUMBER_OF_THREAD = NOF_THREAD};

		auto nof_thread() const-> unsigned{  return NUMBER_OF_THREAD;  }

		
		//template<class S = size_t>
		//struct Process
		//{
		//	Process(S total_loop_size)
		//	:	_total_loop_size(total_loop_size)
		//	,	_partial_loop_size( total_loop_size / S(NOF_THREAD) )
		//	,	_r_loop_size( total_loop_size % S(NOF_THREAD) )
		//	{}


		//	template<class LOOP>
		//	void calc(LOOP&& loop, S idx_begin = 0, S d = NOF_THREAD)
		//	{
		//		if(d != 0)
		//		{
		//			S idx_end = idx_begin + _partial_loop_size + if_remained();

		//			_null_report[d - 1] = std::async(loop, idx_begin, idx_end),
		//			calc(loop, idx_end, d - 1);
		//		}
		//	}


		//	~Process()
		//	{
		//		for(auto& r : _null_report)
		//			r.get();
		//	}

		//private:
		//	S _total_loop_size, _partial_loop_size, _r_loop_size;
		//	indexable< std::future<std::nullptr_t>, NOF_THREAD > _null_report;

		//	auto if_remained()-> S
		//	{  
		//		if(_r_loop_size > 0)
		//		{
		//			--_r_loop_size;
		//		
		//			return 1;
		//		}
		//		else
		//			return 0;
		//	}
		//};


		//template<class S>
		//auto process(S s)-> Process<S>{  return Process<S>(s);  }

	};


	template<>
	struct Parallel<Parallel_Proc::Nof_HW_Core::DYNAMIC> : Parallel_Proc
	{
		/**	returns 0 when std::thread::hardware_concurrency() fails to detect
		*	how many cores the device has
		*/
		static auto nof_core()-> unsigned
		{
			static auto const nof_core = std::thread::hardware_concurrency();

			return nof_core;
		}


		Parallel(unsigned nof_core) : _nof_core(nof_core){}
		
		Parallel(bool throw_excpetion_if_failed_to_detect_nof_core = false)
		:	_nof_core
			(	nof_core() != 0
				?	nof_core()
				:	
				throw_excpetion_if_failed_to_detect_nof_core
				?	Parallel_Proc::Nof_HW_Core::DYNAMIC
				:	1
			)
		{}
		

		auto nof_thread() const-> unsigned
		{
			if(_nof_core == Parallel_Proc::Nof_HW_Core::DYNAMIC)
				throw false;
			else
				return _nof_core;  
		}


		//template<class S = size_t>
		//struct Process
		//{
		//	Process(S total_loop_size, S nof_thread)
		//	:	_total_loop_size(total_loop_size), _nof_thread(nof_thread)
		//	,	_partial_loop_size( total_loop_size / S(NOF_THREAD) )
		//	,	_r_loop_size( total_loop_size % S(NOF_THREAD) )
		//	,	_null_report(nof_thread, nullptr)
		//	{}


		//	template<class LOOP>
		//	void calc(LOOP&& loop, S idx_begin = 0, S d = _nof_thread)
		//	{
		//		if(d != 0)
		//		{
		//			S idx_end = idx_begin + _partial_loop_size + if_remained();

		//			_null_report[d - 1] = std::async(loop, idx_begin, idx_end),
		//			calc(loop, idx_end, d - 1);
		//		}
		//	}


		//	~Process()
		//	{
		//		for(auto& r : _null_report)
		//			r.get();
		//	}

		//private:
		//	S _total_loop_size, _nof_thread, _partial_loop_size, _r_loop_size;
		//	indexable< std::future<std::nullptr_t> > _null_report;

		//	auto if_remained()-> S
		//	{  
		//		if(_r_loop_size > 0)
		//		{
		//			--_r_loop_size;
		//		
		//			return 1;
		//		}
		//		else
		//			return 0;
		//	}
		//};

		//template<class S>
		//auto process(S s)-> Process<S>{  return Process<S>(s, nof_thread());  }


	private:
		unsigned _nof_core;
	};
	//========//========//========//========//=======#//========//========//========//========//===


	class HT_implementation : No_Making
	{
	
		struct _Branch : No_Making
		{
			enum class _Sequancial;
			enum class _Parallel;

			template<class FLAG>
			using impl_t
			=	std::conditional_t
				<	std::is_base_of< Parallel_Proc, std::decay_t<FLAG> >::value
				,	_Parallel, _Sequancial
				>;
		};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template< class FLAG, class = typename _Branch::impl_t<FLAG> > struct _Morph_Helper;


		template<class FLAG>
		struct _Morph_Helper<FLAG, _Branch::_Sequancial> : _Branch
		{
			template
			<	class CON, class FUNC
			,	class y_t = decltype( Declval<FUNC>()(*Declval<CON>().begin()) )
			,	class elem_t = typename FLAG::template type<y_t>
			>
			static auto Morph(FLAG, CON&& con, FUNC&& func)-> indexable<elem_t>
			{
				indexable<elem_t> res(con.size());

				for(auto const& x : con)
					res >> func(x);

				return res;
			}
		};


		template<class FLAG>
		struct _Morph_Helper<FLAG, _Branch::_Parallel> : _Branch
		{
			template
			<	class CON, class FUNC
			,	class y_t = decltype( Declval<FUNC>()(*Declval<CON>().begin()) )
			,	class elem_t = typename FLAG::template type<y_t>
			>
			static auto Morph(FLAG flag, CON&& con, FUNC&& func)-> indexable<elem_t>
			{
				static_assert
				(	sgm::is_random_access_iterator<decltype(con.begin())>::value
				,	"the iterable should have random-access iterator type for parallelization."
				);

				if(con.size() == 0)
					return indexable<elem_t>{};

				indexable<elem_t> res( con.size(), func(*con.begin()) );

				flag.process(con.size() - 1).calc
				(	[&func, &res, &con](ixSize_t idx, ixSize_t idx_end)-> std::nullptr_t
					{
						for(; idx < idx_end; ++idx)
							res[idx] = func(con.begin()[idx]);

						return nullptr;
					}
				,	1
				);

				return res;
			}
		};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------




		template< class FLAG, class = typename _Branch::impl_t<FLAG> > struct _Filter_Helper;


		template<class FLAG>
		struct _Filter_Helper<FLAG, _Branch::_Sequancial> : _Branch
		{
			template
			<	class CON, class FUNC
			,	class x_t = std::remove_reference_t< decltype(*Declval<CON>().begin()) >
			,	class elem_t = typename FLAG::template type<x_t>
			>
			static auto Filter(FLAG, CON&& con, FUNC&& func)-> indexable<elem_t>
			{
				indexable<elem_t> res(con.size());

				for(auto const& x : con)
					if( func(x) )
						res >> x;

				return res;
			}
		};


		template<class FLAG>
		struct _Filter_Helper<FLAG, _Branch::_Parallel> : _Branch
		{	
			template
			<	class CON, class FUNC
			,	class x_t = std::remove_reference_t< decltype(*Declval<CON>().begin()) >
			,	class elem_t = typename FLAG::template type<x_t>
			>
			static auto Filter(FLAG flag, CON&& con, FUNC&& func)-> indexable<elem_t>
			{
				indexable<bool> const truth_table
				=	_Morph_Helper<Parallel>::Morph( con, std::forward<FUNC>(func) );

				auto const nof_true
				=	[](indexable<bool> const& tt)-> ixSize_t
					{
						int res = 0, i = 0;

					//#pragma omp parallel for default(shared) private(i) reduction(+:res)
					//	for(i = 0; i < tt.size(); i++)
					//		if(tt[i])
					//			res = res + 1;

						return static_cast<ixSize_t>(res);
					}(truth_table);

				indexable<elem_t> res(nof_true);

				for(ixSize_t idx = 0; idx < con.size(); ++idx)
					if(truth_table[idx])
						res >> con.begin()[idx];

				return res;
			}
		};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		struct _Fold_Helper : No_Making
		{

			template<class FLAG> struct Accumulate;

			template<>
			struct Accumulate<_Branch::_Sequancial> : No_Making
			{
				template<class ITR, class FUNC, class res_t>
				static auto calc(ITR itr, ITR const ei, FUNC&& func, res_t&& res)-> res_t
				{
					while(itr != ei)
						res = func(res, *itr++);

					return res;
				}
			};


			template<>
			struct Accumulate<_Branch::_Parallel> : No_Making
			{
				template<class ITR, class FUNC, class res_t>
				static auto calc(ITR itr, ITR const ei, FUNC&& func, res_t&& res)-> res_t
				{
					//while(itr != ei)
					//	res = func(res, *itr++);

					//return res;
				}
			};


			template<bool FORWARD> struct itrMethod;

			template<>
			struct itrMethod<true>
			{
				template<class CON>
				static auto begin(CON&& con) SGM_DECLTYPE_AUTO(  con.begin()  )

				template<class CON>
				static auto end(CON&& con) SGM_DECLTYPE_AUTO(  con.end()  )
			};

			template<>
			struct itrMethod<false>
			{
				template<class CON>
				static auto begin(CON&& con) SGM_DECLTYPE_AUTO(  con.rbegin()  )

				template<class CON>
				static auto end(CON&& con) SGM_DECLTYPE_AUTO(  con.rend()  )
			};

		};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template<bool HAS_INIT, bool FORWARD, class FLAG> struct _Fold_impl;


		template<bool FWD, class FLAG>
		struct _Fold_impl<true, FWD, FLAG> : _Fold_Helper
		{
			template<class CON, class FUNC, class res_t>
			static auto fold(CON&& con, FUNC&& func, res_t&& res)-> res_t
			{
				return
				Accumulate< _Branch::impl_t<FLAG> >::calc
				(	itrMethod<FWD>::begin(con), itrMethod<FWD>::end(con)
				,	std::forward<FUNC>(func)
				,	std::forward<res_t>(res)
				);
			}
		};


		template<bool FWD, class FLAG>
		struct _Fold_impl<false, FWD, FLAG> : _Fold_Helper
		{
			template<class CON, class FUNC>
			static auto fold(CON&& con, FUNC&& func, std::nullptr_t)
			->	std::decay_t< decltype( *itrMethod<FWD>::begin(con) ) >
			{
				auto bi = itrMethod<FWD>::begin(con);
				auto ei = itrMethod<FWD>::end(con);

				assert(bi != ei && L"the container has nothing to fold.\n");

				return
				Accumulate< _Branch::impl_t<FLAG> >
				::	calc( ++bi, ei, std::forward<FUNC>(func), *bi );
			}
		};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


	};
	//========//========//========//========//=======#//========//========//========//========//===


//	
//#ifndef _SGM_MORPH_AND_FILTER_INTERFACE
//	#define _SGM_MORPH_AND_FILTER_INTERFACE(NAME)	\
//		template		\
//		<	class DECO = No_Deco, class CON, class FUNC		\
//		,	class = Guaranteed_t< is_iterable<CON>::value >	\
//		>	\
//		static auto NAME(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO		\
//		(	\
//			HT_implementation	\
//			::	template NAME<DECO>( std::forward<CON>(con), std::forward<FUNC>(func) )	\
//		)	\
//		\
//		\
//		template		\
//		<	class...ARGS, class DECO, class CON, class FUNC	\
//		,	class = Guaranteed_t< sizeof...(ARGS) <= 2 >	\
//		>	\
//		static auto NAME(DECO, CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO	\
//		(	\
//			NAME<DECO>	\
//			(	std::conditional_t		\
//				<	sizeof...(ARGS) >= 1, First_t<ARGS...>, decltype(con)	\
//				>	\
//				(con)	\
//			,	std::conditional_t		\
//				<	sizeof...(ARGS) >= 2, Nth_t<1, ARGS...>, decltype(func) \
//				>	\
//				(func)	\
//			)	\
//		)
//
//		_SGM_MORPH_AND_FILTER_INTERFACE(Morph)
//		_SGM_MORPH_AND_FILTER_INTERFACE(Filter)
//
//	#undef _SGM_MORPH_AND_FILTER_INTERFACE
//#else
//	#error _SGM_MORPH_AND_FILTER_INTERFACE was already defined somewhere else.
//#endif


}// end of namespace sgm


#endif// end of #ifndef _SGM_HIGH_TEMPLAR11_