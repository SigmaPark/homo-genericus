#pragma once

#ifndef _SGM_HIGH_TEMPLAR12_
#define _SGM_HIGH_TEMPLAR12_


namespace sgm
{
	namespace ht12
	{
		
		template<class ITR, class FUNC, class DECO>  class Morph_iterator;
		template<class FS, class RNG, class FUNC, bool ASK_MUTABLE>  class Morph_range;

		template<class ITR, class FUNC, class DECO>  class Filter_iterator;
		template<class FS, class RNG, class FUNC>  class Filter_range;

		template<class FS, class...RNGS>
		class Plait_range;

	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "..\interface_Traits\interface_Traits.hpp"
#include <cstdlib>


template<class ITR, class FUNC, class DECO>
class sgm::ht12::Morph_iterator
{
	using _Y = decltype( Declval<FUNC>()(*Declval<ITR>()) );
	using Y = typename DECO::template type<_Y>;

	using _Self = Morph_iterator;

public:
	template<  class ITR2, class = Enable_if_t< is_Convertible<ITR2, ITR>::value >  >
	Morph_iterator(ITR2 const xitr2, FUNC const &func) : _xitr(xitr2), _pfunc(&func){}


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	Morph_iterator(Morph_iterator<ITR2, FUNC, DECO> const mitr) 
	:	_xitr(mitr._xitr), _pfunc(mitr._pfunc){}


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	auto operator=(Morph_iterator<ITR2, FUNC, DECO> const mitr)
	->	_Self&{  return _xitr = mitr._xitr,  _pfunc = mitr._pfunc,  *this;  }


	auto operator*() const-> Y{  return (*_pfunc)(*_xitr);  }
	auto operator*()-> Y{  return (*_pfunc)(*_xitr);  }

	auto operator++()-> _Self&{  return ++*this;  }
	auto operator++(int)-> _Self{  _Self const itr = *this;  return ++*this,  itr;  }

	bool operator==(_Self const itr) const{  return _xitr == itr._xitr;  }
	bool operator!=(_Self const itr) const{  return !(*this == itr);  }


	auto operator--()-> _Self&{  return --*this;  }
	auto operator--(int)-> _Self{  _Self const itr = *this;  return --*this,  itr;  }


	auto operator[](ptrdiff_t const d) const-> Y{  return (*_pfunc)( *(_xitr + d) );  }
	auto operator[](ptrdiff_t const d)-> Y{  return (*_pfunc)( *(_xitr + d) );  }

	auto operator+(ptrdiff_t const d) const-> _Self{  return{_xitr + d, _pfunc};  }
	auto operator-(ptrdiff_t const d) const-> _Self{  return{_xitr - d, _pfunc};  }
	auto operator+=(ptrdiff_t const d)-> _Self&{  return _xitr += d,  *this;  }
	auto operator-=(ptrdiff_t const d)-> _Self&{  return _xitr -= d,  *this;  }

	auto operator-(_Self const itr) const-> ptrdiff_t{  return _xitr - itr._xitr;  }

	bool operator<(_Self const itr) const{  return _xitr < itr._xitr;  }
	bool operator>(_Self const itr) const{  return _xitr > itr._xitr;  }
	bool operator<=(_Self const itr) const{  return !(*this > itr);  }
	bool operator>=(_Self const itr) const{  return !(*this < itr);  }


	ITR _xitr;
	FUNC const *_pfunc;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FS, class RNG, class FUNC, bool ASK_MUTABLE>
class sgm::ht12::Morph_range
{
public:
	
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class ITR, class FUNC, class DECO>
class sgm::ht12::Filter_iterator
{
	using X = typename DECO::template type< decltype(*Declval<ITR>()) >;
	using _Self = Filter_iterator;

public:
	template<  class ITR2, class = Enable_if_t< is_Convertible<ITR2, ITR>::value >  >
	Filter_iterator(ITR2 const xitr2, FUNC const &func) 
	:	_xitr(xitr2), _pfunc(&func){  _shift_until_valid();  }


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	Filter_iterator(Filter_iterator<ITR2, FUNC, DECO> const fitr) 
	:	_xitr(fitr._xitr), _pfunc(fitr._pfunc){  _shift_until_valid();  }


	template
	<	class ITR2
	,	class = Enable_if_t< is_Convertible<ITR2, ITR>::value && !is_Same<ITR2, ITR>::value >  
	>
	auto operator=(Filter_iterator<ITR2, FUNC, DECO> const fitr)
	->	_Self&{  return _xitr = fitr._xitr,  _pfunc = fitr._pfunc,  *this;  }


	ITR _xitr;
	FUNC const *_pfunc;


private:
	void _shift_until_valid(){  for(;  !(*_pfunc)(*_xitr);  _xitr++);  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#



#endif // end of #ifndef _SGM_HIGH_TEMPLAR12_
