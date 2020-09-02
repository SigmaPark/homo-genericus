#pragma once

#ifndef _SGM_NUM_GOLDEN_SECTION_METHOD_
#define _SGM_NUM_GOLDEN_SECTION_METHOD_

#include "Mathematics.hpp"


namespace sgm
{
	namespace num
	{
		enum class Extreme{MINIMUM, MAXIMUM};

		template<class T>
		struct Real_Range;

		template<class X, class Y>
		struct XY_Pair;

		template<Extreme XT>
		class GoldenSection;
	}
}


template<class T>
struct sgm::num::Real_Range{  T t1, t2;  };


template<class X, class Y>
struct sgm::num::XY_Pair{  X x;  Y y;  };


#include <type_traits>


template<sgm::num::Extreme XT>
class sgm::num::GoldenSection
{
private:
	template<Extreme> struct Helper;

	template<>
	struct Helper<Extreme::MINIMUM>
	{
		template<class Y>
		static bool Left(Y const& fc, Y const& fd){  return fc < fd;  }
	};

	template<>
	struct Helper<Extreme::MAXIMUM>
	{
		template<class Y>
		static bool Left(Y const& fc, Y const& fd){  return fc > fd;  }
	};

	template<bool B, class...ARGS>
	static bool then(ARGS&&...){  return B;  }

	template<class T>
	static T middle(T const& a, T const& b){  return T(.5)*(a + b);  }


public:
	template
	<	class X, class FUNC
	,	class Y 
		=	std::decay_t
			<	decltype
				(	( *(std::decay_t<FUNC>*)(nullptr) )( *(std::decay_t<X>*)(nullptr) )  
				)
			> 
	>
	static auto search(FUNC&& func, Real_Range<X> const x_range, X epsilon)-> XY_Pair<X, Y>
	{
		static X const iphi = X(.5) * (  sqrt( X(5) ) - X(1)  );

		if(epsilon < 0)
			epsilon *= X(-1);

		X	a = x_range.t1, b = x_range.t2;

		if(a > b)
			std::swap(a, b);
	
		X	M = iphi*(b - a), c = b - M, d = a + M;
		Y	fc = func(c), fd = func(d);
			
		while( d - c > epsilon && then<true>(M *= iphi) )
			if( Helper<XT>::Left(fc, fd) )
				b = d,  d = c,  c = b - M,  fd = fc,  fc = func(c);
			else
				a = c,  c = d,  d = a + M,  fc = fd,  fd = func(d);

		return {middle(c, d), middle(fc, fd)};
	}
};


#endif // end of #ifndef _SGM_NUM_GOLDEN_SECTION_METHOD_