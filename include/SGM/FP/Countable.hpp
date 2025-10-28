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
#include "SGM/TMP/Type_Analysis.hpp"


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
	static auto shift(T const n, T const diff) noexcept
	->	Enable_if_t<IS_FORWARD == PLUS_DIR, T>{  return n + diff;  }

	template<bool IS_FORWARD, bool PLUS_DIR, class T>
	static auto shift(T const n, T const diff) noexcept
	->	Enable_if_t<IS_FORWARD != PLUS_DIR, T>{  return n - diff;  }


	template<bool IS_FORWARD, class T>
	static auto less(T const n1, T const n2) noexcept
	->	Enable_if_t<IS_FORWARD, bool>{  return n1 < n2;  }
	
	template<bool IS_FORWARD, class T>
	static auto less(T const n1, T const n2) noexcept
	->	Enable_if_t<!IS_FORWARD, bool>{  return n1 > n2;  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, bool IS_INCREASING>
class sgm::Count_iterator
{
private:
	using _itr_t = Count_iterator;


	template<bool IS_PLUS_DIR>
	static auto _shift(T const n, T const diff = 1) noexcept-> SGM_DECLTYPE_AUTO
	(	
		_Count_iterator_Helper::shift<IS_INCREASING, IS_PLUS_DIR>(n, diff)
	)
	

	static auto _Less(T const n1, T const n2) noexcept-> SGM_DECLTYPE_AUTO
	(
		_Count_iterator_Helper::less<IS_INCREASING>(n1, n2)
	)


	T _number;


public:
	using value_type = T;


	Count_iterator(T const number = std::numeric_limits<T>::max()) noexcept : _number(number){}

	auto operator*() const noexcept-> T const&{  return _number;  }

	auto operator++(int) noexcept-> _itr_t
	{  
		auto const iter = *this;

		return _number = _shift<true>(**this),  iter;
	}


	auto operator--(int) noexcept-> _itr_t
	{
		auto const iter = *this;

		return _number = _shift<false>(**this),  iter;
	}


	template<class _T>
	auto operator+(_T const diff) const noexcept
	->	_itr_t{  return {_shift<true>( **this, static_cast<T>(diff) )};  }

	template<class _T>
	auto operator-(_T const diff) const noexcept
	->	_itr_t{  return {_shift<false>( **this, static_cast<T>(diff) )};  }


	template<class _T>
	auto operator+=(_T const diff) noexcept-> _itr_t&{  return *this = *this + diff;  }
	
	template<class _T>
	auto operator-=(_T const diff) noexcept-> _itr_t&{  return *this = *this - diff;  }

	auto operator++() noexcept-> _itr_t&{  return *this += 1;  }
	auto operator--() noexcept-> _itr_t&{  return *this -= 1;  }

	auto operator[](ptrdiff_t const diff) const noexcept
	->	T const&{  return *( *this + diff );  }


	auto operator==(_itr_t const itr) const noexcept-> bool{  return **this == *itr;  }
	auto operator!=(_itr_t const itr) const noexcept-> bool{  return !(*this == itr);  }
	
	auto operator<(_itr_t const itr) const noexcept-> bool{  return _Less(*this, itr);  }
	auto operator>(_itr_t const itr) const noexcept-> bool{  return _Less(itr, *this);  }
	auto operator<=(_itr_t const itr) const noexcept-> bool{  return !(*this > itr);  }
	auto operator>=(_itr_t const itr) const noexcept-> bool{  return !(*this < itr);  }


	auto operator-(_itr_t const itr) const noexcept-> ptrdiff_t
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
	constexpr Countable(T const length, T const offset = 0) noexcept
	:	_length(length), _offset(offset){}


	using iterator = Count_iterator<T, true>;
	using reverse_iterator = Count_iterator<T, false>;
	using const_iterator = iterator;
	using const_reverse_iterator = reverse_iterator;


	auto cbegin() const noexcept-> iterator{  return {_offset};  }
	auto begin() const noexcept-> SGM_DECLTYPE_AUTO(  cbegin()  )
	auto cend() const noexcept-> iterator{  return {_offset + _length};  }
	auto end() const noexcept-> SGM_DECLTYPE_AUTO(  cend()  )

	auto crbegin() const noexcept-> reverse_iterator{  return {_offset + _length - 1};  }
	auto rbegin() const noexcept-> SGM_DECLTYPE_AUTO(  crbegin()  )
	auto crend() const noexcept-> reverse_iterator{  return {_offset - 1};  }
	auto rend() const noexcept-> SGM_DECLTYPE_AUTO(  crend()  )

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