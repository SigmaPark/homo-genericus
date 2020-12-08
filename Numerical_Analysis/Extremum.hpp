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
struct sgm::num::_Result_of : No_Making
{
	using type = std::decay_t< decltype( Declval<FUNC>()(Declval<X>()) ) >;
};


template<sgm::num::Extreme XT>
class sgm::num::GoldenSection : No_Making
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
			
		while( d - c > epsilon && then(M *= iphi) )
			if( Helper<XT>::Left(fc, fd) )
				b = d,  d = c,  c = b - M,  fd = fc,  fc = func(c);
			else
				a = c,  c = d,  d = a + M,  fc = fd,  fd = func(d);

		return {middle(c, d), middle(fc, fd)};
	}
};


template<sgm::num::Extreme XT, class LINE_SEARCHER>
class sgm::num::Powell : No_Making
{
private:
	template<class Y, class FUNC, class G>
	static auto line_search
	(	FUNC const& func, Serial<G> const& x, Serial<G> const& u, G const radius, G const eps_g
	)->	Serial<G>
	{
		Serial<G> const gamma_domain{-radius, radius};

		auto xf
		=	[&x, &u](G gamma)-> Serial<G>
			{
				Serial<G> s = x;

				for
				(	auto duo = Dual_iteration(s.begin(), u.begin()) 
				;	duo._1 != s.end()
				;	*duo._1 = *duo._1 + gamma * *duo._2,  duo++
				);

				return s;
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
	static auto sqrNorm(Serial<X>&& vec)-> X
	{
		X res = 0;

		for(auto x : vec)
			res += x*x;

		return res;
	}


	template<class CON>
	static auto Minus(CON x1, CON const& x2)-> CON
	{
		for( auto duo = Dual_iteration(x1.begin(), x2.begin());  duo._1 != x1.end();  duo++ )
			*duo._1 -= *duo._2;

		return x1;
	}


	template<size_t N, class X>
	struct _Apply_Helper
	{
		template<class FUNC, class...ARGS>
		static auto calc(FUNC&& func, Serial<X> const& vec, ARGS const&...args) SGM_DECLTYPE_AUTO
		(
			_Apply_Helper<N-1, X>::calc(func, vec, vec[N-1], args...)
		)
	};

	template<class X>
	struct _Apply_Helper<0, X>
	{
		template<class FUNC, class...ARGS>
		static auto calc(FUNC&& func, Serial<X> const&, ARGS const&...args) SGM_DECLTYPE_AUTO
		(
			func(args...)
		)
	};


	template<size_t N, class FUNC, class X>
	class _Apply
	{
	private:
		FUNC const& _func;

	public:
		_Apply(FUNC const& func) : _func(func){}

		auto operator=(_Apply const&)-> _Apply& = delete;

		auto operator()(Serial<X> const& vec) const SGM_DECLTYPE_AUTO
		(
			_Apply_Helper<N, X>::calc(_func, vec)
		)
	};


	template<class FUNC, class X, class XITR, class Y>
	static bool _search
	(	FUNC&& func, Serial<X> x, X const radius, X const epsilon, unsigned max_iteration
	,	XITR xitr, Y& yout
	)
	{
		auto U = Basis<X>(x.size());

		auto line_sweep
		=	[radius, epsilon, &func](Serial<X> const& v, Serial<X> const& u)-> Serial<X>
			{
				return line_search<Y>(func, v, u, radius, epsilon);
			};

		X const sqrEpsilon = pow(epsilon, 2);

		for
		(	X sqrDelta = pow(radius, 2)
		;	sqrDelta > sqrEpsilon && max_iteration > 0
		;	--max_iteration
		)
		{
			auto x2 = x;

			for(auto const& u : U)
				x2 = line_sweep(x2, u);

			for(size_t idx = 0;  idx < U.size() - 1;  ++idx)
				U[idx] = U[idx+1];

			U.back() = Minus(x2, x);

			x2 = line_sweep(x, U.back());

			sqrDelta = sqrNorm( Minus(x2, x) );

			x = x2;
		}

		bool const success = max_iteration != 0;

		if(success)
			for
			(	auto duo = Dual_iteration(x.begin(), xitr)
			;	duo._1 != x.end() || lastly( yout = func(x) )
			;	*duo._2++ = *duo._1++
			);

		return success;
	}


	template<class FUNC, class XV, class X, class XITR>
	static bool _search
	(	FUNC&& func, XV&& init_x, X const radius, X const epsilon, unsigned max_iteration
	,	XITR xitr
	)
	{
		std::decay_t<decltype( func(init_x) )> y = 0;

		return
		_search
		(	std::forward<FUNC>(func)
		,	std::forward<XV>(init_x), radius, epsilon, max_iteration, xitr, y
		);
	}


public:
	template<size_t NOF_ARGUMENT, class FUNC, class XITR, class Y, class XV, class X>
	static bool search
	(	FUNC&& func, XV&& init_x, X const radius, X const epsilon, unsigned max_iteration
	,	XITR xitr_out, Y& yout
	)
	{
		return
		_search
		(	_Apply<NOF_ARGUMENT, decltype(func), X>( std::forward<FUNC>(func) )
		,	iterable_cast< Serial<X> >( std::forward<XV>(init_x) )
		,	radius, epsilon, max_iteration, xitr_out, yout
		);
	}


	template<size_t NOF_ARGUMENT, class FUNC, class XITR, class XV, class X>
	static bool search
	(	FUNC&& func, XV&& init_x, X const radius, X const epsilon, unsigned max_iteration
	,	XITR xitr_out
	)
	{
		return
		_search
		(	_Apply<NOF_ARGUMENT, decltype(func), X>( std::forward<FUNC>(func) )
		,	iterable_cast< Serial<X> >( std::forward<XV>(init_x) )
		,	radius, epsilon, max_iteration, xitr_out
		);
	}


};


#endif // end of #ifndef _SGM_NUM_GOLDEN_SECTION_METHOD_