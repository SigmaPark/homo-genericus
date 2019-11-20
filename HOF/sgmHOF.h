/**	higher order functions in sgm::High_Templar library as sgm::Functor format
*/
#pragma once

#ifndef _SGM_HOF_
#define _SGM_HOF_

#include "../Functor/Functor.h"
#include "../High_Templar/High_Templar.h"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{
#ifndef SGM_HOF_FACTORY
	#define SGM_HOF_FACTORY(temfunc)	\
		template<unsigned D, typename...TYPES>	\
		static auto const temfunc##_c	\
		=	[](auto&&...args)	\
			{	\
				if constexpr( sizeof...(TYPES) == 0 )	\
					return temfunc( std::forward<decltype(args)>(args)... );	\
				else	\
					return temfunc<TYPES...>( std::forward<decltype(args)>(args)... );	\
			} / Dim<D>
#else
	#error SGM_HOF_FACTORY was already defined elsewhere.
#endif

	SGM_HOF_FACTORY(Morph);
	SGM_HOF_FACTORY(Repack);
	SGM_HOF_FACTORY(Filter);
	SGM_HOF_FACTORY(Fold);
	SGM_HOF_FACTORY(Sort);
	SGM_HOF_FACTORY(Rankers);
	SGM_HOF_FACTORY(Take);

	#undef SGM_HOF_FACTORY

	template<unsigned D, size_t N>
	static auto const RankerN_c
	=	[](auto&&...args)
		{	
			return Rankers<N>( std::forward<decltype(args)>(args)... );
		} / Dim<D>;

}

#endif	//	End of #ifndef _SGM_HOF_