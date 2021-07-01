#pragma once

#ifndef _SGM_HIGH_TEMPLAR12_
#define _SGM_HIGH_TEMPLAR12_


namespace sgm
{
	namespace ht12
	{
		
		template<class FS, class RNG, class FUNC, bool ASK_MUTABLE>  class Morph_range;
		template<class ITR, class PFUNC, class DECO>  class Morph_iterator;


		template<class FS, class RNG, class FUNC>
		class Filter_range;

		template<class FS, class...RNGS>
		class Plait_range;
	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "..\interface_Traits\interface_Traits.hpp"
#include <cstdlib>


template<class ITR, class PFUNC, class DECO>
class sgm::ht12::Morph_iterator
{
	using _Y = decltype( Declval<PFUNC>()(*Declval<ITR>()) );

	template<bool IS_MUTABLE>
	using Y 
	=	Selective_t
		<	IS_MUTABLE
		,	typename DECO::template type<_Y>
		,	typename DECO::template type<_Y const> 
		>;

	using iter_t = Morph_iterator;

public:
	Morph_iterator(ITR const xitr, PFUNC const *pfunc) : _xitr(xitr), _pfunc(pfunc){}

	auto operator[](ptrdiff_t const d) const-> Y<false>{  return (*_pfunc)( *(_xitr + d) );  }
	auto operator[](ptrdiff_t const d)-> Y<true>{  return (*_pfunc)( *(_xitr + d) );  }
	auto operator*() const-> Y<false>{  return (*this)[0];  }
	auto operator*()-> Y<true>{  return (*this)[0];  }

	auto operator+(ptrdiff_t const d) const-> iter_t{  return{_xitr + d, _pfunc};  }
	auto operator-(ptrdiff_t const d) const-> iter_t{  return{_xitr - d, _pfunc};  }
	auto operator+=(ptrdiff_t const d)-> iter_t&{  return _xitr += d,  *this;  }
	auto operator-=(ptrdiff_t const d)-> iter_t&{  return _xitr -= d,  *this;  }

	auto operator++()-> iter_t&{  return *this += 1;  }
	auto operator--()-> iter_t&{  return *this -= 1;  }
	auto operator++(int)-> iter_t{  iter_t const itr = *this;  return ++*this,  itr;  }
	auto operator--(int)-> iter_t{  iter_t const itr = *this;  return --*this,  itr;  }

	auto operator-(iter_t const itr) const-> ptrdiff_t{  return _xitr - itr._xitr;  }

	bool operator==(iter_t const itr) const{  return _xitr == itr._xitr;  }
	bool operator!=(iter_t const itr) const{  return !(*this == itr);  }
	bool operator<(iter_t const itr) const{  return _xitr < itr._xitr;  }
	bool operator>(iter_t const itr) const{  return _xitr > itr._xitr;  }
	bool operator<=(iter_t const itr) const{  return !(*this > itr);  }
	bool operator>=(iter_t const itr) const{  return !(*this < itr);  }

private:
	ITR _xitr;
	PFUNC const *_pfunc;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FS, class RNG, class FUNC, bool ASK_MUTABLE>
class sgm::ht12::Morph_range
{
public:
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#



#endif // end of #ifndef _SGM_HIGH_TEMPLAR12_
