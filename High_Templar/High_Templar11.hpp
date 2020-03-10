/**	Higher order functions in FP paradigm as template function format
*/
#pragma once

#ifndef _SGM_HIGH_TEMPLAR11_
#define _SGM_HIGH_TEMPLAR11_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

#include "..\Carrier\Carrier.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


#ifndef _ITERATOR_
	#ifndef _ITERATING_METHOD
		#define _ITERATING_METHOD(method)	\
			template<class CON>	\
			static auto method(CON&& con) SGM_DECLTYPE_AUTO(con.method())
	#else
		#error _ITERATING_METHOD was already defined somewhere else.
	#endif

	_ITERATING_METHOD(cbegin)
	_ITERATING_METHOD(cend)
	_ITERATING_METHOD(begin)
	_ITERATING_METHOD(end)

	#undef _ITERATING_METHOD
#endif


	template<class T = size_t>
	static auto indices(size_t N, T offset = 0)-> Carrier<T>
	{
		Carrier<T> res(N);

		while(N-->0)
			res >> offset++;

		return res;
	}


	template
	<	class CON, class FUNC
	,	class y_t 
		=	_Result_of_t
			<	FUNC
			,	_Original_t< decltype(*Declval<CON>().begin()) >
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
	<	class CON, class FUNC
	,	class x_t = _Original_t< decltype(*Declval<CON>().begin()) >
	>
	static auto Filter(CON&& con, FUNC&& func)-> Carrier<x_t>
	{
		Carrier<x_t> res(con.size());

		for(auto const& x : con)
			if( func(x) )
				res >> x;

		return res;
	}


	template<class CON, class FUNC, class res_t>
	static auto Fold(CON&& con, FUNC&& func, res_t&& init)-> res_t
	{
		res_t res = init;

		for(auto const& x : con)
			res = func(res, x);

		return res;
	}

	template<class CON, class FUNC>
	static auto Fold(CON&& con, FUNC&& func)-> _Original_t< decltype(*Declval<CON>().begin()) >
	{
		assert(con.begin() != con.end() && L"the container has nothing to fold.");

		auto res = *con.begin();
		
		for(auto itr = ++con.begin(); itr != con.end(); itr++)
			res = func(res, *itr);

		return res;
	}


}

#endif // end of #ifndef _SGM_HIGH_TEMPLAR11_