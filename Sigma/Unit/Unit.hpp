#pragma once

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version of language support is required.
#endif


////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{
	
}


#if 0

	Length<float> L1 = 2 * Meter;

	Length<float> L2 = 2000 * Milli< Meter<2> >;

	Length<float, 2> A1 = 40000 * Milli< Meter<2> >;

	float fa1 = Milli< Meter<2> >(A1); //
	
	Quality< float, Dimen<Mass>, Dimen<Length>, Dimen<Time, -2> > F1
	=	56 * Kilo<Gram> * Meter / Sec<2>;

	float fL1 = Kilo<inch>(L1);



#endif