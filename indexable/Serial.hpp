#pragma once


#include <new>
#include <cassert>
#include "..\interface_Traits\interface_Traits.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	template<class T>
	class _sr_iterator_Helper : No_Making
	{
		template<class, size_t, bool, bool> friend class Serial_iterator;


		template
		<	size_t S, bool M1, bool M2, bool F1, bool F2
		,	class itr1_t = Serial_iterator<T, S, M1, F1>
		,	class itr2_t = Serial_iterator<T, S, M2, F2>
		>
		static auto Substitution
		(	Serial_iterator<T, S, M1, F1>& itr1, Serial_iterator<T, S, M2, F2> const itr2
		)->	itr1_t&
		{
			static_assert(!M1 || M2, "cannot bind immutable iterator to mutable one.");
			
			itr1._arr = itr2._arr,  itr1._idx = itr2._idx;
		
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


	template< class ITR, bool = sgm::Has_Operator_index<ITR>::value > 
	struct _iterator_Distance;

	
	template<class ITR>
	struct _iterator_Distance<ITR, true> : No_Making
	{
		static_assert(is_iterator<ITR>::value, "ITR doesn't have iterator interface");

		static auto calc(ITR bi, ITR const ei)-> size_t
		{	
			return static_cast<size_t>(ei - bi);
		}
	};


	template<class ITR>
	struct _iterator_Distance<ITR, false> : No_Making
	{
		static_assert(is_iterator<ITR>::value, "ITR doesn't have iterator interface");

		static auto calc(ITR bi, ITR const ei, size_t dist = 0)-> size_t
		{
			return bi == ei ? dist : calc(++bi, ei, ++dist);
		}
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T, size_t S, bool IS_MUTABLE, bool IS_FORWARD>
	class Serial_iterator
	{
		static_assert(!std::is_const<T>::value, "T should be mutable here");

		using value_t = std::conditional_t<IS_MUTABLE, T, T const>;
		using iter_t = Serial_iterator;
		
		enum : size_t{SIZE = S};


	public:
		Serial_iterator(value_t* arr, size_t idx) : _arr(arr), _idx(idx){}
		Serial_iterator(iter_t const&) = default;


		auto operator=(iter_t const&)-> iter_t& = default;


		template
		<	bool _M, bool _F
		,	class 
			=	std::enable_if_t<IS_MUTABLE  ?  IS_FORWARD != _F  :  _M || _F != IS_FORWARD>
		>
		auto operator=(Serial_iterator<T, SIZE, _M, _F> const itr)-> iter_t&
		{
			return _sr_iterator_Helper<T>::Substitution(*this, itr);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto operator[](size_t const interval) const-> value_t const&
		{
			return *( _arr + shifted(_idx, true, interval) - (IS_FORWARD ? 0 : 1) );
		}

		auto operator[](size_t const interval)-> value_t&
		{
			return *( _arr + shifted(_idx, true, interval) - (IS_FORWARD ? 0 : 1) );
		}

		auto operator*() const-> value_t const&	{  return (*this)[0];  }
		auto operator*()-> value_t&			{  return (*this)[0];  }
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
			return iter_t( _arr, shifted(_idx, true, interval) );
		}

		auto operator-(size_t const interval) const-> iter_t
		{
			return iter_t( _arr, shifted(_idx, false, interval) );
		}


		auto operator-(iter_t const itr) const-> signed long long
		{
			bool const has_greater_idx = _idx > itr._idx;
			size_t const du = has_greater_idx ? _idx - itr._idx : itr._idx - _idx;

			assert(du <= LLONG_MAX && L"the difference exceeds maximum capacity.");

			return 
			IS_FORWARD == has_greater_idx
			?	static_cast<signed long long>(du)
			:	-static_cast<signed long long>(du);
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

		bool operator<=(iter_t const itr) const{  return *this < itr || *this == itr;  }
		bool operator>=(iter_t const itr) const{  return *this > itr || *this == itr;  }


	private:
		value_t* _arr;
		size_t _idx;


		static size_t shifted
		(	size_t const idx, bool const plus_dir = true, size_t const interval = 1
		)
		{
			return _sr_iterator_Helper<T>::shifted<IS_FORWARD>(idx, plus_dir, interval);
		}

		static bool Less(iter_t const itr1, iter_t const itr2)
		{
			return _sr_iterator_Helper<T>::Less<IS_FORWARD>(itr1._idx, itr2._idx);
		}
	};
	//========//========//========//========//=======#//========//========//========//========//===


	struct srSize : No_Making{  enum : size_t{INTERFACE = 0, DYNAMIC = -1};  };


	template<class T, size_t S = srSize::DYNAMIC>
	class Serial
	{
		template<class Q, size_t _S> struct _Core : No_Making{  using type = Q[S];  };
		template<class Q> struct _Core<Q, srSize::INTERFACE> : No_Making{  using type = Q*;  };

	public:
		enum : size_t{SIZE = S};
		using core_t = typename _Core<T, S>::type;
		
		core_t _core;


		auto cdata() const-> T const*	{  return _core;  }
		auto data() const-> T const*	{  return cdata();  }
		auto data()-> T*				{  return _core;  }

		auto operator[](size_t const idx) const-> T const&		{  return _core[idx];  }
		auto operator[](size_t const idx)-> T&				{  return _core[idx];  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		using iter_t = Serial_iterator<T, SIZE, true, true>;
		using citer_t = Serial_iterator<T, SIZE, false, true>;
		using riter_t = Serial_iterator<T, SIZE, true, false>;
		using criter_t = Serial_iterator<T, SIZE, false, false>;


	#ifndef _WHEN_STATIC 
		#define _WHEN_STATIC template< class = std::enable_if_t<S != srSize::INTERFACE> >


		_WHEN_STATIC auto size() const-> size_t { return SIZE; }

		auto cbegin() const-> citer_t{  return citer_t(_core, 0);  }
		auto begin() const-> citer_t{  return cbegin();  }
		auto begin()-> iter_t{  return iter_t(_core, 0);  }

		auto crend() const-> criter_t{  return criter_t(_core, 0);  }
		auto rend() const-> criter_t{  return crend();  }
		auto rend()-> riter_t{  return riter_t(_core, 0);  }

		_WHEN_STATIC auto cend() const-> citer_t{  return citer_t(_core, SIZE);  }
		_WHEN_STATIC auto end() const-> citer_t{  return cend();  }
		_WHEN_STATIC auto end()-> iter_t{  return iter_t(_core, SIZE);  }

		_WHEN_STATIC auto crbegin() const-> criter_t{  return criter_t(_core, SIZE);  }
		_WHEN_STATIC auto rbegin() const-> criter_t{  return crbegin();  }
		_WHEN_STATIC auto rbegin()-> riter_t{  return riter_t(_core, SIZE);  }


		auto front() const-> T const&{  return *cbegin();  }
		auto front()-> T&{  return *begin();  }

		_WHEN_STATIC auto back() const-> T const&{  return *crbegin();  }
		_WHEN_STATIC auto back()-> T&{  return *rbegin();  }


		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	S != srSize::INTERFACE
				&&	!std::is_same< std::decay_t<CON>, Serial >::value  
				>
		> 
		operator CON() const{  return std::decay_t<CON>(begin(), end());  }


		_WHEN_STATIC auto swap(Serial& sr)-> Serial&
		{
			for 
			(	auto itr = begin(), sitr = sr.begin()
			;	itr != end()
			;	std::swap(*itr++, *sitr++) 
			);

			return *this;
		}


		#undef _WHEN_STATIC
	#else
		#error _WHEN_STATIC was already defined somewhere else.
	#endif
	

	};// end of class Serial
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T>
	class Serial<T, srSize::DYNAMIC> : public Serial<T, srSize::INTERFACE>
	{
		using Helper = Serial<T, srSize::INTERFACE>;
		using value_t = std::remove_reference_t<T>;


		size_t _capacity, _size;


		static auto _alloc(size_t const capa)-> value_t*
		{
			return 
			static_cast<value_t*>
			(	capa == 0 
				?	nullptr 
				:	::operator new( sizeof(value_t) * capa )  
			);
		}


		template<  class ITR, class = std::enable_if_t< is_iterator<ITR>::value >  >
		auto _cloning(ITR bi, ITR const ei)-> Serial&
		{
			assert
			(	capacity() >= _iterator_Distance<ITR>::calc(bi, ei) 
			&&	L"out of capacity.\n" 
			);

			auto itr = begin();

			for ( ;  bi != ei && itr != end();  *itr++ = static_cast<value_t>(*bi++) );

			return bi != ei ? merge_back(bi, ei) : pop_back_from(itr);
		}


	public:
		Serial() : _capacity(0), _size(0){  _core = nullptr;  }


		template<  class ITR, class = std::enable_if_t< is_iterator<ITR>::value >  >
		Serial(ITR bi, ITR const ei) 
		:	_capacity( _iterator_Distance<ITR>::calc(bi, ei) ), _size(0)
		{
			_core = _alloc(capacity()),  merge_back(bi, ei);
		}


		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	!std::is_same< Serial, std::decay_t<CON> >::value
				>
		>
		Serial(CON&& con) : Serial(con.begin(), con.end()){}


		template<  class Q, class = std::enable_if_t< std::is_convertible<Q, T>::value >  >
		Serial(std::initializer_list<Q>&& iL) : Serial(iL.begin(), iL.end()){}


		Serial(Serial const& sr) : Serial(sr.cbegin(), sr.cend()){}


		Serial(Serial&& sr) : _capacity(sr.capacity()), _size(sr.size())
		{
			_core = sr._core,  sr._capacity = sr._size = 0,  sr._core = nullptr;
		}


		explicit Serial(size_t const capa) : _capacity(capa), _size(0){  _core = _alloc(capa);  }


		template<class...ARGS>
		Serial(size_t const size, ARGS const&... args) : _capacity(size), _size(size)
		{
			_core = _alloc(size);

			for ( size_t idx = 0;  idx < size;  new(_core + idx++) value_t(args...) );
		}


		~Serial(){  clear(),  ::operator delete(_core),  _core = nullptr;  }


		auto operator=(Serial const& sr)-> Serial&
		{
			return 
			capacity() < sr.capacity()
			?	*this = Serial(sr.cbegin(), sr.cend())
			:	_cloning(sr.cbegin(), sr.cend());
		}


		auto operator=(Serial&& sr)-> Serial&
		{
			_capacity = sr.capacity(),  _size = sr.size(),  _core = sr._core,
			sr._capacity = sr._size = 0,  sr._core = nullptr;

			return *this;
		}


		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	!std::is_same< std::decay_t<CON>, Serial >::value  
				>  
		>
		auto operator=(CON&& con)-> Serial&
		{
			return 
			capacity() < con.size()
			?	*this = Serial(con.begin(), con.end())
			:	_cloning(con.begin(), con.end());
		}


		auto size() const-> size_t{  return _size;  }
		auto empty() const-> bool{  return size() == 0;  }
		auto capacity() const-> size_t{  return _capacity;  }


		template<class...ARGS>
		auto emplace_back(ARGS&&...args)-> Serial&
		{
			assert(size() < capacity() && L"can't emplace_back : out of index");

			new(_core + _size++) value_t( std::forward<ARGS>(args)... );

			return *this;
		}


		template<class Q>
		auto operator>>(Q&& q)-> Serial&{  return emplace_back( std::forward<Q>(q) );  }


		template<  class ITR, class = std::enable_if_t< is_iterator<ITR>::value >  >
		auto merge_back(ITR bi, ITR const ei)-> Serial&
		{
			assert
			(	size() + _iterator_Distance<ITR>::calc(bi, ei) <= capacity() 
			&&	L"can't merge_back : out of index"
			);

			for ( ;  bi != ei;  *this >> static_cast<value_t>(*bi++) );

			return *this;
		}


		template<  class ITR, class = std::enable_if_t< is_iterator<ITR>::value >  >
		auto pop_back_from(ITR const itr)-> Serial&
		{
			for(auto d = end() - itr;  d-->0;  _core[--_size].~value_t());

			return *this;
		}


		auto pop_back(size_t const n = 1)-> Serial&{  return pop_back_from(end() - n);  }
		auto clear()-> Serial&{  return pop_back_from(begin());  }


		auto swap(Serial& sr)-> Serial&
		{
			using std::swap;

			swap(_capacity, sr._capacity),  swap(_size, sr._size),  swap(_core, sr._core);

			return *this;
		}


		using citer_t = typename Helper::citer_t;
		using iter_t = typename Helper::iter_t;
		using criter_t = typename Helper::criter_t;
		using riter_t = typename Helper::riter_t;

		auto cend() const-> citer_t{  return citer_t(_core, size());  }
		auto end() const-> citer_t{  return cend();  }
		auto end()-> iter_t{  return iter_t(_core, size());  }

		auto crbegin() const-> criter_t{  return criter_t(_core, size());  }
		auto rbegin() const-> criter_t{  return crbegin();  }
		auto rbegin()-> riter_t{  return riter_t(_core, size());  }

		auto back() const-> T const&{  return *crbegin();  }
		auto back()-> T&{  return *rbegin();  }


		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	!std::is_same< std::decay_t<CON>, Serial >::value  
				>
		> 
		operator CON() const{  return std::decay_t<CON>(Helper::begin(), end());  }

	};


}// end of namespace sgm


#include <iterator>


namespace std
{


	template<class T, size_t S, bool M, bool F>
	struct iterator_traits< sgm::Serial_iterator<T, S, M, F> >
	{
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = signed long long;
		using pointer = std::conditional_t<M, T*, T const*>;
		using reference = std::conditional_t<M, T&, T const&>;
	};


}
