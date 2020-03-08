/**	Higher order functions in FP paradigm as template function format
*/
#pragma once

#ifndef _SGM_HIGH_TEMPLAR11_
#define _SGM_HIGH_TEMPLAR11_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

#ifndef SGM_DECLTYPE_AUTO
	#define SGM_DECLTYPE_AUTO(...)	-> decltype(__VA_ARGS__){  return __VA_ARGS__;  }
#endif


namespace sgm
{


#ifndef _ITERATOR_
	#define _ITERATING_METHOD(method)	\
	template<class CON>	\
	static auto method(CON&& con) SGM_DECLTYPE_AUTO(con.method())

	_ITERATING_METHOD(cbegin)
	_ITERATING_METHOD(cend)
	_ITERATING_METHOD(begin)
	_ITERATING_METHOD(end)

	#undef _ITERATING_METHOD
#endif


	template<class CON, class F>
	static auto Morph(CON&& con, F&& f){}


}

#endif // end of #ifndef _SGM_HIGH_TEMPLAR11_