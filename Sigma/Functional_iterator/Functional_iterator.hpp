#pragma once

#ifndef _SGM_FUNCTIONAL_ITERATOR_
#define _SGM_FUNCTIONAL_ITERATOR_


#include <type_traits>
#include "..\interface_Traits\interface_Traits.hpp"


namespace sgm
{

	template<bool IS_MUTABLE, bool IS_FORWARD, class FTR>
	class Functional_iterator;

	template<class FTR, bool ASK_MUTABLE>
	class Functional_iterable;

	template<class FTR>
	static auto as_iterable(FTR const, size_t const)-> Functional_iterable<FTR, true>;

	template<class FTR>
	static auto as_citerable(FTR const, size_t const)-> Functional_iterable<FTR, false>;

	struct _Fitr_Helper;

}
//========//========//========//========//=======#//========//========//========//========//=======#


#include <cassert>


struct sgm::_Fitr_Helper : No_Making
{
private:
	template<bool IS_MUTABLE, bool IS_FORWARD, class FTR> 
	friend class sgm::Functional_iterator;


	template<class FTR, bool M, bool F>
	static auto _cast(Functional_iterator<M, F, FTR> const itr)-> Functional_iterator<!M, F, FTR>
	{
		static_assert(M, "cannot convert immutable iterator to mutable one.");		

		return{itr._ftr, itr._idx};  
	}


	template<class FTR, bool M1, bool M2, bool FWD>
	static auto _diff
	(	Functional_iterator<M1, FWD, FTR> const bi, Functional_iterator<M2, FWD, FTR> const ei
	)->	ptrdiff_t
	{
		auto const &i1 = bi._idx, &i2 = ei._idx;
		bool const is_increasing = i1 <= i2;
		auto const du = is_increasing ? i2 - i1 : i1 - i2;

		assert( du <= 0x7fff'ffff'ffff'ffffULL && L"the difference exceeds maximum capacity.\n");

		auto const LLdu = static_cast<ptrdiff_t>(du);

		return FWD == is_increasing ? LLdu : -LLdu;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<bool IS_MUTABLE, bool IS_FORWARD, class FTR>
class sgm::Functional_iterator
{
private:
	friend struct sgm::_Fitr_Helper;  

	using _FH = _Fitr_Helper;
	using iter_t = Functional_iterator;
	template<bool M>  using _fnitr_t = Functional_iterator<M, IS_FORWARD, FTR>;

public:
	static_assert
	(	std::is_invocable_v<FTR, size_t>
	&&	std::is_copy_constructible_v<FTR> && std::is_copy_assignable_v<FTR>
	);


	Functional_iterator(FTR const ftr, size_t const idx) : _ftr(ftr), _idx(idx){}
	Functional_iterator(_fnitr_t<!IS_MUTABLE> const itr) : iter_t( _FH::_cast(itr) ){}

	decltype(auto) operator=(_fnitr_t<!IS_MUTABLE> const itr){  return *this = iter_t(itr);  }
	
	operator _fnitr_t<!IS_MUTABLE>() const{  return *this;  }

	decltype(auto) operator[](ptrdiff_t const d) const{  return _indexing(this, d);  }
	decltype(auto) operator[](ptrdiff_t const d){  return _indexing(this, d);  }
	decltype(auto) operator*() const{  return (*this)[0];  }
	decltype(auto) operator*(){  return (*this)[0];  }

	auto operator+(ptrdiff_t const d) const-> iter_t{  return{_ftr, _shifted<true>(d)};  }
	auto operator-(ptrdiff_t const d) const-> iter_t{  return{_ftr, _shifted<false>(d)};  }
	auto operator+=(ptrdiff_t const d)-> iter_t&{  return _idx = _shifted<true>(d),  *this;  }
	auto operator-=(ptrdiff_t const d)-> iter_t&{  return _idx = _shifted<false>(d),  *this;  }

	decltype(auto) operator++(){  return *this += 1;  }
	decltype(auto) operator--(){  return *this -= 1;  }
	auto operator++(int){  iter_t const itr = *this;  return ++*this,  itr;  }
	auto operator--(int){  iter_t const itr = *this;  return --*this,  itr;  }

	template<bool M>  
	auto operator-(_fnitr_t<M> const itr) const{  return _FH::_diff(*this, itr);  }

	template<bool M>  bool operator==(_fnitr_t<M> const itr) const{  return *this - itr == 0;  }
	template<bool M>  bool operator!=(_fnitr_t<M> const itr) const{  return !(*this == itr);  }
	template<bool M>  bool operator<(_fnitr_t<M> const itr) const{  return *this - itr < 0;  }
	template<bool M>  bool operator>(_fnitr_t<M> const itr) const{  return *this - itr > 0;  }
	template<bool M>  bool operator<=(_fnitr_t<M> const itr) const{  return !(*this > itr);  }
	template<bool M>  bool operator>=(_fnitr_t<M> const itr) const{  return !(*this < itr);  }

private:
	FTR _ftr;
	size_t _idx;


	template<bool IS_PLUS>
	auto _shifted(ptrdiff_t const diff = 1) const-> size_t
	{
		return 
		[i = _idx, d = diff < 0 ? -diff : diff]
		{
			if constexpr(IS_FORWARD == IS_PLUS)
				return i + d;
			else
				return i - d;
		}();	
	}


	template<class ME>
	static decltype(auto) _indexing(ME *p, ptrdiff_t const d) 
	{
		ptrdiff_t constexpr OFFSET = IS_FORWARD ? 0 : 1;

		return p->_ftr( p->template _shifted<true>(d) - OFFSET );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FTR, bool ASK_MUTABLE>
class sgm::Functional_iterable
{
	template<bool IS_MUTABLE, bool IS_FORWARD>
	using _fnitr_t = Functional_iterator<IS_MUTABLE, IS_FORWARD, FTR>;

public:
	Functional_iterable(FTR const ftr, size_t const range) : _ftr(ftr), _range(range){}

	auto cbegin() const{  return _fnitr_t<false, true>(_ftr, 0);  }
	auto begin() const{  return cbegin();  }
	auto begin(){  return _fnitr_t<ASK_MUTABLE, true>(_ftr, 0);  }

	auto cend() const{  return _fnitr_t<false, true>(_ftr, _range);  }
	auto end() const{  return cend();  }
	auto end(){  return _fnitr_t<ASK_MUTABLE, true>(_ftr, _range);  }

	auto crbegin() const{  return _fnitr_t<false, false>(_ftr, _range);  }
	auto rbegin() const{  return crbegin();  }
	auto rbegin(){  return _fnitr_t<ASK_MUTABLE, false>(_ftr, _range);  }

	auto crend() const{  return _fnitr_t<false, false>(_ftr, 0);  }
	auto rend() const{  return crend();  }
	auto rend(){  return _fnitr_t<ASK_MUTABLE, false>(_ftr, 0);  }

private:
	FTR _ftr;
	size_t _range;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FTR>
auto sgm::as_iterable(FTR const ftr, size_t const range)
->	Functional_iterable<FTR, true>{  return{ftr, range};  }


template<class FTR>
auto sgm::as_citerable(FTR const ftr, size_t const range)
->	Functional_iterable<FTR, false>{  return{ftr, range};  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#include <iterator>


template<class FTR, bool M, bool F>
struct std::iterator_traits< sgm::Functional_iterator<M, F, FTR> >
{
	using iterator_category = random_access_iterator_tag;
	using value_type = decay_t< std::invoke_result_t<FTR, size_t> >;
	using difference_type = ptrdiff_t;
	using pointer = conditional_t<M, value_type, value_type const>*;
	using reference = remove_pointer_t<pointer>&;
};


#endif // end of #ifndef _SGM_FUNCTIONAL_ITERATOR_