#pragma once

#ifndef _SGM_COUNTABLE_
#define _SGM_COUNTABLE_

#include <type_traits>
#include <cassert>
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	

	template<class N, bool INCREASING>
	class Count_iterator
	{
		using iter_t = Count_iterator;

		N _number;

		
		template<bool FORWARD, bool PLUS_DIR>
		struct Direction : Direction<!FORWARD, !PLUS_DIR>{};

		template<>
		struct Direction<true, true>
		{
			static N shifted(N const n, N const interval){  return n + interval;  }
		};

		template<>
		struct Direction<true, false>
		{
			static N shifted(N const n, N const interval){  return n - interval;  }
		};

		template<bool PLUS>
		static N _shifted(N const n, N const interval = 1)
		{
			return Direction<INCREASING, PLUS>::shifted(n, interval);
		}


		template<bool FORWARD> struct Order;

		template<>
		struct Order<true>{  static bool less(N const n1, N const n2){  return n1 < n2;  }  };

		template<>
		struct Order<false>{  static bool less(N const n1, N const n2){  return n1 > n2;  }  };

		static bool _less(N const n1, N const n2){  return Order<INCREASING>::less(n1, n2);  }


	public:
		Count_iterator(N const number) : _number(number){}


		auto operator*() const-> N{  return _number;  }


		auto operator=(Count_iterator<N, !INCREASING> const itr)-> iter_t
		{
			_number = *itr;

			return *this;
		}


		auto operator++(int)-> iter_t
		{
			iter_t iter = *this;

			_number = _shifted<true>(**this);

			return iter;
		}

		auto operator--(int)-> iter_t
		{
			iter_t iter = *this;

			_number = _shifted<false>(**this);

			return iter;
		}

		template<  class _N, class = std::enable_if_t< std::is_convertible<_N, N>::value >  >
		auto operator+(_N const interval) const-> iter_t
		{	
			return iter_t(  _shifted<true>( **this, static_cast<N>(interval) )  );
		}

		template<  class _N, class = std::enable_if_t< std::is_convertible<_N, N>::value >  >
		auto operator-(_N const interval) const-> iter_t
		{
			return iter_t(  _shifted<false>( **this, static_cast<N>(interval) )  );
		}

		auto operator-(iter_t const itr) const-> long long
		{
			bool const greater_mine = **this > *itr;
			N const du = greater_mine ? **this - *itr : *itr - **this;

			assert(du <= LLONG_MAX && L"the difference exceeds maximum capacity.");

			return
			INCREASING == greater_mine
			?	static_cast<long long>(du)
			:	-static_cast<long long>(du);
		}

		template<  class _N, class = std::enable_if_t< std::is_convertible<_N, N>::value >  >
		auto operator+=(_N const interval)-> iter_t&{  return *this = *this + interval;  }
		
		template<  class _N, class = std::enable_if_t< std::is_convertible<_N, N>::value >  >
		auto operator-=(_N const interval)-> iter_t&{  return *this = *this - interval;  }

		auto operator++()-> iter_t&{  return *this += 1;  }
		auto operator--()-> iter_t&{  return *this -= 1;  }


		auto operator[](size_t const interval) const-> N
		{
			return *( *this + static_cast<N>(interval) );  
		}


		bool operator==(iter_t const itr) const{  return **this == *itr;  }
		bool operator!=(iter_t const itr) const{  return !(*this == itr);  }

		bool operator<(iter_t const itr) const{  return _less(*this, itr);  }
		bool operator>(iter_t const itr) const{  return _less(itr, *this);  }
		bool operator<=(iter_t const itr) const{  return !(*this > itr);  }
		bool operator>=(iter_t const itr) const{  return !(*this < itr);  }
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class N = size_t>
	class Countable
	{
		N _length, _offset;


	public:
		Countable(N const length, N const offset = 0) : _length(length), _offset(offset){}

		size_t size() const{  return static_cast<size_t>(_length);  }

		using iterator_t = Count_iterator<N, true>;
		using riterator_t = Count_iterator<N, false>;

		auto cbegin() const-> iterator_t{  return iterator_t(_offset);  }
		auto cend() const-> iterator_t{  return iterator_t(_offset + _length);  }
		auto begin() const-> iterator_t{  return cbegin();  }
		auto end() const-> iterator_t{  return cend();  }

		auto crbegin() const-> riterator_t{  return riterator_t(_offset + _length - 1);  }
		auto crend() const-> riterator_t{ return riterator_t(_offset - 1);  }
		auto rbegin() const-> riterator_t{  return crbegin();  }
		auto rend() const-> riterator_t{  return crend();  }
	};


}// end of namespace sgm
//========//========//========//========//=======#//========//========//========//========//=======#


#include <iterator>


template<class N, bool INCREASING>
struct std::iterator_traits< sgm::Count_iterator<N, INCREASING> >
{
	using iterator_category = std::random_access_iterator_tag;
	using value_type = N;
	using difference_type = signed long long;
	using pointer = N const*;
	using reference = N const&;
};


#endif // end of #ifndef _SGM_COUNTABLE_