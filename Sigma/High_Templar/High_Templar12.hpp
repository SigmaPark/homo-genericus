#pragma once

#ifndef _SGM_HIGH_TEMPLAR12_
#define _SGM_HIGH_TEMPLAR12_


#include "..\Flags\Flags.hpp"
#include "..\Type_Decorator\Type_Decorator.hpp"
#include "..\Serial\Serial.hpp"
#include <cstdlib>


namespace sgm
{
	namespace ht12
	{
		
		template<class ITR, class FN, class DECO>  class Morph_iterator;
		template<class FS, class RG, class FN, bool ASK_MUTABLE>  class Morph_range;

		template<class ITR, class FILT, class DECO>  class Filter_iterator;
		template<class FS, class RG, class FN, bool ASK_MUTABLE>  class Filter_range;

		template<class FS, class...RGS>
		class Plait_range;


		template<class T, class maybe_FLAG_SET> struct _Decorated;

		template<class T, class FLAGSET>
		using _Decorated_t = typename _Decorated<T, FLAGSET>::type;

	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, class> struct sgm::ht12::_Decorated{  using type = T;  };

template<class T, class...FLAGS>
struct sgm::ht12::_Decorated< T, sgm::FlagSet<FLAGS...> >
{
	using type = typename Decorated<T>::template by<FLAGS...>::type;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class ITR, class FN, class DECO>
class sgm::ht12::Morph_iterator
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

	auto operator+(ptrdiff_t const d) const-> _Self{  return{_xitr + d, _pfn};  }
	auto operator-(ptrdiff_t const d) const-> _Self{  return{_xitr - d, _pfn};  }
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
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FS, class RG, class FN, bool ASK_MUTABLE>
class sgm::ht12::Morph_range
{
private:
	struct _Deco : No_Making{  template<class Q>  using type = _Decorated_t<Q, FS>;  };

	using _cxitr_t = Decay_t< decltype(Declval<RG const>().begin()) >;
	using _crxitr_t = Decay_t< decltype(Declval<RG const>().rbegin()) >;	
	
	using _xitr_t
	=	Selective_t<  ASK_MUTABLE, Decay_t< decltype(Declval<RG>().begin()) >, _cxitr_t  >;

	using _rxitr_t
	=	Selective_t<  ASK_MUTABLE, Decay_t< decltype(Declval<RG>().rbegin()) >, _crxitr_t  >;

	using _norefFN_t = Referenceless_t<FN>;

	using _cmitr_t = Morph_iterator<_cxitr_t, _norefFN_t, _Deco>;
	using _crmitr_t = Morph_iterator<_crxitr_t, _norefFN_t, _Deco>;
	using _mitr_t = Morph_iterator<_xitr_t, _norefFN_t, _Deco>;
	using _rmitr_t = Morph_iterator<_rxitr_t, _norefFN_t, _Deco>;

public:
	using value_type = Decay_t<decltype(*Declval<_mitr_t>())>;

	Morph_range(RG rg, FN fn) : _rg( Forward<RG>(rg) ), _fn( Forward<FN>(fn) ){}

	auto cbegin() const-> _cmitr_t{  return {_rg.begin(), _fn};  }
	auto begin() const-> SGM_DECLTYPE_AUTO(  cbegin()  )
	auto begin()-> _mitr_t{  return {_rg.begin(), _fn};  }

	auto cend() const-> _cmitr_t{  return {_rg.end(), _fn};  }
	auto end() const-> SGM_DECLTYPE_AUTO(  cend()  )
	auto end()-> _mitr_t{  return {_rg.end(), _fn};  }

	auto crbegin() const-> _crmitr_t{  return {_rg.rbegin(), _fn};  }
	auto rbegin() const-> SGM_DECLTYPE_AUTO(  crbegin()  )
	auto rbegin()-> _rmitr_t{  return {_rg.rbegin(), _fn};  }

	auto crend() const-> _crmitr_t{  return {_rg.rend(), _fn};  }
	auto rend() const-> SGM_DECLTYPE_AUTO(  crend()  )
	auto rend()-> _rmitr_t{  return {_rg.rend(), _fn};  }

	auto eval() const-> Serial<value_type>{  return {begin(), end()};  }
	operator Serial<value_type>() const{  return eval();  }

private:
	RG _rg;
	FN _fn;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class ITR, class FILT, class DECO>
class sgm::ht12::Filter_iterator
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
class sgm::ht12::Filter_range
{
private:
	struct _Deco : No_Making{  template<class Q>  using type = _Decorated_t<Q, FS>;  };

	using _cxitr_t = Decay_t< decltype(Declval<RG const>().begin()) >;
	using _crxitr_t = Decay_t< decltype(Declval<RG const>().rbegin()) >;	

	using _xitr_t
	=	Selective_t<  ASK_MUTABLE, Decay_t< decltype(Declval<RG>().begin()) >, _cxitr_t  >;

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

	auto cbegin() const-> _cfitr_t{  return { _rg.begin(), _filt_fn(this) };  }
	auto begin() const-> SGM_DECLTYPE_AUTO(  cbegin()  )
	auto begin()-> _fitr_t{  return { _rg.begin(), _filt_fn(this) };  }

	auto cend() const-> _cfitr_t{  return { _rg.end(), _filt_fn(this) };  }
	auto end() const-> SGM_DECLTYPE_AUTO(  cend()  )
	auto end()->	_fitr_t{  return { _rg.end(), _filt_fn(this) };  }

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


	template<  class ME, class itr_t = Decay_t< decltype(Declval<ME>()._rg.end()) >  >
	static auto _filt_fn(ME *p)-> _Filter_FN<itr_t>{  return {&p->_fn, p->_rg.end()};  }

	template<  class ME, class itr_t = Decay_t< decltype(Declval<ME>()._rg.rend()) >  >
	static auto _rfilt_fn(ME *p)-> _Filter_FN<itr_t>{  return {&p->_fn, p->_rg.rend()};  }
};


template<class FS, class RG, class FN, bool ASK_MUTABLE>  template<class itr_t>
class sgm::ht12::Filter_range<FS, RG, FN, ASK_MUTABLE>::_Filter_FN
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


namespace sgm
{
	namespace ht12
	{

		template<class...FLAGS, class RG, class FN>
		static auto Morph(RG &&rg, FN &&fn)
		->	Morph_range< FlagSet<FLAGS...>, RG&&, FN&&, !is_immutable<RG>::value >
		{
			return {Forward<RG>(rg), Forward<FN>(fn)};
		}


		template<class...FLAGS, class RG, class FN>
		static auto Filter(RG &&rg, FN &&fn)
		->	Filter_range< FlagSet<FLAGS...>, RG&&, FN&&, !is_immutable<RG>::value >
		{
			return {Forward<RG>(rg), Forward<FN>(fn)};
		}		

	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#include <iterator>


template<class ITR, class FN, class DECO>
struct std::iterator_traits< sgm::ht12::Morph_iterator<ITR, FN, DECO> >
{
private:
	using _mitr_t = sgm::ht12::Morph_iterator<ITR, FN, DECO>;
	using _deref_t = decltype(*sgm::Declval<_mitr_t>().begin());

public:
	using iterator_category
	=	sgm::Selective_t
		<	sgm::is_random_access_iterator<ITR>::value
		,	random_access_iterator_tag
		,	sgm::Selective_t
			<	sgm::Has_Operator_Post_Decrease<ITR>::value
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
struct std::iterator_traits< sgm::ht12::Filter_iterator<ITR, FN, DECO> >
{
private:
	using _fitr_t = sgm::ht12::Filter_iterator<ITR, FN, DECO>;
	using _deref_t = decltype(*sgm::Declval<_fitr_t>().begin());

public:
	using iterator_category = forward_iterator_tag;
	using value_type = sgm::Decay_t<_deref_t>;
	using difference_type = ptrdiff_t;
	
	using pointer
	=	sgm::Selective_t< sgm::is_immutable<_deref_t>::value, value_type const, value_type >*;

	using reference = sgm::Pointerless_t<pointer>&;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_HIGH_TEMPLAR12_
