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
	static auto shift(T const n, T const diff)
	->	Enable_if_t<IS_FORWARD == PLUS_DIR, T>{  return n + diff;  }

	template<bool IS_FORWARD, bool PLUS_DIR, class T>
	static auto shift(T const n, T const diff)
	->	Enable_if_t<IS_FORWARD != PLUS_DIR, T>{  return n - diff;  }


	template<bool IS_FORWARD, class T>
	static auto less(T const n1, T const n2)
	-> Enable_if_t<IS_FORWARD, bool>{  return n1 < n2;  }
	
	template<bool IS_FORWARD, class T>
	static auto less(T const n1, T const n2)
	-> Enable_if_t<!IS_FORWARD, bool>{  return n1 > n2;  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, bool IS_INCREASING>
class sgm::Count_iterator
{
private:
	using _itr_t = Count_iterator;


public:
	using value_type = T;


	Count_iterator(T const number) : _number(number){}

	auto operator*() const-> T{  return _number;  }

	auto operator++(int)->	_itr_t
	{  
		auto const iter = *this;

		return _number = _shift<true>(**this),  iter;
	}


	auto operator--(int)-> _itr_t
	{
		auto const iter = *this;

		return _number = _shift<false>(**this),  iter;
	}


	template<class _T>
	auto operator+(_T const diff) const
	->	_itr_t{  return {_shift<true>( **this, static_cast<T>(diff) )};  }

	template<class _T>
	auto operator-(_T const diff) const
	->	_itr_t{  return {_shift<false>( **this, static_cast<T>(diff) )};  }


	auto operator-(_itr_t const itr) const-> ptrdiff_t
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


	template<class _T>
	auto operator+=(_T const diff)-> _itr_t&{  return *this = *this + diff;  }
	
	template<class _T>
	auto operator-=(_T const diff)-> _itr_t&{  return *this = *this - diff;  }

	auto operator++()-> _itr_t&{  return *this += 1;  }
	auto operator--()-> _itr_t&{  return *this -= 1;  }

	auto operator[](ptrdiff_t const diff) const-> T{  return *( *this + diff );  }

	auto operator==(_itr_t const itr) const-> bool{  return **this == *itr;  }
	auto operator!=(_itr_t const itr) const-> bool{  return !(*this == itr);  }
	
	auto operator<(_itr_t const itr) const-> bool{  return _Less(*this, itr);  }
	auto operator>(_itr_t const itr) const-> bool{  return _Less(itr, *this);  }
	auto operator<=(_itr_t const itr) const-> bool{  return !(*this > itr);  }
	auto operator>=(_itr_t const itr) const-> bool{  return !(*this < itr);  }


private:
	T _number;


	template<bool IS_PLUS_DIR>
	static auto _shift(T const n, T const diff = 1)
	->	T{  return _Count_iterator_Helper::shift<IS_INCREASING, IS_PLUS_DIR>(n, diff);  }


	static auto _Less(T const n1, T const n2)
	->	bool{  return _Count_iterator_Helper::less<IS_INCREASING>(n1, n2);  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Countable
{
public:
	Countable(T const length, T const offset = 0) : _length(length), _offset(offset){}

	using iterator = Count_iterator<T, true>;
	using reverse_iterator = Count_iterator<T, false>;
	using const_iterator = iterator;
	using const_reverse_iterator = reverse_iterator;

	auto cbegin() const-> iterator{  return {_offset};  }
	auto begin() const-> iterator{  return cbegin();  }
	auto cend() const-> iterator{  return {_offset + _length};  }
	auto end() const-> iterator{  return cend();  }

	auto crbegin() const-> reverse_iterator{  return {_offset + _length - 1};  }
	auto rbegin() const-> reverse_iterator{  return crbegin();  }
	auto crend() const-> reverse_iterator{  return {_offset - 1};  }
	auto rend() const-> reverse_iterator{  return crend();  }

	auto size() const-> size_t{  return _length;  }

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