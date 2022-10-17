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
#include <cassert>


namespace sgm
{

	template<class T>
	struct List_Node;

	template<class T>
	struct _List_Empty_Node;


	template<class T>
	using _Default_List_Allocator_t = Allocator< List_Node<T> >;

	template< class T, class ALLOC = _Default_List_Allocator_t<T> >
	class List;


	template<class T, bool IS_MUTABLE, bool IS_FORWARD>
	class List_iterator;

	struct _List_itr_Helper;
	struct _List_node_Helper;

	enum class _List_by_Tag{};

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
struct sgm::List_Node
{
	using value_type = T;

	List_Node *front_ptr = nullptr, *back_ptr = nullptr;
	T value;
};


template<class T>
struct sgm::_List_Empty_Node
{
	using value_type = T;

	List_Node<T> *front_ptr = nullptr, *back_ptr = nullptr;
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


	template<class T, bool IS_MUTABLE>
	static auto is_end_itr(List_iterator<T, IS_MUTABLE, true> const& itr) noexcept
	->	bool{  return node_ptr(itr)->back_ptr == nullptr;  }

	template<class T, bool IS_MUTABLE>
	static auto is_end_itr(List_iterator<T, IS_MUTABLE, false> const& itr) noexcept
	->	bool{  return node_ptr(itr)->front_ptr == nullptr;  }
};


struct sgm::_List_node_Helper : Unconstructible
{
	template<bool IS_PLUS, bool IS_FORWARD, class T>
	static auto shift(List_Node<T>* nptr) noexcept
	->	Enable_if_t< IS_PLUS == IS_FORWARD, List_Node<T>*& >{  return nptr->back_ptr;  }
	
	template<bool IS_PLUS, bool IS_FORWARD, class T>
	static auto shift(List_Node<T>* nptr) noexcept
	->	Enable_if_t< IS_PLUS != IS_FORWARD, List_Node<T>*& >{  return nptr->front_ptr;  }
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


	List_iterator(_node_t* const node_ptr = nullptr) : _node_ptr(node_ptr){}

	List_iterator(_itr_t const&) = default;

	List_iterator(List_iterator<T, !IS_MUTABLE, IS_FORWARD> const& itr) 
	:	_node_ptr( _List_itr_Helper::node_ptr(itr) )
	{
		static_assert(!IS_MUTABLE, "cannot assign immutable iterator to mutable one .");
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
		_node_ptr = _List_node_Helper::shift<true, IS_FORWARD>(_node_ptr);

		return *this;
	}


	auto operator--()-> _itr_t&
	{
		_node_ptr = _List_node_Helper::shift<false, IS_FORWARD>(_node_ptr);

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
	auto operator!=(Q const& q) const noexcept-> bool{  return !(*this == q);  }


private:
	_node_t* _node_ptr;
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
	using _node_hp = _List_node_Helper;
	using _itr_hp = _List_itr_Helper;


public:
	using value_type = Decay_t<T>;
	using allocator_t = ALLOC;

	using iterator = List_iterator<T, true, true>;
	using const_iterator = List_iterator<T, false, true>;
	using reverse_iterator = List_iterator<T, true, false>;
	using const_reverse_iterator = List_iterator<T, false, false>;


	List() : _rend_node(), _end_node(), _allocator()
	{
		_rend_node.back_ptr = _end_nptr();
		_end_node.front_ptr = _rend_nptr();
	}

	template<  class ITR, class = Enable_if_t< is_iterator<ITR>::value >  >
	List(ITR begin_itr, ITR const end_itr) 
	:	List(){  _construct_from_iterators(begin_itr, end_itr);  }

	List(List const& Li) : List(Li.cbegin(), Li.cend()){}

	List(List&& Li) noexcept : List()
	{
		_rend_node.back_ptr = Li._rend_node.back_ptr;
		_end_node.front_ptr = Li._end_node.front_ptr;

		Li._rend_node.back_ptr = Li._end_nptr();
		Li._end_node.front_ptr = Li._rend_nptr();
	}


	List(std::initializer_list<T>&& Li) noexcept 
	:	List(){  _construct_from_iterators(Li.begin(), Li.end());  }

	template<  class RG, class = Enable_if_t< is_iterable<RG>::value >  >
	List(RG&& rg) noexcept(is_Rvalue_Reference<RG&&>::value) 
	:	List(){  _construct_from_iterators( fBegin<RG>(rg), fEnd<RG>(rg) );  }


	~List(){  clear();  }


	auto operator=(List const& Li)-> List&{  return _clone(Li.cbegin(), Li.cend());  }
	
	auto operator=(List&& Li) noexcept-> List&
	{
		clear();

		_rend_node.back_ptr = Li._rend_node.back_ptr;
		_end_node.front_ptr = Li._end_node.front_ptr;

		Li._rend_node.back_ptr = Li._end_nptr();
		Li._end_nptr().front_ptr = Li._rend_nptr();

		return *this;
	}
	
	template<  class Q, class = Enable_if_t< is_Convertible<Q, T>::value >  >
	auto operator=(std::initializer_list<Q>&& initLi) noexcept
	->	List&{  return _clone(initLi.begin(), initLi.end());  }

	template
	<	class RG
	,	class
		=	Enable_if_t<  is_iterable<RG>::value && !is_Same< Decay_t<RG>, List >::value  >
	>
	auto operator=(RG&& rg) noexcept(is_Rvalue_Reference<RG&&>::value)
	->	List&{  return _clone( fBegin<RG>(rg), fEnd<RG>(rg) );  }


	template
	<	class RG
	,	class = Enable_if_t<  is_iterable<RG>::value && !is_Same< Decay_t<RG>, List >::value  >
	> 
	operator RG() const{  return Decay_t<RG>(cbegin(), cend());  }


	void swap(List& Li) noexcept
	{
		{
			_node_t
				*const my_first = _rend_node.back_ptr,
				*const my_last = _end_node.front_ptr,
				*const its_first = Li._rend_node.back_ptr,
				*const its_last = Li._end_node.front_ptr;

			_link_nodes(_rend_nptr(), its_first);
			_link_nodes(Li._rend_nptr(), my_first);
	
			_link_nodes(its_last, _end_nptr());
			_link_nodes(my_last, Li._end_nptr());
		}
		{
			auto temp = Move(_allocator);

			_allocator = Move(Li._allocator);
			Li._allocator = Move(temp);
		}
	}


	auto cbegin() const-> const_iterator{  return _rend_node.back_ptr;  }
	auto begin() const-> SGM_DECLTYPE_AUTO(  cbegin()  )
	auto begin()-> iterator{  return _rend_node.back_ptr;  };

	auto cend() const-> const_iterator{  return _end_nptr();  }
	auto end() const-> SGM_DECLTYPE_AUTO(  cend()  )
	auto end()-> iterator{  return _end_nptr();  }

	auto crbegin() const-> const_reverse_iterator{  return _end_node.front_ptr;  }
	auto rbegin() const-> SGM_DECLTYPE_AUTO(  crbegin()  )
	auto rbegin()-> reverse_iterator{  return _end_node.front_ptr;  }

	auto crend() const-> const_reverse_iterator{  return _rend_nptr();  }
	auto rend() const-> SGM_DECLTYPE_AUTO(  crend()  )
	auto rend()-> reverse_iterator{  return _rend_nptr();  }


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
	

	auto is_empty() const-> bool{  return cbegin() == cend();  }


	auto clear()-> List&
	{
		while(!is_empty())
			pop_back();

		return *this;
	}


	template<class...ARGS>
	auto emplace_back(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	->	List&{  return emplace_next( Prev(end()), Forward<ARGS>(args)... ),  *this;  }

	template<class...ARGS>
	auto emplace_front(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	->	List&{  return emplace_next( Prev(rend()), Forward<ARGS>(args)... ),  *this;  }


	template<class ITR, class...ARGS>
	auto emplace_next(ITR const itr, ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)-> ITR
	{
		static_assert
		(	(	is_Convertible<ITR, const_iterator>::value 
			||	is_Convertible<ITR, const_reverse_iterator>::value
			)
		,	""
		);

		assert( !_itr_hp::is_end_itr(itr) );

		ITR const 
			behind_itr = Next(itr),
			new_itr(  _alloc( nullptr, nullptr, Forward<ARGS>(args)... )  );

		_link(itr, new_itr, behind_itr);

		return new_itr;
	}

	template<class ITR, class...ARGS>
	auto emplace_prev(ITR const itr, ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)-> ITR
	{
		static_assert
		(	(	is_Convertible<ITR, const_iterator>::value 
			||	is_Convertible<ITR, const_reverse_iterator>::value
			)
		,	""
		);

		ITR const 
			prior_itr = Prev(itr),
			new_itr(  _alloc( nullptr, nullptr, Forward<ARGS>(args)... )  );

		_link(prior_itr, new_itr, itr);

		return new_itr;
	}


	auto pop_back()-> List&{  return pop(rbegin()),  *this;  }
	auto pop_front()-> List&{  return pop(begin()),  *this;  }


	template<class ITR>
	auto pop(ITR const itr)-> ITR
	{
		assert( !_itr_hp::is_end_itr(itr) );

		return pop( itr, Next(itr) );  
	}


	template<class ITR>
	auto pop(ITR bi, ITR const ei)-> ITR
	{
		static_assert
		(	(	is_Convertible<ITR, const_iterator>::value 
			||	is_Convertible<ITR, const_reverse_iterator>::value
			)
		,	""
		);

		_unlink_range(bi, ei);

		while(bi != ei)
		{
			auto itr = bi++;

			_node_t* cur_node_ptr = _itr_hp::node_ptr(itr);

			cur_node_ptr->front_ptr = cur_node_ptr->back_ptr = nullptr;
			
			_destroy(cur_node_ptr);
		}

		return ei;
	}
	

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
	_List_Empty_Node<T> _rend_node, _end_node;
	allocator_t _allocator;


	auto _rend_nptr() const noexcept
	->	_node_t*{  return const_cast<_node_t*>( reinterpret_cast<_node_t const*>(&_rend_node) );  }

	auto _end_nptr() const noexcept
	->	_node_t*{  return const_cast<_node_t*>( reinterpret_cast<_node_t const*>(&_end_node) );  }


	template<class...ARGS>
	auto _alloc(ARGS&&...args)-> _node_t*
	{
		_node_t* const pres = _allocator.allocate( sizeof(_node_t) );

		_allocator.construct( pres, _node_t{Forward<ARGS>(args)...} );

		return pres;
	}

	void _destroy(_node_t* p)
	{
		_allocator.destroy(p);

		_allocator.deallocate( p, sizeof(_node_t) );
	}


	template<class ITR>
	void _construct_from_iterators(ITR bi, ITR const ei) noexcept(is_Move_iterator<ITR>::value)
	{
		while(bi != ei)
			emplace_back(*bi++);
	}


	template
	<	class ITR, class = Enable_if_t< is_iterator<ITR>::value >
	,	bool IS_ASSIGNABLE_ELEM
		=	(	Has_Operator_Copy_Assignment<value_type>::value
			||	(	is_Move_iterator<ITR>::value 
				&&	Has_Operator_Move_Assignment<value_type>::value
				)
			)
	>
	auto _clone(ITR bi, ITR const ei)-> Enable_if_t<IS_ASSIGNABLE_ELEM, List&>
	{
		auto itr = begin();

		while(itr != end() && bi != ei)
			*itr++ = *bi++;

		if(itr != end())
			pop(itr, end());
		else
			while(bi != ei)
				emplace_back(*bi++);

		return *this;
	}

	template
	<	class ITR, class = Enable_if_t< is_iterator<ITR>::value >
	,	bool IS_ASSIGNABLE_ELEM
		=	(	Has_Operator_Copy_Assignment<value_type>::value
			||	(	is_Move_iterator<ITR>::value 
				&&	Has_Operator_Move_Assignment<value_type>::value
				)
			)
	>
	auto _clone(ITR bi, ITR const ei)-> Enable_if_t<!IS_ASSIGNABLE_ELEM, List&>
	{
		clear();

		while(bi != ei)
			emplace_back(*bi++);

		return *this;
	}


	template<class ITR>
	static void _link(ITR const prev_itr, ITR const cur_itr, ITR const next_itr)
	{
		_node_t
			&cur_node = *_itr_hp::node_ptr(cur_itr),
			*const prev_node_ptr = _itr_hp::node_ptr(prev_itr),
			*const next_node_ptr = _itr_hp::node_ptr(next_itr);		

		bool constexpr is_fwd_v = Decay_t<ITR>::is_forward_v;

		_node_hp::shift<true, is_fwd_v>(&cur_node) = next_node_ptr;
		_node_hp::shift<false, is_fwd_v>(&cur_node) = prev_node_ptr;

		_node_hp::shift<true, is_fwd_v>(prev_node_ptr) = &cur_node;
		_node_hp::shift<false, is_fwd_v>(next_node_ptr) = &cur_node;
	}


	template<class ITR>
	static void _unlink_range(ITR const bi, ITR const ei)
	{
		if(bi == ei)
			return;

		bool constexpr is_fwd_v = Decay_t<ITR>::is_forward_v;

		_node_t
			&bnode = *_itr_hp::node_ptr(bi),
			*const bfr_bnode_ptr = _node_hp::shift<false, is_fwd_v>(&bnode),
			*const enode_ptr = _itr_hp::node_ptr(ei);

		_node_hp::shift<true, is_fwd_v>(bfr_bnode_ptr) = enode_ptr;
		_node_hp::shift<false, is_fwd_v>(enode_ptr) = bfr_bnode_ptr;		
	}


	static void _link_nodes(_node_t* nptr0, _node_t* nptr1)
	{
		assert(nptr0 != nullptr && nptr1 != nullptr);

		nptr0->back_ptr = nptr1,  nptr1->front_ptr = nptr0;
	}


	friend class sgm::List<value_type>;


	template<class A>
	List(_List_by_Tag, A&& alc) : _rend_node(), _end_node(), _allocator( Forward<A>(alc) )
	{
		_rend_node.back_ptr = _end_nptr();
		_end_node.front_ptr = _rend_nptr();		
	}

	template<  class A, class ITR, class = Enable_if_t< is_iterator<ITR>::value >  >
	List(_List_by_Tag tag, A&& alc, ITR bi, ITR const ei)
	:	List( tag, Forward<A>(alc) ){  _construct_from_iterators(bi, ei);  }

	template<class A>
	List(_List_by_Tag tag, A&& alc, List const& Li)
	:	List( tag, Forward<A>(alc), Li.cbegin(), Li.cend() ){}

	template<class A>
	List(_List_by_Tag tag, A&& alc, List&& Li) noexcept : List( tag, Forward<A>(alc) )
	{
		_rend_node.back_ptr = Li._rend_node.back_ptr;
		_end_node.front_ptr = Li._end_node.front_ptr;

		Li._rend_node.back_ptr = Li._end_nptr();
		Li._end_node.front_ptr = Li._rend_nptr();
	}

	template<class A>
	List(_List_by_Tag tag, A&& alc, std::initializer_list<T>&& iL) noexcept
	:	List( tag, Forward<A>(alc) ){  _construct_from_iterators(iL.begin(), iL.end());  }

	template<  class RG, class A, class = Enable_if_t< is_iterable<RG>::value >  >
	List(_List_by_Tag tag, A&& alc, RG&& rg) noexcept(is_Rvalue_Reference<RG&&>::value)
	:	List( tag, Forward<A>(alc) ){  _construct_from_iterators( fBegin<RG>(rg), fEnd<RG>(rg) );  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace std
{

	template<class T>
	static void swap(sgm::List<T>& L0, sgm::List<T>& L1) noexcept{  L0.swap(L1);  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_LIST_

