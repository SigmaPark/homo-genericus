#pragma once

#ifndef _SGM_HIGH_TEMPLAR11_
#define _SGM_HIGH_TEMPLAR11_

#include "..\Serial\Serial.hpp"
#include "..\Concurrency\Concurrency.hpp"
#include "..\Type_Decorator\Type_Decorator.hpp"
#include "..\Flags\Flags.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	namespace ht
	{

		enum : unsigned{AUTO_OR = par::Nof_HW_Core::DYNAMIC};

		template<class maybe_Par> struct is_Par;

		template<unsigned NOF_TASK1 = sgm::ht::AUTO_OR, unsigned NOF_TASK2 = NOF_TASK1>
		struct Par;


		template<class T, class maybe_FLAG_SET> struct _Decorated;

		template<class T, class FLAGSET>
		using _Decorated_t = typename _Decorated<T, FLAGSET>::type;


		enum class _impl_Mode{SEQUANCIAL, MULTI_THREAD};


		template
		<	class FLAG_SET
		,	_impl_Mode
			=	FLAG_SET::template has<par::_Parallel_Helper>::value
				?	_impl_Mode::MULTI_THREAD
				:	_impl_Mode::SEQUANCIAL
		>
		struct _Morph_impl;


		template
		<	class FLAG_SET
		,	_impl_Mode
			=	FLAG_SET::template has<par::_Parallel_Helper>::value
				?	_impl_Mode::MULTI_THREAD
				:	_impl_Mode::SEQUANCIAL
		>
		struct _Filter_impl;


		template
		<	bool HAS_INIT, bool FORWARD, class FLAG_SET
		,	_impl_Mode
			=	FLAG_SET::template has<par::_Parallel_Helper>::value
				?	_impl_Mode::MULTI_THREAD
				:	_impl_Mode::SEQUANCIAL
		> 
		struct _Fold_impl;


		struct _Fold_Helper;

	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class maybe_Par>
struct sgm::ht::is_Par : std::is_base_of<par::_Parallel_Helper, maybe_Par>{};


/**	
*	<N, whatever> : N tasks
*	AUTO_OR, AUTO_OR : Auto detection or sequancial if failed. Default setting
*	AUTO_OR, N : Auto detection or N tasks if failed.
*	AUTO_OR, 0 : Auto detection or throw exception if failed.
*/
template<unsigned NOF_TASK1, unsigned NOF_TASK2>
struct sgm::ht::Par : Flag< Par<NOF_TASK1, NOF_TASK2> >, par::Parallel<NOF_TASK1>{};


template<unsigned N>
struct sgm::ht::Par<sgm::ht::AUTO_OR, N> : Flag< Par<AUTO_OR, N> >, par::Parallel<>
{
	template<class F>
	void operator()(size_t const nof_iteration, F&& func) const
	{
		if(par::Parallel<>::number_of_task() != 1)
			par::Parallel<>::operator()( nof_iteration, std::forward<F>(func) );
		else
			par::Parallel<N>()( nof_iteration, std::forward<F>(func) );
	}
};


template<>
struct sgm::ht::Par<sgm::ht::AUTO_OR, 0> : Flag< Par<AUTO_OR, 0> >, par::Parallel<>
{
	Par() : par::Parallel<>(false){}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, class> struct sgm::ht::_Decorated{  using type = T;  };

template<class T, class...FLAGS>
struct sgm::ht::_Decorated< T, sgm::Flag<FLAGS...> >
{
	using type = typename Decorated<T>::template by<FLAGS...>::type;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FS>
struct sgm::ht::_Morph_impl<FS, sgm::ht::_impl_Mode::SEQUANCIAL>
{
	template
	<	class CON, class FUNC
	,	class y_t = decltype( Declval<FUNC>()(*Declval<CON>().begin()) )
	,	class elem_t = _Decorated_t<y_t, FS>
	>
	static auto calc(CON&& con, FUNC&& func)-> Serial<elem_t>
	{
		Serial<elem_t> res(con.size());

		for(auto const& x : con)
			res >> func(x);

		return res;
	}
};


template<class FS>
struct sgm::ht::_Morph_impl<FS, sgm::ht::_impl_Mode::MULTI_THREAD>
{
	template
	<	class CON, class FUNC
	,	class y_t = decltype( Declval<FUNC>()(*Declval<CON>().begin()) )
	,	class elem_t = _Decorated_t<y_t, FS>
	>
	static auto calc(CON&& con, FUNC&& func)-> Serial<elem_t>
	{
		if(con.size() == 0)
			return Serial<elem_t>{};
		else
		{
			Serial<elem_t> res( con.size(), func(*con.begin()) );

			Satisfying_flag<is_Par, FS>()
			(	res.size()
			,	[&con, &res, &func](size_t idx_begin, size_t const idx_end, unsigned const)
				{
					for
					(	auto itr = Next(con.begin(), idx_begin)
					;	idx_begin < idx_end
					;	res[idx_begin++] = func(*itr++)
					);
				}
			);

			return res;
		}
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FS>
struct sgm::ht::_Filter_impl<FS, sgm::ht::_impl_Mode::SEQUANCIAL>
{
	template
	<	class CON, class FUNC
	,	class x_t = std::remove_reference_t< decltype(*Declval<CON>().begin()) >
	,	class elem_t = _Decorated_t<x_t, FS>
	>
	static auto calc(CON&& con, FUNC&& func)-> Serial<elem_t>
	{
		Serial<elem_t> res(con.size());

		for(auto const& x : con)
			if( func(x) )
				res >> x;

		return res;
	}
};


template<class FS>
struct sgm::ht::_Filter_impl<FS, sgm::ht::_impl_Mode::MULTI_THREAD>
{
	template
	<	class CON, class FUNC
	,	class x_t = std::remove_reference_t< decltype(*Declval<CON>().begin()) >
	,	class elem_t = _Decorated_t<x_t, FS>
	>
	static auto calc(CON&& con, FUNC&& func)-> Serial<elem_t>
	{
		auto const truth_table
		=	_Morph_impl<  Flag< Satisfying_flag<is_Par, FS> >  >::calc
			(	con, std::forward<FUNC>(func) 
			);

		auto const nof_true
		=	[&truth_table](size_t res)-> size_t
			{
				for(auto b : truth_table)
					if(b) 
						++res;
						
				return res;
			}(0);

		Serial<elem_t> res(nof_true);

		for
		(	auto duo = Dual_iteration<size_t>(0, con.begin())
		;	duo._1 < truth_table.size()
		;	duo++
		)
			if(truth_table[duo._1])
				res >> *duo._2;

		return res;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::ht::_Fold_Helper : No_Making
{
protected:
	template<class ITR, class FUNC, class res_t>
	static auto Accumulate(ITR itr, ITR const ei, FUNC&& func, res_t res)-> res_t
	{
		while(itr != ei)
			res = func(res, *itr++);

		return res;
	}


	template<bool FORWARD> struct itrMethod;

	template<>
	struct itrMethod<true>
	{
		template<class CON> static auto begin(CON&& con) SGM_DECLTYPE_AUTO(  con.begin()  )
		template<class CON> static auto end(CON&& con) SGM_DECLTYPE_AUTO(  con.end()  )
	};

	template<>
	struct itrMethod<false>
	{
		template<class CON> static auto begin(CON&& con) SGM_DECLTYPE_AUTO(  con.rbegin()  )
		template<class CON> static auto end(CON&& con) SGM_DECLTYPE_AUTO(  con.rend()  )
	};
};


template<bool FWD, class FS>
struct sgm::ht::_Fold_impl<true, FWD, FS, sgm::ht::_impl_Mode::SEQUANCIAL> : _Fold_Helper
{
	template<class CON, class FUNC, class res_t>
	static auto calc(CON&& con, FUNC&& func, res_t&& res)-> res_t
	{
		return
		Accumulate
		(	itrMethod<FWD>::begin(con), itrMethod<FWD>::end(con)
		,	std::forward<FUNC>(func), std::forward<res_t>(res)
		);
	}
};


template<bool FWD, class FS>
struct sgm::ht::_Fold_impl<false, FWD, FS, sgm::ht::_impl_Mode::SEQUANCIAL> : _Fold_Helper
{
	template<class CON, class FUNC>
	static auto calc(CON&& con, FUNC&& func, std::nullptr_t)
	->	std::decay_t< decltype( *itrMethod<FWD>::begin(con) ) >
	{
		auto const bi = itrMethod<FWD>::begin(con), ei = itrMethod<FWD>::end(con);

		assert(bi != ei && L"the container has nothing to fold.\n");

		return Accumulate( Next(bi), ei, std::forward<FUNC>(func), *bi );
	}
};


template<bool FWD, class FS>
struct sgm::ht::_Fold_impl<false, FWD, FS, sgm::ht::_impl_Mode::MULTI_THREAD> : _Fold_Helper
{
private:
	template<class CON, class FUNC>
	static auto _Seq_Fold(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
	(
		_Fold_impl<false, FWD, FS, _impl_Mode::SEQUANCIAL>::calc
		(	std::forward<CON>(con), std::forward<FUNC>(func), nullptr
		)		
	)

public:
	template
	<	class CON, class FUNC
	,	class res_t = std::decay_t<  decltype(*Declval< std::decay_t<CON> >().begin())  >
	>
	static auto calc(CON&& con, FUNC&& func, std::nullptr_t)-> res_t
	{
		auto const tasker = Satisfying_flag<is_Par, FS>();

		if(con.size() <= tasker.number_of_task())
			return _Seq_Fold( std::forward<CON>(con), std::forward<FUNC>(func) );
		else
		{
			Serial<res_t> sum( static_cast<size_t>(tasker.number_of_task()), *con.begin() );

			tasker
			(	con.size()
			,	[&con, &func, &sum]
				(	size_t const idx_begin, size_t const idx_end, unsigned const task_id
				)
				{	
					auto const 
						bi = Next(con.begin(), idx_begin),
						bi_1 = Next(bi), 
						ei = Next(bi, idx_end - idx_begin);
					 
					sum[task_id] = Accumulate(bi_1, ei, func, *bi);
				}
			);

			return _Seq_Fold( sum, std::forward<FUNC>(func) );
		}
	}
};


template<bool FWD, class FS>
struct sgm::ht::_Fold_impl<true, FWD, FS, sgm::ht::_impl_Mode::MULTI_THREAD> : _Fold_Helper
{
private:
	template<bool> struct _Last_fold;

	template<>
	struct _Last_fold<true> : No_Making
	{
		template<class FUNC, class T1, class T2>
		static auto calc(FUNC&& func, T1&& t1, T2&& t2) SGM_DECLTYPE_AUTO
		(
			func( std::forward<T1>(t1), std::forward<T2>(t2) )
		)
	};

	template<>
	struct _Last_fold<false> : No_Making
	{
		template<class FUNC, class T1, class T2>
		static auto calc(FUNC&& func, T1&& t1, T2&& t2) SGM_DECLTYPE_AUTO
		(
			func( std::forward<T2>(t2), std::forward<T1>(t1) )
		)
	};


public:
	template<class CON, class FUNC, class res_t>
	static auto calc(CON&& con, FUNC&& func, res_t&& res)-> res_t
	{
		static_assert
		(	std::is_convertible< res_t, std::decay_t<decltype(*con.begin())> >::value
		,	"for parallelization, folding function should be asocciative."
		);

		return 
		_Last_fold<FWD>::calc
		(	func, std::forward<res_t>(res)
		,	_Fold_impl<false, FWD, FS, _impl_Mode::MULTI_THREAD>::calc
			(	std::forward<CON>(con), func, nullptr
			)
		);
	}
};
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	namespace ht
	{

		template
		<	class...FLAGS, class CON, class FUNC
		,	class = std::enable_if_t< is_iterable<CON>::value >
		>
		static auto Morph(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
		(
			_Morph_impl< Flag<FLAGS...> >::calc
			(	std::forward<CON>(con), std::forward<FUNC>(func)
			)
		)
		
		
		template
		<	class...FLAGS, class CON, class FUNC
		,	class = Guaranteed_t< is_iterable<CON>::value >
		>
		static auto Filter(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
		(
			_Filter_impl< Flag<FLAGS...> >::calc
			(	std::forward<CON>(con), std::forward<FUNC>(func)
			)
		)
		
		
		template
		<	class...FLAGS, class CON, class FUNC, class init_t
		,	class = std::enable_if_t< is_iterable<CON>::value >
		>
		static auto Fold(CON&& con, FUNC&& func, init_t&& init) SGM_DECLTYPE_AUTO
		(
			_Fold_impl< true, true, Flag<FLAGS...> >::calc
			(	std::forward<CON>(con), std::forward<FUNC>(func), std::forward<init_t>(init)
			)
		)
		
		template
		<	class...FLAGS, class CON, class FUNC
		,	class = std::enable_if_t< is_iterable<CON>::value >
		>
		static auto Fold(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
		(
			_Fold_impl< false, true, Flag<FLAGS...> >::calc
			(	std::forward<CON>(con), std::forward<FUNC>(func), nullptr
			)
		)
		
		
		template
		<	class...FLAGS, class CON, class FUNC, class init_t
		,	class = std::enable_if_t< is_iterable<CON>::value >
		>
		static auto rFold(CON&& con, FUNC&& func, init_t&& init) SGM_DECLTYPE_AUTO
		(
			_Fold_impl< true, false, Flag<FLAGS...> >::calc
			(	std::forward<CON>(con), std::forward<FUNC>(func), std::forward<init_t>(init)
			)
		)
		
		template
		<	class...FLAGS, class CON, class FUNC
		,	class = std::enable_if_t< is_iterable<CON>::value >
		>
		static auto rFold(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
		(
			_Fold_impl< false, false, Flag<FLAGS...> >::calc
			(	std::forward<CON>(con), std::forward<FUNC>(func), nullptr
			)
		)

	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


#endif// end of #ifndef _SGM_HIGH_TEMPLAR11_