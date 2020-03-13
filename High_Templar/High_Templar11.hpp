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
//#include <algorithm>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	class PWT : public Flag_t
	{
	public:
		template<class FLAGSET, class T>
		using if_flag_is_in_t
		=	std::conditional_t< FLAGSET:: template has<PWT>::value, Pinweight<T>, T >;
	};


	class PAR : public Flag_t{};
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
	<	class FLAGS = Flags<>
	,	class CON = std::nullptr_t, class FUNC = std::nullptr_t
	,	class y_t 
		=	PWT::if_flag_is_in_t
			<	FLAGS
			,	std::result_of_t
				<	FUNC( std::decay_t< decltype(*Declval<CON>().begin()) > )
				>
			>
	>
	static auto Morph(CON&& con, FUNC&& func)-> Carrier<y_t>
	{
		Carrier<y_t> res(con.size());

		for(auto const& x : con)
			res >> func(x);

		return res;
	}


	template
	<	class FLAGS = Flags<>
	,	class CON1 = std::nullptr_t, class CON2 = std::nullptr_t, class FUNC = std::nullptr_t
	,	class y_t
		=	PWT::if_flag_is_in_t
			<	FLAGS
			,	std::result_of_t
				<	FUNC
					(	std::decay_t< decltype(*Declval<CON1>().begin()) >
					,	std::decay_t< decltype(*Declval<CON2>().begin()) >
					)
				>
			>
	>
	static auto Morph(CON1&& con1, CON2&& con2, FUNC&& func)-> Carrier<y_t>
	{
		assert(con1.size() == con2.size() && L"dismatched size.");

		Carrier<y_t> res(con1.size());

		{
			auto itr1 = con1.begin();
			auto itr2 = con2.begin();

			while(itr1 != con1.begin())
				res >> func(*itr1++, *itr2++);
		}

		return res;
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template
	<	class FLAGS = Flags<>
	,	class CON = std::nullptr_t, class FUNC = std::nullptr_t
	,	class x_t 
		=	PWT::if_flag_is_in_t
			<	FLAGS
			,	std::decay_t< decltype(*Declval<CON>().begin()) >
			>
	>
	static auto Filter(CON&& con, FUNC&& func)-> Carrier<x_t>
	{
		Carrier<x_t> res(con.size());

		for(auto const& x : con)
			if( func(x) )
				res >> x;

		return res;
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class ITR, class FUNC, class res_t>
	static auto _Accumulate(ITR bi, ITR ei, FUNC&& func, res_t&& init)-> res_t
	{
		res_t res = std::forward<res_t>(init);

		for(ITR itr = bi; itr != ei; ++itr)
			res = func(res, *itr)

		return res;
	}

	template<class ITR1, class ITR2, class FUNC1, class FUNC2, class res_t>
	static auto _Inner_Product
	(	ITR1 bi1, ITR1 ei1, ITR2 bi2, FUNC1&& func1, FUNC2&& func2, res_t&& init
	)->	res_t
	{
		res_t res = std::forward<res_t>(init);

		{
			ITR1 itr1 = bi1;
			ITR2 itr2 = bi2;

			while(itr1 != ei1)
				res = func2( res, func1(*itr1++, *itr2++) );
		}

		return res;
	}


	template<class CON, class FUNC, class res_t>
	static auto Fold(CON&& con, FUNC&& func, res_t&& init)-> res_t
	{
		return 
		_Accumulate
		(	con.begin(), con.end(), std::forward<FUNC>(func), std::forward<res_t>(init)
		);
	}
	
	template<class CON, class FUNC>
	static auto Fold(CON&& con, FUNC&& func)-> std::decay_t< decltype(*Declval<CON>().begin()) >
	{
		assert(con.begin() != con.end() && L"the container has nothing to fold.");

		return _Accumulate( ++con.begin(), con.end(), std::forward<FUNC>(func), *con.begin() );
	}

	template<class CON1, class CON2, class FUNC1, class FUNC2, class res_t>
	static auto Fold
	(	CON1&& con1, CON2&& con2, FUNC1&& func1, FUNC2&& func2, res_t&& init
	)->	res_t
	{
		assert(con1.size() == con2.size() && L"dismatched sizes.");

		return
		_Inner_Product
		(	con1.begin(), con1.end(), con2.begin()
		,	std::forward<FUNC1>(func1), std::forward<FUNC2>(func2), std::forward<res_t>(init)
		);
	}

	template<class CON1, class CON2, class FUNC1, class FUNC2>
	static auto Fold
	(	CON1&& con1, CON2&& con2, FUNC1&& func1, FUNC2&& func2
	)->	std::result_of_t
		<	FUNC1( decltype(*Declval<CON1>().begin()), decltype(*Declval<CON2>().begin()) )
		>
	{
		assert(con1.begin() != con1.end() && L"the container has nothing to fold.");
		assert(con1.size() == con2.size() && L"dismatched sizes.");

		return
		_Inner_Product
		(	++con1.begin(), con1.end(), ++con2.begin()
		,	std::forward<FUNC1>(func1), std::forward<FUNC2>(func2)
		,	func1(*con1.begin(), *con2.end())
		);
	}


	template<class CON, class FUNC, class res_t>
	static auto rFold(CON&& con, FUNC&& func, res_t&& init)-> res_t
	{
		return 
		_Accumulate
		(	con.rbegin(), con.rend(), std::forward<FUNC>(func), std::forward<res_t>(init)
		);
	}
	
	template<class CON, class FUNC>
	static auto rFold(CON&& con, FUNC&& func)
	->	std::decay_t< decltype(*Declval<CON>().rbegin()) >
	{
		assert(con.rbegin() != con.rend() && L"the container has nothing to fold.");

		return _Accumulate( ++con.rbegin(), con.rend(), std::forward<FUNC>(func), *con.rbegin() );
	}

	template<class CON1, class CON2, class FUNC1, class FUNC2, class res_t>
	static auto rFold
	(	CON1&& con1, CON2&& con2, FUNC1&& func1, FUNC2&& func2, res_t&& init
	)->	res_t
	{
		assert(con1.size() == con2.size() && L"dismatched sizes.");

		return
		_Inner_Product
		(	con1.rbegin(), con1.rend(), con2.rbegin()
		,	std::forward<FUNC1>(func1), std::forward<FUNC2>(func2), std::forward<res_t>(init)
		);
	}

	template<class CON1, class CON2, class FUNC1, class FUNC2>
	static auto rFold
	(	CON1&& con1, CON2&& con2, FUNC1&& func1, FUNC2&& func2
	)->	std::result_of_t
		<	FUNC1( decltype(*Declval<CON1>().rbegin()), decltype(*Declval<CON2>().rbegin()) )
		>
	{
		assert(con1.rbegin() != con1.rend() && L"the container has nothing to fold.");
		assert(con1.size() == con2.size() && L"dismatched sizes.");

		return
		_Inner_Product
		(	++con1.rbegin(), con1.rend(), ++con2.rbegin()
		,	std::forward<FUNC1>(func1), std::forward<FUNC2>(func2)
		,	func1(*con1.rbegin(), *con2.rend())
		);
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

}

#endif // end of #ifndef _SGM_HIGH_TEMPLAR11_