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


		template<Extreme XT>
		class GoldenSection;


		template< Extreme XT, class LINE_SEARCHER = GoldenSection<XT> >
		class Powell;
	}
}


template<class X, class Y>
struct sgm::num::XY_Pair{  X x;  Y y;  };


#include "..\interface_Traits\interface_Traits17.hpp"
#include "..\Serial\Serial.hpp"


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
	,	class Y = std::invoke_result_t<FUNC, X>
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
				(	auto [sitr, uitr] = Dual_iteration(s.begin(), u.begin()) 
				;	sitr != s.end()
				;	*sitr++ += gamma * *uitr++ 
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
		for
		(	auto[itr1, itr2] = Dual_iteration(x1.begin(), x2.begin())
		;	itr1 != x1.end()
		;	*itr1++ -= *itr2++
		);

		return x1;
	}


	template<size_t N, class X>
	struct _Apply_Helper
	{
		template<class FUNC, class...ARGS>
		static decltype(auto) calc(FUNC&& func, Serial<X> const& vec, ARGS const&...args)
		{
			return _Apply_Helper<N-1, X>::calc(func, vec, vec[N-1], args...);
		}
	};

	template<class X>
	struct _Apply_Helper<0, X>
	{
		template<class FUNC, class...ARGS>
		static decltype(auto) calc(FUNC&& func, Serial<X> const&, ARGS const&...args) 
		{
			return func(args...);
		}
		
	};


	template<size_t N, class FUNC, class X>
	class _Apply
	{
	private:
		FUNC const& _func;

	public:
		_Apply(FUNC const& func) : _func(func){}

		auto operator=(_Apply const&)-> _Apply& = delete;

		decltype(auto) operator()(Serial<X> const& vec) const 
		{
			return _Apply_Helper<N, X>::calc(_func, vec);
		}
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
			(	auto[xi_in, xi_out] = Dual_iteration(x.begin(), xitr)
			;	xi_in != x.end() || lastly( yout = func(x) )
			;	*xi_out++ = *xi_in++
			);

		return success;
	}


	template<class FUNC, class XV, class X, class XITR>
	static bool _search
	(	FUNC&& func, XV&& init_x, X const radius, X const epsilon, unsigned const max_iteration
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
	template
	<	size_t NOF_ARGUMENT = ULLONG_MAX
	,	class FUNC, class XITR, class XV, class X
	,	class Y = None_t const
	>
	static bool search
	(	FUNC&& func, XV&& init_x, X const radius, X const epsilon, unsigned const max_iteration
	,	XITR xitr_out, [[maybe_unused]]Y& yout = None
	)
	{
		auto constexpr NOF_ARGS 
		=	NOF_ARGUMENT == ULLONG_MAX ? nof_Arguments_v<FUNC, X> : NOF_ARGUMENT;	

		if constexpr(is_None_v< std::decay_t<Y> >)
			return
			_search
			(	_Apply<NOF_ARGS, decltype(func), X>( std::forward<FUNC>(func) )
			,	iterable_cast< Serial<X> >( std::forward<XV>(init_x) )
			,	radius, epsilon, max_iteration, xitr_out
			);			
		else
			return
			_search
			(	_Apply<NOF_ARGS, decltype(func), X>( std::forward<FUNC>(func) )
			,	iterable_cast< Serial<X> >( std::forward<XV>(init_x) )
			,	radius, epsilon, max_iteration, xitr_out, yout
			);
	}


};


#endif // end of #ifndef _SGM_NUM_GOLDEN_SECTION_METHOD_