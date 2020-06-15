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


	template<class T, ixSize_t S>
	struct ix_iterator : No_Making
	{
		using type = indexable_iterator<T, S, true, true>;  
	};

	template<class T, ixSize_t S>
	struct ix_const_iterator : No_Making
	{
		using type = indexable_iterator<T, S, false, true>;  
	};

	template<class T, ixSize_t S>
	struct ix_riterator : No_Making
	{
		using type = indexable_iterator<T, S, true, false>;
	};

	template<class T, ixSize_t S>
	struct ix_const_riterator : No_Making
	{
		using type = indexable_iterator<T, S, false, false>;
	};
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
	//========//========//========//========//=======#//========//========//========//========//===


	template< class ITR, bool = sgm::Has_Operator_index<ITR>::value > 
	struct _itrDist_Helper;

	
	template<class ITR>
	struct _itrDist_Helper<ITR, true> : No_Making
	{
		static_assert(is_iterator<ITR>::value, "ITR doesn't have iterator interface");

		static auto calc(ITR bi, ITR ei)-> ixSize_t
		{	
			return static_cast<ixSize_t>(ei - bi);
		}
	};


	template<class ITR>
	struct _itrDist_Helper<ITR, false> : No_Making
	{
		static_assert(is_iterator<ITR>::value, "ITR doesn't have iterator interface");

		static auto calc(ITR bi, ITR ei, ixSize_t dist = 0)-> ixSize_t
		{
		#if 1
			return bi == ei ? dist : calc(++bi, ei, ++dist);
		#else
			while(bi++ != ei)
				++dist;

			return dist;
		#endif
		}
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<class, ixSize_t SIZE = ixSize::DYNAMIC> 
	class _ixMemory_Helper
	{
	public: 
		_ixMemory_Helper(...){}  

		template<class Q>
		void release(Q& arr)
		{
			for(ixSize_t idx = 0; idx < get_size(); ++idx)
				arr[idx].~value_t();
		}

		auto get_size() const-> ixSize_t{  return SIZE;  }
		auto get_capa() const-> ixSize_t{  return get_size();  }


	};


	template<class T> 
	class _ixMemory_Helper<T, ixSize::DYNAMIC>
	{
		using value_t = std::remove_reference_t<T>;

	public:
		ixSize_t size, capa;

		_ixMemory_Helper(ixSize_t s = 0, ixSize_t c = s) : size(s), capa(c){}


		static auto alloc(ixSize_t capa)-> value_t*
		{
			return static_cast<value_t*>(  std::malloc( sizeof(value_t) * capa )  );
		}

		template<class Q>
		void release(Q& arr)
		{
			for(ixSize_t idx = 0; idx < size; ++idx)
				(arr + idx)->~value_t();

			size = capa = 0,
			std::free(arr), arr = nullptr;
		}

		auto get_size() const-> ixSize_t{  return size;  }
		auto get_capa() const-> ixSize_t{  return capa;  }

		
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T, ixSize_t SIZE>
	class indexable_impl
	{
		using arr_t = std::conditional_t<SIZE == ixSize::DYNAMIC, T*, T[SIZE]>;
		using Helper = _ixMemory_Helper<T, SIZE>;


		_ixMemory_Helper<SIZE> _memory_info;
		arr_t _arr;


	protected:
		template< class = std::enable_if_t<SIZE == ixSize::DYNAMIC> >
		explicit indexable_impl(ixSize_t capa) 
		:	_memory_info( Helper(0, capa) ), _arr( Helper::alloc(capa) )
		{}


		indexable_impl() : indexable_impl( ixSize_t(0) ){}


		template<class...ARGS, class = std::enable_if_t<SIZE == ixSize::DYNAMIC> >
		indexable_impl(ixSize_t size, ARGS const&...args) 
		:	_memory_info( Helper(size) )
		,	_arr
			(	[size, &args...](T* const arr)-> T*
				{
					for(ixSize_t idx = 0; idx < size; ++idx)
						new(arr + idx) T(args...);

					return arr;
				}( Helper::alloc(size) )
			)
		{}


		template
		<	class ITR
		,	class 
			=	std::enable_if_t
				<	!std::is_integral<ITR>::value && is_iterator<ITR>::value 
				&&	SIZE != ixSize::DYNAMIC
				>
		>
		indexable_impl( ITR bi, decltype(bi) ) : _memory_info(Helper()), _arr()
		{
			for(ixSize_t idx = 0; idx < SIZE; ++idx)
				_arr[idx] = static_cast<T>(*bi++);
		}


		template
		<	class ITR
		,	class 
			=	std::enable_if_t
				<	!std::is_integral<ITR>::value && is_iterator<ITR>::value 
				&&	SIZE == ixSize::DYNAMIC
				>
		>
		indexable_impl(ITR bi, ITR ei) 
		:	_memory_info(  Helper( 0, _itrDist_Helper<ITR>::calc(bi, ei) )  )
		,	_arr
			(	[bi, ei](T* const arr) mutable-> T*
				{
					for(ixSize_t idx = 0; bi != ei; ++idx, ++bi)
						new(arr + idx) T(*bi);

					return arr;
				}( Helper::alloc(_memory_info.capa) )
			)
		{}


		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	!std::is_base_of< indexable_impl, std::decay_t<CON> >::value
				>
		>
		indexable_impl(CON&& con) : indexable_impl(con.begin(), con.end()){}


		template<  class Q, class = std::enable_if_t< std::is_convertible<Q, T>::value >  >
		indexable_impl(std::initializer_list<Q>&& iL) : indexable_impl(iL.begin(), iL.end()){}


		//indexable_impl(indexable_impl const& ix_impl)
		//:	_memory_info(ix_impl._memory_info)
		//,	_arr
		//	(	!ix_impl.empty()
		//		?	Helper::copy(ix_impl.size(), ix_impl._arr)
		//		:
		//		ix_impl.capacity() > 0
		//		?	Helper::alloc(ix_impl.capacity())
		//		:	nullptr
		//	)
		//{}


		~indexable_impl(){  clear();  }



		auto size() const-> ixSize_t{  return _memory_info.get_size();  }
		bool empty() const-> ixSize_t{  return size() == 0;  }

		auto data() const-> T const*	{  return _arr;  }
		auto data()-> T*				{  return _arr;  }

		auto at(ixSize_t idx) const-> T const&	{  return _arr[idx];  }
		auto at(ixSize_t idx)-> T&				{  return _arr[idx];  }

		void clear(){  _memory_info.release(_arr);  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		using iterator = indexable_iterator<T, S, true, true>;
		using const_iterator = indexable_iterator<T, S, false, true>;
		using riterator = indexable_iterator<T, S, true, false>;
		using const_riterator = indexable_iterator<T, S, false, false>;


		auto cbegin() const	SGM_DECLTYPE_AUTO(  const_iterator(_arr, 0)  )
		auto begin()			SGM_DECLTYPE_AUTO(  iterator(_arr, 0)  )

		auto cend() const		SGM_DECLTYPE_AUTO(  const_iterator(_arr, size())  )
		auto end()			SGM_DECLTYPE_AUTO(  iterator(_arr, size())  )

		auto crbegin() const	SGM_DECLTYPE_AUTO(  const_riterator(_arr, size())  )
		auto rbegin()			SGM_DECLTYPE_AUTO(  riterator(_arr, size())  )

		auto crend() const		SGM_DECLTYPE_AUTO(  const_riterator(_arr, 0)  )
		auto rend()			SGM_DECLTYPE_AUTO(  riterator(_arr, 0)  )
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


	#ifndef _WHEN_DYNAMIC
		#define _WHEN_DYNAMIC class = std::enable_if_t<SIZE == ixSize::DYNAMIC>


		template<_WHEN_DYNAMIC>
		auto capacity() const-> ixSize_t{  return _memory_info.get_capa();  }


		template<class...ARGS, _WHEN_DYNAMIC>
		void emplace_back(ARGS&&...args)
		{
			assert(size() < capacity() && L"can't emplace_back : out of index");

			new(_arr + _memory_info.size++) T( std::forward<ARGS>(args)... );
		}


		template
		<	class ITR
		,	class = std::enable_if_t< is_iterator<ITR>::value && SIZE == ixSize::DYNAMIC >
		>
		void pop_back_from(ITR itr)
		{
			for
			(	auto d = end() - itr
			;	d-->0
			;	(_arr + --_memory_info.size)-> ~T()
			);
		}


		#undef _WHEN_DYNAMIC
	#else
		#error _WHEN_DYNAMIC was already defined somewhere else.
	#endif

	};



}