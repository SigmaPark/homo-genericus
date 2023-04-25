/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_COUNTABLE_
#define _SGM_COUNTABLE_


#include <cassert>
#include <limits>
#include <iterator>
#include "../Type_Analysis/Type_Analysis.hpp"


namespace sgm
{

	template<class T, bool IS_INCREASING>
	class Count_iterator;
	
	template<class T = std::size_t>
	class Countable;


	struct _Count_iterator_Helper;

}
//========//========//========//========//=======#//========//========//========//========//=======#


struct sgm::_Count_iterator_Helper : Unconstructible
{
private:
	template<class T, bool IS_INCREASING>
	friend class sgm::Count_iterator;


	template<bool IS_FORWARD, bool PLUS_DIR, class T>
	static auto shift(T const n, T const diff) SGM_TRY_NOEXCEPT(n + diff)
	->	Enable_if_t<IS_FORWARD == PLUS_DIR, T>{  return n + diff;  }

	template<bool IS_FORWARD, bool PLUS_DIR, class T>
	static auto shift(T const n, T const diff) SGM_TRY_NOEXCEPT(n - diff)
	->	Enable_if_t<IS_FORWARD != PLUS_DIR, T>{  return n - diff;  }


	template<bool IS_FORWARD, class T>
	static auto less(T const n1, T const n2) SGM_TRY_NOEXCEPT(n1 < n2)
	->	Enable_if_t<IS_FORWARD, bool>{  return n1 < n2;  }
	
	template<bool IS_FORWARD, class T>
	static auto less(T const n1, T const n2) SGM_TRY_NOEXCEPT(n1 > n2)
	->	Enable_if_t<!IS_FORWARD, bool>{  return n1 > n2;  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, bool IS_INCREASING>
class sgm::Count_iterator
{
private:
	using _itr_t = Count_iterator;


	template<bool IS_PLUS_DIR>
	static auto _shift(T const n, T const diff = 1) SGM_TRY_NOEXCEPT_DECLTYPE_AUTO
	(	
		_Count_iterator_Helper::shift<IS_INCREASING, IS_PLUS_DIR>(n, diff)
	)
	

	static auto _Less(T const n1, T const n2) SGM_TRY_NOEXCEPT_DECLTYPE_AUTO
	(
		_Count_iterator_Helper::less<IS_INCREASING>(n1, n2)
	)


	T _number;


public:
	using value_type = T;


	Count_iterator(T const number = std::numeric_limits<T>::max()) SGM_TRY_NOEXCEPT( T(number) )
	:	_number(number){}

	auto operator*() const noexcept-> T const&{  return _number;  }

	auto operator++(int) SGM_TRY_NOEXCEPT( _shift<true>(*Mock<_itr_t>()) )-> _itr_t
	{  
		auto const iter = *this;

		return _number = _shift<true>(**this),  iter;
	}


	auto operator--(int) SGM_TRY_NOEXCEPT( _shift<false>(*Mock<_itr_t>()) )-> _itr_t
	{
		auto const iter = *this;

		return _number = _shift<false>(**this),  iter;
	}


	template<class _T>
	auto operator+(_T const diff) const 
	SGM_TRY_NOEXCEPT( _shift<true>(*Mock<_itr_t>(), Mock<T>()) )
	->	_itr_t{  return {_shift<true>( **this, static_cast<T>(diff) )};  }

	template<class _T>
	auto operator-(_T const diff) const 
	SGM_TRY_NOEXCEPT( _shift<false>(*Mock<_itr_t>(), Mock<T>()) )
	->	_itr_t{  return {_shift<false>( **this, static_cast<T>(diff) )};  }


	template<class _T>
	auto operator+=(_T const diff) SGM_TRY_NOEXCEPT(Mock<_itr_t>() + diff)
	->	_itr_t&{  return *this = *this + diff;  }
	
	template<class _T>
	auto operator-=(_T const diff) SGM_TRY_NOEXCEPT(Mock<_itr_t>() - diff)
	->	_itr_t&{  return *this = *this - diff;  }

	auto operator++() SGM_TRY_NOEXCEPT(Mock<_itr_t>() += 1)-> _itr_t&{  return *this += 1;  }
	auto operator--() SGM_TRY_NOEXCEPT(Mock<_itr_t>() -= 1)-> _itr_t&{  return *this -= 1;  }

	auto operator[](ptrdiff_t const diff) const SGM_TRY_NOEXCEPT( *(Mock<_itr_t>() + diff) )
	->	T const&{  return *( *this + diff );  }


	auto operator==(_itr_t const itr) const SGM_TRY_NOEXCEPT(*itr == *itr)
	->	bool{  return **this == *itr;  }
	
	auto operator!=(_itr_t const itr) const SGM_TRY_NOEXCEPT( !(itr == itr) )
	->	bool{  return !(*this == itr);  }
	

	auto operator<(_itr_t const itr) const SGM_TRY_NOEXCEPT( _Less(itr, itr) )
	->	bool{  return _Less(*this, itr);  }
	
	auto operator>(_itr_t const itr) const SGM_TRY_NOEXCEPT( _Less(itr, itr) )
	->	bool{  return _Less(itr, *this);  }
	
	auto operator<=(_itr_t const itr) const SGM_TRY_NOEXCEPT( !(itr > itr) )
	->	bool{  return !(*this > itr);  }
	
	auto operator>=(_itr_t const itr) const SGM_TRY_NOEXCEPT( !(itr < itr) )
	->	bool{  return !(*this < itr);  }


	auto operator-(_itr_t const itr) const 
	noexcept( noexcept(*itr > *itr) && noexcept(*itr - *itr) )
	->	ptrdiff_t
	{
		bool const greater_mine = **this > *itr;
		size_t const diff_ULL = greater_mine ? **this - *itr : *itr - **this;

		assert
		(	diff_ULL <= static_cast<size_t>(std::numeric_limits<ptrdiff_t>::max()) 
		&&	"the difference exceeds maximum threshold value.\n"
		);

		auto const diff = static_cast<ptrdiff_t>(diff_ULL);

		return IS_INCREASING == greater_mine  ?  diff  :  -diff;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Countable
{
public:
	constexpr Countable(T const length, T const offset = 0) SGM_TRY_NOEXCEPT( T(length) )
	:	_length(length), _offset(offset){}


	using iterator = Count_iterator<T, true>;
	using reverse_iterator = Count_iterator<T, false>;
	using const_iterator = iterator;
	using const_reverse_iterator = reverse_iterator;


	auto cbegin() const SGM_TRY_NOEXCEPT( iterator{T(0)} )
	->	iterator{  return {_offset};  }
	
	auto begin() const SGM_TRY_NOEXCEPT_DECLTYPE_AUTO(  cbegin()  )
	
	auto cend() const SGM_TRY_NOEXCEPT( iterator{T(0)} )
	->	iterator{  return {_offset + _length};  }
	
	auto end() const SGM_TRY_NOEXCEPT_DECLTYPE_AUTO(  cend()  )

	auto crbegin() const SGM_TRY_NOEXCEPT( reverse_iterator{T(0)} )
	->	reverse_iterator{  return {_offset + _length - 1};  }

	auto rbegin() const SGM_TRY_NOEXCEPT_DECLTYPE_AUTO(  crbegin()  )

	auto crend() const SGM_TRY_NOEXCEPT( reverse_iterator{T(0)} )
	->	reverse_iterator{  return {_offset - 1};  }

	auto rend() const SGM_TRY_NOEXCEPT_DECLTYPE_AUTO(  crend()  )

	auto size() const noexcept-> size_t{  return _length;  }


private:
	T _length, _offset;
};
//========//========//========//========//=======#//========//========//========//========//=======#


namespace std
{

	template<class T, bool IS_INCREASING>
	struct iterator_traits< sgm::Count_iterator<T, IS_INCREASING> >
	{
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T const*;
		using reference = T const&;
	};

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_COUNTABLE_