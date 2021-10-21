#ifndef _SGM_F18_FUNCTOR_
#define _SGM_F18_FUNCTOR_

#include "..\interface_Traits\interface_Traits.hpp"
#include "Multiple.hpp"

namespace sgm::fp
{
	
	struct Blank;

	template<class closure_t, class invoker_t, unsigned MIN_D = 0xffff'ffffUL>  struct FnExpr;

	struct _FXP_Helper;

	struct identical_Functor;


	template<class hFXP>  class Functor;

	struct _Functor_Helper;


	template<unsigned... INDICES>  struct indexer;

	struct _indexer_Helper;


	template<class mtp_t = Multiple<>, unsigned... INDICES>  struct Closure;

	struct _Closure_Helper;

	struct _Evaluation;

	struct _Composition;

	template<class... FS>
	inline static decltype(auto) Compose(FS&&... fs);


	template<unsigned... INDICES>  struct _Permute_Helper;

	template<size_t IDX, class... ARGS>
	inline static decltype(auto) _Try_nth_elem(ARGS&&... args) noexcept;
}


template<class FN>
static decltype(auto) constexpr operator*(FN&& fn, sgm::fp::identical_Functor) 
noexcept(sgm::is_RvalueReference<FN&&>::value);


template<class FN, class... TYPES>
static decltype(auto) operator*(FN&& fn, sgm::fp::Multiple<TYPES...>&& mtp) noexcept;
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


struct sgm::fp::_Functor_Helper : No_Making
{
	template<   class FTR, class = Boolean_type<  is_Same_v< FTR, Decay_t<FTR> >  >   >  struct is_Functor;

	template<class T>  struct is_Functor<T, False_t> : is_Functor< Decay_t<T> >{};
	template<class T>  struct is_Functor<T, True_t> : False_t{};
	template<class FXP>  struct is_Functor< Functor<FXP>, True_t > : True_t{};
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<unsigned... INDICES>
struct sgm::fp::indexer
{
	constexpr indexer() = default;

	static size_t constexpr nof_indices_v = sizeof...(INDICES);

	template<unsigned... _INDICES>
	auto operator+(indexer<_INDICES...>) const noexcept-> indexer<INDICES..., _INDICES...>{  return {};  }
};


struct sgm::fp::_indexer_Helper : No_Making
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
struct sgm::fp::Closure
{
public:
	static auto constexpr indices = indexer<INDICES...>{};

	mtp_t mtp;


	Closure() = default;

	template<class INDEXER, class MTP>
	Closure(INDEXER, MTP&& mtp_) noexcept(is_RvalueReference<MTP&&>::value) : mtp( Forward<MTP>(mtp_) ){}


	template<unsigned QN>
	static bool constexpr has_idx_v 
	=	_indexer_Helper::template _Place_Helper< QN, indexer<INDICES...> >::value != _indexer_Helper::INVALID_N;


	template<unsigned QN>
	decltype(auto) get() const&
	{
		unsigned constexpr NTH = _indexer_Helper::template _Place_Helper< QN, indexer<INDICES...> >::value;

		static_assert(NTH != _indexer_Helper::INVALID_N);

		return mtp.template forward<NTH>();	
	}

	template<unsigned QN>
	decltype(auto) get() &
	{
		unsigned constexpr NTH = _indexer_Helper::template _Place_Helper< QN, indexer<INDICES...> >::value;

		static_assert(NTH != _indexer_Helper::INVALID_N);

		return mtp.template forward<NTH>();
	}

	template<unsigned QN>
	decltype(auto) get() && noexcept
	{
		unsigned constexpr NTH = _indexer_Helper::template _Place_Helper< QN, indexer<INDICES...> >::value;

		static_assert(NTH != _indexer_Helper::INVALID_N);

		return mtp.template forward<NTH>();
	}
};


namespace sgm::fp
{
	template<class MTP, unsigned... INDICES>
	Closure(indexer<INDICES...>, MTP&&)-> Closure< Alephless_t<MTP>, INDICES... >;
}


struct sgm::fp::_Closure_Helper : No_Making
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


struct sgm::fp::_FXP_Helper : No_Making
{
	template<   class T, class = Boolean_type<  is_Same_v< T, Decay_t<T> >  >   >  struct is_FnExpr;

	template<class T>  struct is_FnExpr<T, False_t> : is_FnExpr< Decay_t<T> >{};
	template<class T>  struct is_FnExpr<T, True_t> : False_t{};
	
	template<class _closure_t, class _invoker_t, unsigned MIN_D>
	struct is_FnExpr< FnExpr<_closure_t, _invoker_t, MIN_D>, True_t > : True_t{};

	template<class T>  static bool constexpr is_FnExpr_v = is_FnExpr<T>::value;


	template<  class T, class = Enable_if_t< !is_Multiple_v<T> >  >
	static decltype(auto) harden(T&& t) noexcept(is_RvalueReference<T&&>::value)
	{
		if constexpr(is_FnExpr_v<T>)
		{
			using closure_t = decltype(t.closure);
			using invoker_t = decltype(t.invoker);
			unsigned constexpr MIN_D = t.MINIMAL_DIMENSION;

			return
			FnExpr< decltype( harden(Declval<closure_t>()) ), decltype( harden(Declval<invoker_t>()) ), MIN_D >
			{	harden( Forward<closure_t>(t.closure) ), harden( Forward<invoker_t>(t.invoker) )
			};
		}
		else if constexpr(_Closure_Helper::is_Closure_v<T>)
			return Closure(  t.indices, harden( CopiedRef_t<T&&, decltype(t.mtp)>(t.mtp) )  );
		else
			return Alephless_t<T>(t);
	}

	template<  class MTP, class... ARGS, class = Enable_if_t< is_Multiple_v<MTP> >  >
	static decltype(auto) harden(MTP&& mtp, ARGS&&... args) noexcept(Aleph_Check<MTP&&, ARGS&&...>::value)
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


struct sgm::fp::Blank
{
	constexpr Blank() = default;

	template<class T>
	static bool constexpr is_Blank_v = is_Same_v< Decay_t<T>, Blank >;

	template<class... TYPES>
	static bool constexpr has_Blank_v = Has_Type<Blank>::template among< Decay_t<TYPES>... >::value; 
};

namespace sgm::fp
{
	static Blank constexpr _{};
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


struct sgm::fp::identical_Functor
{
	constexpr identical_Functor() = default;

	static auto constexpr identity
	=	[](auto&& a, auto&&... args) constexpr
		->	Selective_t< sizeof...(args) != 0, Multiple<decltype(a), decltype(args)...>, decltype(a) >
		{
			return {Forward<decltype(a)>(a), Forward<decltype(args)>(args)...};
		};

	using identical_FnExpr_t = FnExpr<Closure<>, decltype(identity)>;
};


template<class FN>
decltype(auto) constexpr operator*(FN&& fn, sgm::fp::identical_Functor) noexcept(sgm::is_RvalueReference<FN&&>::value)
{
	using namespace sgm::fp;

	if constexpr(_FXP_Helper::is_FnExpr_v<FN> || _Functor_Helper::is_Functor<FN>::value)
		return sgm::Forward<FN>(fn);
	else
		return FnExpr< Closure<>, sgm::Alephless_t<FN&&> >( Closure{}, sgm::Forward<FN>(fn) );
}


template<class FN, class... TYPES>
decltype(auto) operator*(FN&& fn, sgm::fp::Multiple<TYPES...>&& mtp) noexcept
{
	return sgm::fp::Apply( sgm::Forward<FN>(fn)*sgm::fp::identical_Functor{}, sgm::Move(mtp) );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


struct sgm::fp::_Evaluation : No_Making
{
private:
	template<class closure_t, class invoker_t, unsigned MIN_D>  friend struct sgm::fp::FnExpr;


	template
	<	unsigned NTH = 0, unsigned NOF_BLANK = 0, class NEW_INDEXER = indexer<>, class ME, class... TYPES
	,	class... ARGS
	>
	static decltype(auto) partial_eval(ME&& me, Multiple<TYPES...>&& inputs, ARGS&&... args) noexcept
	{
		using _closure_t = decltype(me.closure);

		size_t constexpr nof_closure = _closure_t::indices.nof_indices_v,  input_idx = sizeof...(ARGS) + NOF_BLANK;

		if constexpr( NTH == nof_closure + sizeof...(TYPES) )
		{
			Closure merged_closure
			(	_closure_t::indices + NEW_INDEXER{}
			,	me.closure.mtp + Multiple<ARGS&&...>( Forward<ARGS>(args)... )
			);

			return
			FnExpr< decltype(merged_closure), CopiedRef_t<ME&&, decltype(me.invoker)>, NOF_BLANK >
			(	Move(merged_closure), CopiedRef_t<ME&&, decltype(me.invoker)>(me.invoker)
			);
		}
		else if constexpr(_closure_t::template has_idx_v<NTH>)
			return
			partial_eval<NTH+1, NOF_BLANK, NEW_INDEXER>( Forward<ME>(me), Move(inputs), Forward<ARGS>(args)... );
		else if constexpr(Blank::is_Blank_v< Nth_t<input_idx, TYPES...> >)
			return
			partial_eval<NTH+1, NOF_BLANK+1, NEW_INDEXER>( Forward<ME>(me), Move(inputs), Forward<ARGS>(args)... );
		else 
			return
			partial_eval<NTH+1, NOF_BLANK, decltype(NEW_INDEXER{} + indexer<NTH>{})>
			(	Forward<ME>(me), Move(inputs), Forward<ARGS>(args)..., inputs.template forward<input_idx>()
			);
	}


	template<unsigned INPUT_IDX = 0, class ME, class... TYPES, class... ARGS>
	static decltype(auto) evaluate(ME&& me, Multiple<TYPES...>&& inputs, ARGS&&... args) noexcept
	{
		using _closure_t = decltype(me.closure);

		size_t constexpr nof_closure = _closure_t::indices.nof_indices_v,  nof_args = sizeof...(ARGS);

		if constexpr( nof_closure + sizeof...(TYPES) == nof_args )
			return me.invoker( Forward<ARGS>(args)... );
		else if constexpr(_closure_t::template has_idx_v<nof_args>)
			return 
			evaluate<INPUT_IDX>
			(	Forward<ME>(me), Move(inputs), Forward<ARGS>(args)...
			,	Move_if< is_RvalueReference<ME&&>::value >::cast(me.closure.template get<nof_args>())
			);
		else
			return
			evaluate<INPUT_IDX+1>
			(	Forward<ME>(me), Move(inputs), Forward<ARGS>(args)...
			,	inputs.template forward<INPUT_IDX>()
			);	
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


struct sgm::fp::_Composition : No_Making
{
private:
	template<class closure_t, class invoker_t, unsigned MIN_D>  friend struct sgm::fp::FnExpr;

	static auto constexpr func
	=	[](auto&& fn2, auto&& fn1, auto&&... args)-> decltype(auto)
		{
			return Forward<decltype(fn2)>(fn2)(  Forward<decltype(fn1)>(fn1)( Forward<decltype(args)>(args)... )  );
		};
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class closure_t, class invoker_t, unsigned MIN_D>
struct sgm::fp::FnExpr
{
public:
	static unsigned constexpr MINIMAL_DIMENSION = MIN_D;

	closure_t closure;
	invoker_t invoker;
	

	template<class _closure_t, class _invoker_t>
	FnExpr(_closure_t&& clsr, _invoker_t&& ivk) noexcept(Aleph_Check<_closure_t&&, _invoker_t&&>::value)
	:	closure( Forward<_closure_t>(clsr) ), invoker( Forward<_invoker_t>(ivk) ){}


	auto operator=(FnExpr const&)-> FnExpr& = delete;


	template<class... ARGS>
	decltype(auto) operator()(ARGS&&... args) && noexcept
	{
		return _invoke(  Move(*this), Forward_as_Flat_MTP( Forward<ARGS>(args)... )  );
	}

	template<class... ARGS>
	decltype(auto) operator()(ARGS&&... args) const& noexcept( (is_RvalueReference<ARGS&&>::value ||...) )
	{
		return _invoke(  *this, Forward_as_Flat_MTP( Forward<ARGS>(args)... )  );
	}

	template<class... ARGS>
	decltype(auto) operator()(ARGS&&... args) & noexcept( (is_RvalueReference<ARGS&&>::value ||...) )
	{
		return _invoke(  *this, Forward_as_Flat_MTP( Forward<ARGS>(args)... )  );
	}


	template<  class FN, class = Enable_if_t< !is_Same_v< Decay_t<FN>, identical_Functor > >  >
	decltype(auto) operator*(FN&& fn) && noexcept
	{
		return _composite( Move(*this), Forward<FN>(fn) );
	}

	template<  class FN, class = Enable_if_t< !is_Same_v< Decay_t<FN>, identical_Functor > >  >
	decltype(auto) operator*(FN&& fn) & noexcept(is_RvalueReference<FN&&>::value)
	{
		return _composite( *this, Forward<FN>(fn) );
	}

	template<  class FN, class = Enable_if_t< !is_Same_v< Decay_t<FN>, identical_Functor > >  >
	decltype(auto) operator*(FN&& fn) const& noexcept(is_RvalueReference<FN&&>::value)
	{
		return _composite( *this, Forward<FN>(fn) );
	}


	auto eval() && noexcept
	{
		Functor res = Move(*this);

		return res;
	}


private:
	template<class ME, class... TYPES>
	static decltype(auto) _invoke(ME&& me, Multiple<TYPES...>&& inputs) noexcept
	{
		if constexpr(Blank::has_Blank_v<TYPES...>)
			return _Evaluation::partial_eval( Forward<ME>(me), Move(inputs) );
		else
			return _Evaluation::evaluate( Forward<ME>(me), Move(inputs) );
	}


	template<class ME, class FN>
	static decltype(auto) _composite(ME&& me, FN&& fn) noexcept(Aleph_Check<ME&&, FN&&>::value)
	{
		auto&& fxp = Forward<FN>(fn) * identical_Functor{};
		using fxp_t = decltype(fxp);
		auto constexpr D = fxp.MINIMAL_DIMENSION;

		Closure c
		(	indexer<0, 1>{}
		,	Multiple<ME&&, fxp_t>( Forward<ME>(me), Forward<fxp_t>(fxp) )
		);

		return FnExpr<decltype(c), decltype(_Composition::func), D>( Move(c), _Composition::func );
	}
};


namespace sgm::fp
{
	template<class closure_t, class invoker_t>
	FnExpr(closure_t&&, invoker_t&&)-> FnExpr< Alephless_t<closure_t>, invoker_t&& >;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class hFXP>
class sgm::fp::Functor : public Operator_interface<hFXP>
{
public:
	using closure_t = decltype(Declval<hFXP>().closure);
	using invoker_t = decltype(Declval<hFXP>().invoker);
	static unsigned constexpr MINIMAL_DIMENSION = Decay_t<hFXP>::MINIMAL_DIMENSION;


	template<class _closure_t, class _invoker_t, unsigned D>
	Functor(FnExpr<_closure_t, _invoker_t, D>&& fxp) noexcept
	:	_hfxp(  _FXP_Helper::harden( Move(fxp) )  ){  static_cast< Operator_interface<hFXP>& >(*this) = &_hfxp;  }

	template
	<	class FN, class = Enable_if_t<  !_FXP_Helper::is_FnExpr_v<FN> && !is_Same_v< Decay_t<FN>, Functor >  >  
	>
	Functor(FN&& fn) noexcept(is_RvalueReference<FN&&>::value) : Functor( Forward<FN>(fn) * identical_Functor{} ){}

	Functor(Functor const &rhs) : _hfxp(rhs._hfxp){  static_cast< Operator_interface<hFXP>& >(*this) = &_hfxp;  }

	Functor(Functor&& rhs) noexcept 
	:	_hfxp( Move(rhs._hfxp) ){  static_cast< Operator_interface<hFXP>& >(*this) = &_hfxp;  }


	auto operator=(Functor const&)-> Functor& = delete;

private:
	hFXP _hfxp;
};


template<>
class sgm::fp::Functor<typename sgm::fp::identical_Functor::identical_FnExpr_t> : public identical_Functor
{
public:
	template<class...ARGS>
	decltype(auto) operator()(ARGS&&... args) const
	{
		return identical_Functor::identity( Forward<ARGS>(args)... );
	}


	template<   class FN, class = Enable_if_t<  !is_Same_v< Decay_t<FN>, identical_Functor >  >   >
	decltype(auto) operator*(FN&& fn) const{  return Forward<FN>(fn) * identical_Functor{};  }

	auto operator=(Functor const&)-> Functor& = delete;
};


namespace sgm::fp
{
	Functor()-> Functor<typename identical_Functor::identical_FnExpr_t>;

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


template<unsigned IDX, unsigned...INDICES>
struct sgm::fp::_Permute_Helper<IDX, INDICES...>
{
	template<class...TYPES, class...ARGS>
	static decltype(auto) calc(Multiple<TYPES...> &&mtp, ARGS&&...args) noexcept
	{
		return _Permute_Helper<INDICES...>::calc( Move(mtp), Forward<ARGS>(args)..., mtp.template forward<IDX>() );
	}
};


template<>
struct sgm::fp::_Permute_Helper<>
{
	template<class...TYPES, class...ARGS>
	static decltype(auto) calc(Multiple<TYPES...>&&, ARGS&&...args) noexcept
	{
		return Forward_as_Flat_MTP( Forward<ARGS>(args)... );
	}
};


namespace sgm::fp
{
	template<unsigned... INDICES>
	inline static Functor const Permute 
	=	[](auto&&... args)-> decltype(auto)
		{
			return _Permute_Helper<INDICES...>::calc(  Forward_as_Flat_MTP( Forward<decltype(args)>(args)... )  );
		};
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#include "..\High_Templar\High_Templar.hpp"


#ifndef SGM_1st_Class_Citizen
	#define SGM_1st_Class_Citizen(NAME, ...)	\
		sgm::fp::Functor	\
		{	[](auto&&... args)-> decltype(auto){  return NAME __VA_ARGS__ ( decltype(args)(args)... );  }	\
		}

#else
	#error SGM_1st_Class_Citizen was already defined somewhere else.
#endif


template<size_t IDX, class... ARGS>
decltype(auto) sgm::fp::_Try_nth_elem([[maybe_unused]]ARGS&&... args) noexcept
{
	if constexpr( IDX < sizeof...(ARGS) )
		return sgm::Nth_elem<IDX>(args...);
	else
		return sgm::none;
}

#ifndef SGM_LAMBDA
	#define SGM_LAMBDA(...)		\
	sgm::fp::Functor	\
	{	[&](auto&&... args)-> decltype(auto)	\
		{	\
			[[maybe_unused]]auto&& _0 = sgm::fp::_Try_nth_elem<0>(args...);	\
			[[maybe_unused]]auto&& _1 = sgm::fp::_Try_nth_elem<1>(args...);	\
			[[maybe_unused]]auto&& _2 = sgm::fp::_Try_nth_elem<2>(args...);	\
			[[maybe_unused]]auto&& _3 = sgm::fp::_Try_nth_elem<3>(args...);	\
			[[maybe_unused]]auto&& _4 = sgm::fp::_Try_nth_elem<4>(args...);	\
			[[maybe_unused]]auto&& _5 = sgm::fp::_Try_nth_elem<5>(args...);	\
			[[maybe_unused]]auto&& _6 = sgm::fp::_Try_nth_elem<6>(args...);	\
			[[maybe_unused]]auto&& _7 = sgm::fp::_Try_nth_elem<7>(args...);	\
			[[maybe_unused]]auto&& _8 = sgm::fp::_Try_nth_elem<8>(args...);	\
			[[maybe_unused]]auto&& _9 = sgm::fp::_Try_nth_elem<9>(args...);	\
			\
			return __VA_ARGS__;	\
		}	\
	}
	
#else
	#error SGM_LAMBDA was already defined somewhere else.
#endif


template<class... FS>
decltype(auto) sgm::fp::Compose(FS&&... fs)
{
	return ( Functor{} * ... * Forward<FS>(fs) );
}


namespace sgm::fp
{

	inline static Functor const integers = [](auto... args){  return Countable<>(args...);  };


#ifndef _SGM_FP_HIGH_TEMPLAR
	#define _SGM_FP_HIGH_TEMPLAR(NAME)	\
		inline static Functor const NAME = SGM_1st_Class_Citizen(ht::NAME) 

	_SGM_FP_HIGH_TEMPLAR(Morph);
	_SGM_FP_HIGH_TEMPLAR(Filter);
	_SGM_FP_HIGH_TEMPLAR(Fold);
	_SGM_FP_HIGH_TEMPLAR(rFold);
	_SGM_FP_HIGH_TEMPLAR(Plait);
	_SGM_FP_HIGH_TEMPLAR(cPlait);

	#undef _SGM_FP_HIGH_TEMPLAR
#else
	#error _SGM_FP_HIGH_TEMPLAR was already defined somewhere else.
#endif


	template<class... FLAGS>
	inline static Functor const Fold_par
	=	[](auto&&... args)-> decltype(auto){  return ht::Fold<FLAGS...>( Forward<decltype(args)>(args)... );  };

	template<class... FLAGS>
	inline static Functor const rFold_par
	=	[](auto&&... args)-> decltype(auto){  return ht::rFold<FLAGS...>( Forward<decltype(args)>(args)... );  };

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#endif