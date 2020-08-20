#pragma once

#ifndef _SGM_RANDOM_ACCESS_ITERATOR_
#define _SGM_RANDOM_ACCESS_ITERATOR_

#include "interface_Traits.hpp"
#include <cassert>
#include <climits>
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{


	template<class FUNC>
	class _Random_Access_iterator_Helper : No_Making
	{
		template<class , bool, bool> friend class Random_Access_iterator;


		template<class _FUNC, bool M, bool F>
		using RA_iter_t = Random_Access_iterator<_FUNC, M, F>;


		template
		<	bool M1, bool M2, bool F1, bool F2
		,	class itr1_t = RA_iter_t<FUNC, M1, F1>
		,	class itr2_t = RA_iter_t<FUNC, M2, F2>
		>
		static auto Substitution
		(	RA_iter_t<FUNC, M1, F1>& itr1, RA_iter_t<FUNC, M2, F2> const itr2
		)->	itr1_t&
		{
			static_assert(!M1 || M2, "cannot bind immutable iterator to mutable one.");
			
			itr1._func = itr2._func,  itr1._idx = itr2._idx;
		
			return itr1;
		}


		template<bool IS_FORWARD> 
		static size_t shifted(size_t const idx, bool const plus_dir, size_t const interval);

		template<>
		static size_t shifted<true>
		(	size_t const idx, bool const plus_dir, size_t const interval
		)
		{
			return plus_dir ? idx + interval : idx - interval;
		}

		template<>
		static size_t shifted<false>
		(	size_t const idx, bool const plus_dir, size_t const interval
		)
		{
			return plus_dir ? idx - interval : idx + interval;
		}


		template<bool IS_FOWARD> static bool Less(size_t const idx1, size_t const idx2);

		template<>
		static bool Less<true>(size_t const idx1, size_t const idx2){  return idx1 < idx2;  }

		template<>
		static bool Less<false>(size_t const idx1, size_t const idx2){  return idx1 > idx2;;  }
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class FUNC, bool IS_MUTABLE, bool IS_FORWARD>
	class Random_Access_iterator
	{
	private:
		using _value_t = std::decay_t< decltype(  Declval<FUNC>()( size_t(0) )  ) >;
		using value_t = std::conditional_t<IS_MUTABLE, _value_t, _value_t const>;
		using iter_t = Random_Access_iterator;

		friend class _Random_Access_iterator_Helper<FUNC>;


	public:
		Random_Access_iterator(FUNC const& func, size_t idx) : _func(func), _idx(idx){}
		Random_Access_iterator(iter_t const&) = default;


		auto operator=(iter_t const&)-> iter_t& = default;


		template
		<	bool _M, bool _F
		,	class 
			=	std::enable_if_t<IS_MUTABLE  ?  IS_FORWARD != _F  :  _M || _F != IS_FORWARD>
		>
		auto operator=(Random_Access_iterator<FUNC, _M, _F> const itr)-> iter_t&
		{
			return _Random_Access_iterator_Helper<FUNC>::Substitution(*this, itr);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto operator[](size_t const interval) const-> value_t const&
		{
			return _func( shifted(_idx, true, interval) - (IS_FORWARD ? 0 : 1) );
		}

		auto operator[](size_t const interval)-> value_t&
		{
			return _func( shifted(_idx, true, interval) - (IS_FORWARD ? 0 : 1) );
		}

		auto operator*() const-> value_t const&	{  return (*this)[0];  }
		auto operator*()-> value_t&				{  return (*this)[0];  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		//	itr++	(rvalue) 
		auto operator++(int)-> iter_t
		{
			iter_t const itr = *this;
			
			_idx = shifted(_idx);
			
			return itr;
		}

		//	itr--	(rvalue)
		auto operator--(int)-> iter_t
		{
			iter_t const itr = *this;

			_idx = shifted(_idx, false);

			return itr;
		}

		auto operator+(size_t const interval) const-> iter_t
		{
			return iter_t( _func, shifted(_idx, true, interval) );
		}

		auto operator-(size_t const interval) const-> iter_t
		{
			return iter_t( _func, shifted(_idx, false, interval) );
		}


		auto operator-(iter_t const itr) const-> long long
		{
			auto const has_greater_idx = _idx > itr._idx;
			auto const du = has_greater_idx ? _idx - itr._idx : itr._idx - _idx;

			assert
			(	du <= static_cast<size_t>(std::numeric_limits<long long>::max()) 
			&&	L"the difference exceeds maximum capacity."
			);

			auto const LL_du = static_cast<long long>(du);

			return IS_FORWARD == has_greater_idx ? LL_du : -LL_du;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto operator+=(size_t const interval)-> iter_t&
		{
			_idx = shifted(_idx, true, interval);  
			
			return *this;
		}

		auto operator-=(size_t const interval)-> iter_t&
		{
			_idx = shifted(_idx, false, interval);  
			
			return *this;
		}


		auto operator++()-> iter_t&{  return *this += 1;  }
		auto operator--()-> iter_t&{  return *this -= 1;  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		bool operator!=(iter_t const itr) const{  return _idx != itr._idx;  }
		bool operator==(iter_t const itr) const{  return _idx == itr._idx;  }
		
		bool operator<(iter_t const itr) const{  return Less(*this, itr);  }
		bool operator>(iter_t const itr) const{  return Less(itr, *this);  }

		bool operator<=(iter_t const itr) const{  return !(*this > itr);  }
		bool operator>=(iter_t const itr) const{  return !(*this < itr);  }


	private:
		FUNC _func;
		size_t _idx;


		static size_t shifted
		(	size_t const idx, bool const plus_dir = true, size_t const interval = 1
		)
		{
			return 
			_Random_Access_iterator_Helper<FUNC>::shifted<IS_FORWARD>(idx, plus_dir, interval);
		}

		static bool Less(iter_t const itr1, iter_t const itr2)
		{
			return 
			_Random_Access_iterator_Helper<FUNC>::Less<IS_FORWARD>(itr1._idx, itr2._idx);
		}		
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T, bool IS_MUTABLE>
	class Serial_Memory_indexer
	{
	private:
		T* _arr;


	public:
		Serial_Memory_indexer(T* const& arr) : _arr(arr){}
		Serial_Memory_indexer(Serial_Memory_indexer const&) = default;

		auto operator=(Serial_Memory_indexer const&)-> Serial_Memory_indexer& = default;


		auto operator()(size_t const idx) const-> T const&
		{
			return *(_arr + idx);
		}

		auto operator()(size_t const idx)-> std::conditional_t<IS_MUTABLE, T, T const>&
		{
			return *(_arr + idx);
		}
	};




}

#endif// end of #ifndef _SGM_RANDOM_ACCESS_ITERATOR_