/**	FP style 1st-class citizen callable object, sgm::fp::Functor
*/
#pragma once


#ifndef _SGM_FUNCTOR_
#define _SGM_FUNCTOR_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif

#include <tuple>
#include "..\Pinweight\Pinweight.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm::fp
{

	template<unsigned D, class F>
	class Functor;
	

	struct Blank;


	template<class...TYPES> 
	struct Multiple;

	template<class T, class...TYPES>
	static decltype(auto) Params(T&& t, TYPES&&...params);


	template<unsigned D, class...ARGS>
	class Dimension;

	template<unsigned D>
	static auto const Dim = Dimension<D>();


	template<unsigned _D, class _F>
	static decltype(auto) constexpr operator/(_F&&, Dimension<_D>);

	template<unsigned _D, class _F, class..._ARGS>
	static decltype(auto) constexpr operator/(_F&&, Dimension<_D, _ARGS...>&&);
	//--------//--------//--------//--------//-------#//-------//--------//--------//--------//---


	template<class T>
	using remove_rvalue_reference_t
	=	std::conditional_t< std::is_rvalue_reference_v<T>, std::remove_reference_t<T>, T >;


	template<class TU1, class TU2, class...ARGS>
	static auto Merged_Tuple(TU1&& tu1, TU2&& tu2, ARGS&&...args);


	template<class> 
	struct _is_Multiple;

	template<class>
	struct _is_tuple;

	template<class MTP>
	static bool constexpr is_Multiple_v = _is_Multiple< std::decay_t<MTP> >::value;

	template<class TU>
	static bool constexpr is_tuple_v = _is_tuple< std::decay_t<TU> >::value;


	template<class T>
	static decltype(auto) to_Multiple(T&&);


	template<class>
	struct _is_Functor;

	template<class FTR>
	static bool constexpr is_Functor_v = _is_Functor< std::decay_t<FTR> >::value;


	template<unsigned D, class F>
	class _Evaluator;


	template<unsigned D>
	struct _Apply_Helper;

	template<class F, class MTP>
	static decltype(auto) Apply(F&& f, MTP&& mtp);


	template<unsigned NOF_PART, unsigned S, unsigned...SIZES>
	struct _Partition_Helper;

	template<unsigned...SIZES>
	struct _Partition;


	template<signed D>
	struct _rPass_Helper;


	template<unsigned...INDICES>
	struct _Permute_Helper;

}
//========//========//========//========//=======#//========//========//========//========//=======#


struct sgm::fp::Blank
{
private:
	template<unsigned N, typename T, typename...TYPES>
	static unsigned constexpr _count_Blank()
	{
		if constexpr(std::is_same_v< Blank, std::decay_t<T> >)
			return _count_Blank<N + 1, TYPES...>();
		else
			return _count_Blank<N, TYPES...>();
	}

	template<unsigned N>
	static unsigned constexpr _count_Blank(){  return N;  }


public:
	template<class...ARGS>
	static bool constexpr has_front_blank_v
	=	std::is_same_v<  std::decay_t< First_t<ARGS...> >, Blank  >;

	template<class...ARGS>
	static bool constexpr has_rear_blank_v
	=	std::is_same_v<  std::decay_t< Last_t<ARGS...> >, Blank  >;


	template<class...ARGS>
	static bool constexpr is_well_used_v
	=	(	_count_Blank<0, ARGS...>() == 0 
		||	(	_count_Blank<0, ARGS...>() == 1 
			&&	(has_front_blank_v<ARGS...> || has_rear_blank_v<ARGS...>)
			)
		);
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#



//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class TU1, class TU2, class...ARGS>
auto sgm::fp::Merged_Tuple(TU1&& tu1, TU2&& tu2, ARGS&&...args)
{
	using pure_tu1_t = std::decay_t<TU1>;
	using pure_tu2_t = std::decay_t<TU2>;

	if constexpr
	(	auto constexpr
			tu1_size = std::tuple_size_v<pure_tu1_t>,
			tu2_size = std::tuple_size_v<pure_tu2_t>,
			nof_args = sizeof...(args)
	;	nof_args == tu1_size + tu2_size
	)
		return std::tuple<decltype(args)...>( std::forward<ARGS>(args)... );
	else if constexpr(bool constexpr EXTRACTING_TU1 = nof_args >= tu2_size; EXTRACTING_TU1)
	{
		auto constexpr IDX = tu1_size + tu2_size - 1 - nof_args;
		using elem_t = std::tuple_element_t<IDX, pure_tu1_t>;

		return
		Merged_Tuple
		(	std::forward<TU1>(tu1), std::forward<TU2>(tu2)
		,	static_cast<elem_t&&>( std::get<IDX>(tu1) )
		,	std::forward<ARGS>(args)...
		);
	}
	else 
	{
		auto constexpr IDX = tu2_size - 1 - nof_args;
		using elem_t = std::tuple_element_t<IDX, pure_tu2_t>;

		return
		Merged_Tuple
		(	std::forward<TU1>(tu1), std::forward<TU2>(tu2)
		,	static_cast<elem_t&&>( std::get<IDX>(tu2) )
		,	std::forward<ARGS>(args)...
		);
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...TYPES>
struct sgm::fp::Multiple : public std::tuple<TYPES...>
{
	using tuple_t = std::tuple<TYPES...>;
	static auto constexpr DIMENSION = sizeof...(TYPES);

	Multiple(tuple_t& tu) : tuple_t( std::move(tu) ){}
	Multiple(tuple_t&& tu) noexcept : tuple_t( std::move(tu) ){}
	Multiple(TYPES...types) : tuple_t( std::forward<TYPES>(types)... ){}
	


	decltype(auto) operator*() const{  return static_cast<tuple_t const&>(*this);  }
	decltype(auto) operator*(){  return static_cast<tuple_t&>(*this);  }


	template<  class MTP, class = std::enable_if_t< is_Multiple_v<MTP> >  >
	auto operator+(MTP&& mtp){  return to_Multiple( Merged_Tuple(**this, *mtp) );  }


};


template<class>
struct sgm::fp::_is_Multiple : std::false_type, No_Making{};

template<class...TYPES>
struct sgm::fp::_is_Multiple< sgm::fp::Multiple<TYPES...> > : std::true_type, No_Making{};


template<class>
struct sgm::fp::_is_tuple : std::false_type, No_Making{};

template<class...TYPES>
struct sgm::fp::_is_tuple< std::tuple<TYPES...> > : std::true_type, No_Making{};


template<class T>
decltype(auto) sgm::fp::to_Multiple(T&& t)
{
	if constexpr(is_Multiple_v<T>)
		return std::forward<T>(t);
	else if constexpr(is_tuple_v<T>)
		return Multiple( std::forward<T>(t) );
	else
		return Multiple( std::forward<T>(t) );
}


template<class T, class...TYPES>
decltype(auto) sgm::fp::Params(T&& t, TYPES&&...types)
{
	if constexpr( sizeof...(TYPES) > 0 )
		return Params( std::forward<T>(t) ) + Params( std::forward<TYPES>(types)... );
	else 
		return to_Multiple( std::forward<T>(t) );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned D, class...ARGS>
class sgm::fp::Dimension
{
public:
	Dimension(ARGS...args) : _mtp(  std::tuple<ARGS...>( static_cast<ARGS>(args)... )  ){}

	template<class..._ARGS>
	auto operator()(_ARGS&&..._args) const
	{
		return Dimension<D, decltype(_args)...>( static_cast<decltype(_args)>(_args)... );
	}


private:
	template<unsigned _D, class _F, class..._ARGS>
	friend decltype(auto) constexpr sgm::fp::operator/(_F&&, Dimension<_D, _ARGS...>&&);


	Multiple<ARGS...> _mtp;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned _D, class _F> 
decltype(auto) constexpr sgm::fp::operator/(_F&& f, Dimension<_D>)
{
	return Functor<_D, _F>( std::forward<_F>(f) );
}


template<unsigned _D, class _F, class..._ARGS>
decltype(auto) constexpr sgm::fp::operator/(_F&& f, Dimension<_D, _ARGS...>&& d)
{
	return ( std::forward<_F>(f) / Dim<_D> )(d._mtp);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned D, class PWF>
class sgm::fp::_Evaluator
{
public:
	_Evaluator(PWF const& pwf) : _pwf(pwf){}


	template<class...ARGS>
	decltype(auto) operator()(ARGS&&...args) const
	{
		static_assert(Blank::is_well_used_v<ARGS...>, "Blank is misused.");

		if constexpr(Blank::has_front_blank_v<ARGS...>)
			return _cut_front( std::forward<ARGS>(args)... );
		else if constexpr(Blank::has_rear_blank_v<ARGS...>)
			return _cut_rear( std::forward<ARGS>(args)... );
		else if constexpr( sizeof...(ARGS) == D )
			return _pwf.value()( std::forward<ARGS>(args)... );
	}


private:
	//	Partial evaluation from front side
	template<class...ARGS>
	decltype(auto) _push_front(ARGS&&...args) const
	{
		static_assert( D > sizeof...(ARGS), "function over estimation." );

		return
		[cpy_pwf = _pwf, args...](auto&&...params)
		{
			return cpy_pwf.value()( args..., std::forward<decltype(params)>(params)... );
		} / Dim<D - sizeof...(ARGS)>;
	}


	//	Partial evaluation from rear side
	template<class...ARGS>
	decltype(auto) _push_rear(ARGS&&...args) const
	{
		static_assert( D > sizeof...(ARGS), "function over estimation." );

		return
		[cpy_pwf = _pwf, args...](auto&&...params)
		{
			return cpy_pwf.value()( std::forward<decltype(params)>(params)..., args... );
		} / Dim<D - sizeof...(ARGS)>;
	}


	template<class T, class...TYPES>
	decltype(auto) _cut_front(T&&, TYPES&&...types) const
	{
		return _push_rear( std::forward<TYPES>(types)... );
	}


	template<unsigned N, class...TYPES, class...ARGS>
	decltype(auto) _cut_rear_helper
	(	[[maybe_unused]] std::tuple<TYPES...>&& tu, ARGS&&...args
	)	const
	{
		if constexpr(N == 0)
			return _push_front( std::forward<ARGS>(args)... );
		else
			return
			_cut_rear_helper<N - 1>
			(	std::move(tu)
			,	static_cast< Nth_t<N - 1, TYPES...> >( std::get<N - 1>(tu) )
			,	std::forward<ARGS>(args)...
			);
	}

	template<class...TYPES>
	decltype(auto) _cut_rear(TYPES&&...types) const
	{
		return
		_cut_rear_helper<sizeof...(TYPES) - 1>
		(	std::forward_as_tuple( std::forward<TYPES>(types)... )
		);
	}


	PWF const& _pwf;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned...SIZES>
struct sgm::fp::_Partition : No_Making
{
	template<class...ARGS>
	static decltype(auto) constexpr calc(ARGS&&...args)
	{
		return
		_Partition_Helper<sizeof...(SIZES), SIZES...>::calc
		(	std::tuple(), std::tuple(), std::forward<ARGS>(args)...
		);
	}
};


template<unsigned NOF_PART, unsigned S, unsigned...SIZES>
struct sgm::fp::_Partition_Helper : No_Making
{
	template<class TTU, class TU, class A, class...ARGS>
	static decltype(auto) constexpr calc(TTU&& ttu, TU&& tu, A&& a, ARGS&&...args)
	{
		if constexpr(S == 0)
			return 
			_Partition_Helper<NOF_PART - 1, SIZES...>::calc
			(	Merged_Tuple(  std::move(ttu), std::make_tuple( std::move(tu) )  )
			,	std::tuple(), std::forward<A>(a), std::forward<ARGS>(args)...
			);
		else
			return
			_Partition_Helper<NOF_PART, S - 1, SIZES...>::calc
			(	std::move(ttu)
			,	Merged_Tuple(  std::move(tu), std::forward_as_tuple( std::forward<A>(a) )  )
			,	std::forward<ARGS>(args)...
			);
	}
};


template<>
struct sgm::fp::_Partition_Helper<1, 0> : No_Making
{
	template<class TTU, class TU>
	static decltype(auto) constexpr calc(TTU&& ttu, TU&& tu)
	{
		return Merged_Tuple(  std::move(ttu), std::make_tuple( std::move(tu) )  );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class F, class MTP>
decltype(auto) sgm::fp::Apply(F&& f, MTP&& mtp)
{
	return
	_Apply_Helper< std::decay_t<MTP>::DIMENSION >::calc
	(	std::forward<F>(f), std::forward<MTP>(mtp) 
	);
}


template<unsigned D>
struct sgm::fp::_Apply_Helper : No_Making
{
	template<class F, class MTP, class...ARGS>
	static decltype(auto) constexpr calc(F&& f, [[maybe_unused]]MTP&& mtp, ARGS&&...args)
	{
		if constexpr(D == 0)
			return f( std::forward<ARGS>(args)... );
		else
			return
			_Apply_Helper<D - 1>::calc
			(	std::forward<F>(f), std::forward<MTP>(mtp)
			,	std::tuple_element_t< D - 1, std::decay_t<decltype(*mtp)> > 
				(	std::get<D - 1>(mtp)
				)
			,	std::forward<ARGS>(args)...
			);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned D, class F>
class sgm::fp::Functor
{
public:
	static auto constexpr DIMENSION = D;


	//	Evaluation
	template<class...ARGS>
	auto operator()(ARGS&&...args) const
	{
		using eval_t = _Evaluator< D, constPinweight<F> >;

		if constexpr( Check_All<_is_Multiple>::template for_any< std::decay_t<ARGS>... >::value )
			return Apply(  eval_t(_pwf), Params( std::forward<ARGS>(args)... )  );
		else
			return eval_t(_pwf)( std::forward<ARGS>(args)... );
	}


	//	Functional composition
	template<class FTR>
	decltype(auto) operator|(FTR&& ftr) const
	{
		static_assert(is_Functor_v<FTR>, "Functor type is needed after Functor::operator|");

		return
		[clone = *this, ftr](auto&&...args)
		{	
			return clone(  ftr( std::forward<decltype(args)>(args)... )  );
		} / Dim< std::decay_t<FTR>::DIMENSION >;
	}


	//	Merging two Functor objects into one which returns multiple output
	template<typename FTR>
	decltype(auto) operator+(FTR&& ftr) const
	{
		static_assert(is_Functor_v<FTR>, "Functor type is needed after Functor::operator+");

		return
		[clone = *this, ftr](auto&&...args)
		{
			auto [tu1, tu2]
			=	_Partition< D, std::decay_t<FTR>::DIMENSION >::calc
				(	std::forward<decltype(args)>(args)... 
				);

			return Params(  clone( Params(tu1) )  ) + Params(  ftr( Params(tu2) )  );
		} / Dim< D + std::decay_t<FTR>::DIMENSION >;
	}


private:
	template<unsigned _D, class _F>
	friend decltype(auto) constexpr sgm::fp::operator/(_F&&, Dimension<_D>);


	Functor(F&& f) noexcept : _pwf( std::move(f) ){}
	Functor(F const& f) : _pwf(f){}


	constPinweight<F> _pwf;
};


template<class>
struct sgm::fp::_is_Functor : std::false_type, No_Making{};

template<unsigned D, class F>
struct sgm::fp::_is_Functor< sgm::fp::Functor<D, F> > : std::true_type, No_Making{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<signed D>
struct sgm::fp::_rPass_Helper : No_Making
{
	template<class TU, class...ARGS>
	static decltype(auto) calc([[maybe_unused]] TU&& tu, ARGS&&...args)
	{
		if constexpr(D == 0)
			return
			Params
			(	std::tuple< remove_rvalue_reference_t<decltype(args)>... > 
				(	static_cast< remove_rvalue_reference_t<decltype(args)> >(args)...
				)
			);
		else
		{
			size_t constexpr IDX = std::tuple_size_v< std::decay_t<TU> > - D;
			using elem_t = std::tuple_element_t< IDX, std::decay_t<TU> >;

			return
			_rPass_Helper<D - 1>::calc
			(	std::move(tu), static_cast<elem_t>( std::get<IDX>(tu) )
			,	std::forward<ARGS>(args)...
			);
		}
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned IDX, unsigned...INDICES>
struct sgm::fp::_Permute_Helper<IDX, INDICES...>
{
	template<class...TYPES, class...ARGS>
	static decltype(auto) calc(std::tuple<TYPES...>&& src_tu, ARGS&&...args)
	{
		return 
		_Permute_Helper<INDICES...>::calc
		(	std::move(src_tu)
		,	std::forward<ARGS>(args)...
		,	static_cast< Nth_t<IDX, TYPES...> >( std::get<IDX>(src_tu) )
		);
	}
};


template<>
struct sgm::fp::_Permute_Helper<>
{
	template<class...TYPES, class...ARGS>
	static decltype(auto) calc(std::tuple<TYPES...>&&, ARGS&&...args)
	{
		return Params( std::forward<ARGS>(args)... );
	}
};
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm::fp
{

	static Blank constexpr __;


	template<signed D>
	inline static auto const Pass
	=	[](auto&&...args)
		{
			if constexpr(D >= 0)
				return 
				Params
				(	std::tuple< remove_rvalue_reference_t<decltype(args)>... > 
					(	static_cast< remove_rvalue_reference_t<decltype(args)> >(args)...
					)
				);
			else
				return
				_rPass_Helper<-D>::calc
				(	std::tuple<decltype(args)...>( std::forward<decltype(args)>(args)... )
				);
		} / Dim<(D >= 0 ? D : -D)>;


	template<unsigned...INDICES>
	inline static auto const Permute
	=	[](auto&&...args)
		{
			return
			_Permute_Helper<INDICES...>::calc
			(	std::forward_as_tuple( std::forward<decltype(args)>(args)... )
			);
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
			return TemFunc __VA_ARGS__ ( std::forward<decltype(args)>(args)... );	\
		}	\
	/	sgm::fp::Dim<(Dimension)>	\
	)
#else
	#error macro SGM_FUNCTOR is already defined elsewhere.
#endif


#endif // end of #ifndef _SGM_FUNCTOR_