/**	FP style 1st-class citizen callable object, sgm::fp::Functor
*/
#pragma once


#ifndef _SGM_FUNCTOR_
#define _SGM_FUNCTOR_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif

#include "..\idiom\idiom.hpp"
#include "Multiple.hpp"
#include "..\Pinweight\Pinweight.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm::fp
{
	
	template<unsigned D, class F>
	class Functor;
	

	struct Blank;


	template<unsigned D, class...ARGS>
	class Dimension;

	template<unsigned D>
	static inline auto const Dim = Dimension<D>();


	SGM_USER_DEFINED_TYPE_CHECK17
	(	_, Functor, SGM_MACROPACK(unsigned D, class T), SGM_MACROPACK(D, T) 
	);


	template<unsigned D, class F>
	class _Evaluator;


	struct _rPass_Helper;


	template<unsigned...INDICES>
	struct _Permute_Helper;

	
	template<class...ARGS>
	static auto Params(ARGS&&...);

}


template<unsigned _D, class _F>
static decltype(auto) constexpr operator/(_F&&, sgm::fp::Dimension<_D>);

template<unsigned _D, class _F, class..._ARGS>
static decltype(auto) constexpr operator/(_F&&, sgm::fp::Dimension<_D, _ARGS...>&&);
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


template<unsigned D, class...ARGS>
class sgm::fp::Dimension
{
public:
	Dimension(ARGS...args) : _mtp( static_cast<ARGS>(args)... ){}

	template<class..._ARGS>
	auto operator()(_ARGS&&..._args) const
	{
		return Dimension<D, decltype(_args)...>( static_cast<decltype(_args)>(_args)... );
	}


private:
	template<unsigned _D, class _F, class..._ARGS>
	friend decltype(auto) constexpr ::operator/(_F&&, Dimension<_D, _ARGS...>&&);


	Multiple<ARGS...> _mtp;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned _D, class _F> 
decltype(auto) constexpr operator/(_F&& f, sgm::fp::Dimension<_D>)
{
	return sgm::fp::Functor<_D, _F>( std::forward<_F>(f) );
}


template<unsigned _D, class _F, class..._ARGS>
decltype(auto) constexpr operator/(_F&& f, sgm::fp::Dimension<_D, _ARGS...>&& d)
{
	return ( std::forward<_F>(f) / sgm::fp::Dim<_D> )(d._mtp);
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
	(	[[maybe_unused]] Multiple<TYPES...>&& mtp, ARGS&&...args
	)	const
	{
		if constexpr(N == 0)
			return _push_front( std::forward<ARGS>(args)... );
		else
			return
			_cut_rear_helper<N - 1>
			(	std::move(mtp), mtp.template forward<N - 1>(), std::forward<ARGS>(args)...
			);
	}

	template<class...TYPES>
	decltype(auto) _cut_rear(TYPES&&...types) const
	{
		return
		_cut_rear_helper<sizeof...(TYPES) - 1>
		(	Forward_as_Multiple( std::forward<TYPES>(types)... )
		);
	}


	PWF const& _pwf;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned D, class F>
class sgm::fp::Functor
{
public:
	static auto constexpr DIMENSION = D;


	//	Evaluation
	template<class...ARGS>
	decltype(auto) operator()(ARGS&&...args) const
	{
		using eval_t = _Evaluator< D, constPinweight<F> >;

		if constexpr( Check_All<is_Multiple>::template for_any< std::decay_t<ARGS>... >::value )
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
	template<class FTR>
	decltype(auto) operator+(FTR&& ftr) const
	{
		static_assert(is_Functor_v<FTR>, "Functor type is needed after Functor::operator+");

		enum {D2 = ftr.DIMENSION};

		return
		[clone = *this, ftr](auto&&...args)
		{
			auto[mtp1, mtp2] = *Forward_as_2FMTP<D, D2>( std::forward<decltype(args)>(args)... );

			using res1_t = decltype( clone(mtp1) );
			using res2_t = decltype( ftr(mtp2) );

			return
			(	std::conditional_t< is_Multiple_v<res1_t>, res1_t, Multiple<res1_t> >
				(	clone(mtp1) 
				)
			+	std::conditional_t< is_Multiple_v<res2_t>, res2_t, Multiple<res2_t> >	
				(	ftr(mtp2)
				)
			);
		} / Dim<D + D2>;
	}


private:
	template<unsigned _D, class _F>
	friend decltype(auto) constexpr ::operator/(_F&&, Dimension<_D>);


	Functor(F&& f) noexcept : _pwf( std::move(f) ){}
	Functor(F const& f) : _pwf(f){}


	constPinweight<F> _pwf;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...ARGS>
auto sgm::fp::Params(ARGS&&...args)
{  
	return Forward_as_Flat_MTP( std::forward<ARGS>(args)... );  
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::fp::_rPass_Helper : No_Making
{
	template<class MTP, class...ARGS>
	static decltype(auto) calc([[maybe_unused]]MTP&& mtp, ARGS&&...args)
	{
		if constexpr( auto constexpr IDX = sizeof...(ARGS);  mtp.DIMENSION == IDX )
			return Forward_as_Multiple( std::forward<ARGS>(args)... );
		else
			return 
			calc
			(	std::forward<MTP>(mtp), mtp.template forward<IDX>(), std::forward<ARGS>(args)...  
			);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned IDX, unsigned...INDICES>
struct sgm::fp::_Permute_Helper<IDX, INDICES...>
{
	template<class...TYPES, class...ARGS>
	static decltype(auto) calc(Multiple<TYPES...>&& mtp, ARGS&&...args)
	{
		return 
		_Permute_Helper<INDICES...>::calc
		(	std::move(mtp), std::forward<ARGS>(args)..., mtp.template forward<IDX>()
		);
	}
};


template<>
struct sgm::fp::_Permute_Helper<>
{
	template<class...TYPES, class...ARGS>
	static decltype(auto) calc(Multiple<TYPES...>&&, ARGS&&...args)
	{
		return Params( std::forward<ARGS>(args)... );
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
				return Params( std::forward<decltype(args)>(args)... );
			else
				return _rPass_Helper::calc(  Params( std::forward<decltype(args)>(args)... )  );
		} / Dim<(D >= 0 ? D : -D)>;


	template<unsigned...INDICES>
	inline static auto const Permute
	=	[](auto&&...args)
		{
			return
			_Permute_Helper<INDICES...>::calc
			(	Params( std::forward<decltype(args)>(args)... )  
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