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
	struct List_Node;


	template<class T>
	using _Default_List_Allocator_t = Allocator< List_Node<T> >;

	template< class T, class ALLOC = _Default_List_Allocator_t<T> >
	class List;


	template<class T, bool IS_MUTABLE, bool IS_FORWARD>
	class List_iterator;

	struct _List_itr_Helper;

	enum class _List_by_Tag{};

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
struct sgm::List_Node
{
	using value_type = T;

	List_Node *front_ptr, *back_ptr;
	T value;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::_List_itr_Helper : Unconstructible
{
	template<class T, bool IS_MUTABLE, bool IS_FORWARD>
	static auto node_ptr(List_iterator<T, IS_MUTABLE, IS_FORWARD>& itr) noexcept
	->	List_Node<T>*&{  return itr._node_ptr;  }

	template<class T, bool IS_MUTABLE, bool IS_FORWARD>
	static auto node_ptr(List_iterator<T, IS_MUTABLE, IS_FORWARD> const& itr) noexcept
	->	List_Node<T>* const&{  return itr._node_ptr;  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, bool IS_MUTABLE, bool IS_FORWARD>
class sgm::List_iterator
{
private:
	using _itr_t = List_iterator;
	using _elem_t = Selective_t<IS_MUTABLE, T, T const>;
	using _node_t = List_Node<T>;
	
	friend struct _List_itr_Helper;


public:
	using value_type = T;
	static bool constexpr is_mutable_v = IS_MUTABLE, is_forward_v = IS_FORWARD;


	List_iterator(_node_t* node_ptr = nullptr) : _node_ptr(node_ptr){}

	List_iterator(_itr_t const&) = default;

	List_iterator(List_iterator<T, !IS_MUTABLE, IS_FORWARD> const itr) : _node_ptr(itr._node_ptr)
	{
		static_assert(!IS_MUTABLE, "cannot assign immutable iterator to mutable one .");
	}


	operator List_iterator<T, !IS_MUTABLE, IS_FORWARD>() const
	{
		static_assert(IS_MUTABLE, "cannot assign immutable iterator to mutable one .");

		return *this;
	}


	auto operator=(_itr_t const&)-> _itr_t& = default;

	auto operator=(List_iterator<T, !IS_MUTABLE, IS_FORWARD> const& itr)-> _itr_t&
	{
		static_assert(!IS_MUTABLE, "cannot assign immutable iterator to mutable one .");
		
		_node_ptr = _List_itr_Helper::node_ptr(itr);

		return *this;
	}


	auto operator*() const noexcept-> _elem_t&{  return _node_ptr->value;  }

	auto operator->() const noexcept-> _elem_t*{  return &**this;  }


	auto operator++()-> _itr_t&
	{
		_update_node<true>(_node_ptr);

		return *this;
	}


	auto operator--()-> _itr_t&
	{
		_update_node<false>(_node_ptr);

		return *this;
	}


	auto operator++(int)-> _itr_t
	{
		auto res = *this;

		++*this;

		return res;
	}

	auto operator--(int)-> _itr_t
	{
		auto res = *this;

		--*this;

		return res;
	}


	template<bool M>
	auto operator==(List_iterator<T, M, IS_FORWARD> const& itr) const noexcept
	->	bool{  return _node_ptr == _List_itr_Helper::node_ptr(itr);  }

	template<class Q>
	auto operator!=(Q&& q) const noexcept-> bool{  return !( *this == Forward<Q>(q) );  }


private:
	_node_t* _node_ptr;


	template<bool IS_PLUS>
	static auto _update_node(_node_t*& node_ptr)
	->	Enable_if_t<IS_PLUS == IS_FORWARD>{  node_ptr = node_ptr->back_ptr;  }

	template<bool IS_PLUS>
	static auto _update_node(_node_t*& node_ptr)
	->	Enable_if_t<IS_PLUS != IS_FORWARD>{  node_ptr = node_ptr->front_ptr;  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace std
{

	template<class T, bool M, bool F>
	struct iterator_traits< sgm::List_iterator<T, M, F> >
	{
		using iterator_category = bidirectional_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = sgm::Selective_t<M, T, T const>*;
		using reference = sgm::Pointerless_t<pointer>&;
	};

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, class ALLOC>
class sgm::List
{
private: 
	using _node_t = List_Node<T>;

public:
	using value_type = Decay_t<T>;
	using allocator_t = ALLOC;

	using iterator = List_iterator<T, true, true>;
	using const_iterator = List_iterator<T, false, true>;
	using reverse_iterator = List_iterator<T, true, false>;
	using const_reverse_iterator = List_iterator<T, false, false>;


	List() : _pnode_begin(nullptr), _pnode_rbegin(nullptr), _allocator(){}

	template<  class ITR, class = Enable_if_t< is_iterator<ITR>::value >  >
	List(ITR begin_itr, ITR const end_itr) 
	:	List(){  _try_move_from_iterators<false>(begin_itr, end_itr);  }

	List(List const& Li) : List(Li.cbegin(), Li.cend()){}

	List(List&& Li) noexcept : List()
	{
		_pnode_begin = Li._pnode_begin;
		_pnode_rbegin = Li._pnode_rbegin;

		Li._pnode_begin = Li._pnode_rbegin = nullptr;
	}


	List(std::initializer_list<T>&& Li) noexcept 
	:	List(){  _try_move_from_iterators<true>(Li.begin(), Li.end());  }

	template<  class RG, class = Enable_if_t< is_iterable<RG>::value >  >
	List(RG&& rg) noexcept(is_Rvalue_Reference<RG&&>::value) : List()
	{
		_try_move_from_iterators< is_Rvalue_Reference<RG&&>::value >( Begin(rg), End(rg) );
	}


	~List(){  clear();  }


	auto operator=(List const& Li)-> List&{  return _clone<false>(Li.cbegin(), Li.cend());  }
	
	auto operator=(List&& Li) noexcept-> List&
	{
		clear();

		_pnode_begin = Li._pnode_begin;
		_pnode_rbegin = Li._pnode_rbegin;

		Li._pnode_begin = Li._pnode_rbegin = nullptr;

		return *this;
	}
	
	template<  class Q, class = Enable_if_t< is_Convertible<Q, T>::value >  >
	auto operator=(std::initializer_list<Q>&& initLi) noexcept
	->	List&{  return _clone<true>(initLi.begin(), initLi.end());  }


	template
	<	class RG
	,	class
		=	Enable_if_t<  is_iterable<RG>::value && !is_Same< Decay_t<RG>, List >::value  >
	>
	auto operator=(RG&& rg) noexcept(is_Rvalue_Reference<RG&&>::value)
	->	List&{  return _clone< is_Rvalue_Reference<RG&&>::value >( Begin(rg), End(rg) );  }


	template
	<	class RG
	,	class = Enable_if_t<  is_iterable<RG>::value && !is_Same< Decay_t<RG>, List >::value  >
	> 
	operator RG() const{  return Decay_t<RG>(cbegin(), cend());  }


	auto cbegin() const-> const_iterator{  return _pnode_begin;  }
	auto begin() const-> SGM_DECLTYPE_AUTO(  cbegin()  )
	auto begin()-> iterator{  return _pnode_begin;  };

	auto cend() const-> const_iterator
	{
		_node_t* p = _pnode_rbegin == nullptr ? nullptr : _pnode_rbegin->back_ptr;  

		return p;
	}
	
	auto end() const-> SGM_DECLTYPE_AUTO(  cend()  )
	
	auto end()-> iterator
	{
		_node_t* p = _pnode_rbegin == nullptr ? nullptr : _pnode_rbegin->back_ptr;  

		return p;  
	}


	auto crbegin() const-> const_reverse_iterator{  return _pnode_rbegin;  }
	auto rbegin() const-> SGM_DECLTYPE_AUTO(  crbegin()  )
	auto rbegin()-> reverse_iterator{  return _pnode_rbegin;  }


	auto crend() const-> const_reverse_iterator
	{
		_node_t* p = _pnode_begin == nullptr ? nullptr : _pnode_begin->front_ptr;

		return p;
	}

	auto rend() const-> SGM_DECLTYPE_AUTO(  crend()  )
	
	auto rend()-> reverse_iterator
	{	
		_node_t* p = _pnode_begin == nullptr ? nullptr : _pnode_begin->front_ptr; 

		return p;
	}


	auto front() const-> T const&{  return *cbegin();  }
	auto front()-> T&{  return *begin();  }
	auto back() const-> T const&{  return *crbegin();  }
	auto back()-> T&{  return *rbegin();  }


	auto size() const-> size_t
	{	
		size_t res = 0;

		for(auto itr = cbegin();  itr != cend();  ++res,  ++itr);

		return res;  
	}
	

	auto is_empty() const
	->	bool{  return _pnode_begin == nullptr || _pnode_rbegin == nullptr;  }


	auto clear()-> List&
	{
		while(!is_empty())
			pop_back();

		return *this;
	}


	template<class...ARGS>
	auto emplace_back(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)-> List&
	{
		_node_t& new_node = *_alloc( nullptr, nullptr, Forward<ARGS>(args)... );

		if(is_empty())
			_pnode_begin = _pnode_rbegin = &new_node;
		else
		{
			_node_t &cur_node = *_pnode_rbegin;

			new_node.front_ptr = &cur_node;
			_pnode_rbegin = cur_node.back_ptr = &new_node;
		}

		return *this;
	}

	template<class...ARGS>
	auto emplace_front(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)-> List&
	{
		_node_t& new_node = *_alloc( nullptr, nullptr, Forward<ARGS>(args)... );

		if(is_empty())
			_pnode_rbegin = _pnode_begin = &new_node;
		else
		{
			_node_t& cur_node = *_pnode_begin;

			new_node.back_ptr = &cur_node;
			_pnode_begin = cur_node.front_ptr = &new_node;
		}

		return *this;
	}

	template<class ITR, class...ARGS>
	auto emplace(ITR const itr, ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)-> ITR
	{
		static_assert
		(	(	is_Convertible<ITR, const_iterator>::value 
			||	is_Convertible<ITR, const_reverse_iterator>::value
			)
		,	""
		);

		

		ITR const behind_itr = Next(itr);
		ITR const new_itr(  _alloc( nullptr, nullptr, Forward<ARGS>(args)... )  );

		_link< Decay_t<ITR>::is_forward_v >(itr, new_itr, behind_itr);

		return new_itr;
	}


	auto pop_back()-> List&
	{
		if(!is_empty())
		{
			auto p = _pnode_rbegin->front_ptr;

			_destroy(_pnode_rbegin);

			_pnode_rbegin = p;

			if(_pnode_rbegin == nullptr)
				_pnode_begin = nullptr;
			else
				_pnode_rbegin->back_ptr = nullptr;
		}
	
		return *this;
	}
	
	auto pop_front()-> List&
	{
		if(!is_empty())
		{
			auto p = _pnode_begin->back_ptr;

			_destroy(_pnode_begin);

			_pnode_begin = p;

			if(_pnode_begin == nullptr)
				_pnode_rbegin = nullptr;
			else
				_pnode_begin->front_ptr = nullptr;
		}
		
		return *this;
	}


	template<class ITR>
	auto pop(ITR itr)-> ITR
	{
		static_assert
		(	(	is_Convertible<ITR, const_iterator>::value 
			||	is_Convertible<ITR, const_reverse_iterator>::value
			)
		,	""
		);

		List_Node<T> 
			&cur_node = *itr->_node_ptr,
			&front_node = *cur_node.front_ptr,
			&back_node = *cur_node.back_ptr;


		front_node.back_ptr = &back_node;
		back_node.front_ptr = &front_node;

		cur_node.front_ptr = cur_node.back_ptr = nullptr;

		_destroy(itr->_node_ptr);

		return &back_node;
	}

	auto pop(const_iterator bi, const_iterator ei)-> iterator;
	

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
	_node_t *_pnode_begin, *_pnode_rbegin;
	allocator_t _allocator;


	template<class...ARGS>
	auto _alloc(ARGS&&...args)-> List_Node<T>*
	{
		List_Node<T>* pres = _allocator.allocate( sizeof(List_Node<T>) );

		_allocator.construct( pres, List_Node<T>{Forward<ARGS>(args)...} );

		return pres;
	}

	void _destroy(List_Node<T>*& p)
	{
		_allocator.destroy(p);

		_allocator.deallocate( p, sizeof(List_Node<T>) );

		p = nullptr;
	}


	template<bool WANT_MOVE, class ITR>
	void _try_move_from_iterators(ITR bi, ITR const ei) noexcept(WANT_MOVE)
	{
		for(  ;  bi != ei;  emplace_back( Move_if<WANT_MOVE>(*bi++) )  );
	}


	template
	<	bool TEMP_HOST = false
	,	class ITR, class = Enable_if_t< is_iterator<ITR>::value >
	,	bool IS_ASSIGNABLE_ELEM
		=	(	Has_Operator_Copy_Assignment<value_type>::value
			||	(TEMP_HOST && Has_Operator_Move_Assignment<value_type>::value)
			)
	>
	auto _clone(ITR bi, ITR const ei)-> Enable_if_t<IS_ASSIGNABLE_ELEM, List&>
	{
		auto itr = begin();

		while(itr != end() && bi != ei)
			*itr++ = Move_if<TEMP_HOST>(*bi++);

		if(itr != end())
			pop(itr, end());
		else
			while(bi != ei)
				emplace_back( Move_if<TEMP_HOST>(*bi++) );

		return *this;
	}

	template
	<	bool TEMP_HOST = false
	,	class ITR, class = Enable_if_t< is_iterator<ITR>::value >
	,	bool IS_ASSIGNABLE_ELEM
		=	(	Has_Operator_Copy_Assignment<value_type>::value
			||	(TEMP_HOST && Has_Operator_Move_Assignment<value_type>::value)
			)
	>
	auto _clone(ITR bi, ITR const ei)-> Enable_if_t<!IS_ASSIGNABLE_ELEM, List&>
	{
		clear();

		while(bi != ei)
			emplace_back( Move_if<TEMP_HOST>(*bi++) );

		return *this;
	}


	template<bool IS_FORWARD, class ITR>
	static auto _link
	(	const_iterator const prev_itr
	,	const_iterator const cur_itr
	,	const_iterator const next_itr
	)->	Enable_if_t<IS_FORWARD, void>
	{
		List_Node<T>
			&node0 = *prev_itr->_node_ptr, 
			&node1 = *cur_itr->_node_ptr, 
			&node2 = *next_itr->_node_ptr;

		node0.back_ptr = &node1,  node1.back_ptr = &node2;
		node2.front_ptr = &node1,  node1.front_ptr = &node0;
	}

	template<bool IS_FORWARD, class ITR>
	static auto _link
	(	const_reverse_iterator const prev_itr
	,	const_reverse_iterator const cur_itr
	,	const_reverse_iterator const next_itr
	)->	Enable_if_t<!IS_FORWARD, void>
	{
		List_Node<T>
			&node2 = *prev_itr->_node_ptr, 
			&node1 = *cur_itr->_node_ptr, 
			&node0 = *next_itr->_node_ptr;

		node0.back_ptr = &node1,  node1.back_ptr = &node2;
		node2.front_ptr = &node1,  node1.front_ptr = &node0;
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
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#

#endif // end of #ifndef _SGM_LIST_

