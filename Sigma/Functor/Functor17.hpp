/**	FP style 1st-class citizen callable object, sgm::fp::Functor
*/
#pragma once


#ifndef _SGM_FUNCTOR_
#define _SGM_FUNCTOR_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif


#include "Multiple.hpp"
#include "..\Avatar\Avatar.hpp"


namespace sgm::fp
{
	
	template<unsigned D, class F>  class Functor;

	struct Blank;

	template<unsigned D>  class Dimension;

	template<unsigned D>  
	static inline auto const Dim = Dimension<D>();

	template<unsigned D, class...ARGS>  struct _FunctorExecutant;


	template<   class T, class = Boolean_type<  is_Same< T, Decay_t<T> >::value  >   >  
	struct is_Functor;

	template<class...ARGS>  static auto constexpr is_Functor_v = is_Functor<ARGS...>::value;


	template<unsigned D, class refF>  class _Evaluator;

	struct _rPass_Helper;

	template<unsigned...INDICES>  struct _Permute_Helper;
	
	template<class...ARGS>  static auto Params(ARGS&&...);

	template<class T>  static auto Try_Refer(Referenceless_t<T>&)-> Avatar<T>;
	template<class T>  static auto Try_Refer(Referenceless_t<T>&&)-> Referenceless_t<T>;

}


template<unsigned _D, class _F>
static decltype(auto) constexpr operator/(_F&&, sgm::fp::Dimension<_D>);

template<unsigned _D, class _F, class..._ARGS>
static decltype(auto) constexpr operator/(_F&&, sgm::fp::_FunctorExecutant<_D, _ARGS...>&&);
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>  
struct sgm::fp::is_Functor<T, sgm::False_t> : sgm::fp::is_Functor< Decay_t<T> >{};

template<class T>  struct sgm::fp::is_Functor<T, sgm::True_t> : False_t{};

template<unsigned D, class T>  
struct sgm::fp::is_Functor< sgm::fp::Functor<D, T>, sgm::True_t > : True_t{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::fp::Blank
{
private:
	template<unsigned N, typename T, typename...TYPES>
	static unsigned constexpr _count_Blank()
	{
		if constexpr(is_Same_v< Blank, Decay_t<T> >)
			return _count_Blank<N + 1, TYPES...>();
		else
			return _count_Blank<N, TYPES...>();
	}

	template<unsigned N>
	static unsigned constexpr _count_Blank(){  return N;  }


public:
	template<class...ARGS>
	static bool constexpr has_front_blank_v
	=	is_Same_v<  Decay_t< First_t<ARGS...> >, Blank  >;

	template<class...ARGS>
	static bool constexpr has_rear_blank_v
	=	is_Same_v<  Decay_t< Last_t<ARGS...> >, Blank  >;


	template<class...ARGS>
	static bool constexpr is_well_used_v
	=	(	_count_Blank<0, ARGS...>() == 0 
		||	(	_count_Blank<0, ARGS...>() == 1 
			&&	(has_front_blank_v<ARGS...> || has_rear_blank_v<ARGS...>)
			)
		);
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned D>
class sgm::fp::Dimension
{
public:
	template<class...ARGS>
	auto operator()(ARGS&&...args) const
	->	_FunctorExecutant< D, Alephless_t<ARGS&&>... >{  return { Forward<ARGS>(args)... };  }
};


template<unsigned D, class...ARGS>
struct sgm::fp::_FunctorExecutant
{
private:
	template<unsigned _D, class _F, class..._ARGS>
	friend decltype(auto) constexpr ::operator/
	(	_F&&, sgm::fp::_FunctorExecutant<_D, _ARGS...>&&
	);

	template<unsigned _D>  friend class sgm::fp::Dimension;

	_FunctorExecutant(ARGS...args) : _mtp( static_cast<ARGS>(args)... ){}

	Multiple<ARGS...> _mtp;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned _D, class _F> 
decltype(auto) constexpr operator/(_F &&f, sgm::fp::Dimension<_D>)
{
	return sgm::fp::Functor< _D, sgm::Alephless_t<_F&&> >( sgm::Forward<_F>(f) );
}


template<unsigned _D, class _F, class..._ARGS>
decltype(auto) constexpr operator/(_F &&f, sgm::fp::_FunctorExecutant<_D, _ARGS...> &&d)
{
	return ( sgm::Forward<_F>(f) / sgm::fp::Dimension<_D>() )(d._mtp);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>  
auto sgm::fp::Try_Refer(Referenceless_t<T> &t)-> Avatar<T>{  return t;  }

template<class T>  
auto sgm::fp::Try_Refer(Referenceless_t<T> &&t)-> Referenceless_t<T>{  return Move(t);  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned D, class F>
class sgm::fp::_Evaluator
{
public:
	_Evaluator(F f) : _f( Forward<F>(f) ){}


	template<class...ARGS>
	decltype(auto) operator()(ARGS&&...args) const
	{
		static_assert(Blank::is_well_used_v<ARGS...>, "Blank is misused.");

		if constexpr(Blank::has_front_blank_v<ARGS...>)
			return _cut_front( Forward<ARGS>(args)... );
		else if constexpr(Blank::has_rear_blank_v<ARGS...>)
			return _cut_rear( Forward<ARGS>(args)... );
		else if constexpr( sizeof...(ARGS) == D )
			return _f( Forward<ARGS>(args)... );
	}


private:
	//	Partial evaluation from front side
	template<class...ARGS>
	decltype(auto) _push_front(ARGS&&...args) const
	{
		static_assert( D > sizeof...(ARGS), "function over estimation." );

		return
		[f = Try_Refer<F>(_f), mtp = Multiple< Alephless_t<ARGS&&>... >(args...)]
		(auto&&...params)
		{
			return
			Apply
			(	[&f, &params...](auto&&...unpacked)
				{
					return f( unpacked..., Forward<decltype(params)>(params)... );
				}
			,	mtp
			);
		} /	Dim<D - sizeof...(ARGS)>;
	}


	//	Partial evaluation from rear side
	template<class...ARGS>
	decltype(auto) _push_rear(ARGS&&...args) const
	{
		static_assert( D > sizeof...(ARGS), "function over estimation." );

		return
		[f = Try_Refer<F>(_f), mtp = Multiple< Alephless_t<ARGS&&>... >(args...)]
		(auto&&...params)
		{
			return
			Apply
			(	[&f, &params...](auto&&...unpacked)
				{
					return f( Forward<decltype(params)>(params)..., unpacked... );
				}
			,	mtp
			);
		} /	Dim<D - sizeof...(ARGS)>;
	}


	template<class T, class...TYPES>
	decltype(auto) _cut_front(T&&, TYPES&&...types) const
	{
		return _push_rear( Forward<TYPES>(types)... );
	}


	template<unsigned N, class...TYPES, class...ARGS>
	decltype(auto) _cut_rear_helper
	(	[[maybe_unused]] Multiple<TYPES...> &&mtp, ARGS&&...args
	)	const
	{
		if constexpr(N == 0)
			return _push_front( Forward<ARGS>(args)... );
		else
			return
			_cut_rear_helper<N - 1>
			(	Move(mtp), mtp.template forward<N - 1>(), Forward<ARGS>(args)...
			);
	}

	template<class...TYPES>
	decltype(auto) _cut_rear(TYPES&&...types) const
	{
		return
		_cut_rear_helper<sizeof...(TYPES) - 1>
		(	Forward_as_Multiple( Forward<TYPES>(types)... )
		);
	}


	F _f;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned D, class F>
class sgm::fp::Functor
{
public:
	static auto constexpr DIMENSION = D;


	template<class...ARGS>
	decltype(auto) operator()(ARGS&&...args) const&
	{
		return _invoke( *this, Forward<ARGS>(args)... );
	}

	template<class...ARGS>
	decltype(auto) operator()(ARGS&&...args) && noexcept
	{
		return _invoke( Move(*this), Forward<ARGS>(args)... );
	}


	template<class FTR>
	decltype(auto) operator|(FTR &&ftr) const&
	{
		return _Composition( *this, Forward<FTR>(ftr) );
	}

	template<class FTR>
	decltype(auto) operator|(FTR &&ftr) && noexcept
	{
		return _Composition( Move(*this), Forward<FTR>(ftr) );
	}


	template<class FTR>
	decltype(auto) operator+(FTR &&ftr) const&
	{
		return _Merging( *this, Forward<FTR>(ftr) );
	}

	template<class FTR>
	decltype(auto) operator+(FTR &&ftr) && noexcept
	{
		return _Merging( Move(*this), Forward<FTR>(ftr) );
	}


private:
	F _f;


	template<unsigned _D, class _F>
	friend decltype(auto) constexpr ::operator/(_F&&, Dimension<_D>);


	Functor(F f) : _f(f){}


	//	Evaluation
	template<class ME, class...ARGS>
	static decltype(auto) _invoke(ME &&me, ARGS&&...args)
	{
		using _carrying_t = CopiedRef_t<ME&&, F>;

		if constexpr
		(	_Evaluator<D, _carrying_t> evaluator = static_cast<_carrying_t>(me._f)
		;	(is_Multiple_v< Decay_t<ARGS> > || ...) 
		)
			return Apply(  Move(evaluator), Params( Forward<ARGS>(args)... )  );
		else
			return evaluator( Forward<ARGS>(args)... );
	}


	//	Functional composition
	template<class ME, class FTR>
	static decltype(auto) _Composition(ME &&me, FTR &&ftr)
	{
		static_assert(is_Functor_v<FTR>, "Functor type is needed after Functor::operator|");

		return
		[clone = Try_Refer<ME>(me), _ftr = Try_Refer<FTR>(ftr)](auto&&...args)
		{	
			return clone(  _ftr( Forward<decltype(args)>(args)... )  );
		} / Dim< Decay_t<FTR>::DIMENSION >;
	}


	//	Merging two Functor objects into one which returns multiple output
	template<class ME, class FTR>
	static decltype(auto) _Merging(ME &&me, FTR &&ftr)
	{
		static_assert(is_Functor_v<FTR>, "Functor type is needed after Functor::operator+");

		enum {D2 = ftr.DIMENSION};

		return
		[clone = Try_Refer<ME>(me), _ftr = Try_Refer<FTR>(ftr)](auto&&...args)
		{
			auto try_multiple_f
			=	[](auto &&q)-> decltype(auto)
				{
					using Q = decltype(q);

					if constexpr(is_Multiple_v<Q>)
						return Q(q);
					else
						return Make_Multiple( Q(q) );
				};
			
			auto[mtp1, mtp2] = *Forward_as_2FMTP<D, D2>( Forward<decltype(args)>(args)... );

			return try_multiple_f( clone(mtp1) ) + try_multiple_f( _ftr(mtp2) );
		} / Dim<D + D2>;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...ARGS>
auto sgm::fp::Params(ARGS&&...args)
{  
	return Forward_as_Flat_MTP( Forward<ARGS>(args)... );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::fp::_rPass_Helper : No_Making
{
	template<class MTP, class...ARGS>
	static decltype(auto) calc([[maybe_unused]]MTP &&mtp, ARGS&&...args)
	{
		if constexpr( auto constexpr IDX = sizeof...(ARGS);  mtp.DIMENSION == IDX )
			return Forward_as_Multiple( Forward<ARGS>(args)... );
		else
			return 
			calc( Forward<MTP>(mtp), mtp.template forward<IDX>(), Forward<ARGS>(args)... );
	}
};


template<unsigned IDX, unsigned...INDICES>
struct sgm::fp::_Permute_Helper<IDX, INDICES...>
{
	template<class...TYPES, class...ARGS>
	static decltype(auto) calc(Multiple<TYPES...> &&mtp, ARGS&&...args)
	{
		return 
		_Permute_Helper<INDICES...>::calc
		(	Move(mtp), Forward<ARGS>(args)..., mtp.template forward<IDX>()
		);
	}
};


template<>
struct sgm::fp::_Permute_Helper<>
{
	template<class...TYPES, class...ARGS>
	static decltype(auto) calc(Multiple<TYPES...>&&, ARGS&&...args)
	{
		return Params( Forward<ARGS>(args)... );
	}
};
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm::fp
{

	static sgm::fp::Blank constexpr __;


	template<signed D>
	inline static auto const Pass
	=	[](auto&&...args)
		{
			if constexpr(D >= 0)
				return Params( Forward<decltype(args)>(args)... );
			else
				return _rPass_Helper::calc(  Params( Forward<decltype(args)>(args)... )  );
		} / Dim<(D >= 0 ? D : -D)>;


	template<unsigned...INDICES>
	inline static auto const Permute
	=	[](auto&&...args)
		{
			return
			_Permute_Helper<INDICES...>::calc(  Params( Forward<decltype(args)>(args)... )  );
		} / Dim<sizeof...(INDICES)>;

}


#ifndef SGM_FUNCTOR
	/**	Macro function which converts template function into sgm::Functor

	*	You can use template-specializing signature < type1, type2, ... >
		as variadic parameters "..." 
	*/
	#define SGM_FUNCTOR(TemFunc, Dimension, ...)	\
	(	[](auto&&...args)		\
		{	\
			return TemFunc __VA_ARGS__ ( sgm::Forward<decltype(args)>(args)... );	\
		}	\
	/	sgm::fp::Dim<(Dimension)>	\
	)
#else
	#error macro SGM_FUNCTOR is already defined elsewhere.
#endif


#endif // end of #ifndef _SGM_FUNCTOR_