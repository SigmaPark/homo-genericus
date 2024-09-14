/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_FP_FUNCTOR_
#define _SGM_FP_FUNCTOR_


#include "Parameters.hpp"
#include "SGM/Operators_of/Operators_of.hpp"
#include "SGM/High_Templar/Countable.hpp"
#include "SGM/High_Templar/High_Templar.hpp"


namespace sgm::fp
{

	template<class hFXP>  
	class Functor;


	SGM_USER_DEFINED_TYPE_CHECK
	(	class hFXP
	,	Functor, <hFXP>
	);

	
	struct Blank;


	template< class CLSR, class NVK, unsigned MIN_D = std::numeric_limits<unsigned>::max() >  
	struct _FuncExpr;


	SGM_USER_DEFINED_TYPE_CHECK
	(	SGM_MACROPACK(class CLSR, class NVK, unsigned MIN_D)
	,	_FuncExpr, <CLSR, NVK, MIN_D>
	);

	struct _FXP_Harden;


	template<unsigned...INDICES>  
	struct _indexer;


	template<class PPK = Param_Pack<>, unsigned...INDICES>
	struct _Closure;


	SGM_USER_DEFINED_TYPE_CHECK
	(	SGM_MACROPACK(class PPK, unsigned...INDICES)
	,	_Closure, <PPK, INDICES...>
	);


	struct _Evaluation;

	template<unsigned...INDICES>  
	struct _Permute_Helper;

}
//========//========//========//========//=======#//========//========//========//========//=======#


struct sgm::fp::Blank
{
	constexpr Blank() = default;

	template<class T>
	static bool constexpr is_Blank_v = is_Same< Decay_t<T>, Blank >::value;

	template<class...TYPES>
	static bool constexpr has_Blank_v 
	=	Has_Type<Blank>::template among< Decay_t<TYPES>... >::value; 
};


namespace sgm::fp
{

	static Blank constexpr _{};

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned...INDICES>
struct sgm::fp::_indexer
{
	constexpr _indexer() = default;

	static size_t constexpr nof_indices_v = sizeof...(INDICES);

	template<unsigned..._INDICES>
	auto operator+(_indexer<_INDICES...>) const noexcept
	->	_indexer<INDICES..., _INDICES...>{  return {};  }
};


namespace sgm::fp::_indexer_detail
{

	static unsigned constexpr INVALID_N = std::numeric_limits<unsigned>::max();


	template<unsigned QN, class idxr_t, unsigned = 0>  
	struct Nth;

	template<unsigned QN, unsigned NTH>
	struct Nth<QN, _indexer<>, NTH> : As_value_itself<unsigned, INVALID_N>{};

	template<unsigned QN, unsigned NTH, unsigned IDX, unsigned...INDICES>
	struct Nth< QN, _indexer<IDX, INDICES...>, NTH >
	:	Selective_t
		<	QN == IDX
		,	As_value_itself<unsigned, NTH>
		,	Nth< QN, _indexer<INDICES...>, NTH+1 >
		>
	{};

};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class PPK, unsigned...INDICES>
struct sgm::fp::_Closure
{
	static auto constexpr indices = _indexer<INDICES...>{};

	PPK param_pack;


	_Closure() = default;

	template<class INDEXER, class PPK_>
	_Closure(INDEXER, PPK_&& param_pack_) noexcept(is_Rvalue_Reference<PPK_&&>::value) 
	:	param_pack( Forward<PPK_>(param_pack_) ){}


	template<unsigned QN>
	static bool constexpr has_idx_v 
	=	(	_indexer_detail::Nth< QN, _indexer<INDICES...> >::value 
		!=	_indexer_detail::INVALID_N
		);


	template<unsigned QN>
	decltype(auto) get() const&
	{
		unsigned constexpr NTH = _indexer_detail::Nth< QN, _indexer<INDICES...> >::value;

		static_assert(NTH != _indexer_detail::INVALID_N);

		return param_pack.template forward<NTH>();
	}

	template<unsigned QN>
	decltype(auto) get() &
	{
		unsigned constexpr NTH = _indexer_detail::Nth< QN, _indexer<INDICES...> >::value;

		static_assert(NTH != _indexer_detail::INVALID_N);

		return param_pack.template forward<NTH>();
	}

	template<unsigned QN>
	decltype(auto) get() const&& noexcept
	{
		unsigned constexpr NTH = _indexer_detail::Nth< QN, _indexer<INDICES...> >::value;

		static_assert(NTH != _indexer_detail::INVALID_N);

		return Move(param_pack).template forward<NTH>();
	}

	template<unsigned QN>
	decltype(auto) get() && noexcept
	{
		unsigned constexpr NTH = _indexer_detail::Nth< QN, _indexer<INDICES...> >::value;

		static_assert(NTH != _indexer_detail::INVALID_N);

		return Move(param_pack).template forward<NTH>();
	}
};


namespace sgm::fp
{

	template<class PPK_, unsigned...INDICES>
	_Closure(_indexer<INDICES...>, PPK_&&)-> _Closure< Alephless_t<PPK_>, INDICES... >;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::fp::_FXP_Harden : Unconstructible
{
	template<  class T, class = Enable_if_t< !is_Param_Pack<T>::value >  >
	static decltype(auto) calc(T&& t) noexcept(is_Rvalue_Reference<T&&>::value)
	{
		if constexpr(is__FuncExpr<T>::value)
		{
			using closure_t = decltype(t.closure);
			using invoker_t = decltype(t.invoker);
			unsigned constexpr MIN_D = Decay_t<T>::MINIMAL_DIMENSION;

			return
			_FuncExpr
			<	decltype( calc(Mock<closure_t>()) )
			,	decltype( calc(Mock<invoker_t>()) ), MIN_D 
			>
			{	calc( Forward<closure_t>(t.closure) )
			,	calc( Forward<invoker_t>(t.invoker) )
			};
		}
		else if constexpr(is__Closure<T>::value)
			return 
			_Closure
			(	t.indices, calc( Qualify_Like_t<T&&, decltype(t.param_pack)>(t.param_pack) )  
			);
		else
			return Alephless_t<T>(t);
	}

	template<  class PPK, class...ARGS, class = Enable_if_t< is_Param_Pack<PPK>::value >  >
	static decltype(auto) calc(PPK&& ppk, ARGS&&...args) 
	noexcept(Aleph_Check<PPK&&, ARGS&&...>::value)
	{
		size_t constexpr 
			nof_max_args = std::tuple_size< typename Decay_t<PPK>::fam_t >::value,
			nof_args = sizeof...(ARGS);

		if constexpr(nof_max_args == 0)
			return Param_Pack<>{};
		else if constexpr(nof_args == nof_max_args)
			return 
			Param_Pack< decltype(  calc( Forward<ARGS>(args) )  )... >
			(	calc( Forward<ARGS>(args) )...  
			);
		else
			return 
			calc( Forward<PPK>(ppk), Forward<ARGS>(args)..., ppk.template forward<nof_args>() );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm::fp::_identity_Functor_detail
{

	enum class Tag{};


	static auto constexpr identity
	=	[](auto&& a, auto&&...args) constexpr
		->	Selective_t
		<	sizeof...(args) != 0, Param_Pack<decltype(a), decltype(args)...>, decltype(a) 
		>
		{
			return {Forward<decltype(a)>(a), Forward<decltype(args)>(args)...};
		};


	using identical_FuncExpr_t = _FuncExpr<_Closure<>, decltype(identity)>;

}


template<class FUNC>
decltype(auto) constexpr operator*(FUNC&& func, sgm::fp::_identity_Functor_detail::Tag)	
noexcept(sgm::is_Rvalue_Reference<FUNC&&>::value)
{
	using namespace sgm::fp;

	if constexpr(is__FuncExpr<FUNC>::value || is_Functor<FUNC>::value)
		return sgm::Forward<FUNC>(func);
	else
		return 
		_FuncExpr< _Closure<>, sgm::Alephless_t<FUNC&&> >
		(	_Closure{}, sgm::Forward<FUNC>(func) 
		);
}


template<class FUNC, class...TYPES>
decltype(auto) operator*(FUNC&& func, sgm::fp::Param_Pack<TYPES...>&& ppk) noexcept
{
	return 
	sgm::fp::Apply_Params
	(	sgm::Forward<FUNC>(func) * sgm::fp::_identity_Functor_detail::Tag{}
	,	sgm::Move(ppk) 
	);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::fp::_Evaluation : Unconstructible
{
private:
	template<class CLSR, class NVK, unsigned MIN_D>  
	friend struct sgm::fp::_FuncExpr;


	template
	<	unsigned IDX = 0, unsigned NOF_BLANK = 0, class INDEXER = _indexer<>
	,	class ME, class...TYPES, class...ARGS
	>
	static decltype(auto) partial_eval
	(	ME&& me, [[maybe_unused]] Param_Pack<TYPES...>&& inputs, ARGS&&...args
	)	noexcept
	{
		using _closure_t = decltype(me.closure);

		size_t constexpr 
			nof_closure_v = _closure_t::indices.nof_indices_v,  
			input_idx_v = sizeof...(ARGS) + NOF_BLANK;

		if constexpr( IDX == nof_closure_v + sizeof...(TYPES) )
		{
			_Closure merged_closure
			(	_closure_t::indices + INDEXER{}
			,	Merge_Params
				(	me.closure.param_pack, Param_Pack<ARGS&&...>( Forward<ARGS>(args)... )
				)
			);

			return
			_FuncExpr
			<	decltype(merged_closure), Qualify_Like_t<ME&&, decltype(me.invoker)>
			,	NOF_BLANK 
			>
			(	Move(merged_closure), Qualify_Like_t<ME&&, decltype(me.invoker)>(me.invoker)
			);
		}
		else if constexpr(_closure_t::template has_idx_v<IDX>)
			return
			partial_eval<IDX+1, NOF_BLANK, INDEXER>
			(	Forward<ME>(me), Move(inputs), Forward<ARGS>(args)... 
			);
		else if constexpr(Blank::is_Blank_v< Nth_t<input_idx_v, TYPES...> >)
			return
			partial_eval<IDX+1, NOF_BLANK+1, INDEXER>
			(	Forward<ME>(me), Move(inputs), Forward<ARGS>(args)... 
			);
		else 
			return
			partial_eval<IDX+1, NOF_BLANK, decltype(INDEXER{} + _indexer<IDX>{})>
			(	Forward<ME>(me), Move(inputs), Forward<ARGS>(args)...
			,	inputs.template forward<input_idx_v>()
			);
	}


	template<unsigned INPUT_IDX = 0, class ME, class...TYPES, class...ARGS>
	static decltype(auto) 
	evaluate(ME&& me, [[maybe_unused]] Param_Pack<TYPES...>&& inputs, ARGS&&...args) noexcept
	{
		using _closure_t = decltype(me.closure);

		size_t constexpr 
			nof_closure_v = _closure_t::indices.nof_indices_v,
			nof_args_v = sizeof...(ARGS);

		if constexpr( nof_closure_v + sizeof...(TYPES) == nof_args_v )
			return me.invoker( Forward<ARGS>(args)... );
		else if constexpr(_closure_t::template has_idx_v<nof_args_v>)
			return 
			evaluate<INPUT_IDX>
			(	Forward<ME>(me), Move(inputs), Forward<ARGS>(args)...
			,	Move_if< is_Rvalue_Reference<ME&&>::value >
				(	me.closure.template get<nof_args_v>()
				)
			);
		else
			return
			evaluate<INPUT_IDX+1>
			(	Forward<ME>(me), Move(inputs), Forward<ARGS>(args)...
			,	inputs.template forward<INPUT_IDX>()
			);	
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm::fp::_Composition_detail
{
	
	static auto constexpr func
	=	[](auto&& func2, auto&& func1, auto&&...args)-> decltype(auto)
		{
			return 
			Forward<decltype(func2)>(func2)
			(	Forward<decltype(func1)>(func1)( Forward<decltype(args)>(args)... )  
			);
		};

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class CLSR, class NVK, unsigned MIN_D>
struct sgm::fp::_FuncExpr
{
public:
	static unsigned constexpr MINIMAL_DIMENSION = MIN_D;

	CLSR closure;
	NVK invoker;
	

	template<class _CLSR, class _NVK>
	_FuncExpr(_CLSR&& clsr, _NVK&& ivk) 
	noexcept(Aleph_Check<_CLSR&&, _NVK&&>::value)
	:	closure( Forward<_CLSR>(clsr) ), invoker( Forward<_NVK>(ivk) ){}


	auto operator=(_FuncExpr const&)-> _FuncExpr& = delete;


	template<class...ARGS>
	decltype(auto) operator()(ARGS&&...args) && noexcept
	{
		return _invoke(  Move(*this), Parameters( Forward<ARGS>(args)... )  );
	}

	template<class...ARGS>
	decltype(auto) operator()(ARGS&&...args) const&& noexcept
	{
		return _invoke(  Move(*this), Parameters( Forward<ARGS>(args)... )  );
	}

	template<class...ARGS>
	decltype(auto) operator()(ARGS&&...args) const& noexcept(Aleph_Check<ARGS&&...>::value)
	{
		return _invoke(  *this, Parameters( Forward<ARGS>(args)... )  );
	}

	template<class...ARGS>
	decltype(auto) operator()(ARGS&&...args) & noexcept(Aleph_Check<ARGS&&...>::value)
	{
		return _invoke(  *this, Parameters( Forward<ARGS>(args)... )  );
	}


	template
	<	class FUNC
	,	class = Enable_if_t<  !is_Same< Decay_t<FUNC>, _identity_Functor_detail::Tag >::value  >  
	>
	decltype(auto) operator*([[maybe_unused]] FUNC&& func) && noexcept
	{
		if constexpr
		(	is_Same
			<	FUNC&&, Functor<typename _identity_Functor_detail::identical_FuncExpr_t>&& 
			>::	value
		)
			return Functor( Move(*this) );
		else
			return _composite( Move(*this), Forward<FUNC>(func) );
	}

	template
	<	class FUNC
	,	class = Enable_if_t<  !is_Same< Decay_t<FUNC>, _identity_Functor_detail::Tag >::value  >  
	>
	decltype(auto) operator*(FUNC&& func) const&& noexcept
	{
		return _composite( Move(*this), Forward<FUNC>(func) );
	}

	template
	<	class FUNC
	,	class = Enable_if_t<  !is_Same< Decay_t<FUNC>, _identity_Functor_detail::Tag >::value  >
	>
	decltype(auto) operator*(FUNC&& func) & noexcept(is_Rvalue_Reference<FUNC&&>::value)
	{
		return _composite( *this, Forward<FUNC>(func) );
	}

	template
	<	class FUNC
	,	class = Enable_if_t<  !is_Same< Decay_t<FUNC>, _identity_Functor_detail::Tag >::value  >
	>
	decltype(auto) operator*(FUNC&& func) const& noexcept(is_Rvalue_Reference<FUNC&&>::value)
	{
		return _composite( *this, Forward<FUNC>(func) );
	}


	auto eval() && noexcept
	{
		Functor res = Move(*this);

		return res;
	}


private:
	template<class ME, class...TYPES>
	static decltype(auto) _invoke(ME&& me, Param_Pack<TYPES...>&& inputs) noexcept
	{
		if constexpr(Blank::has_Blank_v<TYPES...>)
			return _Evaluation::partial_eval( Forward<ME>(me), Move(inputs) );
		else
			return _Evaluation::evaluate( Forward<ME>(me), Move(inputs) );
	}


	template<class ME, class FUNC>
	static decltype(auto) _composite(ME&& me, FUNC&& func) 
	noexcept(Aleph_Check<ME&&, FUNC&&>::value)
	{
		auto&& fxp = Forward<FUNC>(func) * _identity_Functor_detail::Tag{};
		using fxp_t = decltype(fxp);
		auto constexpr D = Decay_t<fxp_t>::MINIMAL_DIMENSION;

		_Closure c
		(	_indexer<0, 1>{}
		,	Param_Pack<ME&&, fxp_t>( Forward<ME>(me), Forward<fxp_t>(fxp) )
		);

		return 
		_FuncExpr<decltype(c), decltype(_Composition_detail::func), D>
		(	Move(c), _Composition_detail::func 
		);
	}
};


namespace sgm::fp
{

	template<class CLSR, class NVK>
	_FuncExpr(CLSR&&, NVK&&)-> _FuncExpr< Alephless_t<CLSR>, NVK&& >;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class hFXP>
class sgm::fp::Functor : public Operators_of<hFXP>
{
public:
	using closure_t = decltype(Mock<hFXP>().closure);
	using invoker_t = decltype(Mock<hFXP>().invoker);
	static unsigned constexpr MINIMAL_DIMENSION = Decay_t<hFXP>::MINIMAL_DIMENSION;


	template<class CLSR, class NVK, unsigned D>
	Functor(_FuncExpr<CLSR, NVK, D>&& fxp) noexcept
	:	_hfxp(  _FXP_Harden::calc( Move(fxp) )  )
	{  
		Operators_of<hFXP>::_p = Address_of(_hfxp);
	}

	template
	<	class FUNC
	,	class 
		=	Enable_if_t
			<	!is__FuncExpr<FUNC>::value && !is_Same< Decay_t<FUNC>, Functor >::value  
			>  
	>
	Functor(FUNC&& func) noexcept(is_Rvalue_Reference<FUNC&&>::value) 
	:	Functor( Forward<FUNC>(func) * _identity_Functor_detail::Tag{} ){}

	Functor(Functor const &rhs) 
	:	_hfxp(rhs._hfxp){  Operators_of<hFXP>::_p = Address_of(_hfxp);  }

	Functor(Functor&& rhs) noexcept 
	:	_hfxp( Move(rhs._hfxp) ){  Operators_of<hFXP>::_p = Address_of(_hfxp);  }


	auto operator=(Functor const&)-> Functor& = delete;

private:
	hFXP _hfxp;
};


template<>
class sgm::fp::Functor<typename sgm::fp::_identity_Functor_detail::identical_FuncExpr_t> 
{
public:
	template<class...ARGS>
	decltype(auto) operator()(ARGS&&...args) const
	{
		return _identity_Functor_detail::identity( Forward<ARGS>(args)... );
	}


	template
	<	class FUNC
	,	class 
		=	Enable_if_t<  !is_Same< Decay_t<FUNC>, _identity_Functor_detail::Tag >::value  >   
	>
	decltype(auto) operator*(FUNC&& func) const
	{ 
		return Forward<FUNC>(func) * _identity_Functor_detail::Tag{};  
	}

	auto operator=(Functor const&)-> Functor& = delete;
};


namespace sgm::fp
{

	Functor()-> Functor<typename _identity_Functor_detail::identical_FuncExpr_t>;

	template<class F>
	Functor(F&&)
	->	Functor
		<	Selective_t
			<	is__FuncExpr<F>::value
			,	decltype( _FXP_Harden::calc(Mock<F>()) )
			,	_FuncExpr< _Closure<>, Alephless_t<F> >
			>
		>;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned IDX, unsigned...INDICES>
struct sgm::fp::_Permute_Helper<IDX, INDICES...>
{
	template<class...TYPES, class...ARGS>
	static decltype(auto) calc(Param_Pack<TYPES...> &&ppk, ARGS&&...args) noexcept
	{
		return 
		_Permute_Helper<INDICES...>::calc
		(	Move(ppk), Forward<ARGS>(args)..., ppk.template forward<IDX>() 
		);
	}
};


template<>
struct sgm::fp::_Permute_Helper<>
{
	template<class...TYPES, class...ARGS>
	static decltype(auto) calc(Param_Pack<TYPES...>&&, ARGS&&...args) noexcept
	{
		return Parameters( Forward<ARGS>(args)... );
	}
};


namespace sgm::fp
{

	template<unsigned...INDICES>
	inline static Functor const Permute 
	=	[](auto&&...args)-> decltype(auto)
		{
			return 
			_Permute_Helper<INDICES...>::calc
			(	Parameters( Forward<decltype(args)>(args)... )  
			);
		};

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#ifndef SGM_1st_Class_Citizen
	#define SGM_1st_Class_Citizen(NAME, ...)	\
		sgm::fp::Functor	\
		{	[](auto&&...args)-> decltype(auto)	\
			{	\
				return NAME __VA_ARGS__ ( decltype(args)(args)... );  \
			}	\
		}

#else
	#error SGM_1st_Class_Citizen was already defined somewhere else.
#endif


namespace sgm::fp::_Lambda_detail
{

#pragma warning(push)
//	All parameters of args... don't have to be used .
#pragma warning(disable : 4100)
	template<size_t IDX, class...ARGS>
	static decltype(auto) Try_Nth_Param([[maybe_unused]] ARGS&&...args) noexcept
	{
		if constexpr( IDX < sizeof...(ARGS) )
			return Nth_Param<IDX>(args...);
		else
			return None{};
	}	
#pragma warning(pop)

}


#ifndef SGM_LAMBDA
	#define SGM_LAMBDA(...)		\
	sgm::fp::Functor	\
	{	[&](auto&&...args)-> decltype(auto)	\
		{	\
			using sgm::fp::_Lambda_detail::Try_Nth_Param;	\
			\
			[[maybe_unused]] auto&& _0 = Try_Nth_Param<0>(args...);	\
			[[maybe_unused]] auto&& _1 = Try_Nth_Param<1>(args...);	\
			[[maybe_unused]] auto&& _2 = Try_Nth_Param<2>(args...);	\
			[[maybe_unused]] auto&& _3 = Try_Nth_Param<3>(args...);	\
			[[maybe_unused]] auto&& _4 = Try_Nth_Param<4>(args...);	\
			[[maybe_unused]] auto&& _5 = Try_Nth_Param<5>(args...);	\
			[[maybe_unused]] auto&& _6 = Try_Nth_Param<6>(args...);	\
			[[maybe_unused]] auto&& _7 = Try_Nth_Param<7>(args...);	\
			[[maybe_unused]] auto&& _8 = Try_Nth_Param<8>(args...);	\
			[[maybe_unused]] auto&& _9 = Try_Nth_Param<9>(args...);	\
			\
			return __VA_ARGS__;	\
		}	\
	}
	
#else
	#error SGM_LAMBDA was already defined somewhere else.
#endif


namespace sgm::fp
{

	inline static Functor const integers = [](auto...args){  return Countable<>(args...);  };


#ifndef _SGM_FP_HIGH_TEMPLAR
	#define _SGM_FP_HIGH_TEMPLAR(TITLE)	\
		inline static Functor const TITLE##_f = SGM_1st_Class_Citizen(TITLE) 

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


	template<class FNJ_FLAG = Fork_and_Join_Flag<>>
	inline static Functor const Fold_par_f
	=	[](auto&&...args)
		->	decltype(auto){  return Fold<FNJ_FLAG>( Forward<decltype(args)>(args)... );  };

	template<class FNJ_FLAG = Fork_and_Join_Flag<>>
	inline static Functor const rFold_par_f
	=	[](auto&&...args)
		->	decltype(auto){  return rFold<FNJ_FLAG>( Forward<decltype(args)>(args)... );  };

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_FP_FUNCTOR_