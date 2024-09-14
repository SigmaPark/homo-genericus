/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_ARRAY_
#define _SGM_ARRAY_


#include "SGM/iterable/iterable.hpp"
#include "SGM/Allocator/Allocator.hpp"
#include <initializer_list>
#include <cassert>
#include <iterator>
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	
	struct arrSize : Unconstructible
	{
		static size_t constexpr INTERFACE = std::numeric_limits<size_t>::max(),  DYNAMIC = 0;
	};


	template<class T>
	using _Default_Array_Allocator_t = Allocator<T>;


	template< class T, size_t N = arrSize::DYNAMIC, class ALC = _Default_Array_Allocator_t<T> >
	class Array;


	template<class T, bool IS_MUTABLE, bool IS_FORWARD, class CORE>
	class Array_iterator;


	struct Unsafe_Construction_for_Array;


	class _arr_itr_Helper;
	class _arr_itr_operation_Helper;


	template<class ITR1, class ITR2>
	struct Dual_iterator;


	template<class Q, size_t _S>
	struct _Array_Core;

	enum class _Array_by_Tag{};

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class Q, std::size_t _S>
struct sgm::_Array_Core
{
	using type = Q[_S];  

	type data;	
};

template<class Q>
struct sgm::_Array_Core<Q, sgm::arrSize::INTERFACE>
{
	using type = Q*; 
	
	type data = nullptr;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::_arr_itr_Helper : Unconstructible
{
private:
	template<class, bool, bool, class>
	friend class sgm::Array_iterator;


	template<class T, bool M, bool FWD, class CORE>
	static auto core_ptr(Array_iterator<T, M, FWD, CORE> itr) noexcept
	->	SGM_DECLTYPE_AUTO(  itr._core_ptr  )


	template<class T, bool M, bool FWD, class CORE>
	static auto idx(Array_iterator<T, M, FWD, CORE> itr) noexcept
	->	SGM_DECLTYPE_AUTO(  itr._idx  )
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::_arr_itr_operation_Helper : Unconstructible
{
private:
	template<class, bool, bool, class>
	friend class sgm::Array_iterator;


	template<bool IS_FORWARD>
	static auto shifted(size_t const idx, bool const plus_dir, ptrdiff_t const diff) noexcept
	->	Enable_if_t<IS_FORWARD, size_t>{  return plus_dir ? idx + diff : idx - diff;  }

	template<bool IS_FORWARD>
	static auto shifted(size_t const idx, bool const plus_dir, ptrdiff_t const diff) noexcept
	->	Enable_if_t<!IS_FORWARD, size_t>{  return plus_dir ? idx - diff : idx + diff;  }


	template<bool IS_FORWARD>
	static auto Less(size_t const idx1, size_t const idx2) noexcept
	->	Enable_if_t<IS_FORWARD, bool>{  return idx1 < idx2;  }

	template<bool IS_FORWARD>
	static auto Less(size_t const idx1, size_t const idx2) noexcept
	->	Enable_if_t<!IS_FORWARD, bool>{  return idx1 > idx2;  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, bool IS_MUTABLE, bool IS_FORWARD, class CORE>
class sgm::Array_iterator
{
private:
	static_assert(!is_Const<T>::value, "T should be mutable here");

	friend class sgm::_arr_itr_Helper;

	using _itr_t = Array_iterator;
	using _core_t = Selective_t<IS_MUTABLE, CORE, CORE const>;
	using _elem_t = Selective_t<IS_MUTABLE, T, T const>;

public:
	using value_type = Decay_t<_elem_t>;


	Array_iterator(_core_t& core, size_t const idx) : _core_ptr(&core), _idx(idx){}


	Array_iterator(Array_iterator<T, !IS_MUTABLE, IS_FORWARD, CORE> const itr)
	:	_core_ptr( _arr_itr_Helper::core_ptr(itr) ), _idx( _arr_itr_Helper::idx(itr) )
	{
		static_assert(!IS_MUTABLE, "cannot assign immutable iterator to mutable one.");
	}


	auto operator=(Array_iterator<T, !IS_MUTABLE, IS_FORWARD, CORE> const itr)-> _itr_t&
	{
		static_assert(!IS_MUTABLE, "cannot assign immutable iterator to mutable one.");

		_core_ptr = _arr_itr_Helper::core_ptr(itr);
		_idx = _arr_itr_Helper::idx(itr);

		return *this;
	}


	auto operator[](ptrdiff_t const diff) const noexcept
	->	_elem_t&{  return *( _arr() + _shifted(_idx, true, diff) - (IS_FORWARD ? 0 : 1) );  }

	auto operator*() const noexcept-> _elem_t&{  return (*this)[0];  }

	auto operator->() const noexcept-> _elem_t*{  return &**this;  }

	auto operator++(int) noexcept-> _itr_t
	{
		_itr_t const itr = *this;

		_idx = _shifted(_idx, true);

		return itr;
	}

	auto operator--(int) noexcept-> _itr_t
	{
		_itr_t const itr = *this;

		_idx = _shifted(_idx, false);

		return itr;
	}

	
	auto operator+(ptrdiff_t const diff) const noexcept
	->	_itr_t{  return _itr_t( *_core_ptr, _shifted(_idx, true, diff) );  }

	auto operator-(ptrdiff_t const diff) const noexcept-> _itr_t{  return *this + -diff;  }


	auto operator-(_itr_t const itr) const noexcept-> ptrdiff_t
	{
		bool const has_greater_idx = _idx > itr._idx;
		auto const diff_ULL = has_greater_idx ? _idx - itr._idx : itr._idx - _idx;

		assert
		(	diff_ULL <= static_cast<size_t>(std::numeric_limits<ptrdiff_t>::max())
		&&	"the difference exceeds maximum threshold value.\n"
		);

		auto const diff = static_cast<ptrdiff_t>(diff_ULL);

		return IS_FORWARD == has_greater_idx ? diff : -diff;
	}


	auto operator+=(ptrdiff_t const diff) noexcept-> _itr_t&
	{
		_idx = _shifted(_idx, true, diff);

		return *this;
	}

	auto operator-=(ptrdiff_t const diff) noexcept-> _itr_t&{  return *this += -diff;  }

	auto operator++() noexcept-> _itr_t&{  return *this += ptrdiff_t(1);  }
	auto operator--() noexcept-> _itr_t&{  return *this -= ptrdiff_t(1);  }


	template<bool M>
	auto operator==(Array_iterator<T, M, IS_FORWARD, CORE> const itr) const noexcept
	->	bool{  return _idx == _arr_itr_Helper::idx(itr);  }

	template<class Q>
	auto operator!=(Q const& q) const noexcept-> SGM_DECLTYPE_AUTO(  !(*this == q)  )


	auto operator<(_itr_t const itr) const noexcept-> bool{  return _Less(*this, itr);  }
	auto operator>(_itr_t const itr) const noexcept-> bool{  return _Less(itr, *this);  }

	auto operator<(Array_iterator<T, !IS_MUTABLE, IS_FORWARD, CORE> const itr) const noexcept
	->	bool
	{
		using _citr_t = Array_iterator<T, false, IS_FORWARD, CORE>;

		return static_cast<_citr_t>(*this) < static_cast<_citr_t>(itr);
	}

	auto operator>(Array_iterator<T, !IS_MUTABLE, IS_FORWARD, CORE> const itr) const noexcept
	->	bool
	{
		using _citr_t = Array_iterator<T, false, IS_FORWARD, CORE>;

		return static_cast<_citr_t>(*this) > static_cast<_citr_t>(itr);
	}


	template<class Q>
	auto operator<=(Q const& q) const noexcept-> SGM_DECLTYPE_AUTO(  !(*this > q)  )

	template<class Q>
	auto operator>=(Q const& q) const noexcept-> SGM_DECLTYPE_AUTO(  !(*this < q)  )


private:
	_core_t* _core_ptr;
	size_t _idx;


	auto _arr() const noexcept-> _elem_t*{  return _core_ptr->data;  }


	static auto _shifted(size_t const idx, bool const plus_dir, ptrdiff_t const diff = 1)
	noexcept-> size_t
	{
		return _arr_itr_operation_Helper::shifted<IS_FORWARD>(idx, plus_dir, diff);
	}

	static auto _Less(_itr_t const itr1, _itr_t const itr2) noexcept-> bool
	{
		return _arr_itr_operation_Helper::Less<IS_FORWARD>(itr1._idx, itr2._idx);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class ITR1, class ITR2>
struct sgm::Dual_iterator
{
	ITR1 _1;
	ITR2 _2;

	
	auto operator++(int) noexcept-> Dual_iterator
	{
		auto const itr = *this;

		_1++,  _2++;

		return itr;
	}

	auto operator++() noexcept-> Dual_iterator&
	{
		++_1,  ++_2;

		return *this;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, std::size_t N, class>
class sgm::Array
{
protected:
	template
	<	class ITR, class RG
	,	class RES_ITR = decltype( Begin(Mock<RG>()) )
	,	class = Enable_if_t< is_iterator<ITR>::value && is_iterable<RG>::value >
	>
	static auto _copy_AMAP(ITR bi, ITR const ei, RG& rg) noexcept(is_Move_iterator<ITR>::value)
	->	Dual_iterator<ITR, RES_ITR>
	{
		RES_ITR itr = Begin(rg);

		for
		(	auto const rg_end = End(rg)
		;	bi != ei && itr != rg_end
		;	*itr++ = *bi++
		);

		return {bi, itr};
	}


public:
	static size_t constexpr size_v = N;
	using core_t = _Array_Core<T, N>;
	using data_t = typename core_t::type;


	core_t _core;


	auto operator+() const noexcept-> data_t const&{  return _core.data;  }
	auto operator+() noexcept-> data_t&{  return _core.data;  }


	Array() = default;
	Array(Array const&) = default;

	auto operator=(Array const&)-> Array& = default;

	auto operator=(Array&& rhs_arr) noexcept-> Array&
	{
		for
		(	auto Litr = begin(), ritr = rhs_arr.begin()
		;	Litr != end()
		;	*Litr++ = Move(*ritr++)
		);

		return *this;
	}


	auto cdata() const noexcept-> T const*{  return _core.data;  }
	auto data() const noexcept-> SGM_DECLTYPE_AUTO(  cdata()  )
	auto data() noexcept-> T*{  return _core.data;  }

	auto operator[](size_t const idx) const noexcept
	->	T const&{  return _core.data[static_cast<ptrdiff_t>(idx)];  }

	auto operator[](size_t const idx) noexcept
	->	T&{  return _core.data[static_cast<ptrdiff_t>(idx)];  }

	auto constexpr size() const noexcept-> size_t{  return size_v;  }


	using iterator = Array_iterator<T, true, true, core_t>;
	using const_iterator = Array_iterator<T, false, true, core_t>;
	using reverse_iterator = Array_iterator<T, true, false, core_t>;
	using const_reverse_iterator = Array_iterator<T, false, false, core_t>;

	auto cbegin() const noexcept-> const_iterator{  return {_core, 0};  }
	auto begin() const noexcept-> const_iterator{  return cbegin();  }
	auto begin() noexcept-> iterator{  return {_core, 0};  }

	auto crend() const noexcept-> const_reverse_iterator{  return {_core, 0};  }
	auto rend() const noexcept-> const_reverse_iterator{  return crend();  }
	auto rend() noexcept-> reverse_iterator{  return {_core, 0};  }

	auto cend() const noexcept-> const_iterator{  return {_core, size_v};  }
	auto end() const noexcept-> const_iterator{  return cend();  }
	auto end() noexcept-> iterator{  return {_core, size_v};  }

	auto crbegin() const noexcept-> const_reverse_iterator{  return {_core, size_v};  }
	auto rbegin() const noexcept-> const_reverse_iterator{  return crbegin();  }
	auto rbegin() noexcept-> reverse_iterator{  return {_core, size_v};  }


	auto front() const noexcept-> T const&{  return *cbegin();  }
	auto front() noexcept-> T&{  return *begin();  }

	auto back() const noexcept-> T const&{  return *crbegin();  }
	auto back() noexcept-> T&{  return *rbegin();  }


	template
	<	class RG
	,	class
		=	Enable_if_t
			<	is_iterable<RG>::value && N != arrSize::INTERFACE
			&&	!is_Same< Decay_t<RG>, Array >::value
			>
	>
	operator RG() const{  return Decay_t<RG>(begin(), end());  }


	template
	<	class RG
	,	class
		=	Enable_if_t
			<	is_iterable<RG>::value && N != arrSize::INTERFACE
			&&	!is_Same< Decay_t<RG>, Array >::value
			>
	>
	auto operator=(RG&& rg) noexcept(is_Rvalue_Reference<RG&&>::value)-> Array&
	{
		_copy_AMAP( fBegin<RG>(rg), fEnd<RG>(rg), *this );

		return *this;
	}


	template
	<	class Q
	,	class = Enable_if_t< N != arrSize::INTERFACE && is_Convertible<Q, T>::value >
	>
	auto operator=(std::initializer_list<Q>&& iL) noexcept-> Array&
	{
		_copy_AMAP(iL.begin(), iL.end(), *this);

		return *this;
	}


	auto swap(Array& arr) noexcept-> Array&
	{
		for
		(	auto desitr = begin(),  srcitr = arr.begin()
		;	desitr != end()
		;	Swap(*desitr++, *srcitr++)
		);

		return *this;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, class ALC>
class sgm::Array<T, sgm::arrSize::DYNAMIC, ALC> : public Array<T, arrSize::INTERFACE, ALC>
{
public:
	static_assert(!is_Const<T>::value, "T should be mutable for dynamic allocation.");

	using allocator_t = ALC;
	using value_type = typename allocator_t::value_type;


private:
	friend struct sgm::Unsafe_Construction_for_Array;


	using _base_t = Array<T, arrSize::INTERFACE, allocator_t>;
	using _base_t::_core;


	size_t _capacity, _size;
	allocator_t _alc;


	auto _base() const noexcept-> _base_t const&{  return *this;  }
	auto _base() noexcept-> _base_t&{  return *this;  }


	void _alloc(size_t const capa) 
	{
		_core.data = capa != 0  ?  _alc.allocate(capa)  :  (value_type*)nullptr;

		_capacity = capa,  _size = 0;
	}


	template
	<	class ME, class ITR, class = Enable_if_t< is_iterator<ITR>::value >
	,	bool IS_ASSIGNABLE_ELEM 
		=	(	Has_Operator_Copy_Assignment< typename Decay_t<ME>::value_type >::value
			||	(	is_Move_iterator<ITR>::value 
				&&	Has_Operator_Move_Assignment< typename Decay_t<ME>::value_type >::value
				)
			)
	>
	static auto _cloning(ITR const bi, ITR const ei, size_t const len, ME& me)
	noexcept(is_Move_iterator<ITR>::value)-> Enable_if_t<IS_ASSIGNABLE_ELEM, Array&>
	{
		if(me.capacity() < len)
			me.~Array(),  me._alloc(len);

		auto const itrs = me._base()._copy_AMAP(bi, ei, me);

		return itrs._1 != ei ? me.merge_back(itrs._1, ei) : me.pop_back_from(itrs._2);
	}

	template
	<	class ME, class ITR, class = Enable_if_t< is_iterator<ITR>::value >
	,	bool IS_ASSIGNABLE_ELEM 
		=	(	Has_Operator_Copy_Assignment< typename Decay_t<ME>::value_type >::value
			||	(	is_Move_iterator<ITR>::value 
				&&	Has_Operator_Move_Assignment< typename Decay_t<ME>::value_type >::value
				)
			)
	>
	static auto _cloning(ITR const bi, ITR const ei, size_t const len, ME& me)
	noexcept(is_Move_iterator<ITR>::value)-> Enable_if_t<!IS_ASSIGNABLE_ELEM, Array&>
	{
		if(me.capacity() < len)
			me.~Array(),  me._alloc(len);
		else
			me.clear();

		return me.merge_back(bi, ei);
	}


public:
	Array() : _base_t{}, _capacity(0), _size(0), _alc(){}

	explicit Array(size_t const capa) : Array(){  _alloc(capa);  }

	template<class...ARGS>
	Array(size_t const s, ARGS&&... args) : Array()
	{
		_alloc(s);

		while(_size < s)
			_alc.construct( _core.data + _size++, Forward<ARGS>(args)... );
	}

	template<  class ITR, class = Enable_if_t< is_iterator<ITR>::value >  >
	Array(ITR bi, ITR const ei) : Array()
	{
		_alloc( Difference(bi, ei) );
		_cloning(bi, ei, capacity(), *this);
	}

	template
	<	class RG
	,	class
		=	Enable_if_t<  is_iterable<RG>::value && !is_Same< Array, Decay_t<RG> >::value  >
	>
	Array(RG&& rg) : Array()
	{
		_alloc( Size(rg) );
		_cloning( fBegin<RG>(rg), fEnd<RG>(rg), capacity(), *this );
	}

	template<  class Q, class = Enable_if_t< is_Convertible<Q, T>::value >  >
	Array(std::initializer_list<Q>&& iL) : Array()
	{
		_alloc(iL.size());
		_cloning(iL.begin(), iL.end(), capacity(), *this);
	}

	Array(Array const& arr) : _base_t{}, _capacity(0), _size(0), _alc(arr._alc)
	{
		_alloc( Size(arr) );
		_cloning(arr.cbegin(), arr.cend(), capacity(), *this);
	}

	Array(Array&& arr) noexcept
	:	_base_t{arr._core}, _capacity(arr.capacity()), _size(arr.size()), _alc( Move(arr._alc) )
	{
		arr._capacity = arr._size = 0;
		arr._core.data = nullptr;
	}


	~Array()
	{  
		clear();  
		_alc.deallocate(_core.data, capacity());

		_core.data = nullptr,  _capacity = 0;  
	}


	auto operator=(Array const& arr)
	->	Array&{  return _cloning(arr.cbegin(), arr.cend(), arr.capacity(), *this);  }

	auto operator=(Array&& arr) noexcept-> Array&
	{
		auto temp = Move(arr);

		swap(temp);

		return *this;
	}

	template
	<	class RG
	,	class
		=	Enable_if_t<  is_iterable<RG>::value && !is_Same< Decay_t<RG>, Array >::value  >
	>
	auto operator=(RG&& rg) noexcept(is_Rvalue_Reference<RG&&>::value)-> Array&
	{	
		return _cloning( fBegin<RG>(rg), fEnd<RG>(rg), Size(rg), *this );
	}

	template<  class Q, class = Enable_if_t< is_Convertible<Q, T>::value >  >
	auto operator=(std::initializer_list<Q>&& iL) noexcept-> Array&
	{
		return _cloning(iL.begin(), iL.end(), iL.size(), *this);
	}


	auto capacity() const noexcept-> size_t{  return _capacity;  }
	auto size() const noexcept-> size_t{  return _size;  }

	bool is_null() const noexcept {  return _base().cdata() == nullptr;  }
	bool is_empty() const noexcept {  return size() == 0;  }


	template<class...ARGS>
	auto emplace_back(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)-> Array&
	{
		assert(size() < capacity() && L"Array::emplace_back Error : out of index.\n");

		_alc.construct( _core.data + _size++, Forward<ARGS>(args)... );

		return *this;
	}


	template<class Q>
	auto operator>>(Q&& q) noexcept(is_Rvalue_Reference<Q&&>::value)
	->	Array&{  return emplace_back( Forward<Q>(q) );  }


	template<  class ITR, class = Enable_if_t< is_iterator<ITR>::value >  >
	auto merge_back(ITR bi, ITR const ei) noexcept(is_Move_iterator<ITR>::value)-> Array&
	{
		assert
		(	size() + Difference(bi, ei) <= capacity() 
		&&	L"Array::merge_back Error : out of index.\n"
		);

		while(bi != ei)
			*this >> *bi++;

		return *this;
	}


	template<  class ITR, class = Enable_if_t< is_iterator<ITR>::value >  >
	auto pop_back_from(ITR const itr) noexcept-> Array&
	{
		for(auto d = end() - itr;  d-->0;  pop_back());

		return *this;
	}


	auto pop_back() noexcept-> Array&
	{
		assert(!is_empty() && L"Array::pop_back Error : out of index.\n");

		return _alc.destroy(_core.data + --_size),  *this;
	}

	auto pop_back(size_t const n) noexcept-> Array&
	{
		assert(n <= size() && L"Array::pop_back Error : out of index.\n");

		return pop_back_from(end() - n);  
	}


	auto clear() noexcept-> Array&{  return pop_back_from(_base().begin());  }


	auto swap(Array& arr) noexcept-> Array&
	{
		Swap(_capacity, arr._capacity);
		Swap(_size, arr._size);
		Swap(_core, arr._core);
		Swap(_alc, arr._alc);

		return *this;
	}

	
	using typename _base_t::iterator;
	using typename _base_t::const_iterator;
	using typename _base_t::reverse_iterator;
	using typename _base_t::const_reverse_iterator;

	auto cend() const noexcept-> const_iterator{  return {_core, size()};  }
	auto end() const noexcept-> const_iterator{  return cend();  }
	auto end() noexcept-> iterator{  return {_core, size()};  }

	auto crbegin() const noexcept-> const_reverse_iterator{  return{_core, size()};  }
	auto rbegin() const noexcept-> const_reverse_iterator{  return crbegin();  }
	auto rbegin() noexcept-> reverse_iterator{  return {_core, size()};  }


	auto back() const noexcept-> T const&{  return *crbegin();  }
	auto back() noexcept-> T&{  return *rbegin();  }


	template
	<	class RG
	,	class = Enable_if_t<  is_iterable<RG>::value && !is_Same< Decay_t<RG>, Array >::value  >
	> 
	operator RG() const{  return Decay_t<RG>(_base().begin(), end());  }


	template
	<	class _ALC, class...ARGS
	,	class _A = Decay_t<_ALC>
	,	class = Guaranteed_t< is_Allocator<_A>::value > 
	>
	static auto by(_ALC&& alloc, ARGS&&...args)-> SGM_DECLTYPE_AUTO
	(
		Array<value_type, arrSize::DYNAMIC, _A>
		(	_Array_by_Tag{}, Forward<_ALC>(alloc), Forward<ARGS>(args)...
		)
	)


private:
	friend class Array< value_type, arrSize::DYNAMIC, _Default_Array_Allocator_t<value_type> >;


	template<class A>
	Array(_Array_by_Tag, A&& alc) 
	:	_base_t{nullptr}, _capacity(0), _size(0), _alc( Forward<A>(alc) ){}

	template<  class ITR, class A, class = Enable_if_t< is_iterator<ITR>::value >  >
	Array(_Array_by_Tag tag, A&& alc, ITR bi, ITR const ei) : Array( tag, Forward<A>(alc) )
	{
		_alloc( Difference(bi, ei) );
		_cloning(bi, ei, capacity(), *this);
	}

	template
	<	class RG, class A
	,	class
		=	Enable_if_t<  is_iterable<RG>::value && !is_Same< Array, Decay_t<RG> >::value  >
	>
	Array(_Array_by_Tag tag, A&& alc, RG&& rg) : Array( tag, Forward<A>(alc) )
	{
		_alloc( Size(rg) );
		_cloning( fBegin<RG>(rg), fEnd<RG>(rg), capacity(), *this );
	}

	template<  class Q, class A, class = Enable_if_t< is_Convertible<Q, T>::value >  >
	Array(_Array_by_Tag tag, A&& alc, std::initializer_list<Q>&& iL) 
	:	Array( tag, Forward<A>(alc) )
	{
		_alloc(iL.size()),
		_cloning(iL.begin(), iL.end(), capacity(), *this);
	}

	template<class A>
	explicit Array(_Array_by_Tag tag, A&& alc, size_t const capa) 
	:	Array( tag, Forward<A>(alc) ){  _alloc(capa);  }

	template<class A, class...ARGS>
	Array(_Array_by_Tag tag, A&& alc, size_t const s, ARGS&&... args) 
	:	Array( tag, Forward<A>(alc) )
	{
		_alloc(s);

		while(_size < s)
			_alc.construct( _core.data + _size++, Forward<ARGS>(args)... );
	}
};


namespace sgm
{

	template<class T>
	static void Swap(Array<T>& arr0, Array<T>& arr1) noexcept{  arr0.swap(arr1);  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::Unsafe_Construction_for_Array : Unconstructible
{
	template< class T, class ALC = _Default_Array_Allocator_t<T> >
	static auto create
	(	T* data_arr, size_t const size, size_t const capacity, ALC&& allocator = {}
	)->	Array< T, arrSize::DYNAMIC, Decay_t<ALC> >
	{
		Array< T, arrSize::DYNAMIC, Decay_t<ALC> > res;
	
		res._core.data = data_arr;
		res._capacity = capacity;
		res._size = size;
		res._alc = Forward<ALC>(allocator);
	
		return res;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace std
{

	template<class T, bool M, bool F, class CORE>
	struct iterator_traits< sgm::Array_iterator<T, M, F, CORE> >
	{
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = sgm::Selective_t<M, T, T const>*;
		using reference = sgm::Pointerless_t<pointer>&;
	};

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_ARRAY_