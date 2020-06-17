#pragma once

#ifndef _SGM_HIGH_TEMPLAR_INTERFACE_
#define _SGM_HIGH_TEMPLAR_INTERFACE_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

#include "..\interface_Traits\interface_Traits.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{
	

	class HT_implementation;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template
	<	class T = size_t, class IMPL = HT_implementation
	,	class = Provided_t< std::is_integral<T>::value >
	>
	static auto indices( size_t size, T offset = size_t(0) ) SGM_DECLTYPE_AUTO
	(
		IMPL::indices(size, offset)
	)


	struct No_Deco{  template<class T> using type = T;  };
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
	
	
#ifndef _SGM_MORPH_AND_FILTER_INTERFACE
	#define _SGM_MORPH_AND_FILTER_INTERFACE(NAME)	\
		template		\
		<	class DECO = No_Deco, class CON, class FUNC, class IMPL = HT_implementation	\
		,	class = Provided_t< is_iterable<CON>::value >	\
		>	\
		static auto NAME(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO		\
		(	\
			IMPL::template NAME<DECO>( std::forward<CON>(con), std::forward<FUNC>(func) )	\
		)	\
		\
		\
		template		\
		<	class...ARGS, class DECO, class CON, class FUNC	\
		,	class = Provided_t< sizeof...(ARGS) <= 2 >	\
		>	\
		static auto NAME(DECO, CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO	\
		(	\
			NAME<DECO>	\
			(	std::conditional_t		\
				<	sizeof...(ARGS) >= 1, First_t<ARGS...>, decltype(con)	\
				>	\
				(con)	\
			,	std::conditional_t		\
				<	sizeof...(ARGS) >= 2, Nth_t<1, ARGS...>, decltype(func) \
				>	\
				(func)	\
			)	\
		)

		_SGM_MORPH_AND_FILTER_INTERFACE(Morph)
		_SGM_MORPH_AND_FILTER_INTERFACE(Filter)

	#undef _SGM_MORPH_AND_FILTER_INTERFACE
#else
	#error _SGM_MORPH_AND_FILTER_INTERFACE was already defined somewhere else.
#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


#ifndef _SGM_FORWARD_AND_REVERSE_FOLD_INTERFACE
	#define _SGM_FORWARD_AND_REVERSE_FOLD_INTERFACE(NAME)	\
		template		\
		<	class DECO = No_Deco, class CON, class FUNC, class init_t	\
		,	class IMPL = HT_implementation	\
		,	class = std::enable_if_t< is_iterable<CON>::value >	\
		>	\
		static auto NAME(CON&& con, FUNC&& func, init_t&& init) SGM_DECLTYPE_AUTO		\
		(	\
			IMPL::template NAME<DECO>	\
			(	std::forward<CON>(con), std::forward<FUNC>(func)	\
			,	std::forward<init_t>(init)	\
			)	\
		)	\
		\
		\
		template<class DECO = No_Deco, class CON, class FUNC>	\
		static auto NAME(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO	\
		(	\
			NAME<DECO>	\
			(	std::forward<CON>(con), std::forward<FUNC>(func), std::nullptr_t()	\
			)	\
		)	\
		\
		\
		template		\
		<	class...ARGS, class DECO	\
		,	class CON, class FUNC, class init_t	\
		,	class = std::enable_if_t< !is_iterable<DECO>::value && sizeof...(ARGS) <= 3 >	\
		>	\
		static auto NAME(DECO, CON&& con, FUNC&& func, init_t&& init) SGM_DECLTYPE_AUTO	\
		(	\
			NAME<DECO>	\
			(	std::conditional_t		\
				<	sizeof...(ARGS) >= 1, First_t<ARGS...>, decltype(con)		\
				>	\
				(con)	\
			,	std::conditional_t		\
				<	sizeof...(ARGS) >= 2, Nth_t<1, ARGS...>, decltype(func)		\
				>	\
				(func)	\
			,	std::conditional_t		\
				<	sizeof...(ARGS) >= 3, Nth_t<2, ARGS...>, decltype(init)		\
				>	\
				(init)	\
			)	\
		)	\
		\
		\
		template		\
		<	class...ARGS, class DECO, class CON, class FUNC	\
		,	class = std::enable_if_t< !is_iterable<DECO>::value && sizeof...(ARGS) <= 2 >	\
		>	\
		static auto NAME(DECO, CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO		\
		(	\
			NAME<DECO>	\
			(	std::conditional_t		\
				<	sizeof...(ARGS) >= 1, First_t<ARGS...>, decltype(con)	\
				>	\
				(con)	\
			,	std::conditional_t		\
				<	sizeof...(ARGS) >= 2, Nth_t<1, ARGS...>, decltype(func)		\
				>	\
				(func)	\
			,	std::nullptr_t()	\
			)	\
		)

		_SGM_FORWARD_AND_REVERSE_FOLD_INTERFACE(Fold)
		_SGM_FORWARD_AND_REVERSE_FOLD_INTERFACE(rFold)

	#undef _SGM_FORWARD_AND_REVERSE_FOLD_INTERFACE
#else
	#error _SGM_FORWARD_AND_REVERSE_FOLD_INTERFACE was already defined somewhere else.
#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


}

#endif