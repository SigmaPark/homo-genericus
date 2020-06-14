#pragma once

#include "indexable_interface.hpp"

#include <cstdlib>
#include <cassert>
#include <new>
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#


namespace sgm
{
	

	template
	<	class T, ixSize_t S = ixSize::DYNAMIC
	,	bool IS_MUTABLE = true, bool IS_FORWARD = true
	>
	class indexable_iterator;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	class _ix_iterator_Helper : No_Making
	{
		template<class, ixSize_t, bool, bool> friend class indexable_iterator;


		template
		<	ixSize_t S, bool M1, bool M2, bool F1, bool F2
		,	class itr1_t = indexable_iterator<T, S, M1, F1>
		,	class itr2_t = indexable_iterator<T, S, M2, F2>
		>
		static auto Substitution
		(	indexable_iterator<T, S, M1, F1>& itr1, indexable_iterator<T, S, M2, F2> itr2
		)->	itr1_t&
		{
			static_assert(!M1 || M2, "cannot bind immutable iterator to mutable one.");
			
			itr1._arr = itr2._arr, itr1._idx = itr2._idx;
		
			return itr1;
		}


		template<bool IS_FORWARD> 
		static ixSize_t shifted(ixSize_t idx, bool plus_dir, ixSize_t interval);

		template<>
		static ixSize_t shifted<true>(ixSize_t idx, bool plus_dir, ixSize_t interval)
		{
			return plus_dir ? idx + interval : idx - interval;
		}

		template<>
		static ixSize_t shifted<false>(ixSize_t idx, bool plus_dir, ixSize_t interval)
		{
			return plus_dir ? idx - interval : idx + interval;
		}


		template<bool IS_FOWARD> static bool Less(ixSize_t idx1, ixSize_t idx2);

		template<>
		static bool Less<true>(ixSize_t idx1, ixSize_t idx2){  return idx1 < idx2;  }

		template<>
		static bool Less<false>(ixSize_t idx1, ixSize_t idx2){  return idx1 > idx2;;  }

	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T, ixSize_t S, bool IS_MUTABLE, bool IS_FORWARD>
	class indexable_iterator
	{
		static_assert(!std::is_const<T>::value, "T should be mutable here");

		using iter_t = indexable_iterator;
		enum : ixSize_t{SIZE = S};


	public:
		indexable_iterator(T* arr, ixSize_t idx) : _arr(arr), _idx(idx){}
		indexable_iterator(iter_t const&) = default;


		auto operator=(iter_t const&)-> iter_t& = default;

		template
		<	bool _M, bool _F
		,	class = std::enable_if_t<_M != IS_MUTABLE || _F != IS_FORWARD>
		>
		auto operator=(indexable_iterator<T, SIZE, _M, _F> itr)-> iter_t&
		{
			return _ix_iterator_Helper<T>::Substitution(*this, itr);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		//	itr++	(rvalue) 
		auto operator++(int) const-> iter_t
		{
			iter_t const itr = *this;
			
			_idx = shifted(_idx);
			
			return itr;		
		}

		//	itr--	(rvalue)
		auto operator--(int) const-> iter_t
		{
			iter_t const itr = *this;

			_idx = shifted(_idx, false);

			return itr;
		}

		auto operator+(ixSize_t interval) const-> iter_t
		{
			return iter_t( _arr, shifted(_idx, true, interval) );
		}

		auto operator-(ixSize_t interval) const-> iter_t
		{
			return iter_t( _arr, shifted(_idx, false, interval) );
		}


		auto operator-(iter_t itr) const-> signed long long
		{
			bool const has_greater_idx = _idx > itr._idx;
			ixSize_t const du = has_greater_idx ? _idx - itr._idx : itr._idx - _idx;

			assert(du <= LLONG_MAX && L"the difference exceeds maximum capacity.");

			return 
			IS_FORWARD == has_greater_idx
			?	static_cast<signed long long>(du)
			:	-static_cast<signed long long>(du);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


	#ifndef _RW_MEMBERS
		#define _RW_MEMBERS(NAME, ARGS, RES, ...)		\
			auto NAME (ARGS)	\
			->	std::conditional_t<IS_MUTABLE, RES, const RES>		{__VA_ARGS__}	\
			\
			auto NAME (ARGS) const-> const RES					{__VA_ARGS__}


		_RW_MEMBERS(operator++, , iter_t&, return *this += 1;)
		_RW_MEMBERS(operator--, , iter_t&, return *this -= 1;)

		_RW_MEMBERS
		(	operator[], ixSize_t interval, T&
		,	return *( _arr _ shifted(_idx, true, interval) - (IS_FORWARD ? 0 : 1) );
		)

		_RW_MEMBERS(operator*, , T&, return (*this)[0];)

		_RW_MEMBERS
		(	operator+=, ixSize_t interval, iter_t&
		,	_idx = shifted(_idx, true, interval);  return *this;
		)

		_RW_MEMBERS
		(	operator-=, ixSize_t interval, iter_t&
		,	idx = shifted(_idx, false, interval);  return *this;
		)


		#undef _RW_MEMBERS
	#else
		#error _RW_MEMBERS was already defined somewhere else.
	#endif
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		bool operator!=(iter_t itr) const{  return _idx != itr._idx;  }	
		bool operator==(iter_t itr) const{  return _idx == itr._idx;  }	
		
		bool operator<(iter_t itr) const{  return Less(*this, itr);  }
		bool operator>(iter_t itr) const{  return Less(itr, *this);  }

		bool operator<=(iter_t itr) const{  return *this < itr || *this == itr;  }
		bool operator>=(iter_t itr) const{  return *this > itr || *this == itr;  }


	private:
		T* _arr;
		ixSize_t mutable _idx;


		static ixSize_t shifted(ixSize_t idx, bool plus_dir = true, ixSize_t interval = 1)
		{
			return _ix_iterator_Helper<T>::shifted<IS_FORWARD>(idx, plus_dir, interval);
		}

		static bool Less(iter_t itr1, iter_t itr2)
		{
			return _ix_iterator_Helper<T>::Less<IS_FORWARD>(itr1._idx, itr2._idx);
		}
	};


}