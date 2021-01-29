#pragma once

#ifndef _SGM_SERIAL_
#define _SGM_SERIAL_

#include <new>
#include <cassert>
#include "..\interface_Traits\interface_Traits.hpp"
#include "..\idiom\idiom.hpp"


#ifndef _SGM_NOEXCEPT
	#define _SGM_NOEXCEPT throw()
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	template<class T>
	class _sr_iterator_Helper;

	template< class ITR, bool = sgm::is_random_access_iterator<ITR>::value >
	struct _iterator_Distance;

	template<class T, bool IS_MUTABLE, bool IS_FORWARD>
	class Serial_iterator;


	struct srSize : No_Making
	{	
		enum : size_t{ INTERFACE = ULLONG_MAX, DYNAMIC = 0};  
	};


	template<bool TEMP_HOST> struct Move_if;


	template<class T, size_t S = srSize::DYNAMIC>
	class Serial;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
class sgm::_sr_iterator_Helper : No_Making
{
	template<class, bool, bool> friend class Serial_iterator;


	template
	<	bool M1, bool M2, bool F1, bool F2
	,	class itr1_t = Serial_iterator<T, M1, F1>
	,	class itr2_t = Serial_iterator<T, M2, F2>
	>
	static auto Substitution
	(	Serial_iterator<T, M1, F1>& itr1, Serial_iterator<T, M2, F2> const itr2
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
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class ITR>
struct sgm::_iterator_Distance<ITR, true> : No_Making
{
	static auto calc(ITR bi, ITR const ei)-> size_t
	{	
		return static_cast<size_t>(ei - bi);
	}
};


template<class ITR>
struct sgm::_iterator_Distance<ITR, false> : No_Making
{
	static_assert(is_iterator<ITR>::value, "ITR doesn't have iterator interface");

	static auto calc(ITR bi, ITR const ei)-> size_t
	{
		size_t dist = 0;

		for(;  bi != ei;  ++bi,  ++dist);

		return dist;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, bool IS_MUTABLE, bool IS_FORWARD>
class sgm::Serial_iterator
{
	static_assert(!isConst<T>::value, "T should be mutable here");

	friend class _sr_iterator_Helper<T>;

	using value_t = Selective_t<IS_MUTABLE, T, T const>;
	using iter_t = Serial_iterator;
	

public:
	Serial_iterator(value_t* arr, size_t idx) : _arr(arr), _idx(idx){}
	Serial_iterator(iter_t const&) = default;


	auto operator=(iter_t const&)-> iter_t& = default;


	template
	<	bool _M, bool _F
	,	class 
		=	Enable_if_t<IS_MUTABLE  ?  IS_FORWARD != _F  :  _M || _F != IS_FORWARD>
	>
	auto operator=(Serial_iterator<T, _M, _F> const itr)-> iter_t&
	{
		return _sr_iterator_Helper<T>::Substitution(*this, itr);
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


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
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


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


	auto operator-(iter_t const itr) const-> long long
	{
		bool const has_greater_idx = _idx > itr._idx;
		auto const du = has_greater_idx ? _idx - itr._idx : itr._idx - _idx;

		assert( du <= 0x7fffffffffffffffi64 && L"the difference exceeds maximum capacity.\n" );

		auto const LL_du = static_cast<long long>(du);

		return IS_FORWARD == has_greater_idx ? LL_du : -LL_du;
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


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
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	bool operator!=(iter_t const itr) const{  return _idx != itr._idx;  }
	bool operator==(iter_t const itr) const{  return _idx == itr._idx;  }
	
	bool operator<(iter_t const itr) const{  return Less(*this, itr);  }
	bool operator>(iter_t const itr) const{  return Less(itr, *this);  }

	bool operator<=(iter_t const itr) const{  return !(*this > itr);  }
	bool operator>=(iter_t const itr) const{  return !(*this < itr);  }


private:
	value_t* _arr;
	size_t _idx;


	static size_t shifted
	(	size_t const idx, bool const plus_dir = true, size_t const interval = 1
	)
	{
		return _sr_iterator_Helper<T>::template shifted<IS_FORWARD>(idx, plus_dir, interval);
	}

	static bool Less(iter_t const itr1, iter_t const itr2)
	{
		return _sr_iterator_Helper<T>::template Less<IS_FORWARD>(itr1._idx, itr2._idx);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<>
struct sgm::Move_if<false> : No_Making
{
	template<class T>
	static auto cast(T&& t)-> decltype( Forward<T>(t) ){  return Forward<T>(t);  }
};

template<>
struct sgm::Move_if<true> : No_Making
{
	template<class T>
	static auto cast(T&& t)-> decltype( Move(t) ){  return Move(t);  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, size_t S>
class sgm::Serial
{
	template<class Q, size_t _S> struct _Core : No_Making{  using type = Q[S];  };
	template<class Q> struct _Core<Q, srSize::INTERFACE> : No_Making{  using type = Q*;  };


protected:
	template
	<	bool TEMP_HOST = false, class ITR, class CON
	,	class RES_ITR = decltype(Declval<CON>().begin())
	,	class = Enable_if_t< is_iterator<ITR>::value && is_iterable<CON>::value >  
	>
	static auto _copy_AMAP(ITR bi, ITR const ei, CON& con)-> Dual_iterator<ITR, RES_ITR>
	{
		RES_ITR itr = con.begin();

		for( ;  bi != ei && itr != con.end();  *itr++ = Move_if<TEMP_HOST>::cast(*bi++) );

		return Dual_iteration(bi, itr);
	}


public:
	enum : size_t{SIZE = S};
	using core_t = typename _Core<T, S>::type;
	
	core_t _core;


	auto operator*()-> core_t&{  return _core;  }
	auto operator*() const-> core_t const&{  return _core;  }


	Serial() = default;
	Serial(Serial const&) = default;


	auto operator=(Serial const&)-> Serial& = default;

	auto operator=(Serial&& sr) _SGM_NOEXCEPT-> Serial&
	{
		for
		(	auto itr = begin(), sitr = sr.begin()
		;	itr != end()
		;	*itr++ = Move(*sitr++)
		);

		return *this;
	}

	auto cdata() const-> T const*	{  return _core;  }
	auto data() const-> T const*	{  return cdata();  }
	auto data()-> T*				{  return _core;  }

	auto operator[](size_t const idx) const-> T const&		{  return _core[idx];  }
	auto operator[](size_t const idx)-> T&				{  return _core[idx];  }
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	auto size() const-> size_t { return SIZE; }


	using iter_t = Serial_iterator<T, true, true>;
	using citer_t = Serial_iterator<T, false, true>;
	using riter_t = Serial_iterator<T, true, false>;
	using criter_t = Serial_iterator<T, false, false>;

	auto cbegin() const-> citer_t	{  return citer_t(_core, 0);  }
	auto begin() const-> citer_t	{  return cbegin();  }
	auto begin()-> iter_t			{  return iter_t(_core, 0);  }

	auto crend() const-> criter_t	{  return criter_t(_core, 0);  }
	auto rend() const-> criter_t	{  return crend();  }
	auto rend()-> riter_t			{  return riter_t(_core, 0);  }

	auto cend() const-> citer_t		{  return citer_t(_core, SIZE);  }
	auto end() const-> citer_t			{  return cend();  }
	auto end()-> iter_t				{  return iter_t(_core, SIZE);  }

	auto crbegin() const-> criter_t		{  return criter_t(_core, SIZE);  }
	auto rbegin() const-> criter_t		{  return crbegin();  }
	auto rbegin()-> riter_t			{  return riter_t(_core, SIZE);  }


	auto front() const-> T const&	{  return *cbegin();  }
	auto front()-> T&				{  return *begin();  }

	auto back() const-> T const&	{  return *crbegin();  }
	auto back()-> T&				{  return *rbegin();  }


	template
	<	class CON
	,	class 
		=	Enable_if_t
			<	is_iterable<CON>::value
			&&	S != srSize::INTERFACE
			&&	!isSame< Decay_t<CON>, Serial >::value  
			>
	> 
	operator CON() const{  return Decay_t<CON>(begin(), end());  }


	template
	<	class CON
	,	class 
		=	Enable_if_t
			<	is_iterable<CON>::value && S != srSize::INTERFACE
			&&	!isSame< Decay_t<CON>, Serial >::value  
			>
	> 
	auto operator=(CON&& con)-> Serial&
	{
		_copy_AMAP< isRvalueReference<decltype(con)>::value >
		(	con.begin(), con.end(), *this
		);

		return *this;
	}


	template
	<	class Q
	,	class 
		=	Enable_if_t< S != srSize::INTERFACE && isConvertible<Q, T>::value >
	>
	auto operator=(std::initializer_list<Q>&& iL)-> Serial&
	{
		_copy_AMAP<true>(iL.begin(), iL.end(), *this);

		return *this;
	}


	auto swap(Serial& sr) _SGM_NOEXCEPT-> Serial&
	{
		for
		(	auto itr = begin(), sitr = sr.begin()
		;	itr != end()
		;	std::swap(*itr++, *sitr++) 
		);

		return *this;
	}


};// end of class Serial
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Serial<T, sgm::srSize::DYNAMIC> : public Serial<T, srSize::INTERFACE>
{
	static_assert(!isConst<T>::value, "T should be mutable for dynamic allocation");


	using Helper = Serial<T, srSize::INTERFACE>;
	using value_t = Referenceless_t<T>;
	using Helper::_core;

	size_t _capacity, _size;


	void _alloc(size_t const capa)
	{
		_core
		=	static_cast<value_t*>
			(	capa != 0 ? ::operator new( sizeof(value_t) * capa ) : nullptr  
			);

		_capacity = capa, _size = 0;
	}


	template
	<	bool TEMP_HOST = false, class ITR
	,	class = Enable_if_t< is_iterator<ITR>::value >  
	>
	auto _cloning(ITR const bi, ITR const ei, size_t const length)-> Serial&
	{
		if(capacity() < length)
			this->~Serial(),  _alloc(length);

		auto const itrs = Helper::template _copy_AMAP<TEMP_HOST>(bi, ei, *this);

		return itrs._1 != ei ? merge_back<TEMP_HOST>(itrs._1, ei) : pop_back_from(itrs._2);
	}


public:
	Serial() : _capacity(0), _size(0){  _core = nullptr;  }


	template<  class ITR, class = Enable_if_t< is_iterator<ITR>::value >  >
	Serial(ITR bi, ITR const ei) 
	{
		_alloc( _iterator_Distance<ITR>::calc(bi, ei) ), 
		_cloning(bi, ei, capacity());
	}


	template
	<	class CON
	,	class 
		=	Enable_if_t<  is_iterable<CON>::value && !isSame< Serial, Decay_t<CON> >::value  >
	>
	Serial(CON&& con)
	{
		_alloc(con.size()), 
		_cloning< isRvalueReference<decltype(con)>::value >
		(	con.begin(), con.end(), capacity()
		);
	}


	template<  class Q, class = Enable_if_t< isConvertible<Q, T>::value >  >
	Serial(std::initializer_list<Q>&& iL)
	{
		_alloc(iL.size()),  _cloning<true>(iL.begin(), iL.end(), capacity());
	}


	Serial(Serial const& sr) : Serial(sr.cbegin(), sr.cend()){}


	Serial(Serial&& sr) _SGM_NOEXCEPT : _capacity(sr.capacity()), _size(sr.size())
	{
		_core = sr._core,  sr._capacity = sr._size = 0,  sr._core = nullptr;
	}


	explicit Serial(size_t const capa){  _alloc(capa);  }


	template<class...ARGS>
	Serial(size_t const size, ARGS const&... args)
	{
		for( _alloc(size);  _size < size;  new(_core + _size++) value_t(args...) );
	}


	~Serial()
	{	
		clear(),  ::operator delete(_core),  _core = nullptr,  _capacity = 0;  
	}


	auto operator=(Serial const& sr)-> Serial&
	{
		return _cloning(sr.cbegin(), sr.cend(), sr.capacity());
	}


	auto operator=(Serial&& sr) _SGM_NOEXCEPT-> Serial&
	{
		_capacity = sr.capacity(),  _size = sr.size(),  _core = sr._core,
		sr._capacity = sr._size = 0,  sr._core = nullptr;

		return *this;
	}


	template
	<	class CON
	,	class 
		=	Enable_if_t<  is_iterable<CON>::value && !isSame< Decay_t<CON>, Serial >::value  >
	>
	auto operator=(CON&& con)-> Serial&
	{
		return 
		_cloning< isRvalueReference<decltype(con)>::value >
		(	con.begin(), con.end(), con.size()
		);
	}


	template<  class Q, class = Enable_if_t< isConvertible<Q, T>::value >  >
	auto operator=(std::initializer_list<Q>&& iL)-> Serial&
	{
		return _cloning<true>(iL.begin(), iL.end(), iL.size());
	}


	auto capacity() const-> size_t{  return _capacity;  }
	auto size() const-> size_t{  return _size;  }

	bool is_null() const{  return Helper::cdata() == nullptr;  }
	bool is_empty() const{  return size() == 0;  }


	template<class...ARGS>
	auto emplace_back(ARGS&&...args)-> Serial&
	{
		assert(size() < capacity() && L"can't emplace_back : out of index.\n");

		new(_core + _size++) value_t( Forward<ARGS>(args)... );

		return *this;
	}


	template<class Q>
	auto operator>>(Q&& q)-> Serial&{  return emplace_back( Forward<Q>(q) );  }


	template
	<	bool TEMP_HOST = false, class ITR
	,	class = Enable_if_t< is_iterator<ITR>::value >  
	>
	auto merge_back(ITR bi, ITR const ei)-> Serial&
	{
		for
		(	assert
			(	size() + _iterator_Distance<ITR>::calc(bi, ei) <= capacity() 
			&&	L"cannot merge_back : out of index.\n"
			)	
		;	bi != ei
		;	*this >> Move_if<TEMP_HOST>::cast(*bi++) 
		);

		return *this;
	}


	template<  class ITR, class = Enable_if_t< is_iterator<ITR>::value >  >
	auto pop_back_from(ITR const itr)-> Serial&
	{
		for(auto d = end() - itr;  d-->0;  _core[--_size].~value_t());

		return *this;
	}


	auto pop_back(size_t const n = 1)-> Serial&
	{
		assert(n <= size() && L"cannot pop_back : out of index.\n");

		return pop_back_from(end() - n);  
	}


	auto clear()-> Serial&{  return pop_back_from(Helper::begin());  }


	auto swap(Serial& sr) _SGM_NOEXCEPT-> Serial&
	{
		using std::swap;

		swap(_capacity, sr._capacity),  swap(_size, sr._size),  swap(_core, sr._core);

		return *this;
	}


	using citer_t = typename Helper::citer_t;
	using iter_t = typename Helper::iter_t;
	using criter_t = typename Helper::criter_t;
	using riter_t = typename Helper::riter_t;

	auto cend() const-> citer_t		{  return citer_t(_core, size());  }
	auto end() const-> citer_t			{  return cend();  }
	auto end()-> iter_t				{  return iter_t(_core, size());  }

	auto crbegin() const-> criter_t		{  return criter_t(_core, size());  }
	auto rbegin() const-> criter_t		{  return crbegin();  }
	auto rbegin()-> riter_t			{  return riter_t(_core, size());  }

	auto back() const-> T const&	{  return *crbegin();  }
	auto back()-> T&				{  return *rbegin();  }


	template
	<	class CON
	,	class 
		=	Enable_if_t
			<	is_iterable<CON>::value && !isSame< Decay_t<CON>, Serial >::value
			>
	> 
	operator CON() const{  return Decay_t<CON>(Helper::begin(), end());  }
};// end of class Serial
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


	#undef _SGM_NOEXCEPT
#else
	#error _SGM_NOEXCEPT was already defined somewhere else.
#endif // end of #ifndef _SGM_NOEXCEPT
//========//========//========//========//=======#//========//========//========//========//=======#


#include <iterator>


template<class T, bool M, bool F>
struct std::iterator_traits< sgm::Serial_iterator<T, M, F> >
{
	using iterator_category = random_access_iterator_tag;
	using value_type = T;
	using difference_type = long long;
	using pointer = conditional_t<M, T*, T const*>;
	using reference = conditional_t<M, T&, T const&>;
};


#endif // end of #ifndef _SGM_SERIAL_