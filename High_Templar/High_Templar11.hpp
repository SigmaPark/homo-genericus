/**	Higher order functions in FP paradigm as template function format
*/
#pragma once

#ifndef _SGM_HIGH_TEMPLAR11_
#define _SGM_HIGH_TEMPLAR11_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

#include "..\Carrier\Carrier.hpp"
#include "..\Pinweight\Pinweight.hpp"
#include "..\Flags\Flags.hpp"
#include "..\Compound\Compound.hpp"

#ifdef _ITERATOR_
	#include <omp.h>
#endif

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	struct PWT : Flag<PWT>
	{
	public:
		//template<class FLAGSET, class T>
		//using if_flag_is_in_t
		//=	std::conditional_t< FLAGSET:: template has<PWT>::value, Pinweight<T>, T >;
	};


	struct PAR : Flag<PAR>{};
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T = size_t>
	static auto indices(size_t N, T offset = 0)-> Carrier<T>
	{
		Carrier<T> res(N);

		while(N-->0)
			res >> offset++;

		return res;
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template
	<	class CON, class FUNC, class FLAGS = Flag<>
	,	class y_t 
		=	/*PWT::if_flag_is_in_t
			<	FLAGS
			,*/	std::result_of_t
				<	FUNC( decltype(*Declval<CON>().begin()) )
				>
			/*>*/
	>
	static auto Morph(CON&& con, FUNC&& func, FLAGS = Flag<>())-> Carrier<y_t>
	{
		Carrier<y_t> res(con.size());

		for(auto const& x : con)
			res >> func(x);

		return res;
	}


	//template
	//<	class FLAGS = Flag<>
	//,	class CON1 = std::nullptr_t, class CON2 = std::nullptr_t, class FUNC = std::nullptr_t
	//,	class y_t
	//	=	PWT::if_flag_is_in_t
	//		<	FLAGS
	//		,	std::result_of_t
	//			<	FUNC
	//				(	decltype(*Declval<CON1>().begin())
	//				,	decltype(*Declval<CON2>().begin())
	//				)
	//			>
	//		>
	//>
	//static auto Morph(CON1&& con1, CON2&& con2, FUNC&& func)-> Carrier<y_t>
	//{
	//	assert(con1.size() == con2.size() && L"dismatched size.");

	//	Carrier<y_t> res(con1.size());

	//	{
	//		auto itr1 = con1.begin();
	//		auto itr2 = con2.begin();

	//		while(itr1 != con1.begin())
	//			res >> func(*itr1++, *itr2++);
	//	}

	//	return res;
	//}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template
	<	class CON, class FUNC, class FLAGS = Flag<>
	,	class x_t 
		=	/*PWT::if_flag_is_in_t
			<	FLAGS
			,*/	std::decay_t< decltype(*Declval<CON>().begin()) >
			/*>*/
	>
	static auto Filter(CON&& con, FUNC&& func, FLAGS = Flag<>())-> Carrier<x_t>
	{
		Carrier<x_t> res(con.size());

		for(auto const& x : con)
			if( func(x) )
				res >> x;

		return res;
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	class _Fold_Helper : No_Making
	{
	public:
		template<class ITR, class FUNC, class res_t>
		static auto Accumulate(ITR itr, ITR const ei, FUNC&& func, res_t res)
		->	res_t
		{
			while(itr != ei)
				res = func(res, *itr++);
	
			return res;
		}
	

		template<class ITR1, class ITR2, class FUNC1, class FUNC2, class res_t>
		static auto Inner_Product
		(	ITR1 itr1, ITR1 const ei1, ITR2 itr2, FUNC1&& func1, FUNC2&& func2, res_t res
		)->	res_t
		{
			while(itr1 != ei1)
				res = func1( res, func2(*itr1++, *itr2++) );
			
			return res;
		}
	};


	template<class CON, class FUNC, class res_t>
	static auto Fold(CON&& con, FUNC&& func, res_t&& init)-> res_t
	{
		return 
		_Fold_Helper::Accumulate
		(	con.begin(), con.end(), std::forward<FUNC>(func), std::forward<res_t>(init)
		);
	}
	
	template<class CON, class FUNC>
	static auto Fold(CON&& con, FUNC&& func)-> std::decay_t< decltype(*Declval<CON>().begin()) >
	{
		assert(con.begin() != con.end() && L"the container has nothing to fold.");

		return 
		_Fold_Helper::Accumulate
		(	++con.begin(), con.end(), std::forward<FUNC>(func), *con.begin() 
		);
	}

	template<class CON1, class CON2, class FUNC1, class FUNC2, class res_t>
	static auto Fold
	(	CON1&& con1, CON2&& con2, FUNC1&& func1, FUNC2&& func2, res_t&& init
	)->	res_t
	{
		assert(con1.size() == con2.size() && L"dismatched sizes.");

		return
		_Fold_Helper::Inner_Product
		(	con1.begin(), con1.end(), con2.begin()
		,	std::forward<FUNC1>(func1), std::forward<FUNC2>(func2), std::forward<res_t>(init)
		);
	}

	template<class CON1, class CON2, class FUNC1, class FUNC2>
	static auto Fold
	(	CON1&& con1, CON2&& con2, FUNC1&& func1, FUNC2&& func2
	)->	std::result_of_t
		<	FUNC2( decltype(*Declval<CON1>().begin()), decltype(*Declval<CON2>().begin()) )
		>
	{
		assert(con1.begin() != con1.end() && L"the container has nothing to fold.");
		assert(con1.size() == con2.size() && L"dismatched sizes.");

		return
		_Fold_Helper::Inner_Product
		(	++con1.begin(), con1.end(), ++con2.begin()
		,	std::forward<FUNC1>(func1), std::forward<FUNC2>(func2)
		,	func2(*con1.begin(), *con2.begin())
		);
	}


	template<class CON, class FUNC, class res_t>
	static auto rFold(CON&& con, FUNC&& func, res_t&& init)-> res_t
	{
		return 
		_Fold_Helper::Accumulate
		(	con.rbegin(), con.rend(), std::forward<FUNC>(func), std::forward<res_t>(init)
		);
	}
	
	template<class CON, class FUNC>
	static auto rFold(CON&& con, FUNC&& func)
	->	std::decay_t< decltype(*Declval<CON>().rbegin()) >
	{
		assert(con.rbegin() != con.rend() && L"the container has nothing to fold.");

		return 
		_Fold_Helper::Accumulate
		(	++con.rbegin(), con.rend(), std::forward<FUNC>(func), *con.rbegin() 
		);
	}

	template<class CON1, class CON2, class FUNC1, class FUNC2, class res_t>
	static auto rFold
	(	CON1&& con1, CON2&& con2, FUNC1&& func1, FUNC2&& func2, res_t&& init
	)->	res_t
	{
		assert(con1.size() == con2.size() && L"dismatched sizes.");

		return
		_Fold_Helper::Inner_Product
		(	con1.rbegin(), con1.rend(), con2.rbegin()
		,	std::forward<FUNC1>(func1), std::forward<FUNC2>(func2), std::forward<res_t>(init)
		);
	}

	template<class CON1, class CON2, class FUNC1, class FUNC2>
	static auto rFold
	(	CON1&& con1, CON2&& con2, FUNC1&& func1, FUNC2&& func2
	)->	std::result_of_t
		<	FUNC2( decltype(*Declval<CON1>().rbegin()), decltype(*Declval<CON2>().rbegin()) )
		>
	{
		assert(con1.rbegin() != con1.rend() && L"the container has nothing to fold.");
		assert(con1.size() == con2.size() && L"dismatched sizes.");

		return
		_Fold_Helper::Inner_Product
		(	++con1.rbegin(), con1.rend(), ++con2.rbegin()
		,	std::forward<FUNC1>(func1), std::forward<FUNC2>(func2)
		,	func2(*con1.rbegin(), *con2.rbegin())
		);
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

}

#endif // end of #ifndef _SGM_HIGH_TEMPLAR11_