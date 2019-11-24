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
		class sgm::Real<(Number)>	\
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
		class sgm::index<(Number)>	\
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
	class Real;


	template<>
	class Real<1>
	{
	public:
		using type = float;
	};


	template<unsigned N = 1>
	using Real_t = typename Real<N>::type;
	//========//========//========//========//=======#//========//========//========//========//===


	template<unsigned>
	class index;


	template<>
	class index<1>
	{
	public:
		using type = size_t;
	};


	template<unsigned N = 1>
	using idx_t = typename index<N>::type;
	//========//========//========//========//=======#//========//========//========//========//===
}


#endif