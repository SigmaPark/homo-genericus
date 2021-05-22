#pragma once

#ifndef _REFORM_GL_
#define _REFORM_GL_

#include <gl/freeglut.h>
#include <type_traits>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{
	
	#define SPECIALIZER_HELPER(NAME, SIZE, TYPE, TAIL)	\
	template<>	\
	class gl##NAME##_Helper<SIZE, TYPE>		\
	{	\
	public:	\
		gl##NAME##_Helper() = delete;	\
	\
		template<typename...ARGS>	\
		static void calc(ARGS&&...args)		\
		{	\
			gl##NAME##SIZE##TAIL( static_cast<TYPE>(args)... );	\
		}	\
	}


	#define PRECISION_NAMING(NAME)	\
	template<unsigned N, typename T>	\
	class gl##NAME##_Helper;	\
	\
	SPECIALIZER_HELPER(NAME, 2, float, f);	\
	SPECIALIZER_HELPER(NAME, 3, float, f);	\
	SPECIALIZER_HELPER(NAME, 4, float, f);	\
	SPECIALIZER_HELPER(NAME, 2, double, d);	\
	SPECIALIZER_HELPER(NAME, 3, double, d);	\
	SPECIALIZER_HELPER(NAME, 4, double, d);	\
	\
	template<typename T, typename...ARGS>	\
	static void gl##NAME(ARGS&&...args)		\
	{	\
		gl##NAME##_Helper<sizeof...(ARGS), T>		\
		::	calc( std::forward<ARGS>(args)... );		\
	}


	PRECISION_NAMING(Normal)
	PRECISION_NAMING(Color)
	PRECISION_NAMING(Vertex)
	PRECISION_NAMING(RasterPos)


	#undef PRECISION_NAMING
	#undef SPECIALIZER_HELPER

}

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

#endif