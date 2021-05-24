/**	higher order functions in sgm::High_Templar library as sgm::Functor format
*/
#pragma once

#ifndef _SGM_HOF_
#define _SGM_HOF_


#include "..\Functor\Functor.hpp"
#include "..\High_Templar\High_Templar.hpp"
#include "..\High_Templar\Rankers.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm::hof
{

#ifndef SGM_HOF_FACTORY
	#define SGM_HOF_FACTORY(temfunc)	\
		template<class...TYPES, unsigned D = 2>	\
		static auto const temfunc##_f	\
		=	[](auto&&...args)	\
			{	\
				if constexpr( sizeof...(TYPES) == 0 )	\
					return temfunc( Forward<decltype(args)>(args)... );	\
				else	\
					return temfunc<TYPES...>( Forward<decltype(args)>(args)... );	\
			} / Dim<D>


	using namespace fp;
	using namespace ht;

	SGM_HOF_FACTORY(Morph);
	SGM_HOF_FACTORY(Filter);
	SGM_HOF_FACTORY(Fold);
	SGM_HOF_FACTORY(rFold);
	SGM_HOF_FACTORY(Rankers);
	SGM_HOF_FACTORY(Plait);


	#undef SGM_HOF_FACTORY
#else
	#error SGM_HOF_FACTORY was already defined elsewhere.
#endif

}

#endif	//	End of #ifndef _SGM_HOF_