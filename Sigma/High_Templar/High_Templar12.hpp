#pragma once
#ifndef _SGM_HIGH_TEMPLAR12_
#define _SGM_HIGH_TEMPLAR12_


#include "..\Concurrency\Concurrency.hpp"
#include "..\Flags\Flags.hpp"
#include "..\Type_Decorator\Type_Decorator.hpp"
#include "..\Serial\Serial.hpp"
#include "..\Family\Family.hpp"


namespace sgm
{
	namespace ht
	{
		
		template<class ITR, class FN, class DECO>  class Morph_iterator;
		template<class FS, class RG, class FN, bool ASK_MUTABLE>  class Morph_range;

		template<class ITR, class FILT, class DECO>  class Filter_iterator;
		template<class FS, class RG, class FN, bool ASK_MUTABLE>  class Filter_range;


		template<class T, class maybe_FLAG_SET> struct _Decorated;

		template<class T, class FLAGSET>
		using _Decorated_t = typename _Decorated<T, FLAGSET>::type;


		enum : unsigned{ AUTO_OR = par::Nof_HW_Core::DYNAMIC };		
		enum class _impl_Mode{ SEQUANCIAL, MULTI_THREAD };

		template<unsigned NOF_TASK1 = AUTO_OR, unsigned NOF_TASK2 = NOF_TASK1>  struct Par;
		template<class maybe_Par> struct is_Par;


		template
		<	bool HAS_INIT, bool FORWARD, class FLAG_SET
		,	_impl_Mode
			=	is_Selected_Flag<par::_Parallel_Helper, FLAG_SET>::value
				?	_impl_Mode::MULTI_THREAD
				:	_impl_Mode::SEQUANCIAL
		> 
		struct _Fold_impl;

		struct _Fold_Helper;


		template<class...CS>  class cPlait_range;
		template<class...CS>  class Plait_range;

		template<class FAM, bool IS_MUTABLE>  class Plait_iterator;


		template
		<	class FLAG_SET
		,	_impl_Mode
			=	is_Selected_Flag<par::_Parallel_Helper, FLAG_SET>::value
				?	_impl_Mode::MULTI_THREAD
				:	_impl_Mode::SEQUANCIAL
		>
		struct _Serial_Evaluator;

	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, class> struct sgm::ht::_Decorated{  using type = T;  };

template<class T, class...FLAGS>
struct sgm::ht::_Decorated< T, sgm::FlagSet<FLAGS...> >
{
	using type = typename Decorated<T>::template by<FLAGS...>::type;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class maybe_Par>
struct sgm::ht::is_Par : is_inherited_from<maybe_Par, par::_Parallel_Helper>{};


/**	
*	<N, whatever> : N tasks
*	AUTO_OR, AUTO_OR : Auto detection or sequancial if failed. Default setting
*	AUTO_OR, N : Auto detection or N tasks if failed.
*	AUTO_OR, 0 : Auto detection or throw exception if failed.
*/
template<unsigned NOF_TASK1, unsigned NOF_TASK2>
struct sgm::ht::Par : par::Parallel<NOF_TASK1>{};


template<unsigned N>
struct sgm::ht::Par<sgm::ht::AUTO_OR, N> : par::Parallel<>
{
	template<class F>
	void operator()(size_t const nof_iteration, F&& func) const
	{
		if(par::Parallel<>::number_of_task() != 1)
			par::Parallel<>::operator()( nof_iteration, Forward<F>(func) );
		else
			par::Parallel<N>()( nof_iteration, Forward<F>(func) );
	}
};


template<>
struct sgm::ht::Par<sgm::ht::AUTO_OR, 0> 
:	par::Parallel<>{  Par() : par::Parallel<>(par::Nof_HW_Core::When_Fails::THROW){}  };
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FLAG_SET>
struct sgm::ht::_Serial_Evaluator<FLAG_SET, sgm::ht::_impl_Mode::SEQUANCIAL> : No_Making
{
	template<  class RG, class elem_t = Decay_t< decltype( *Begin(Declval<RG>()) ) >  >
	static auto calc(RG &&rg)-> Serial<elem_t>{  return {Begin(rg), End(rg)};  }
};


template<class FLAG_SET>
struct sgm::ht::_Serial_Evaluator<FLAG_SET, sgm::ht::_impl_Mode::MULTI_THREAD> : No_Making
{
	template<  class RG, class elem_t = Decay_t< decltype( *Begin(Declval<RG>()) ) >  >
	static auto calc(RG &&rg)-> Serial<elem_t>
	{
		size_t const nof_elem = Size(rg);

		if(nof_elem == 0)
			return {};
		else
		{
			Serial<elem_t> res( nof_elem, *Begin(rg) );

			Satisfying_Flag_t<is_Par, FLAG_SET>()
			(	nof_elem
			,	[&rg, &res](size_t idx_begin, size_t const idx_end, unsigned const)
				{
					for
					(	auto itr = Next( Begin(rg), idx_begin )
					;	idx_begin < idx_end
					;	res[idx_begin++] = *itr++
					);
				}
			);

			return res;
		}
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class ITR, class FN, class DECO>
class sgm::ht::Morph_iterator
{
private:
	using _Y = decltype( Declval<FN>()(*Declval<ITR>()) );
	using Y = typename DECO::template type<_Y>;

	using _Self = Morph_iterator;

public:
	template<  class ITR2, class = Enable_if_t< is_Convertible<ITR2, ITR>::value >  >
	Morph_iterator(ITR2 const xitr2, FN &fn) : _xitr(xitr2), _pfn(&fn){}


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	Morph_iterator(Morph_iterator<ITR2, FN, DECO> const mitr) 
	:	_xitr(mitr._xitr), _pfn(mitr._pfn){}


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	auto operator=(Morph_iterator<ITR2, FN, DECO> const mitr)
	->	_Self&{  return _xitr = mitr._xitr,  _pfn = mitr._pfn,  *this;  }


	auto operator*() const-> Y{  return (*_pfn)(*_xitr);  }
	auto operator*()-> Y{  return (*_pfn)(*_xitr);  }

	auto operator++()-> _Self&{  return ++_xitr,  *this;  }
	auto operator++(int)-> _Self{  _Self const itr = *this;  return ++*this,  itr;  }

	bool operator==(_Self const itr) const{  return _xitr == itr._xitr;  }
	bool operator!=(_Self const itr) const{  return !(*this == itr);  }


	auto operator--()-> _Self&{  return --_xitr,  *this;  }
	auto operator--(int)-> _Self{  _Self const itr = *this;  return --*this,  itr;  }


	auto operator[](ptrdiff_t const d) const-> Y{  return (*_pfn)( *(_xitr + d) );  }
	auto operator[](ptrdiff_t const d)-> Y{  return (*_pfn)( *(_xitr + d) );  }

	auto operator+(ptrdiff_t const d) const-> _Self{  return{_xitr + d, *_pfn};  }
	auto operator-(ptrdiff_t const d) const-> _Self{  return{_xitr - d, *_pfn};  }
	auto operator+=(ptrdiff_t const d)-> _Self&{  return _xitr += d,  *this;  }
	auto operator-=(ptrdiff_t const d)-> _Self&{  return _xitr -= d,  *this;  }

	auto operator-(_Self const itr) const-> ptrdiff_t{  return _xitr - itr._xitr;  }

	bool operator<(_Self const itr) const{  return _xitr < itr._xitr;  }
	bool operator>(_Self const itr) const{  return _xitr > itr._xitr;  }
	bool operator<=(_Self const itr) const{  return !(*this > itr);  }
	bool operator>=(_Self const itr) const{  return !(*this < itr);  }


	ITR _xitr;
	FN *_pfn;
};


template<class ITR, class...ARGS>
struct sgm::is_random_access_iterator< sgm::ht::Morph_iterator<ITR, ARGS...> >
:	is_random_access_iterator<ITR>{};


template<class ITR, class...ARGS>
struct sgm::is_bidirectional_iterator< sgm::ht::Morph_iterator<ITR, ARGS...> >
:	is_bidirectional_iterator<ITR>{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FS, class RG, class FN, bool ASK_MUTABLE>
class sgm::ht::Morph_range
{
private:
	struct _Deco : No_Making{  template<class Q>  using type = _Decorated_t<Q, FS>;  };

	using _cxitr_t = Decay_t< decltype( Begin(Declval<RG const>()) ) >;
	using _crxitr_t = Decay_t< decltype(Declval<RG const>().rbegin()) >;	
	
	using _xitr_t
	=	Selective_t<  ASK_MUTABLE, Decay_t< decltype( Begin(Declval<RG>()) ) >, _cxitr_t  >;

	using _rxitr_t
	=	Selective_t<  ASK_MUTABLE, Decay_t< decltype(Declval<RG>().rbegin()) >, _crxitr_t  >;

	using _norefFN_t = Referenceless_t<FN>;

	using _cmitr_t = Morph_iterator<_cxitr_t, _norefFN_t const, _Deco>;
	using _crmitr_t = Morph_iterator<_crxitr_t, _norefFN_t const, _Deco>;
	using _mitr_t = Morph_iterator<_xitr_t, _norefFN_t, _Deco>;
	using _rmitr_t = Morph_iterator<_rxitr_t, _norefFN_t, _Deco>;

public:
	using value_type = Decay_t<decltype(*Declval<_mitr_t>())>;

	Morph_range(RG rg, FN fn) : _rg( Forward<RG>(rg) ), _fn( Forward<FN>(fn) ){}

	auto cbegin() const-> _cmitr_t{  return {Begin(_rg), _fn};  }
	auto begin() const-> SGM_DECLTYPE_AUTO(  cbegin()  )
	auto begin()-> _mitr_t{  return {Begin(_rg), _fn};  }

	auto cend() const-> _cmitr_t{  return {End(_rg), _fn};  }
	auto end() const-> SGM_DECLTYPE_AUTO(  cend()  )
	auto end()-> _mitr_t{  return {End(_rg), _fn};  }

	auto crbegin() const-> _crmitr_t{  return {_rg.rbegin(), _fn};  }
	auto rbegin() const-> SGM_DECLTYPE_AUTO(  crbegin()  )
	auto rbegin()-> _rmitr_t{  return {_rg.rbegin(), _fn};  }

	auto crend() const-> _crmitr_t{  return {_rg.rend(), _fn};  }
	auto rend() const-> SGM_DECLTYPE_AUTO(  crend()  )
	auto rend()-> _rmitr_t{  return {_rg.rend(), _fn};  }


	template<class...FLAGS>
	auto eval() const
	->	Serial<value_type>{  return _Serial_Evaluator< FlagSet<FLAGS...> >::calc(*this);  }

	operator Serial<value_type>() const{  return eval();  }

private:
	RG _rg;
	FN _fn;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class ITR, class FILT, class DECO>
class sgm::ht::Filter_iterator
{
private:
	using X = typename DECO::template type< decltype(*Declval<ITR>()) >;
	using _Self = Filter_iterator;

public:
	template<  class ITR2, class = Enable_if_t< is_Convertible<ITR2, ITR>::value >  >
	Filter_iterator(ITR2 const xitr2, FILT const filt)
	:	_xitr(xitr2), _filt(filt){  _shift_until_valid();  }


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	Filter_iterator(Filter_iterator<ITR2, FILT, DECO> const fitr) 
	:	_xitr(fitr._xitr), _filt(fitr._filt){  _shift_until_valid();  }


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	auto operator=(Filter_iterator<ITR2, FILT, DECO> const fitr)
	->	_Self&{  return _xitr = fitr._xitr,  _filt = fitr._filt,  *this;  }


	auto operator*() const-> X{  return *_xitr;  }
	auto operator*()-> X{  return *_xitr;  }

	auto operator++()-> _Self&{  return ++_xitr,  _shift_until_valid(),  *this;  }
	auto operator++(int)-> _Self{  _Self const itr = *this;  return ++*this,  itr;  }

	bool operator==(_Self const fitr) const{  return _xitr == fitr._xitr;  }
	bool operator!=(_Self const fitr) const{  return !(*this == fitr);  }


	ITR _xitr;
	FILT _filt;

private:
	void _shift_until_valid(){  for(;  !_filt(_xitr);  _xitr++);  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FS, class RG, class FN, bool ASK_MUTABLE>
class sgm::ht::Filter_range
{
private:
	struct _Deco : No_Making{  template<class Q>  using type = _Decorated_t<Q, FS>;  };

	using _cxitr_t = Decay_t< decltype( Begin(Declval<RG const>()) ) >;
	using _crxitr_t = Decay_t< decltype(Declval<RG const>().rbegin()) >;	

	using _xitr_t
	=	Selective_t<  ASK_MUTABLE, Decay_t< decltype( Begin(Declval<RG>()) ) >, _cxitr_t  >;

	using _rxitr_t
	=	Selective_t<  ASK_MUTABLE, Decay_t< decltype(Declval<RG>().rbegin()) >, _crxitr_t  >;


	template<class itr_t>  class _Filter_FN;

	using _cfitr_t = Filter_iterator< _cxitr_t, _Filter_FN<_cxitr_t>, _Deco >;
	using _crfitr_t = Filter_iterator< _crxitr_t, _Filter_FN<_crxitr_t>, _Deco >;
	using _fitr_t = Filter_iterator< _xitr_t, _Filter_FN<_xitr_t>, _Deco >;
	using _rfitr_t = Filter_iterator< _rxitr_t, _Filter_FN<_rxitr_t>, _Deco >;

public:
	using value_type = Decay_t<decltype(*Declval<_xitr_t>())>;

	Filter_range(RG rg, FN fn) : _rg( Forward<RG>(rg) ), _fn( Forward<FN>(fn) ){}

	auto cbegin() const-> _cfitr_t{  return { Begin(_rg), _filt_fn(this) };  }
	auto begin() const-> SGM_DECLTYPE_AUTO(  cbegin()  )
	auto begin()-> _fitr_t{  return { Begin(_rg), _filt_fn(this) };  }

	auto cend() const-> _cfitr_t{  return { End(_rg), _filt_fn(this) };  }
	auto end() const-> SGM_DECLTYPE_AUTO(  cend()  )
	auto end()->	_fitr_t{  return { End(_rg), _filt_fn(this) };  }

	auto crbegin() const->	_crfitr_t{  return { _rg.rbegin(), _rfilt_fn(this) };  }
	auto rbegin() const-> SGM_DECLTYPE_AUTO(  crbegin()  )
	auto rbegin()-> _rfitr_t{  return { _rg.rbegin(), _rfilt_fn(this) };  }

	auto crend() const-> _crfitr_t{  return { _rg.rend(), _rfilt_fn(this) };  }
	auto rend() const-> SGM_DECLTYPE_AUTO(  crend()  )
	auto rend()-> _rfitr_t{  return { _rg.rend(), _rfilt_fn(this) };  }

	auto eval() const-> Serial<value_type>{  return {begin(), end()};  }
	operator Serial<value_type>() const{  return eval();  }

private:
	RG _rg;
	FN _fn;


	template<  class ME, class itr_t = Decay_t< decltype( End(Declval<ME>()._rg) ) >  >
	static auto _filt_fn(ME *p)-> _Filter_FN<itr_t>{  return {&p->_fn, End(p->_rg)};  }

	template<  class ME, class itr_t = Decay_t< decltype(Declval<ME>()._rg.rend()) >  >
	static auto _rfilt_fn(ME *p)-> _Filter_FN<itr_t>{  return {&p->_fn, p->_rg.rend()};  }
};


template<class FS, class RG, class FN, bool ASK_MUTABLE>  template<class itr_t>
class sgm::ht::Filter_range<FS, RG, FN, ASK_MUTABLE>::_Filter_FN
{
private:
	using _pFN_t = Referenceless_t<FN>*;

public:
	_Filter_FN(_pFN_t const pfn, itr_t const eitr) : _pfn(pfn), _eitr(eitr){}

	auto operator()(itr_t const itr) const-> bool{  return itr == _eitr || (*_pfn)(*itr);  }
	auto operator()(itr_t const itr)-> bool{  return itr == _eitr || (*_pfn)(*itr);  }

private:
	_pFN_t _pfn;
	itr_t _eitr;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::ht::_Fold_Helper : No_Making
{
protected:
	template<class ITR, class FUNC, class res_t>
	static auto Accumulate(ITR itr, ITR const ei, FUNC &&func, res_t res)-> res_t
	{
		while(itr != ei)
			res = func(res, *itr++);

		return res;
	}


	template<bool FORWARD> struct itrMethod;

	template<>
	struct itrMethod<true>
	{
		template<class RG> static auto begin(RG &&rg)-> SGM_DECLTYPE_AUTO(  Begin(rg)  )
		template<class RG> static auto end(RG &&rg)-> SGM_DECLTYPE_AUTO(  End(rg)  )
	};

	template<>
	struct itrMethod<false>
	{
		template<class RG> static auto begin(RG &&rg)-> SGM_DECLTYPE_AUTO(  rg.rbegin()  )
		template<class RG> static auto end(RG &&rg)-> SGM_DECLTYPE_AUTO(  rg.rend()  )
	};
};


template<bool FWD, class FS>
struct sgm::ht::_Fold_impl<true, FWD, FS, sgm::ht::_impl_Mode::SEQUANCIAL> : _Fold_Helper
{
	template<class RG, class FUNC, class res_t>
	static auto calc(RG &&rg, FUNC &&func, res_t &&res)-> res_t
	{
		return
		Accumulate
		(	itrMethod<FWD>::begin(rg), itrMethod<FWD>::end(rg)
		,	Forward<FUNC>(func), Forward<res_t>(res)
		);
	}
};


template<bool FWD, class FS>
struct sgm::ht::_Fold_impl<false, FWD, FS, sgm::ht::_impl_Mode::SEQUANCIAL> : _Fold_Helper
{
	template<class RG, class FUNC>
	static auto calc(RG &&rg, FUNC &&func, None)
	->	Decay_t< decltype( *itrMethod<FWD>::begin(rg) ) >
	{
		auto const bi = itrMethod<FWD>::begin(rg), ei = itrMethod<FWD>::end(rg);

		assert(bi != ei && L"the container has nothing to fold.\n");

		return Accumulate( Next(bi), ei, Forward<FUNC>(func), *bi );
	}
};


template<bool FWD, class FS>
struct sgm::ht::_Fold_impl<false, FWD, FS, sgm::ht::_impl_Mode::MULTI_THREAD> : _Fold_Helper
{
private:
	template<class RG, class FUNC>
	static auto _Seq_Fold(RG &&rg, FUNC &&func)-> SGM_DECLTYPE_AUTO
	(
		_Fold_impl<false, FWD, FS, _impl_Mode::SEQUANCIAL>::calc
		(	Forward<RG>(rg), Forward<FUNC>(func), none
		)		
	)


public:
	template
	<	class RG, class FUNC
	,	class res_t = Decay_t<  decltype( *Begin(Declval< Decay_t<RG> >()) )  >
	>
	static auto calc(RG &&rg, FUNC &&func, None)-> res_t
	{
		auto const tasker = Satisfying_Flag_t<is_Par, FS>();
		auto const nof_elem = Size(rg);

		if(nof_elem <= tasker.number_of_task())
			return _Seq_Fold( Forward<RG>(rg), Forward<FUNC>(func) );
		else
		{
			Serial<res_t> sum( static_cast<size_t>(tasker.number_of_task()), *Begin(rg) );

			tasker
			(	nof_elem
			,	[&rg, &func, &sum]
				(	size_t const idx_begin, size_t const idx_end, unsigned const task_id
				)
				{	
					auto const 
						bi = Next( Begin(rg), idx_begin ),
						bi_1 = Next(bi), 
						ei = Next(bi, idx_end - idx_begin);
					 
					sum[task_id] = Accumulate(bi_1, ei, func, *bi);
				}
			);

			return _Seq_Fold( sum, Forward<FUNC>(func) );
		}
	}
};


template<bool FWD, class FS>
struct sgm::ht::_Fold_impl<true, FWD, FS, sgm::ht::_impl_Mode::MULTI_THREAD> : _Fold_Helper
{
private:
	template<bool> struct _Last_fold;

	template<>
	struct _Last_fold<true> : No_Making
	{
		template<class FUNC, class T1, class T2>
		static auto calc(FUNC &&func, T1 &&t1, T2 &&t2)
		->	SGM_DECLTYPE_AUTO(  func( Forward<T1>(t1), Forward<T2>(t2) )  )
	};

	template<>
	struct _Last_fold<false> : No_Making
	{
		template<class FUNC, class T1, class T2>
		static auto calc(FUNC &&func, T1 &&t1, T2 &&t2)
		->	SGM_DECLTYPE_AUTO(  func( Forward<T2>(t2), Forward<T1>(t1) )  )
	};


public:
	template<class RG, class FUNC, class res_t>
	static auto calc(RG &&rg, FUNC &&func, res_t &&res)-> res_t
	{
		static_assert
		(	is_Convertible< res_t, Decay_t<decltype( *Begin(rg) )> >::value
		,	"for parallelization, folding function should be asocciative."
		);

		return 
		_Last_fold<FWD>::calc
		(	func, Forward<res_t>(res)
		,	_Fold_impl<false, FWD, FS, _impl_Mode::MULTI_THREAD>::calc
			(	Forward<RG>(rg), func, none
			)
		);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FAM, bool IS_MUTABLE>  
class sgm::ht::Plait_iterator
{
private:
	mutable FAM _itr_fam;


	template< size_t _S = std::tuple_size<FAM>::value >
	struct _Deref
	{
		using _fam_t = Selective_t<IS_MUTABLE, FAM, FAM const>;

		template<class ME, class...ARGS>
		static auto calc(ME &me, ARGS&&...args)-> SGM_DECLTYPE_AUTO
		(
			_Deref<_S-1>::calc
			(	me
			,	*std::get<_S-1>( static_cast<_fam_t>(me._itr_fam) )
			,	Forward<ARGS>(args)...
			)
		)
	};

	template<>
	struct _Deref<0>
	{
		template<class ME, class...ARGS>
		static auto calc(ME&, ARGS&&...args)
		->	Family< Alephless_t<ARGS>... >{  return { Forward<ARGS>(args)... };  }
	};


	template< bool PLUS, size_t _S = std::tuple_size<FAM>::value, bool = _S != 0  >  
	struct _Step;

	template<bool PLUS>
	struct _Step<PLUS, 0, false>
	{
		static auto calc(Plait_iterator &me)-> SGM_DECLTYPE_AUTO(  me  )
	};

	template<size_t _S>
	struct _Step<true, _S, true>
	{
		static auto calc(Plait_iterator &me)-> Plait_iterator&
		{  
			return std::get<_S-1>(me._itr_fam)++,  _Step<true, _S-1>::calc(me);  
		}
	};

	template<size_t _S>
	struct _Step<false, _S, true>
	{
		static auto calc(Plait_iterator &me)-> Plait_iterator&
		{  
			return std::get<_S-1>(me._itr_fam)--,  _Step<false, _S-1>::calc(me);
		}
	};


public:
	static_assert(is_Family<FAM>::value, "FAM should be sgm::Family type.");

	Plait_iterator(FAM fam) : _itr_fam(fam){}

	auto operator*() const-> SGM_DECLTYPE_AUTO( _Deref<>::calc(*this)  )
	auto operator*()-> SGM_DECLTYPE_AUTO(  _Deref<>::calc(*this)  )

	auto operator++()-> Plait_iterator&{  return _Step<true>::calc(*this);  }
	auto operator--()-> Plait_iterator&{  return _Step<false>::calc(*this);  }
	auto operator++(int)-> Plait_iterator{  auto const clone = *this;  return ++*this,  clone;  }
	auto operator--(int)-> Plait_iterator{  auto const clone = *this;  return --*this,  clone;  }

	auto operator!=(Plait_iterator const zi) const
	->	bool{  return std::get<0>(this->_itr_fam) != std::get<0>(zi._itr_fam);  }

	auto operator==(Plait_iterator const zi) const-> bool{  return !(*this != zi);  }
};


template<class...CS>  
class sgm::ht::cPlait_range
{
protected:
	Family<CS...> _fam;

	template<bool WANT_BEGIN, size_t _S = sizeof...(CS), bool = _S != 0>  struct _Get_iter;

private:
	size_t _size;


	using _itr_fam_t = Family<  Decay_t< decltype( Begin(Declval<CS const>()) ) >...  >;
	using _iter_t = Plait_iterator<_itr_fam_t, false>;



	template<bool WB>
	struct _Get_iter<WB, 0, false>
	{
		template<class ME, class...ARGS>
		static auto of(ME&, ARGS&&...args)
		->	SGM_DECLTYPE_AUTO(  Make_Family( Forward<ARGS>(args)... )  )
	};

	template<size_t _S>
	struct _Get_iter<true, _S, true>
	{
		template<class ME, class...ARGS>
		static auto of(ME &me, ARGS&&...args)-> SGM_DECLTYPE_AUTO
		(
			_Get_iter<true, _S-1>::of
			(	me, Begin( std::get<_S-1>(me._fam) ), Forward<ARGS>(args)...
			)
		)
	};

	template<size_t _S>
	struct _Get_iter<false, _S, true>
	{
		template<class ME, class...ARGS>
		static auto of(ME &me, ARGS&&...args)-> SGM_DECLTYPE_AUTO
		(
			_Get_iter<false, _S-1>::of
			(	me, End( std::get<_S-1>(me._fam) ), Forward<ARGS>(args)...
			)
		)
	};


	template<class RG>
	struct _is_random_access_range
	:	is_random_access_iterator< decltype( Begin(Declval<RG>()) ) >{};


	template
	<	bool = sizeof...(CS) != 0
	,	bool = Check_All<_is_random_access_range>::template for_any<CS...>::value
	>
	struct _Get_Size;


	template<>
	struct _Get_Size<false, false>
	{
		template<class ME>  static auto of(ME&)-> size_t{  return 0;  }
	};

	template<>
	struct _Get_Size<true, false>
	{
		template<class ME>
		static auto of(ME &me)-> size_t{  return Size( std::get<0>(me._fam) );  }
	};

	template<>
	struct _Get_Size<true, true>
	{
	private:
		using _random_access_range_t
		=	typename Satisfying<_is_random_access_range>::template among<CS...>::type;

	public:
		template<class ME>
		static auto of(ME &me)
		->	size_t{  return Size( std::get<_random_access_range_t>(me._fam) );  }
	};


public:
	static_assert
	(	Check_All<is_iterable>::template for_all<CS...>::value, "iterable types are expected."
	);


	cPlait_range(CS...args) : _fam(args...), _size( _Get_Size<>::of(*this) ){}

	auto cbegin() const-> _iter_t{  return _Get_iter<true>::of(*this);  }
	auto begin() const{  return cbegin();  }

	auto cend() const-> _iter_t{  return _Get_iter<false>::of(*this);  }
	auto end() const{  return cend();  }

	auto size() const{  return _size;  }


	using value_type = Decay_t<decltype(*Declval<_iter_t>())>;

	template<class...FLAGS>
	auto eval() const
	->	Serial<value_type>{  return _Serial_Evaluator< FlagSet<FLAGS...> >::calc(*this);  }

	operator Serial<value_type>() const{  return eval();  }
};


template<class...CS>
class sgm::ht::Plait_range : public cPlait_range<CS...>
{
	using _cz = cPlait_range<CS...>;
	using _itr_fam_t = Family<  Decay_t< decltype( Begin(Declval<CS>()) ) >...  >;
	using _iter_t = Plait_iterator<_itr_fam_t, true>;

public:
	Plait_range(CS...args) : _cz(args...){}
	
	auto begin()-> _iter_t{  return _cz::template _Get_iter<true>::of(*this);  }
	auto end()-> _iter_t{  return _cz::template _Get_iter<false>::of(*this);  }


	using value_type = Decay_t<decltype(*Declval<_iter_t>())>;

	template<class...FLAGS>
	auto eval() const
	->	Serial<value_type>{  return _Serial_Evaluator< FlagSet<FLAGS...> >::calc(*this);  }

	operator Serial<value_type>() const{  return eval();  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{
	namespace ht
	{

		template<class...FLAGS, class RG, class FN>
		static auto Morph(RG &&rg, FN &&fn)
		->	Morph_range
			<	FlagSet<FLAGS...>, Alephless_t<RG>, Alephless_t<FN>, !is_immutable<RG>::value
			>
		{
			return {Forward<RG>(rg), Forward<FN>(fn)};
		}


		template<class...FLAGS, class RG, class FN>
		static auto Filter(RG &&rg, FN &&fn)
		->	Filter_range
			<	FlagSet<FLAGS...>, Alephless_t<RG>, Alephless_t<FN>, !is_immutable<RG>::value
			>
		{
			return {Forward<RG>(rg), Forward<FN>(fn)};
		}	
		

		template<class...FLAGS, class RG, class FUNC, class init_t>
		static auto Fold(RG &&rg, FUNC &&func, init_t &&init)-> SGM_DECLTYPE_AUTO
		(
			_Fold_impl< true, true, FlagSet<FLAGS...> >::calc
			(	Forward<RG>(rg), Forward<FUNC>(func), Forward<init_t>(init)
			)
		)
		
		template<class...FLAGS, class RG, class FUNC>
		static auto Fold(RG &&rg, FUNC &&func)-> SGM_DECLTYPE_AUTO
		(
			_Fold_impl< false, true, FlagSet<FLAGS...> >::calc
			(	Forward<RG>(rg), Forward<FUNC>(func), none
			)
		)
		
		
		template<class...FLAGS, class RG, class FUNC, class init_t>
		static auto rFold(RG &&rg, FUNC &&func, init_t &&init)-> SGM_DECLTYPE_AUTO
		(
			_Fold_impl< true, false, FlagSet<FLAGS...> >::calc
			(	Forward<RG>(rg), Forward<FUNC>(func), Forward<init_t>(init)
			)
		)
		
		template<class...FLAGS, class RG, class FUNC>
		static auto rFold(RG &&rg, FUNC &&func)-> SGM_DECLTYPE_AUTO
		(
			_Fold_impl< false, false, FlagSet<FLAGS...> >::calc
			(	Forward<RG>(rg), Forward<FUNC>(func), none
			)
		)


		template<class...ARGS>
		static auto Plait(ARGS&&...args)-> SGM_DECLTYPE_AUTO
		(
			Plait_range< Alephless_t<ARGS>... >( Forward<ARGS>(args)... )  
		)

		template<class...ARGS>
		static auto cPlait(ARGS&&...args)-> SGM_DECLTYPE_AUTO
		(
			cPlait_range< Alephless_t<ARGS>... >( Forward<ARGS>(args)... )
		)

	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#include <iterator>


template<class ITR, class FN, class DECO>
struct std::iterator_traits< sgm::ht::Morph_iterator<ITR, FN, DECO> >
{
private:
	using _mitr_t = sgm::ht::Morph_iterator<ITR, FN, DECO>;
	using _deref_t = decltype(*sgm::Declval<_mitr_t>());

public:
	using iterator_category
	=	sgm::Selective_t
		<	sgm::is_random_access_iterator<ITR>::value
		,	random_access_iterator_tag
		,	sgm::Selective_t
			<	sgm::is_bidirectional_iterator<ITR>::value
			,	bidirectional_iterator_tag
			,	forward_iterator_tag
			>
		>;

	using value_type = sgm::Decay_t<_deref_t>;
	using difference_type = ptrdiff_t;

	using pointer 
	=	sgm::Selective_t< sgm::is_immutable<_deref_t>::value, value_type const, value_type >*;
	
	using reference = sgm::Pointerless_t<pointer>&;
};


template<class ITR, class FN, class DECO>
struct std::iterator_traits< sgm::ht::Filter_iterator<ITR, FN, DECO> >
{
private:
	using _fitr_t = sgm::ht::Filter_iterator<ITR, FN, DECO>;
	using _deref_t = decltype(*sgm::Declval<_fitr_t>());

public:
	using iterator_category = forward_iterator_tag;
	using value_type = sgm::Decay_t<_deref_t>;
	using difference_type = ptrdiff_t;
	
	using pointer
	=	sgm::Selective_t< sgm::is_immutable<_deref_t>::value, value_type const, value_type >*;

	using reference = sgm::Pointerless_t<pointer>&;
};


template<bool M, class FAM>
struct std::iterator_traits< sgm::ht::Plait_iterator<FAM, M> >
{
private:
	struct _FAM_trait;

public:
	using iterator_category = typename _FAM_trait::template tag_t<FAM>;
	using value_type 
	=	sgm::Decay_t<  decltype(*sgm::Declval< sgm::ht::Plait_iterator<FAM, M> >())  >;
	
	using difference_type = ptrdiff_t;
	using pointer = sgm::Selective_t<M, value_type, value_type const>*;
	using reference = sgm::Pointerless_t<pointer>&;	
};


template<bool M, class FAM>
struct std::iterator_traits< sgm::ht::Plait_iterator<FAM, M> >::_FAM_trait : sgm::No_Making
{
	template<class...ITRS>
	using _tag_t
	=	sgm::Selective_t
		<	sgm::Check_All<sgm::is_random_access_iterator>::template for_all<ITRS...>::value 
		,	std::random_access_iterator_tag
		,	sgm::Selective_t
			<	sgm::Check_All<sgm::Has_Operator_Post_Decrease>::template for_all<ITRS...>::value
			,	std::bidirectional_iterator_tag
			,	std::forward_iterator_tag
			>
		>;	

	template
	<	class...ITRS
	,	class
		=	sgm::Guaranteed_t
			<	sgm::Check_All<sgm::is_iterator>::template for_all<ITRS...>::value 
			>
	>
	/* Declaration Only */ static auto _tag(sgm::Family<ITRS...>) noexcept-> _tag_t<ITRS...>;


	template<class _FAM>  using tag_t = decltype( _tag(sgm::Declval<_FAM>()) );	
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_HIGH_TEMPLAR12_
