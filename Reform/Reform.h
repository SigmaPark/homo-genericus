#pragma once

#ifndef _SGM_REFORM_
#define _SGM_REFORM_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif

#include <limits>
#include <type_traits>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

#ifndef DEFINE_SGM_REAL_TYPE
	#define DEFINE_SGM_REAL_TYPE(Number, Type)	\
		template<>	\
		class sgm::_Real<(Number)>	\
		{	\
		public:	\
			using type = Type;	\
		}
#else
	#error Macro DEFINE_SGM_REAL_TYPE was already defined elsewhere.
#endif


#ifndef DEFINE_SGM_INDEX_TYPE
	#define DEFINE_SGM_INDEX_TYPE(Number, Type)	\
		template<>	\
		class sgm::_index<(Number)>	\
		{	\
		public:	\
			using type = Type;	\
		}
#else
	#error Macro DEFINE_SGM_INDEX_TYPE was already defined elsewhere.
#endif


namespace sgm
{
	
	template<unsigned>
	class _Real;

	DEFINE_SGM_REAL_TYPE(1, float);

	template<unsigned N = 1>
	using Real_t = typename _Real<N>::type;
	//========//========//========//========//=======#//========//========//========//========//===


	template<unsigned>
	class _index;

	DEFINE_SGM_INDEX_TYPE(1, size_t);

	template<unsigned N = 1>
	using idx_t = typename _index<N>::type;
	//========//========//========//========//=======#//========//========//========//========//===
}


#endif