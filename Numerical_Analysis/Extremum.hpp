#pragma once

#ifndef _SGM_NUM_GOLDEN_SECTION_METHOD_
#define _SGM_NUM_GOLDEN_SECTION_METHOD_


#include "Mathematics.hpp"


namespace sgm
{
	namespace num
	{
		enum class Extreme{MINIMUM, MAXIMUM};


		template<class X, class Y>
		struct XY_Pair;


		template<class FUNC, class X>
		struct _Result_of;


		template<Extreme XT>
		class GoldenSection;


		template< Extreme XT, class LINE_SEARCHER = GoldenSection<XT> >
		class Powell;
	}
}


template<class X, class Y>
struct sgm::num::XY_Pair{  X x;  Y y;  };


#include "..\Serial\Serial.hpp"


template<class FUNC, class X>
struct sgm::num::_Result_of
{
	using type = std::decay_t< decltype( Declval<FUNC>()(Declval<X>()) ) >;
};


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
	,	class Y = typename _Result_of<FUNC, X>::type
	,	class GAMMA_DOMAIN
	,	class 
		=	std::enable_if_t
			<	std::is_scalar<X>::value && is_iterable<GAMMA_DOMAIN, X>::value
			>
	>
	static auto search(FUNC&& func, GAMMA_DOMAIN&& x_domain, X epsilon)-> XY_Pair<X, Y>
	{
		static X const iphi = X(.5) * (  sqrt( X(5) ) - X(1)  );

		if(epsilon < 0)
			epsilon *= X(-1);

		X a = *x_domain.begin(), b = *Next(x_domain.begin());

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


template<sgm::num::Extreme XT, class LINE_SEARCHER>
class sgm::num::Powell
{
private:
	template<class Y, class FUNC, class G>
	static auto line_search
	(	FUNC const& func, Serial<G> const& x, Serial<G> const& u, G const radius, G const eps_g
	)->	Serial<G>
	{
		Serial<G> const gamma_domain{-radius, radius};

		auto xf
		=	[x, &u](G gamma) mutable-> Serial<G>
			{
				for
				(	auto duo = Dual_iteration(x.begin(), u.begin()) 
				;	duo._1 != x.end()
				;	*duo._1 = *duo._1 + gamma * *duo._2,  duo++
				);

				return x;
			};

		auto const gamma 
		=	LINE_SEARCHER::search
			(	[&func, &xf](G g)-> Y{  return func( xf(g) );  }, gamma_domain, eps_g
			) . x;

		return xf(gamma);
	}


	template<class X>
	static auto Basis(size_t const size)-> Serial< Serial<X> >
	{
		Serial< Serial<X> > res( size, Serial<X>(size, 0) );

		for(size_t idx = 0;  idx < size;  ++idx)
			res[idx][idx] = 1;

		return res;
	}


	template<class X>
	static auto Norm(Serial<X>&& vec)-> X
	{
		X res = 0;

		for(auto x : vec)
			res += x*x;

		return sqrt(res);
	}


public:
	template
	<	class FUNC, class XV, class X, class Y = typename _Result_of<FUNC, XV>::type
	>
	static auto search
	(	FUNC&& func, XV&& init_x, X const radius, X const epsilon, unsigned max_iteration
	)->	XY_Pair< Serial<X>, Y >
	{
		auto x = iterable_cast< Serial<X> >( std::forward<XV>(init_x) );
		auto const n = x.size();
		auto U = Basis<X>(n);

		auto line_sweep 
		=	[radius, epsilon, &func](Serial<X> const& v, Serial<X> const& u)-> Serial<X>
			{
				return line_search<Y>(func, v, u, radius, epsilon);
			};

		for(X delta = radius;  delta > epsilon && max_iteration > 0;  --max_iteration)
		{
			auto x2 = x;

			for(auto const& u : U)
				x2 = line_sweep(x2, u);

			for(size_t idx = 0;  idx < n - 1;  ++idx)
				U[idx] = U[idx+1];

			U[n - 1] = x2 - x;

			x2 = line_sweep(x, U[n - 1]);

			delta = Norm(x2 - x);

			x = x2;
		}

		return 
		max_iteration == 0
		?	XY_Pair< Serial<X>, Y >{Serial<X>{}, 0}
		:	XY_Pair< Serial<X>, Y >{x, func(x)};
	}


};


#endif // end of #ifndef _SGM_NUM_GOLDEN_SECTION_METHOD_