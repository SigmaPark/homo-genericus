#ifndef _SGM_F18_FUNCTOR_
#define _SGM_F18_FUNCTOR_

#include "Multiple.hpp"


namespace sgm::f18
{
	
	using fp::Multiple;

	//struct _Multiple_Helper;

	struct Blank;

	template<class closure_t, class invoker_t>  struct FnExpr;

	struct _FXP_Helper;

	struct As_Functor;


	template<class hFXP>  class Functor;

	struct _Functor_Helper;


	template<unsigned... INDICES>  struct indexer;

	struct _indexer_Helper;


	template<class mtp_t = Multiple<>, unsigned... INDICES>  struct Closure;

	struct _Closure_Helper;

}


template<class FN>
static decltype(auto) constexpr operator/(FN&& fn, sgm::f18::As_Functor) 
noexcept(sgm::is_RvalueReference<FN&&>::value);
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


struct sgm::f18::_Functor_Helper : No_Making
{
	template<   class FTR, class = Boolean_type<  is_Same_v< FTR, Decay_t<FTR> >  >   >  struct is_Functor;

	template<class T>  struct is_Functor<T, False_t> : is_Functor< Decay_t<T> >{};
	template<class T>  struct is_Functor<T, True_t> : False_t{};
	template<class FXP>  struct is_Functor< Functor<FXP>, True_t > : True_t{};
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<unsigned... INDICES>
struct sgm::f18::indexer
{
	constexpr indexer() = default;

	static size_t constexpr nof_indices_v = sizeof...(INDICES);
};


struct sgm::f18::_indexer_Helper : No_Making
{
	static unsigned constexpr INVALID_N = 0xffff'ffffUL;


	template<unsigned QN, class idxr_t, unsigned = 0>  struct _Place_Helper;

	template<unsigned QN, unsigned NTH>
	struct _Place_Helper<QN, indexer<>, NTH>{  static unsigned constexpr value = INVALID_N;  };

	template<unsigned QN, unsigned NTH, unsigned IDX, unsigned... INDICES>
	struct _Place_Helper< QN, indexer<IDX, INDICES...>, NTH >
	{
		static unsigned constexpr value
		=	QN == IDX  ?  NTH  :  _Place_Helper< QN, indexer<INDICES...>, NTH+1 >::value;
	};


	template<   class T, class = Boolean_type<  is_Same_v< T, Decay_t<T> >  >   >  struct is_indexer;

	template<class T>  struct is_indexer<T, False_t> : is_indexer< Decay_t<T> >{};
	template<class T>  struct is_indexer<T, True_t> : False_t{};
	template<unsigned... INDICES>  struct is_indexer< indexer<INDICES...>, True_t > : True_t{};

};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class mtp_t, unsigned... INDICES>
struct sgm::f18::Closure
{
public:
	static auto constexpr indices = indexer<INDICES...>{};

	mtp_t mtp;


	Closure() = default;

	template<class INDEXER, class MTP>
	Closure(INDEXER, MTP&& mtp_) : mtp( Forward<MTP>(mtp_) ){}


	template<unsigned QN>
	static bool constexpr has_idx_v 
	=	_indexer_Helper::template _Place_Helper< QN, indexer<INDICES...> >::value != _indexer_Helper::INVALID_N;


	template<unsigned QN>
	auto get() const&
	{
		unsigned constexpr NTH = _indexer_Helper::template _Place_Helper< QN, indexer<INDICES...> >::value;

		static_assert(NTH != _indexer_Helper::INVALID_N);

		return mtp.template get<NTH>();	
	}

	template<unsigned QN>
	auto get() &
	{
		unsigned constexpr NTH = _indexer_Helper::template _Place_Helper< QN, indexer<INDICES...> >::value;

		static_assert(NTH != _indexer_Helper::INVALID_N);

		return mtp.template get<NTH>();
	}

	template<unsigned QN>
	auto get() &&
	{
		unsigned constexpr NTH = _indexer_Helper::template _Place_Helper< QN, indexer<INDICES...> >::value;

		static_assert(NTH != _indexer_Helper::INVALID_N);

		return mtp.template forward<NTH>();
	}
};


namespace sgm::f18
{
	template<class FAM, unsigned... INDICES>
	Closure(indexer<INDICES...>, FAM&&)-> Closure< Alephless_t<FAM>, INDICES... >;
}


struct sgm::f18::_Closure_Helper : No_Making
{
private:
	template<class...>  /* Declaration Only */
	static auto constexpr _is_closure(...)-> False_t;

	template<class... TYPES, unsigned... INDICES>  /* Declaration Only */
	static auto constexpr _is_closure(Closure< Multiple<TYPES...>, INDICES... >)-> True_t;

public:
	template<class Q>
	static bool constexpr is_Closure_v = decltype( _is_closure(Declval< Decay_t<Q> >()) )::value;

	template<class Q>
	struct is_Closure : Boolean_type< is_Closure_v<Q> >{};
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


struct sgm::f18::_FXP_Helper : No_Making
{
	template<   class T, class = Boolean_type<  is_Same_v< T, Decay_t<T> >  >   >  struct is_FnExpr;

	template<class T>  struct is_FnExpr<T, False_t> : is_FnExpr< Decay_t<T> >{};
	template<class T>  struct is_FnExpr<T, True_t> : False_t{};
	
	template<class _closure_t, class _invoker_t>
	struct is_FnExpr< FnExpr<_closure_t, _invoker_t>, True_t > : True_t{};

	template<class T>
	static bool constexpr is_FnExpr_v = is_FnExpr<T>::value;


	template<  class T, class = Enable_if_t< !fp::is_Multiple_v<T> >  >
	static decltype(auto) harden(T&& t) noexcept(is_RvalueReference<T&&>::value)
	{
		if constexpr(is_FnExpr_v<T>)
		{
			using closure_t = decltype(t.closure);
			using invoker_t = decltype(t.invoker);

			return
			FnExpr< decltype( harden(Declval<closure_t>()) ), decltype( harden(Declval<invoker_t>()) ) >
			{	harden( Move(t.closure) ), harden( Move(t.invoker) )
			};
		}
		else if constexpr(_Closure_Helper::is_Closure_v<T>)
			return Closure(  t.indices, harden( CopiedRef_t<T&&, decltype(t.mtp)>(t.mtp) )  );
		else
			return Alephless_t<T>(t);
	}

	template<  class MTP, class... ARGS, class = Enable_if_t< fp::is_Multiple_v<MTP> >  >
	static decltype(auto) harden(MTP&& mtp, ARGS&&... args)
	noexcept( is_RvalueReference<MTP&&>::value || (is_RvalueReference<ARGS&&>::value || ...) )
	{
		size_t constexpr nof_max_args = Decay_t<MTP>::DIMENSION,  nof_args = sizeof...(ARGS);

		if constexpr(nof_max_args == 0)
			return Multiple<>{};
		else if constexpr(nof_args == nof_max_args)
			return 
			Multiple< decltype(  harden( Forward<ARGS>(args) )  )... >(  harden( Forward<ARGS>(args) )...  );
		else
			return harden( Forward<MTP>(mtp), Forward<ARGS>(args)..., mtp.template forward<nof_args>() );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


struct sgm::f18::Blank
{
	constexpr Blank() = default;

	template<class T>
	static bool constexpr is_Blank_v = is_Same_v< Decay_t<T>, Blank >;

	template<class... TYPES>
	static bool constexpr has_Blank_v = Has_Type<Blank>::template among< Decay_t<TYPES>... >::value; 
};

namespace sgm::f18
{
	static Blank constexpr _{};
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


struct sgm::f18::As_Functor
{
	constexpr As_Functor() = default;
};

namespace sgm::f18
{
	static As_Functor constexpr as_functor{};
}


template<class FN>
decltype(auto) constexpr operator/(FN&& fn, sgm::f18::As_Functor const) noexcept(sgm::is_RvalueReference<FN&&>::value)
{
	using namespace sgm::f18;

	if constexpr(_FXP_Helper::is_FnExpr_v<FN>)
		return sgm::Move(fn);
	else
		return FnExpr< Closure<sgm::fp::Multiple<>>, FN&& >{Closure{}, sgm::Forward<FN>(fn)};
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


//struct sgm::f18::_Multiple_Helper
//{
//	template<class... TYPES, class... ARGS>
//	static decltype(auto) _next(Multiple<TYPES...>&& mtp, ARGS&&... args)
//	{
//		size_t constexpr nof_types = sizeof...(TYPES);
//
//		static_assert(nof_types != 0);
//
//		if constexpr(nof_types == 1)
//			return Multiple<>{};
//		else if constexpr( auto constexpr IDX = sizeof...(ARGS) + 1;  nof_types == IDX )
//			return Multiple<ARGS&&...>{Forward<ARGS>(args)...};
//		else
//			return _next( Move(mtp), Forward<ARGS>(args)..., mtp.template forward<IDX>() );
//	}	
//};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class closure_t, class invoker_t>
struct sgm::f18::FnExpr
{
public:
	closure_t closure;
	invoker_t invoker;


	template<class... ARGS>
	decltype(auto) operator()(ARGS&&... args) && noexcept
	{
		return _invoke(  Move(*this), fp::Forward_as_Multiple( Forward<ARGS>(args)... )  );
	}

private:
	template<class hFXP>
	friend class sgm::f18::Functor;


	template<class... ARGS>
	decltype(auto) operator()(ARGS&&... args) const&
	{
		return _invoke(  *this, fp::Forward_as_Multiple( Forward<ARGS>(args)... )  );
	}

	template<class... ARGS>
	decltype(auto) operator()(ARGS&&... args) &
	{
		return _invoke(  *this, fp::Forward_as_Multiple( Forward<ARGS>(args)... )  );
	}


	template<class ME, class... TYPES>
	static decltype(auto) _invoke(ME&& me, Multiple<TYPES...>&& inputs)
	{
		if constexpr(Blank::has_Blank_v<TYPES...>)
			return _partial_eval( Forward<ME>(me), Move(inputs) );
		else
			return _evaluate( Forward<ME>(me), Move(inputs) );
	}


	template<unsigned INPUT_IDX = 0, class ME, class... TYPES, class... ARGS>
	static decltype(auto) _evaluate(ME&& me, Multiple<TYPES...>&& inputs, ARGS&&... args)
	{
		using _closure_t = decltype(me.closure);

		size_t constexpr
			nof_closure = _closure_t::indices.nof_indices_v,
			nof_inputs = sizeof...(TYPES),
			nof_args = sizeof...(ARGS),
			max_nof_args = nof_closure + nof_inputs;

		if constexpr(max_nof_args == nof_args)
			return me.invoker( Forward<ARGS>(args)... );
		else if constexpr(_closure_t::template has_idx_v<nof_args>)
		{
			auto&& closure_elem = _closure_t::template get<nof_args>();

			return 
			_evaluate<INPUT_IDX>
			(	Forward<ME>(me), Move(inputs), Forward<ARGS>(args)...
			,	Move_if< is_RvalueReference<ME&&>::value >::cast(closure_elem)
			);
		}
		else
		{
			auto&& input_elem = inputs.template forward<INPUT_IDX>();

			return
			_evaluate<INPUT_IDX+1>
			(	Forward<ME>(me), Move(inputs), Forward<ARGS>(args)...
			,	static_cast<decltype(input_elem)>(input_elem)
			);
		}
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class hFXP>
class sgm::f18::Functor
{
public:
	template<class _closure_t, class _invoker_t>
	Functor(FnExpr<_closure_t, _invoker_t>&& fxp) : _hfxp(  _FXP_Helper::harden( Move(fxp) )  ){}

	template<  class FN, class = Enable_if_t< !_FXP_Helper::is_FnExpr_v<FN> >  >
	Functor(FN&& fn) : Functor( Forward<FN>(fn) / as_functor ){}


	template<class... ARGS>
	decltype(auto) operator()(ARGS&&... args) const
	{
		return _hfxp( Forward<ARGS>(args)... );
	}

	template<class... ARGS>
	decltype(auto) operator()(ARGS&&... args)
	{
		return _hfxp( Forward<ARGS>(args)... );
	}


private:
	hFXP _hfxp;
};


namespace sgm::f18
{
	template<class F>
	Functor(F&&)
	->	Functor
		<	Selective_t
			<	_FXP_Helper::is_FnExpr_v<F>
			,	decltype( _FXP_Helper::harden(Declval<F>()) )
			,	FnExpr< Closure<>, Alephless_t<F> >
			>
		>;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------




#endif