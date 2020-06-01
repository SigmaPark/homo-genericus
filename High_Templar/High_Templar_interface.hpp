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
	

	struct _SequanceSize : No_Making
	{
		using type = size_t;

		enum : type{DYNAMIC = -1};  
	};

	using SeqSize_t = typename _SequanceSize::type;
	//========//========//========//========//=======#//========//========//========//========//===


	/**	Should be defined later
	*/
	template<class T, SeqSize_t N = _SequanceSize::DYNAMIC> class _Sequance_t;


	/**	Flag which should be defined later
	*/
	struct HT_Flag : No_Making
	{
		struct IMMUTABLE;
		struct SHARED;
		struct REFERRED;
	};

	/**	Should be defined later
	*/
	struct HT_impl;
	//========//========//========//========//=======#//========//========//========//========//===


	template<  class T = SeqSize_t, class = std::enable_if_t< std::is_scalar<T>::value >  >
	static auto indices(SeqSize_t N, T offset = 0)-> _Sequance_t<T>
	{
		_Sequance_t<T> res(N);

		while(N-->0)
			res >> offset++;

		return res;
	}
	//========//========//========//========//=======#//========//========//========//========//===


	template
	<	class...FLAGS, class CON, class FUNC
	,	class = std::enable_if_t< is_iterable<CON>::value >
	>
	static auto Morph(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
	(
		HT_impl::Morph<FLAGS...>( std::forward<CON>(con), std::forward<FUNC>(func) )
	)

	template
	<	class...ARGS, class FLAG_SET, class CON, class FUNC
	,	class = std::enable_if_t< is_iterable<CON>::value >
	>
	static auto Morph(FLAG_SET fset, CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
	(
		HT_impl::Morph<ARGS...>( fset, std::forward<CON>(con), std::forward<FUNC>(func) )
	)
	//========//========//========//========//=======#//========//========//========//========//===


	template
	<	class...FLAGS, class CON, class FUNC
	,	class = std::enable_if_t< is_iterable<CON>::value >
	>
	static auto Filter(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
	(
		HT_impl::Filter<FLAGS...>( std::forward<CON>(con), std::forward<FUNC>(func) )
	)

	template
	<	class...ARGS, class FLAG_SET, class CON, class FUNC
	,	class = std::enable_if_t< is_iterable<CON>::value >
	>
	static auto Filter(FLAG_SET fset, CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
	(
		HT_impl::Filter<ARGS...>( fset, std::forward<CON>(con), std::forward<FUNC>(func) )
	)
	//========//========//========//========//=======#//========//========//========//========//===


	template
	<	class...FLAGS, class CON, class FUNC
	,	class res_t = std::nullptr_t
	,	class = std::enable_if_t< is_iterable<CON>::value >
	>
	static auto Fold(CON&& con, FUNC&& func, res_t&& init = nullptr) SGM_DECLTYPE_AUTO
	(
		HT_impl::Fold<FLAGS...>
		(	std::forward<CON>(con), std::forward<FUNC>(func), std::forward<res_t>(init)
		)
	)

	template
	<	class...ARGS, class FLAG_SET, class CON, class FUNC
	,	class res_t = std::nullptr_t
	,	class = std::enable_if_t< is_iterable<CON>::value >
	>
	static auto Fold(FLAG_SET fset, CON&& con, FUNC&& func, res_t&& init = nullptr) 
	SGM_DECLTYPE_AUTO
	(
		HT_impl::Fold<ARGS...>
		(	fset, std::forward<CON>(con), std::forward<FUNC>(func), std::forward<res_t>(init)
		)
	)

	template
	<	class...FLAGS, class CON, class FUNC
	,	class res_t = std::nullptr_t
	,	class = std::enable_if_t< is_iterable<CON>::value >
	>
	static auto rFold(CON&& con, FUNC&& func, res_t&& init = nullptr) SGM_DECLTYPE_AUTO
	(
		HT_impl::rFold<FLAGS...>
		(	std::forward<CON>(con), std::forward<FUNC>(func), std::forward<res_t>(init)
		)
	)

	template
	<	class...ARGS, class FLAG_SET, class CON, class FUNC
	,	class res_t = std::nullptr_t
	,	class = std::enable_if_t< is_iterable<CON>::value >
	>
	static auto rFold(FLAG_SET fset, CON&& con, FUNC&& func, res_t&& init = nullptr) 
	SGM_DECLTYPE_AUTO
	(
		HT_impl::rFold<ARGS...>
		(	fset, std::forward<CON>(con), std::forward<FUNC>(func), std::forward<res_t>(init)
		)
	)
	//========//========//========//========//=======#//========//========//========//========//===

	

}

#endif