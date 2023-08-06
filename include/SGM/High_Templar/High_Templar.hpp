/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_HIGH_TEMPLAR_
#define _SGM_HIGH_TEMPLAR_


#include "../Array/Array.hpp"
#include "../Family/Family.hpp"
#include "../Concurrency/Concurrency.hpp"

#if SGM_CXX_STANDARD >= 2017
#include <execution>
#include <algorithm>
#include "Countable.hpp"
#endif


namespace sgm
{

	struct Nof_Task 
	:	Unconstructible{  static unsigned constexpr AUTO_OR = Nof_Hardware_Core::DYNAMIC;  };

	template<unsigned NOF_TASK1 = Nof_Task::AUTO_OR, unsigned NOF_TASK2 = NOF_TASK1>
	struct Fork_and_Join_Flag;

}


namespace sgm
{
	namespace ht
	{

		template<class REF, bool = is_Lvalue_Reference<REF>::value>
		class _Pointer_Proxy;


		template
		<	class ITR, class FUNC
		,	int 
			=	is_random_access_iterator<ITR>::value ? 3
			:	is_bidirectional_iterator<ITR>::value ? 2
			:	is_iterator<ITR>::value ? 1
			:	/* otherwise */ 0
		>
		class Morph_iterator;


		template<class RG, class FUNC, bool TRY_MUTABLE>
		class Morph_Range;


		template<class ITR, class FUNC>
		class Filter_iterator;

		template<class RG, class FUNC, bool TRY_MUTABLE>
		class Filter_Range;

		template<class RG, class FUNC, class ITR>
		class _Filter_itr_Func;


		enum class _Evaluation_Mode{ SEQUANCIAL, FORK_AND_JOIN, UNDEFINED };

		
		SGM_USER_DEFINED_TYPE_CHECK
		(	SGM_MACROPACK(unsigned NOF_TASK1, unsigned NOF_TASK2)
		,	Fork_and_Join_Flag, <NOF_TASK1, NOF_TASK2>
		);

		
		template
		<	bool HAS_INIT_VAL, bool IS_FORWARD, class EVAL_FLAG = None
		,	_Evaluation_Mode
			=	is_Fork_and_Join_Flag<EVAL_FLAG>::value ? _Evaluation_Mode::FORK_AND_JOIN
			:	is_None<EVAL_FLAG>::value ? _Evaluation_Mode::SEQUANCIAL
			:	/* otherwise */ _Evaluation_Mode::UNDEFINED
		>
		struct _Fold_impl;


		template<class...RGS>
		class cPlait_Range;

		template<class...RGS>
		class Plait_Range;


		template< template<class...> class ITR_TRAIT_TAG, class T >
		struct _Plait_iterator_Trait;


		template
		<	class FAM, bool TRY_MUTABLE
		,	int
			=	_Plait_iterator_Trait<is_random_access_iterator, FAM>::value ? 3
			:	_Plait_iterator_Trait<is_bidirectional_iterator, FAM>::value ? 2
			:	_Plait_iterator_Trait<is_iterator, FAM>::value ? 1
			:	/* otherwise */ 0
		>
		class Plait_iterator;


		template
		<	class EVAL_FLAG = None
		,	_Evaluation_Mode
		#if SGM_CXX_STANDARD >= 2017 && defined(_MSC_VER)
			=	(	is_Fork_and_Join_Flag<EVAL_FLAG>::value
				||	std::is_execution_policy<EVAL_FLAG>::value
				) ?	_Evaluation_Mode::FORK_AND_JOIN
		#else
			=	is_Fork_and_Join_Flag<EVAL_FLAG>::value ? _Evaluation_Mode::FORK_AND_JOIN
		#endif
			:	is_None<EVAL_FLAG>::value ? _Evaluation_Mode::SEQUANCIAL
			:	/* otherwise */ _Evaluation_Mode::UNDEFINED
		>
		struct _Array_Evaluation;


		template<class T, bool TRY_MUTABLE>
		using _Try_Mutable_t = Selective_t<TRY_MUTABLE, T, T const>;

		template<bool TRY_MUTABLE, class T>
		static auto Try_Mut_if(T& t) noexcept-> _Try_Mutable_t<T, TRY_MUTABLE>&{  return t;  }

		template<bool TRY_MUTABLE, class T>
		static auto Try_Mut_if(T* p) noexcept-> _Try_Mutable_t<T, TRY_MUTABLE>*{  return p;  }

	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


/**	
*	<N, whatever> : Execution by N tasks.
*	<AUTO_OR, AUTO_OR> : Auto detection or sequancial execution if failed. (Default setting)
*	<AUTO_OR, N> : Auto detection or execution by N tasks if failed.
*	<AUTO_OR, 0> : Auto detection or throw "sgm::FnJ_Fail_to_get_Nof_Core" exception if failed.
*/
template<unsigned NOF_TASK1, unsigned NOF_TASK2>
struct sgm::Fork_and_Join_Flag : Fork_and_Join<NOF_TASK1>{};


template<unsigned N>
struct sgm::Fork_and_Join_Flag<sgm::Nof_Task::AUTO_OR, N> 
:	Fork_and_Join<Nof_Hardware_Core::DYNAMIC>
{
private:
	auto _dynamic_fnj() const noexcept
	->	Fork_and_Join<Nof_Hardware_Core::DYNAMIC> const&{  return *this;  }


public:
	template<class FUNC>
	void operator()(size_t const nof_iteration, FUNC&& func) const noexcept
	{
		if(_dynamic_fnj().number_of_task() != 1)
			_dynamic_fnj()( nof_iteration, Forward<FUNC>(func) );
		else
			Fork_and_Join<N>()( nof_iteration, Forward<FUNC>(func) );
	}
};


template<>
struct sgm::Fork_and_Join_Flag<sgm::Nof_Task::AUTO_OR, 0>
:	Fork_and_Join<Nof_Hardware_Core::DYNAMIC>
{
	Fork_and_Join_Flag() 
	noexcept
	(	noexcept
		(	Fork_and_Join<Nof_Hardware_Core::DYNAMIC>(Mock<Nof_Hardware_Core::When_Fails>()) 
		)
	)
	:	Fork_and_Join<Nof_Hardware_Core::DYNAMIC>(Nof_Hardware_Core::When_Fails::THROW){}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class EVAL_FLAG, sgm::ht::_Evaluation_Mode MODE>
struct sgm::ht::_Array_Evaluation
{
	static_assert(MODE != _Evaluation_Mode::UNDEFINED, "Error : Undefined behavior .");
};


template<class _D>
struct sgm::ht::_Array_Evaluation<_D, sgm::ht::_Evaluation_Mode::SEQUANCIAL> : Unconstructible
{
	template
	<	class RG, class ELEM = Decay_t< decltype( *Begin(Mock<RG>()) ) >  
	,	class ALC = _Default_Array_Allocator_t<ELEM>
	,	bool _NXCT
		=	noexcept
			(	Array<ELEM>::by
				(	Mock<ALC&&>(), fBegin<RG>(Mock<RG&&>()), fEnd<RG>(Mock<RG&&>())
				)
			)
	>
	static auto Calc(RG&& rg, ALC&& allocator = {}) noexcept(_NXCT)
	->	Array< ELEM, arrSize::DYNAMIC, Decay_t<ALC> >
	{
		return Array<ELEM>::by( Forward<ALC>(allocator), fBegin<RG>(rg), fEnd<RG>(rg) );
	}
};


template<class FNJ_FLAG>
struct sgm::ht::_Array_Evaluation<FNJ_FLAG, sgm::ht::_Evaluation_Mode::FORK_AND_JOIN> 
:	Unconstructible
{
	template
	<	class RG, class ELEM = Decay_t< decltype( *Begin(Mock<RG>()) ) >  
	,	class ALC = _Default_Array_Allocator_t<ELEM>
	>
	static auto Calc(RG&& rg, ALC&& allocator = {}) noexcept
	->	Enable_if_t
		<	is_Fork_and_Join_Flag<FNJ_FLAG>::value
		,	Array< ELEM, arrSize::DYNAMIC, Decay_t<ALC> >
		>
	{
		size_t const nof_elem = Size(rg);

		if(nof_elem == 0)
			return Array<ELEM>::by( Forward<ALC>(allocator) );
		
		ELEM* pdata = allocator.allocate(nof_elem);

		FNJ_FLAG()
		(	nof_elem
		,	[&rg, &allocator, pdata]
			(size_t begin_idx, size_t const end_idx, unsigned const) noexcept
			{	
				for
				(	auto itr = Next( fBegin<RG>(rg), begin_idx )
				;	begin_idx < end_idx
				;	allocator.construct(pdata + begin_idx, *itr),  begin_idx++,  itr++
				);
			}
		);

		return 
		Unsafe_Construction_for_Array::create
		(	pdata, nof_elem, nof_elem, Forward<ALC>(allocator) 
		);
	}

#if SGM_CXX_STANDARD >= 2017 && defined(_MSC_VER)
	template
	<	class RG, class ELEM = Decay_t< decltype( *Begin(Mock<RG>()) ) >  
	,	class ALC = _Default_Array_Allocator_t<ELEM>
	>
	static auto Calc(RG&& rg, ALC&& allocator = {}) noexcept
	->	Enable_if_t
		<	std::is_execution_policy<FNJ_FLAG>::value
		,	Array< ELEM, arrSize::DYNAMIC, Decay_t<ALC> >
		>
	{
		size_t const nof_elem = Size(rg);

		if(nof_elem == 0)
			return Array<ELEM>::by( Forward<ALC>(allocator) );

		ELEM* pdata = allocator.allocate(nof_elem);
		Countable<size_t> const indices(nof_elem);

		std::for_each
		(	FNJ_FLAG{}
		,	indices.cbegin(), indices.cend()
		,	[&rg, &allocator, pdata](size_t const idx) noexcept
			{	
				allocator.construct(  pdata + idx, *Next( fBegin<RG>(rg), idx )  );
			}
		);

		return 
		Unsafe_Construction_for_Array::create
		(	pdata, nof_elem, nof_elem, Forward<ALC>(allocator) 
		);
	}
#endif
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{
	namespace _ht_reverse_xitr_type_detail
	{

		template
		<	class RG
		,	bool 
			=	Boolean_Or
				<	Has_MemFunc_rbegin< Referenceless_t<RG> > 
				,	is_Bounded_Array< Referenceless_t<RG> >
				>::	value
		>  
		struct rxitr;

		template<class RG>  
		struct rxitr<RG, false> 
		:	As_type_itself
			<	Reverse_iterator<  Decay_t< decltype( Begin(Mock<RG>()) ) >  > 
			>
		{};
		
		template<class RG>  
		struct rxitr<RG, true>
		:	As_type_itself<  Decay_t< decltype( rBegin(Mock<RG>()) ) >  >{};
		
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class REF>
class sgm::ht::_Pointer_Proxy<REF, true>
{
public:
	using type = Referenceless_t<REF>*;

	_Pointer_Proxy(REF& t) noexcept : _ptr( Address_of(t) ){}

	operator type() const noexcept{  return _ptr;  }


private:
	type _ptr;
};


template<class REF>
class sgm::ht::_Pointer_Proxy<REF, false>
{
private:
	using _elem_t = Referenceless_t<REF>;

public:
	using type = _Pointer_Proxy;

	_Pointer_Proxy(REF&& t) noexcept : _t( Move(t) ){}

	auto operator->() const noexcept-> _elem_t const*{  return Address_of(_t);  }
	auto operator->() noexcept-> _elem_t*{  return Address_of(_t);  }


private:
	_elem_t _t;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class ITR, class FUNC, int ITR_TRAIT>
class sgm::ht::Morph_iterator 
{
public:
	static_assert(ITR_TRAIT != 0, "sgm::ht::Morph_iterator expects ITR to be an iterator type ");
};


template<class ITR, class FUNC>
class sgm::ht::Morph_iterator<ITR, FUNC, 1>
{
protected:
	using _deref_t = decltype( Mock<FUNC>()(*Mock<ITR>()) );

	static bool constexpr _is_noexcept_func_v = noexcept( Mock<FUNC>()(*Mock<ITR>()) );
	
private:
	using _self_t = Morph_iterator;

public:
	template<  class ITR2, class = Enable_if_t< is_Convertible<ITR2, ITR>::value >  >
	Morph_iterator(ITR2 const xitr2, FUNC& func) noexcept : _xitr(xitr2), _pfunc(&func){}


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	Morph_iterator(Morph_iterator<ITR2, FUNC> const mitr) noexcept
	:	_xitr(mitr._xitr), _pfunc(mitr._pfunc){}	


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	auto operator=(Morph_iterator<ITR2, FUNC> const mitr) noexcept
	->	_self_t&{  return _xitr = mitr._xitr,  _pfunc = mitr._pfunc,  *this;  }


	auto operator*() const noexcept(_is_noexcept_func_v)
	->	_deref_t{  return (*_pfunc)(*_xitr);  }

	auto operator->() const noexcept(_is_noexcept_func_v)
	->	typename _Pointer_Proxy<_deref_t>::type{  return _Pointer_Proxy<_deref_t>(**this);  }

	auto operator==(_self_t const itr) const noexcept-> bool{  return _xitr == itr._xitr;  }
	auto operator!=(_self_t const itr) const noexcept-> bool{  return !(*this == itr);  }

	auto operator++() noexcept-> _self_t&{  return ++_xitr,  *this;  }
	
	auto operator++(int) noexcept
	->	_self_t{  _self_t const itr = *this;  return ++*this,  itr;  }


	ITR _xitr;
	FUNC *_pfunc;
};


template<class ITR, class FUNC>
class sgm::ht::Morph_iterator<ITR, FUNC, 2> : public Morph_iterator<ITR, FUNC, 1>
{
private:
	using _top_t = Morph_iterator<ITR, FUNC, 1>;
	using _self_t = Morph_iterator;

public:
	template<class...ARGS>
	Morph_iterator(ARGS&&...args) noexcept : _top_t( Forward<ARGS>(args)... ){}


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	auto operator=(Morph_iterator<ITR2, FUNC> const mitr) noexcept
	->	_self_t&{  return _top_t::operator=(mitr),  *this;  }


	auto operator++() noexcept-> _self_t&{  return _top_t::operator++(),  *this;  }
	
	auto operator++(int) noexcept
	->	_self_t{  _self_t const itr = *this;  return ++*this,  itr;  }

	auto operator--() noexcept-> _self_t&{  return --_top_t::_xitr,  *this;  }
	
	auto operator--(int) noexcept
	->	_self_t{  _self_t const itr = *this;  return --*this,  itr;  }
};


template<class ITR, class FUNC>
class sgm::ht::Morph_iterator<ITR, FUNC, 3> : public Morph_iterator<ITR, FUNC, 2>
{
private:
	using _top_t = Morph_iterator<ITR, FUNC, 1>;
	using _middle_t = Morph_iterator<ITR, FUNC, 2>;
	using _self_t = Morph_iterator;
	using typename _top_t::_deref_t;

public:
	template<class...ARGS>
	Morph_iterator(ARGS&&...args) noexcept : _middle_t( Forward<ARGS>(args)... ){}


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	auto operator=(Morph_iterator<ITR2, FUNC> const mitr) noexcept
	->	_self_t&{  return _top_t::operator=(mitr),  *this;  }


	auto operator++() noexcept-> _self_t&{  return _middle_t::operator++(),  *this;  }
	
	auto operator++(int) noexcept
	->	_self_t{  _self_t const itr = *this;  return ++*this,  itr;  }

	auto operator--() noexcept-> _self_t&{  return _middle_t::operator--(),  *this;  }
	
	auto operator--(int) noexcept
	->	_self_t{  _self_t const itr = *this;  return --*this,  itr;  }


	auto operator[](ptrdiff_t const d) const noexcept(_top_t::_is_noexcept_func_v)
	->	_deref_t{  return *(*this + d);  }
	
	auto operator[](ptrdiff_t const d) noexcept(_top_t::_is_noexcept_func_v)
	->	_deref_t{  return *(*this + d);  }

	auto operator+(ptrdiff_t const d) const noexcept
	->	_self_t{  return{_top_t::_xitr + d, *_top_t::_pfunc};  }
	
	auto operator-(ptrdiff_t const d) const noexcept
	->	_self_t{  return{_top_t::_xitr - d, *_top_t::_pfunc};  }

	auto operator+=(ptrdiff_t const d) noexcept
	->	_self_t&{  return _top_t::_xitr += d,  *this;  }
	
	auto operator-=(ptrdiff_t const d) noexcept
	->	_self_t&{  return _top_t::_xitr -= d,  *this;  }

	auto operator-(_self_t const itr) const noexcept
	->	ptrdiff_t{  return _top_t::_xitr - itr._xitr;  }

	auto operator<(_self_t const itr) const noexcept{  return _top_t::_xitr < itr._xitr;  }
	auto operator>(_self_t const itr) const noexcept{  return _top_t::_xitr > itr._xitr;  }
	auto operator<=(_self_t const itr) const noexcept{  return !(*this > itr);  }
	auto operator>=(_self_t const itr) const noexcept{  return !(*this < itr);  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class ITR, class FUNC>
struct sgm::is_random_access_iterator< sgm::ht::Morph_iterator<ITR, FUNC> >
:	is_random_access_iterator<ITR>{};


template<class ITR, class FUNC>
struct sgm::is_bidirectional_iterator< sgm::ht::Morph_iterator<ITR, FUNC> >
:	is_bidirectional_iterator<ITR>{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class RG, class FUNC, bool TRY_MUTABLE>
class sgm::ht::Morph_Range
{
private:
	using _cxitr_t = Decay_t< decltype( Begin(Mock<RG const>()) ) >;

	using _xitr_t
	=	Selective_t<  TRY_MUTABLE, Decay_t< decltype( Begin(Mock<RG>()) ) >, _cxitr_t  >;


	using _crxitr_t = typename _ht_reverse_xitr_type_detail::rxitr<RG const>::type;

	using _rxitr_t 
	=	Selective_t
		<	TRY_MUTABLE, typename _ht_reverse_xitr_type_detail::rxitr<RG>::type, _crxitr_t
		>;


	using _func_t = Referenceless_t<FUNC>;


public:
	using iterator = Morph_iterator<_xitr_t, _func_t>;
	using const_iterator = Morph_iterator<_cxitr_t, _func_t const>;
	using reverse_iterator = Morph_iterator<_rxitr_t, _func_t>;
	using const_reverse_iterator = Morph_iterator<_crxitr_t, _func_t const>;

	using value_type = Decay_t< decltype(*Mock<iterator>()) >;


	template
	<	class RG_, class FUNC_
	,	bool _NXCT
		=	(	nxct::is_Nxct_initialization<RG_&&, RG>::value 
			&&	nxct::is_Nxct_initialization<FUNC_&&, FUNC>::value
			)
	>
	Morph_Range(RG_&& rg, FUNC_&& func) noexcept(_NXCT)
	:	_rg( Forward<RG_>(rg) ), _func( Forward<FUNC_>(func) ){}


	auto cbegin() const noexcept-> const_iterator{  return {Begin(_rg), _func};  }
	auto begin() const noexcept-> SGM_DECLTYPE_AUTO(  cbegin()  )
	auto begin() noexcept-> iterator{  return {Begin(_rg), _func};  }

	auto cend() const noexcept-> const_iterator{  return {End(_rg), _func};  }
	auto end() const noexcept-> SGM_DECLTYPE_AUTO(  cend()  )
	auto end() noexcept-> iterator{  return {End(_rg), _func};  }

	auto crbegin() const noexcept-> const_reverse_iterator{  return {rBegin(_rg), _func};  }
	auto rbegin() const noexcept-> SGM_DECLTYPE_AUTO(  crbegin()  )
	auto rbegin() noexcept-> reverse_iterator{  return {rBegin(_rg), _func};  }

	auto crend() const noexcept-> const_reverse_iterator{  return {rEnd(_rg), _func};  }
	auto rend() const noexcept-> SGM_DECLTYPE_AUTO(  crend()  )
	auto rend() noexcept-> reverse_iterator{  return {rEnd(_rg), _func};  }


	template<class EVAL_FLAG = None, class ALC = _Default_Array_Allocator_t<value_type> >
	auto eval(ALC&& allocator = {}) const noexcept
	->	Array< value_type, arrSize::DYNAMIC, Decay_t<ALC> >
	{  
		return _Array_Evaluation<EVAL_FLAG>::Calc( *this, Forward<ALC>(allocator) );
	}


	template< class ALC = _Default_Array_Allocator_t<value_type> >
	operator Array<value_type, arrSize::DYNAMIC, ALC>() const noexcept{  return eval(ALC{});  }


private:
	RG _rg;
	FUNC _func;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class RG, class FUNC, class ITR>
class sgm::ht::_Filter_itr_Func
{
private:
	using _pfunc_t = Referenceless_t<FUNC>*;

	static bool constexpr _is_noexcept_func_v 
	=	noexcept( Mock< Pointerless_t<_pfunc_t> >()(*Mock<ITR>()) );

public:
	_Filter_itr_Func(_pfunc_t const pfunc, ITR const eitr) noexcept 
	:	_pfunc(pfunc), _eitr(eitr){}

	auto operator()(ITR const itr) const noexcept(_is_noexcept_func_v)
	->	bool{  return itr == _eitr || (*_pfunc)(*itr);  }
	
	auto operator()(ITR const itr) noexcept(_is_noexcept_func_v)
	->	bool{  return itr == _eitr || (*_pfunc)(*itr);  }


private:
	_pfunc_t _pfunc;
	ITR _eitr;
};


template<class ITR, class ITR_FUNC>
class sgm::ht::Filter_iterator
{
private:
	using _deref_t = decltype(*Mock<ITR>());
	using _self_t = Filter_iterator;

	static bool constexpr 
		_is_noexcept_func_v = noexcept( Mock<ITR_FUNC>()(Mock<ITR>()) ),
		_is_noexcept_copy_constructible_func_v 
		=	noexcept( ITR_FUNC(Mock<ITR_FUNC const&>()) ),
		_is_noexcept_copy_assignable_func_v
		=	noexcept(Mock<ITR_FUNC&>() = Mock<ITR_FUNC const&>()),
		_is_noexcept_deref_operator_v = noexcept(*Mock<ITR>());



public:
	template<  class ITR2, class = Enable_if_t< is_Convertible<ITR2, ITR>::value >  >
	Filter_iterator(ITR2 const xitr2, ITR_FUNC const itr_func) 
	noexcept(_is_noexcept_copy_constructible_func_v && _is_noexcept_func_v)
	:	_xitr(xitr2), _itr_func(itr_func){  _shift_until_valid();  }


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	Filter_iterator(Filter_iterator<ITR2, ITR_FUNC> const fitr)
	noexcept(_is_noexcept_copy_constructible_func_v && _is_noexcept_func_v)
	:	_xitr(fitr._xitr), _itr_func(fitr._itr_func){  _shift_until_valid();  }


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	auto operator=(Filter_iterator<ITR2, ITR_FUNC> const fitr)
	noexcept(_is_noexcept_copy_assignable_func_v)
	->	_self_t&{  return _xitr = fitr._xitr,  _itr_func = fitr._itr_func,  *this;  }


	auto operator*() const noexcept(_is_noexcept_deref_operator_v)-> _deref_t{  return *_xitr;  }

	auto operator->() const noexcept(_is_noexcept_deref_operator_v)
	->	typename _Pointer_Proxy<_deref_t>::type{  return _Pointer_Proxy<_deref_t>(**this);  }


	auto operator++() noexcept(_is_noexcept_func_v)
	->	_self_t&{  return ++_xitr,  _shift_until_valid(),  *this;  }
	
	auto operator++(int) noexcept
	->	_self_t{  _self_t const itr = *this;  return ++*this,  itr;  }

	bool operator==(_self_t const fitr) const noexcept{  return _xitr == fitr._xitr;  }
	bool operator!=(_self_t const fitr) const noexcept{  return !(*this == fitr);  }


	ITR _xitr;
	ITR_FUNC _itr_func;


private:
	void _shift_until_valid() noexcept(_is_noexcept_func_v)
	{
		for(;  !_itr_func(_xitr);  _xitr++);  
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class RG, class FUNC, bool TRY_MUTABLE>
class sgm::ht::Filter_Range
{
private:
	using _cxitr_t = Decay_t< decltype( Begin(Mock<RG const>()) ) >;

	using _xitr_t
	=	Selective_t<  TRY_MUTABLE, Decay_t< decltype( Begin(Mock<RG>()) ) >, _cxitr_t  >;


	using _crxitr_t = typename _ht_reverse_xitr_type_detail::rxitr<RG const>::type;

	using _rxitr_t 
	=	Selective_t
		<	TRY_MUTABLE, typename _ht_reverse_xitr_type_detail::rxitr<RG>::type, _crxitr_t
		>;


	using _func_t = Referenceless_t<FUNC>;

	template<class XITR>
	using _filter_itr_func_t = _Filter_itr_Func<RG, FUNC, XITR>;


public:
	using iterator = Filter_iterator< _xitr_t, _filter_itr_func_t<_xitr_t> >;
	using const_iterator = Filter_iterator< _cxitr_t, _filter_itr_func_t<_cxitr_t> >;
	using reverse_iterator = Filter_iterator< _rxitr_t, _filter_itr_func_t<_rxitr_t> >;
	using const_reverse_iterator = Filter_iterator< _crxitr_t, _filter_itr_func_t<_crxitr_t> >;

	using value_type = Decay_t< decltype(*Mock<_xitr_t>()) >;


	template
	<	class RG_, class FUNC_
	,	bool _NXCT
		=	(	nxct::is_Nxct_initialization<RG_&&, RG>::value 
			&&	nxct::is_Nxct_initialization<FUNC_&&, FUNC>::value
			)
	>
	Filter_Range(RG_&& rg, FUNC_&& fn) noexcept(_NXCT)
	:	_rg( Forward<RG_>(rg) ), _fn( Forward<FUNC_>(fn) ){}


	auto cbegin() const noexcept-> const_iterator{  return {Begin(_rg), _Fitr_func(this)};  }
	auto begin() const noexcept-> SGM_DECLTYPE_AUTO(  cbegin()  )
	auto begin() noexcept-> iterator{  return {Begin(_rg), _Fitr_func(this)};  }

	auto cend() const noexcept-> const_iterator{  return {End(_rg), _Fitr_func(this)};  }
	auto end() const noexcept-> SGM_DECLTYPE_AUTO(  cend()  )
	auto end() noexcept-> iterator{  return {End(_rg), _Fitr_func(this)};  }

	auto crbegin() const noexcept
	->	const_reverse_iterator{  return {rBegin(_rg), _Fritr_func(this)};  }
	
	auto rbegin() const noexcept-> SGM_DECLTYPE_AUTO(  crbegin()  )
	auto rbegin() noexcept-> reverse_iterator{  return {rBegin(_rg), _Fritr_func(this)};  }

	auto crend() const noexcept
	->	const_reverse_iterator{  return {rEnd(_rg), _Fritr_func(this)};  }
	
	auto rend() const noexcept-> SGM_DECLTYPE_AUTO(  crend()  )
	auto rend() noexcept-> reverse_iterator{  return {rEnd(_rg), _Fritr_func(this)};  }


	template< class ALC = _Default_Array_Allocator_t<value_type> >
	auto eval(ALC&& allocator = {}) const noexcept
	->	Array< value_type, arrSize::DYNAMIC, Decay_t<ALC> >
	{  
		static_assert
		(	!is_Fork_and_Join_Flag<ALC>::value && is_Allocator<ALC>::value
		,	"Filter_Range does NOT have fork and join evaluation method."
		);

		return _Array_Evaluation<None>::Calc( *this, Forward<ALC>(allocator) );
	}


	template< class ALC = _Default_Array_Allocator_t<value_type> >
	operator Array<value_type, arrSize::DYNAMIC, ALC>() const noexcept{  return eval(ALC{});  }


private:
	RG _rg;
	FUNC _fn;


	template<  class ME, class itr_t = Decay_t< decltype( End(Mock<ME>()._rg) ) >  >
	static auto _Fitr_func(ME* p) noexcept
	->	_filter_itr_func_t<itr_t>{  return {&p->_fn, End(p->_rg)};  }

	template<  class ME, class itr_t = Decay_t< decltype( rEnd(Mock<ME>()._rg) ) >  >
	static auto _Fritr_func(ME* p) noexcept
	->	_filter_itr_func_t<itr_t>{  return {&p->_fn, rEnd(p->_rg)};  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{
	namespace _ht_Fold_impl_helper
	{
		
		template
		<	class ITR, class FUNC, class RES
		,	bool _NXCT
			=	(	noexcept( ITR(Mock<ITR>()) ) && noexcept(Mock<ITR>() != Mock<ITR>())
				&&	noexcept( RES(Mock<RES>()) ) 
				&&	noexcept( Mock<RES&>() = Mock<FUNC&&>()(Mock<RES>(), *Mock<ITR>()++) )
				)
		>
		static auto Accumulate(ITR itr, ITR const eitr, FUNC&& func, RES res) noexcept(_NXCT)
		->	RES
		{
			while(itr != eitr)
				res = func(res, *itr++);

			return res;
		}


		template<bool IS_FORWARD> 
		struct Direction_Dependent;
	
		template<>
		struct Direction_Dependent<true> : Unconstructible
		{
			template<class RG> 
			static auto Begin(RG&& rg) noexcept-> SGM_DECLTYPE_AUTO(  fBegin<RG>(rg)  )

			template<class RG> 
			static auto End(RG&& rg) noexcept-> SGM_DECLTYPE_AUTO(  fEnd<RG>(rg)  )


			template
			<	class FUNC, class L, class FOLDED
			,	bool _NXCT = noexcept( Mock<FUNC&&>()(Mock<L&&>(), Mock<FOLDED&&>()) )
			>
			static auto Last_fold(FUNC&& func, L&& last, FOLDED&& folded) noexcept(_NXCT)
			->	SGM_DECLTYPE_AUTO(  func( Forward<L>(last), Forward<FOLDED>(folded) )  )
		};
	
		template<>
		struct Direction_Dependent<false> : Unconstructible
		{
			template<class RG> 
			static auto Begin(RG&& rg) noexcept-> SGM_DECLTYPE_AUTO(  frBegin<RG>(rg)  )
			
			template<class RG> 
			static auto End(RG&& rg) noexcept-> SGM_DECLTYPE_AUTO(  frEnd<RG>(rg)  )


			template
			<	class FUNC, class L, class FOLDED
			,	bool _NXCT = noexcept( Mock<FUNC&&>()(Mock<FOLDED&&>(), Mock<L&&>()) )
			>
			static auto Last_fold(FUNC&& func, L&& last, FOLDED&& folded) noexcept(_NXCT)
			->	SGM_DECLTYPE_AUTO(  func( Forward<FOLDED>(folded), Forward<L>(last) )  )
		};

	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<bool IS_FORWARD, class FNJ_FLAG>
struct sgm::ht::_Fold_impl<true, IS_FORWARD, FNJ_FLAG, sgm::ht::_Evaluation_Mode::SEQUANCIAL>
:	Unconstructible
{
	template
	<	class RG, class FUNC, class RES
	,	class _DD = _ht_Fold_impl_helper::Direction_Dependent<IS_FORWARD>
	,	bool _NXCT
		=	noexcept
			(	_ht_Fold_impl_helper::Accumulate	
				(	_DD::Begin(Mock<RG&&>()), _DD::End(Mock<RG&&>())
				,	Mock<FUNC&&>(), Mock<RES&&>()
				)
			)
	>
	static auto Calc(RG&& rg, FUNC&& func, RES&& res) noexcept(_NXCT)-> RES
	{
		return
		_ht_Fold_impl_helper::Accumulate
		(	_DD::Begin( Forward<RG>(rg) ), _DD::End( Forward<RG>(rg) )
		,	Forward<FUNC>(func), Forward<RES>(res)
		);
	}
};


template<bool IS_FORWARD, class FNJ_FLAG>
struct sgm::ht::_Fold_impl<false, IS_FORWARD, FNJ_FLAG, sgm::ht::_Evaluation_Mode::SEQUANCIAL>
:	Unconstructible
{
	template
	<	class RG, class FUNC
	,	class _DD = _ht_Fold_impl_helper::Direction_Dependent<IS_FORWARD>
	,	class _DEREF = decltype( *_DD::Begin(Mock<RG&&>()) )
	,	bool _NXCT
		=	noexcept
			(	_ht_Fold_impl_helper::Accumulate	
				(	_DD::Begin(Mock<RG&&>()), _DD::End(Mock<RG&&>())
				,	Mock<FUNC&&>(), Mock<_DEREF>()
				)
			)
	>
	static auto Calc(RG&& rg, FUNC&& func, None) noexcept(_NXCT)-> Decay_t<_DEREF>
	{
		auto const bi = _DD::Begin( Forward<RG>(rg) ), ei = _DD::End( Forward<RG>(rg) );

		assert(bi != ei && L"the container has nothing to fold .\n");

		return _ht_Fold_impl_helper::Accumulate( Next(bi), ei, Forward<FUNC>(func), *bi );
	}
};


template<bool IS_FORWARD, class FNJ_FLAG>
struct sgm::ht::_Fold_impl<false, IS_FORWARD, FNJ_FLAG, sgm::ht::_Evaluation_Mode::FORK_AND_JOIN>
:	Unconstructible
{
private:
	template
	<	class RG, class FUNC
	,	class _FM = _Fold_impl<false, IS_FORWARD, FNJ_FLAG, _Evaluation_Mode::SEQUANCIAL>
	,	bool _NXCT = noexcept( _FM::Calc(Mock<RG&&>(), Mock<FUNC&&>(), Mock<None>()) )
	>
	static auto _Sequancial_fold(RG&& rg, FUNC&& func) noexcept(_NXCT)-> SGM_DECLTYPE_AUTO
	(
		_FM::Calc( Forward<RG>(rg), Forward<FUNC>(func), None{} )
	)


public:
	template
	<	class RG, class FUNC
	,	class RES = Decay_t<  decltype( *Begin(Mock< Decay_t<RG> >()) )  >
	>
	static auto Calc(RG&& rg, FUNC&& func, None) noexcept-> RES
	{
		auto const fnj = FNJ_FLAG{};
		size_t const nof_elem = Size(rg),  nof_task = fnj.number_of_task();
		

		if(nof_elem <= nof_task)
			return _Sequancial_fold( Forward<RG>(rg), Forward<FUNC>(func) );
		else
		{
			_Default_Array_Allocator_t<RES> allocator{};
			RES* partial_sum_arr = allocator.allocate(nof_task);

			fnj
			(	nof_elem
			,	[&rg, &func, &allocator, partial_sum_arr]
				(	size_t const begin_idx, size_t const end_idx, unsigned const task_id
				)	noexcept
				{	
					auto const 
						bi = Next( fBegin<RG>(rg), begin_idx ), bi_next = Next(bi), 
						ei = Next(bi, end_idx - begin_idx);
					 
					allocator.construct
					(	partial_sum_arr + task_id
					,	_ht_Fold_impl_helper::Accumulate(bi_next, ei, func, *bi)
					);
				}
			);

			return 
			_Sequancial_fold
			(	Unsafe_Construction_for_Array
				::	create(partial_sum_arr, nof_task, nof_task, allocator)
			,	Forward<FUNC>(func) 
			);
		}
	}
};


template<bool IS_FORWARD, class FNJ_FLAG>
struct sgm::ht::_Fold_impl<true, IS_FORWARD, FNJ_FLAG, sgm::ht::_Evaluation_Mode::FORK_AND_JOIN>
:	Unconstructible
{
	template<class RG, class FUNC, class RES>
	static auto Calc(RG&& rg, FUNC&& func, RES&& res) noexcept-> RES
	{
		static_assert
		(	is_Convertible< RES, Decay_t<decltype( *Begin(rg) )> >::value
		,	"for asyncronous operation, the folding function should be asocciative."
		);

		return 
		_ht_Fold_impl_helper::Direction_Dependent<IS_FORWARD>::Last_fold
		(	func, Forward<RES>(res)
		,	_Fold_impl<false, IS_FORWARD, FNJ_FLAG, _Evaluation_Mode::FORK_AND_JOIN>
			::	Calc( Forward<RG>(rg), func, None{} )
		);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{
	namespace _ht_Plait_detail
	{
		
		template< class ITR_FAM, bool TRY_MUTABLE, size_t N = std::tuple_size<ITR_FAM>::value >
		struct Deref : Unconstructible
		{
			using res_t = typename Deref<ITR_FAM, TRY_MUTABLE, 0>::res_t;

			template<class ME, class...ARGS>
			static auto Calc(ME& me, ARGS&&...args)-> res_t
			{
				return
				Deref<ITR_FAM, TRY_MUTABLE, N-1>::Calc
				(	me
				,	*std::get<N-1>(me._itr_fam), Forward<ARGS>(args)...
				);
			}
		};
	
		template<class ITR_FAM, bool TRY_MUTABLE>
		struct Deref<ITR_FAM, TRY_MUTABLE, 0> : Unconstructible
		{
		private:	
			template<class T>
			using _member_t = Alephless_t< ht::_Try_Mutable_t<T, TRY_MUTABLE> >;

			template<class...ITRS> /* Declaration Only */
			static auto _res(Family<ITRS...>) noexcept
			->	Family<  _member_t< decltype(*Mock<ITRS>()) >...  >;


		public:
			using res_t = decltype( _res(Mock<ITR_FAM>()) );

			template<class ME, class...ARGS>
			static auto Calc(ME&, ARGS&&...args)-> res_t{  return {Forward<ARGS>(args)...};  }
		};


		template<size_t N, bool IS_PLUS_DIR, class PLT_ITR>
		static auto Step(PLT_ITR& plt_itr)-> Enable_if_t<N==0, PLT_ITR&>
		{
			return plt_itr;
		}

		template<size_t N, bool IS_PLUS_DIR, class PLT_ITR>
		static auto Step(PLT_ITR& plt_itr)
		->	Enable_if_t<N != 0 && IS_PLUS_DIR, PLT_ITR&>
		{
			std::get<N-1>(plt_itr._itr_fam)++;

			return Step<N-1, true>(plt_itr);  
		}

		template<size_t N, bool IS_PLUS_DIR, class PLT_ITR>
		static auto Step(PLT_ITR& plt_itr)
		->	Enable_if_t<N != 0 && !IS_PLUS_DIR, PLT_ITR&>
		{
			std::get<N-1>(plt_itr._itr_fam)--;

			return Step<N-1, false>(plt_itr);  
		}


		template<size_t N, class PLT_ITR, class...ITRS>
		static auto Next_Diff(PLT_ITR& plt_itr, ptrdiff_t const diff, ITRS&&...itrs)
		->	Enable_if_t<N != 0, PLT_ITR>
		{
			return 
			Next_Diff<N-1>
			(	plt_itr, diff
			,	std::get<N-1>(plt_itr._itr_fam) + diff, Forward<ITRS>(itrs)...
			);
		}

		template<size_t N, class PLT_ITR, class...ITRS>
		static auto Next_Diff(PLT_ITR&, ptrdiff_t const, ITRS&&...itrs)
		->	Enable_if_t<N == 0, PLT_ITR>
		{
			return { Family< Decay_t<ITRS>... >{Forward<ITRS>(itrs)...} };
		}


		template<class RG, class...RGS>
		static auto Range_Size(Family<RGS...> const& rg_fam)
		->	Enable_if_t< is_None<RG>::value, size_t >
		{
			static_assert( sizeof...(RGS) != 0, "No ranges." );

			return Size( std::get<0>(rg_fam) );		
		}

		template<class RG, class...RGS>
		static auto Range_Size(Family<RGS...> const& rg_fam)
		->	Enable_if_t< !is_None<RG>::value, size_t >
		{
			return Size( std::get<RG>(rg_fam) );
		}


		template
		<	class RG_FAM, bool TRY_MUTABLE, bool IS_FORWARD, bool IS_BEGIN
		,	size_t N = std::tuple_size< Decay_t<RG_FAM> >::value, bool = N != 0
		>
		struct Boundary;

		template<class...RGS, bool TRY_MUTABLE, bool IS_FORWARD, bool IS_BEGIN>
		struct Boundary< Family<RGS...>, TRY_MUTABLE, IS_FORWARD, IS_BEGIN, 0, false >
		:	Unconstructible
		{
			template<class T>
			using _try_mut_t = ht::_Try_Mutable_t<T, TRY_MUTABLE>;

			template<class RG>
			using _boundary_t 
			=	Selective_t
				<	IS_FORWARD
				,	decltype( Begin(Mock<RG>()) ), decltype( rBegin(Mock<RG>()) )
				>;
			
			using _itr_fam_t = Family<  _boundary_t< _try_mut_t<RGS> >...  >;
			
			using res_t = ht::Plait_iterator<_itr_fam_t, TRY_MUTABLE>;


			template<class...ITRS>
			static auto Calc(_try_mut_t< Family<RGS...> >&, ITRS&&...itrs)-> res_t
			{
				return { Family< Decay_t<ITRS>... >{Forward<ITRS>(itrs)...} };
			}
		};

		template<class...RGS, bool TRY_MUTABLE, size_t N>
		struct Boundary< Family<RGS...>, TRY_MUTABLE, true, true, N, true > : Unconstructible
		{
			using res_t
			=	typename Boundary< Family<RGS...>, TRY_MUTABLE, true, true, 0, false >::res_t;

			template<class RG_FAM, class...ITRS>
			static auto Calc(RG_FAM& rg_fam, ITRS&&...itrs)-> res_t
			{
				return
				Boundary< Family<RGS...>, TRY_MUTABLE, true, true, N-1 >::Calc
				(	rg_fam, Begin( std::get<N-1>(rg_fam) ), Forward<ITRS>(itrs)...
				);
			}
		};

		template<class...RGS, bool TRY_MUTABLE, size_t N>
		struct Boundary< Family<RGS...>, TRY_MUTABLE, true, false, N, true > : Unconstructible
		{
			using res_t
			=	typename Boundary< Family<RGS...>, TRY_MUTABLE, true, false, 0, false >::res_t;

			template<class RG_FAM, class...ITRS>
			static auto Calc(RG_FAM& rg_fam, ITRS&&...itrs)-> res_t
			{
				return
				Boundary< Family<RGS...>, TRY_MUTABLE, true, false, N-1 >::Calc
				(	rg_fam, End( std::get<N-1>(rg_fam) ), Forward<ITRS>(itrs)...
				);
			}
		};

		template<class...RGS, bool TRY_MUTABLE, size_t N>
		struct Boundary< Family<RGS...>, TRY_MUTABLE, false, true, N, true > : Unconstructible
		{
			using res_t
			=	typename Boundary< Family<RGS...>, TRY_MUTABLE, false, true, 0, false >::res_t;

			template<class RG_FAM, class...ITRS>
			static auto Calc(RG_FAM& rg_fam, ITRS&&...itrs)-> res_t
			{
				return
				Boundary< Family<RGS...>, TRY_MUTABLE, false, true, N-1 >::Calc
				(	rg_fam, rBegin( std::get<N-1>(rg_fam) ), Forward<ITRS>(itrs)...
				);
			}
		};

		template<class...RGS, bool TRY_MUTABLE, size_t N>
		struct Boundary< Family<RGS...>, TRY_MUTABLE, false, false, N, true > : Unconstructible
		{
			using res_t
			=	typename Boundary< Family<RGS...>, TRY_MUTABLE, false, false, 0, false >::res_t;

			template<class RG_FAM, class...ITRS>
			static auto Calc(RG_FAM& rg_fam, ITRS&&...itrs)-> res_t
			{
				return
				Boundary< Family<RGS...>, TRY_MUTABLE, false, false, N-1 >::Calc
				(	rg_fam, rEnd( std::get<N-1>(rg_fam) ), Forward<ITRS>(itrs)...
				);
			}
		};

	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template< template<class...> class ITR_TRAIT_TAG, class T >
struct sgm::ht::_Plait_iterator_Trait 
:	Selective_t
	<	is_Same< T, Decay_t<T> >::value 
	,	False_t
	,	_Plait_iterator_Trait< ITR_TRAIT_TAG, Decay_t<T> >
	>
{};

template< template<class...> class ITR_TRAIT_TAG >
struct sgm::ht::_Plait_iterator_Trait< ITR_TRAIT_TAG, sgm::Family<> > : True_t{};

template< template<class...> class ITR_TRAIT_TAG, class T, class...TYPES >
struct sgm::ht::_Plait_iterator_Trait< ITR_TRAIT_TAG, sgm::Family<T, TYPES...> >
:	Boolean_And
	<	ITR_TRAIT_TAG<T>
	,	_Plait_iterator_Trait< ITR_TRAIT_TAG, Family<TYPES...> > 
	>
{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{
	namespace _ht_debug
	{

		template<class FAM, class...ARGS>
		static auto _Have_same_sizes(FAM const&, ARGS const&...args)
		->	Enable_if_t< std::tuple_size<FAM>::value == sizeof...(ARGS), bool >
		{
			auto const sizes = { Size(args)... };
			auto itr0 = sizes.begin();
			auto itr1 = itr0+1;

			while(itr1 != sizes.end())
				if(*itr0++ != *itr1++)
					return false;

			return true;
		}

		template<class FAM, class...ARGS>
		static auto _Have_same_sizes(FAM const& fam, ARGS const&...args)
		->	Enable_if_t< std::tuple_size<FAM>::value != sizeof...(ARGS), bool >
		{
			return _Have_same_sizes( fam, args..., std::get<sizeof...(ARGS)>(fam) );
		}


		template<size_t IDX = 0, class FAM>
		static auto _Have_different_itrs(FAM const&, FAM const&)
		->	Enable_if_t< IDX == std::tuple_size<FAM>::value, bool >{  return true;  }

		template<size_t IDX = 0, class FAM>
		static auto _Have_different_itrs(FAM const& fam0, FAM const& fam1)
		->	Enable_if_t< IDX != std::tuple_size<FAM>::value, bool >
		{
			return 
			(	std::get<IDX>(fam0) != std::get<IDX>(fam1)
			&&	_Have_different_itrs<IDX+1>(fam0, fam1)
			);
		}

	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FAM, bool TM, int ITR_TRAIT>
class sgm::ht::Plait_iterator
{
public:
	static_assert(ITR_TRAIT != 0, "sgm::ht::Plait_iterator expects a family of iterator types");
};


template<class FAM, bool TRY_MUTABLE>
class sgm::ht::Plait_iterator<FAM, TRY_MUTABLE, 1>
{
private:
	using _self_t = Plait_iterator;


public:
	static size_t constexpr NUMBER_OF_ITERATORS = std::tuple_size<FAM>::value;	

	static_assert(is_Family<FAM>::value,	"FAM should be sgm::Family type.");
	static_assert(NUMBER_OF_ITERATORS != 0, "the number of iterators should be greater than 0");


	Plait_iterator(FAM const& fam) : _itr_fam(fam){}
	Plait_iterator(FAM&& fam) noexcept : _itr_fam( Move(fam) ){}


	auto operator*() const-> typename _ht_Plait_detail::Deref<FAM, TRY_MUTABLE>::res_t
	{
		return _ht_Plait_detail::Deref<FAM, TRY_MUTABLE>::Calc(*this);
	}


	auto operator!=(_self_t const zi) const noexcept-> bool
	{
		//assert( sgm::_ht_debug::_Have_different_itrs(this->_itr_fam, zi._itr_fam) );

		return std::get<0>(this->_itr_fam) != std::get<0>(zi._itr_fam);  
	}

	auto operator==(_self_t const zi) const noexcept-> bool{  return !(*this != zi);  }


	auto operator++()
	->	_self_t&{  return _ht_Plait_detail::Step<NUMBER_OF_ITERATORS, true>(*this);  }

	auto operator++(int)-> _self_t
	{
		auto const clone = *this;  
		
		return ++*this,  clone;  
	}


	FAM _itr_fam;
};


template<class FAM, bool TRY_MUTABLE>
class sgm::ht::Plait_iterator<FAM, TRY_MUTABLE, 2> : public Plait_iterator<FAM, TRY_MUTABLE, 1>
{
private:
	using _Lv1_t = Plait_iterator<FAM, TRY_MUTABLE, 1>;
	using _self_t = Plait_iterator;


public:
	template<class...ARGS>
	Plait_iterator(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	:	_Lv1_t( Forward<ARGS>(args)... ){}


	auto operator++()-> _self_t&{  return _Lv1_t::operator++(),  *this;  }

	auto operator++(int)-> _self_t
	{
		auto const clone = *this;  
		
		return ++*this,  clone;  
	}


	auto operator--()
	->	_self_t&{  return _ht_Plait_detail::Step<_Lv1_t::NUMBER_OF_ITERATORS, false>(*this);  }

	auto operator--(int)-> _self_t
	{
		auto const clone = *this;  
		
		return --*this,  clone;  
	}
};


template<class FAM, bool TRY_MUTABLE>
class sgm::ht::Plait_iterator<FAM, TRY_MUTABLE, 3> : public Plait_iterator<FAM, TRY_MUTABLE, 2>
{
private:
	using _Lv1_t = Plait_iterator<FAM, TRY_MUTABLE, 1>;
	using _Lv2_t = Plait_iterator<FAM, TRY_MUTABLE, 2>;
	using _self_t = Plait_iterator;	


public:
	template<class...ARGS>
	Plait_iterator(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	:	_Lv2_t( Forward<ARGS>(args)... ){}


	auto operator++()-> _self_t&{  return _Lv2_t::operator++(),  *this;  }

	auto operator++(int)-> _self_t
	{
		auto const clone = *this;  
		
		return ++*this,  clone;  
	}


	auto operator--()-> _self_t&{  return _Lv2_t::operator--(),  *this;  }

	auto operator--(int)-> _self_t
	{
		auto const clone = *this;  
		
		return --*this,  clone;  
	}


	auto operator+(ptrdiff_t const d) const
	->	_self_t{  return _ht_Plait_detail::Next_Diff<_Lv1_t::NUMBER_OF_ITERATORS>(*this, d);  }

	auto operator-(ptrdiff_t const d) const-> _self_t{  return *this + -d;  }

	auto operator+=(ptrdiff_t const d)-> _self_t&{  return *this = *this + d;  }
	auto operator-=(ptrdiff_t const d)-> _self_t&{  return *this = *this - d;  }

	auto operator[](ptrdiff_t const d) const
	->	typename _ht_Plait_detail::Deref<FAM, TRY_MUTABLE>::res_t{  return *(*this + d);  }


	template<class _ITR_FAM, bool M>
	auto operator-(Plait_iterator<_ITR_FAM, M, 3> const itr) const
	->	ptrdiff_t{  return std::get<0>(_Lv1_t::_itr_fam) - std::get<0>(itr._itr_fam);  }


	template<class _ITR_FAM, bool M>
	auto operator<(Plait_iterator<_ITR_FAM, M> const itr) const noexcept
	->	bool{  return *this - itr < 0;  }

	template<class _ITR_FAM, bool M>
	auto operator>(Plait_iterator<_ITR_FAM, M> const itr) const noexcept
	->	bool{  return *this - itr > 0;  }

	template<class Q>
	auto operator<=(Q const& q) const noexcept-> bool{  return !(*this > q);  }

	template<class Q>
	auto operator>=(Q const& q) const noexcept-> bool{  return !(*this < q);  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<bool TM, class...ITRS>
struct sgm::is_bidirectional_iterator<  sgm::ht::Plait_iterator< sgm::Family<ITRS...>, TM >  >
:	ht::_Plait_iterator_Trait< is_bidirectional_iterator, Family<ITRS...> >{};

template<bool TM, class...ITRS>
struct sgm::is_random_access_iterator<  sgm::ht::Plait_iterator< sgm::Family<ITRS...>, TM >  >
:	ht::_Plait_iterator_Trait< is_random_access_iterator, Family<ITRS...> >{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...RGS>  
class sgm::ht::cPlait_Range
{
private:
	template<class RG>
	struct _is_random_access_range
	:	is_random_access_iterator< decltype( Begin(Mock<RG>()) ) >{};


public:
	static_assert(Check_if_All<is_iterable, RGS...>::value, "iterable types are expected.");


	using const_iterator
	=	typename _ht_Plait_detail::Boundary< Family<RGS...>, false, true, true >::res_t;

	using const_reverse_iterator
	=	typename _ht_Plait_detail::Boundary< Family<RGS...>, false, false, true >::res_t;

	using value_type = Decay_t< decltype(*Mock<const_iterator>()) >;


	template<class...RGS_>
	cPlait_Range(RGS_&&...rgs) noexcept(Aleph_Check<RGS_&&...>::value)
	:	_rg_fam( Forward<RGS_>(rgs)... )
	{
		assert( sizeof...(RGS_) < 2 || sgm::_ht_debug::_Have_same_sizes(_rg_fam) );
	}


	auto size() const
	{
		using ran_acc_rg_t = typename Satisfying_Among<_is_random_access_range, RGS...>::type;

		return _ht_Plait_detail::Range_Size<ran_acc_rg_t>(_rg_fam);
	}


	auto cbegin() const-> const_iterator
	{
		return _ht_Plait_detail::Boundary< Family<RGS...>, false, true, true >::Calc(_rg_fam);
	}

	auto cend() const-> const_iterator
	{
		return _ht_Plait_detail::Boundary< Family<RGS...>, false, true, false >::Calc(_rg_fam);
	}

	auto begin() const-> SGM_DECLTYPE_AUTO(  cbegin()  )
	auto end() const-> SGM_DECLTYPE_AUTO(  cend()  )

	auto crbegin() const-> const_reverse_iterator
	{
		return _ht_Plait_detail::Boundary< Family<RGS...>, false, false, true >::Calc(_rg_fam);
	}

	auto crend() const-> const_reverse_iterator
	{
		return _ht_Plait_detail::Boundary< Family<RGS...>, false, false, false >::Calc(_rg_fam);
	}

	auto rbegin() const-> SGM_DECLTYPE_AUTO(  crbegin()  )
	auto rend() const-> SGM_DECLTYPE_AUTO(  crend()  )


	template<class FNJ_FLAG>
	auto eval() const
	->	Array<value_type>{  return _Array_Evaluation<FNJ_FLAG>::Calc(*this);  }

	operator Array<value_type>() const{  return eval();  }


protected:
	Family<RGS...> _rg_fam;
};


template<class...RGS>
class sgm::ht::Plait_Range : public cPlait_Range<RGS...>
{
	using _cplt_rg_t = cPlait_Range<RGS...>;

	using _cplt_rg_t::_rg_fam;

public:
	using typename _cplt_rg_t::value_type; 
	using typename _cplt_rg_t::const_iterator;
	using typename _cplt_rg_t::const_reverse_iterator;

	using iterator
	=	typename _ht_Plait_detail::Boundary< Family<RGS...>, true, true, true >::res_t;

	using reverse_iterator
	=	typename _ht_Plait_detail::Boundary< Family<RGS...>, true, false, true >::res_t;


	template<class...RGS_>
	Plait_Range(RGS_&&...rgs) noexcept(Aleph_Check<RGS_&&...>::value)
	:	_cplt_rg_t( Forward<RGS_>(rgs)... ){}


	auto begin() const-> SGM_DECLTYPE_AUTO(  _cplt_rg_t::begin()  )
	auto end() const-> SGM_DECLTYPE_AUTO(  _cplt_rg_t::end()  )
	auto rbegin() const-> SGM_DECLTYPE_AUTO(  _cplt_rg_t::rbegin()  )
	auto rend() const-> SGM_DECLTYPE_AUTO(  _cplt_rg_t::rend()  )

	auto begin()-> iterator
	{
		return _ht_Plait_detail::Boundary< Family<RGS...>, true, true, true >::Calc(_rg_fam);
	}

	auto end()-> iterator
	{
		return _ht_Plait_detail::Boundary< Family<RGS...>, true, true, false >::Calc(_rg_fam);
	}

	auto rbegin()-> reverse_iterator
	{
		return _ht_Plait_detail::Boundary< Family<RGS...>, true, false, true >::Calc(_rg_fam);
	}

	auto rend()-> reverse_iterator
	{
		return _ht_Plait_detail::Boundary< Family<RGS...>, true, false, false >::Calc(_rg_fam);
	}

	template<class FNJ_FLAG>
	auto eval() const-> Array< typename cPlait_Range<RGS...>::value_type >
	{
		return _Array_Evaluation<FNJ_FLAG>::Calc(*this);  
	}

	operator Array< typename cPlait_Range<RGS...>::value_type >() const{  return eval();  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template
	<	class RG, class FUNC
	,	class _MRG 
		=	ht::Morph_Range< Alephless_t<RG>, Alephless_t<FUNC>, !is_immutable<RG>::value >
	,	bool _NXCT = noexcept( _MRG(Mock<RG&&>(), Mock<FUNC&&>()) )
	>
	static auto Morph(RG&& rg, FUNC&& func) noexcept(_NXCT)
	->	_MRG{  return {Forward<RG>(rg), Forward<FUNC>(func)};  }


	template
	<	class RG, class FUNC
	,	class _FRG 
		=	ht::Filter_Range< Alephless_t<RG>, Alephless_t<FUNC>, !is_immutable<RG>::value >
	,	bool _NXCT = noexcept( _FRG(Mock<RG&&>(), Mock<FUNC&&>()) )
	>
	static auto Filter(RG&& rg, FUNC&& func) noexcept(_NXCT)
	->	_FRG{  return {Forward<RG>(rg), Forward<FUNC>(func)};  }


	template
	<	class FNJ_FLAG = None, class RG, class FUNC, class INIT
	,	class _FM = ht::_Fold_impl<true, true, FNJ_FLAG>
	,	bool _NXCT = noexcept( _FM::Calc(Mock<RG&&>(), Mock<FUNC&&>(), Mock<INIT&&>()) )
	>
	static auto Fold(RG&& rg, FUNC&& func, INIT&& init) noexcept(_NXCT)-> SGM_DECLTYPE_AUTO
	(
		_FM::Calc( Forward<RG>(rg), Forward<FUNC>(func), Forward<INIT>(init) )
	)
	
	template
	<	class FNJ_FLAG = None, class RG, class FUNC
	,	class _FM = ht::_Fold_impl<false, true, FNJ_FLAG>
	,	bool _NXCT = noexcept( _FM::Calc(Mock<RG&&>(), Mock<FUNC&&>(), None{}) )
	>
	static auto Fold(RG&& rg, FUNC&& func) noexcept(_NXCT)-> SGM_DECLTYPE_AUTO
	(
		_FM::Calc( Forward<RG>(rg), Forward<FUNC>(func), None{} )
	)
	
	template
	<	class FNJ_FLAG = None, class RG, class FUNC, class INIT
	,	class _FM = ht::_Fold_impl<true, false, FNJ_FLAG>
	,	bool _NXCT = noexcept( _FM::Calc(Mock<RG&&>(), Mock<FUNC&&>(), Mock<INIT&&>()) )
	>
	static auto rFold(RG&& rg, FUNC&& func, INIT&& init) noexcept(_NXCT)-> SGM_DECLTYPE_AUTO
	(
		_FM::Calc( Forward<RG>(rg), Forward<FUNC>(func), Forward<INIT>(init) )
	)
	
	template
	<	class FNJ_FLAG = None, class RG, class FUNC
	,	class _FM = ht::_Fold_impl<false, false, FNJ_FLAG>
	,	bool _NXCT = noexcept( _FM::Calc(Mock<RG&&>(), Mock<FUNC&&>(), None{}) )
	>
	static auto rFold(RG&& rg, FUNC&& func) noexcept(_NXCT)-> SGM_DECLTYPE_AUTO
	(
		_FM::Calc( Forward<RG>(rg), Forward<FUNC>(func), None{} )
	)

	
	template<class...ARGS>
	static auto Plait(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	->	ht::Plait_Range< Alephless_t<ARGS>... >{  return {Forward<ARGS>(args)...};  }

	template<class...ARGS>
	static auto cPlait(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	->	ht::cPlait_Range< Alephless_t<ARGS>... >{  return {Forward<ARGS>(args)...};  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_HIGH_TEMPLAR_