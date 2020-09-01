#pragma once

#ifndef _SGM_NUM_GOLDEN_SECTION_METHOD_
#define _SGM_NUM_GOLDEN_SECTION_METHOD_

#include "Mathematics.hpp"

namespace sgm
{
	namespace num
	{
		enum class Extreme{MINIMUM, MAXIMUM};

		template<Extreme XT>
		class GoldenSection;
	}
}



#include <initializer_list>
#include <utility>
#include <cmath>


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

	template<class T>
	static bool then(T&&){  return true;  }

	template<class T>
	static T middle(T const& a, T const& b){  return T(.5)*(a + b);  }


public:
	template
	<	class FUNC, class X
	,	class Y = std::decay_t< decltype( std::declval<FUNC>()(std::declval<X>()) ) > 
	>
	static auto search
	(	FUNC&& func, std::initializer_list<X>&& min_max
	,	X const epsilon = Constants::Epsilon<X, 4>()
	)->	std::pair<X, Y>
	{
		static X const iphi = X(.5) * (  sqrt( X(5) ) - X(1)  );
	
		X	a = min_max.begin()[0], b = min_max.begin()[1], M = iphi*(b - a),
			c = b - M, d = a + M;
		Y	fc = func(c), fd = func(d);
			
		while( d - c < epsilon && then(M *= iphi) )
			if( Helper<XT>::Left(fc, fd) )
				b = d,  d = c,  c = b - M,  fd = fc,  fc = func(c);
			else
				a = c,  c = d,  d = a + M,  fc = fd,  fd = func(d);

		return std::make_pair( middle(c, d), middle(fc, fd) );
	}
};


#endif