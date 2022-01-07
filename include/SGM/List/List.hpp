/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_LIST_
#define _SGM_LIST_


#include "../Allocator/Allocator.hpp"
#include "../iterable/iterable.hpp"
#include <initializer_list>
#include <iterator>


namespace sgm
{

	template<class T>
	class List_Node;


	template<class T>
	using _Default_List_Allocator_t = Allocator< List_Node<T> >;

	template< class T, class ALLOC = _Default_List_Allocator_t<T> >
	class List;


	template<class T, bool IS_MUTABLE, bool IS_FORWARD>
	class List_iterator;

	enum class _List_by_Tag{};

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
class sgm::List_Node
{
public:
	using value_type = T;


	List_Node(List_Node* const prev_ptr, List_Node* const next_ptr)
	:	_prev_ptr(prev_ptr), _next_ptr(next_ptr), _has_value(false){}

	template<class Q, class...ARGS>
	List_Node(List_Node* const prev_ptr, List_Node* const next_ptr, Q&& q, ARGS&&...args)
	:	_prev_ptr(prev_ptr), _next_ptr(next_ptr), _has_value(true)
	{
		new(_buffer) T( Forward<Q>(q), Forward<ARGS>(args)... );
	}

	~List_Node()
	{
		_prev_ptr = _next_ptr = nullptr;
		_has_value = false;

		value().~T();
	}


	auto prev_ptr() const-> List_Node*{  return _prev_ptr;  }
	auto prev_ptr()-> List_Node*&{  return _prev_ptr;  }
	auto next_ptr() const-> List_Node*{  return _next_ptr;  }
	auto next_ptr()-> List_Node*&{  return _next_ptr;  }

	auto rprev_ptr() const-> List_Node*{  return _next_ptr;  }
	auto rprev_ptr()-> List_Node*&{  return _next_ptr;  }
	auto rnext_ptr() const-> List_Node*{  return _prev_ptr;  }
	auto rnext_ptr()-> List_Node*&{  return _prev_ptr;  }


	auto has_value() const-> bool{  return _has_value;  }
	auto has_value()-> bool&{  return _has_value;  }

	auto value() const-> T const&{  return *reinterpret_cast<T const*>(_buffer);  }
	auto value()-> T&{  return *reinterpret_cast<T*>(_buffer);  }


private:
	List_Node *_prev_ptr, *_next_ptr;
	bool _has_value;
	unsigned char _buffer[sizeof(T)];
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#



//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, class ALLOC>
class sgm::List
{
public:
	using value_type = T;
	using allocator_t = ALLOC;


	List() : _pnode_begin(nullptr), _pnode_rbegin(nullptr), _allocator(){}

	List(List const& Li) : List()
	{
		if(Li.is_empty())
			return;

		List_Node<T> 
			*pnode_src = Li._pnode_begin,
			*pnode_cur = _alloc(nullptr, nullptr, pnode_src->value());
			
		for(_pnode_begin = pnode_cur;  pnode_src->next_ptr() != nullptr;  )
		{
			pnode_src = pnode_src->next_ptr();

			pnode_cur = pnode_cur->next_ptr() = _alloc(pnode_cur, nullptr, pnode_src->value());
		}

		_pnode_rbegin = pnode_cur;
	}

	List(List&& Li) noexcept : List()
	{
		_pnode_begin = Li._pnode_begin;
		_pnode_rbegin = Li._pnode_rbegin;

		Li._pnode_begin = nullptr;
		Li._pnode_rbegin = nullptr;
	}


	List(std::initializer_list<T>&& Li) noexcept : List()
	{
		if(Li.begin() == Li.end())
			return;

		auto srcitr = Li.begin();
		List_Node<T> *pnode_cur = _alloc( nullptr, nullptr, Move(*srcitr++) );
			
		for(_pnode_begin = pnode_cur;  srcitr != Li.end();  )
			pnode_cur = pnode_cur->next_ptr() = _alloc( pnode_cur, nullptr, Move(*srcitr++) );
		
		_pnode_rbegin = pnode_cur;		
	}

	template<  class RG, class = Enable_if_t< is_iterable<RG>::value >  >
	List(RG&& rg) noexcept(is_Rvalue_Reference<RG&&>::value) : List()
	{
		if(rg.begin() == rg.end())
			return;

		bool constexpr is_temp_rg_v = is_Rvalue_Reference<RG&&>::value;

		auto srcitr = rg.begin();
		
		List_Node<T> *pnode_cur = _alloc( nullptr, nullptr, Move_if<is_temp_rg_v>(*srcitr++) );
			
		for(_pnode_begin = pnode_cur;  srcitr != rg.end();  )
			pnode_cur 
			=	pnode_cur->next_ptr() 
			=	_alloc( pnode_cur, nullptr, Move_if<is_temp_rg_v>(*srcitr++) );
		
		_pnode_rbegin = pnode_cur;
	}


	~List(){  clear();  }


	auto operator=(List const& Li)-> List&;
	auto operator=(List&& Li) noexcept-> List&;
	
	template<  class Q, class = Enable_if_t< is_Convertible<Q, T>::value >  >
	auto operator=(std::initializer_list<Q>&& initLi) noexcept-> List&;

	template
	<	class RG
	,	class
		=	Enable_if_t<  is_iterable<RG>::value && !is_Same< Decay_t<RG>, List >::value  >
	>
	auto operator=(RG&& rg) noexcept(is_Rvalue_Reference<RG&&>::value)-> List&;


	template
	<	class RG
	,	class = Enable_if_t<  is_iterable<RG>::value && !is_Same< Decay_t<RG>, List >::value  >
	> 
	operator RG() const{  return Decay_t<RG>(cbegin(), cend());  }


	using iterator = List_iterator<T, true, true>;
	using const_iterator = List_iterator<T, false, true>;
	using reverse_iterator = List_iterator<T, true, false>;
	using const_reverse_iterator = List_iterator<T, false, false>;

	auto cbegin() const-> const_iterator;
	auto begin() const-> SGM_DECLTYPE_AUTO(  cbegin()  )
	auto begin()-> iterator;

	auto cend() const-> const_iterator;
	auto end() const-> SGM_DECLTYPE_AUTO(  cend()  )
	auto end()-> iterator;

	auto crbegin() const-> const_reverse_iterator;
	auto rbegin() const-> SGM_DECLTYPE_AUTO(  crbegin()  )
	auto rbegin()-> reverse_iterator;

	auto crend() const-> const_reverse_iterator;
	auto rend() const-> SGM_DECLTYPE_AUTO(  crend()  )
	auto rend()-> reverse_iterator;


	auto front() const-> T const&{  return *cbegin();  }
	auto front()-> T&{  return *begin();  }
	auto back() const-> T const&{  return *crbegin();  }
	auto back()-> T&{  return *rbegin();  }


	auto size() const-> size_t;
	auto is_empty() const-> bool{  return cbegin() == cend();  }

	auto clear()-> List&;


	template<class...ARGS>
	auto emplace_back(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)-> List&;

	template<class...ARGS>
	auto emplace_front(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)-> List&;

	template<class...ARGS>
	auto emplace(const_iterator const citr, ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	->	iterator;


	auto pop_back()-> List&;
	auto pop_front()-> List&;
	auto pop(const_iterator const citr)-> iterator;
	auto pop(const_iterator const bi, const_iterator const ei)-> iterator;
	

	template
	<	class _ALC, class...ARGS
	,	class _A = Decay_t<_ALC>
	,	class = Guaranteed_t< is_Allocator<_A>::value >
	>
	static auto by(_ALC&& alloc, ARGS&&...args)-> SGM_DECLTYPE_AUTO
	(	
		List<value_type, _A>( _List_by_Tag{}, Forward<_ALC>(alloc), Forward<ARGS>(args)... )
	)


private:
	List_Node<T> *_pnode_begin, *_pnode_rbegin;
	allocator_t _allocator;


	template<class...ARGS>
	auto _alloc(ARGS&&...args)-> List_Node<T>*
	{
		List_Node<T>* pres = nullptr;

		_allocator.construct( pres, Forward<ARGS>(args)... );

		return pres;
	}


	friend class sgm::List<value_type>;

	template<class A>
	List(_List_by_Tag, A&& alc);
	
	template<class A>
	List(_List_by_Tag, A&& alc, List const&);

	template<class A>
	List(_List_by_Tag, A&& alc, List&&) noexcept;

	template<class A>
	List(_List_by_Tag, A&& alc, std::initializer_list<T>&& iL) noexcept;

	template<  class RG, class A, class = Enable_if_t< is_iterable<RG>::value >  >
	List(_List_by_Tag, A&& alc, RG&& rg) noexcept(is_Rvalue_Reference<RG&&>::value);
};


#endif // end of #ifndef _SGM_LIST_

