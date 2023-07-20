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

	template<class T, bool IS_FORWARD>
	struct _List_Boundary_Node;

	template<class T>
	using _List_End_Node = _List_Boundary_Node<T, true>;

	template<class T>
	using _List_rEnd_Node = _List_Boundary_Node<T, false>;

	
	template<class T>
	using _Naive_List_Allocator_t = Allocator< List_Node<T> >;


	static size_t constexpr default_list_node_chunk_size_v = 0x10;


	template<class T, size_t N>
	class _Chunk_Memory;

	template<class T, size_t N = default_list_node_chunk_size_v>
	class _Allocator_by_Chunk;


	template<class T>
	using _Default_List_Allocator_t = _Allocator_by_Chunk< List_Node<T> >;

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

	List_Node() noexcept( noexcept(T()) ) : front_ptr(nullptr), back_ptr(nullptr), value(){}
	
	template<class...ARGS>
	List_Node(List_Node* fp, List_Node* bp, ARGS&&...args) 
	noexcept(  Aleph_Check<ARGS&&...>::value || noexcept( T(Mock<ARGS&&>()...) )  )
	:	front_ptr(fp), back_ptr(bp), value( Forward<ARGS>(args)... ){}


	List_Node *front_ptr, *back_ptr;
	T value;
};


template<class T, bool IS_FORWARD>
struct sgm::_List_Boundary_Node
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


	List_iterator(_node_t* const node_ptr = nullptr) noexcept : _node_ptr(node_ptr){}

	List_iterator(_itr_t const&) noexcept = default;

	List_iterator(List_iterator<T, !IS_MUTABLE, IS_FORWARD> const& itr) noexcept
	:	_node_ptr( _List_itr_Helper::node_ptr(itr) )
	{
		static_assert(!IS_MUTABLE, "cannot assign immutable iterator to mutable one .");
	}


	auto operator=(_itr_t const&) noexcept-> _itr_t& = default;

	auto operator=(List_iterator<T, !IS_MUTABLE, IS_FORWARD> const& itr) noexcept-> _itr_t&
	{
		static_assert(!IS_MUTABLE, "cannot assign immutable iterator to mutable one .");
		
		_node_ptr = _List_itr_Helper::node_ptr(itr);

		return *this;
	}


	auto operator*() const noexcept-> _elem_t&{  return _node_ptr->value;  }

	auto operator->() const noexcept-> _elem_t*{  return &**this;  }


	auto operator++() noexcept-> _itr_t&
	{
		_node_ptr = _List_node_Helper::shift<true, IS_FORWARD>(_node_ptr);

		return *this;
	}


	auto operator--() noexcept-> _itr_t&
	{
		_node_ptr = _List_node_Helper::shift<false, IS_FORWARD>(_node_ptr);

		return *this;
	}


	auto operator++(int) noexcept-> _itr_t
	{
		auto res = *this;

		++*this;

		return res;
	}

	auto operator--(int) noexcept-> _itr_t
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


	List() noexcept : _bnd_node{}, _allocator(){  _link_nodes(_rend_nptr(), _end_nptr());  }

	template<  class ITR, class = Enable_if_t< is_iterator<ITR>::value >  >
	List(ITR begin_itr, ITR const end_itr) 
	:	List(){  _construct_from_iterators(begin_itr, end_itr);  }

	List(List const& Li) : List(Li.cbegin(), Li.cend()){}

	List(List&& Li) noexcept : List()
	{
		_link_nodes(_rend_nptr(), Li._bnd_node.rend.back_ptr);
		_link_nodes(Li._bnd_node.end.front_ptr, _end_nptr());

		_allocator = Move(Li._allocator);

		_link_nodes(Li._rend_nptr(), Li._end_nptr());
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

		_link_nodes(_rend_nptr(), Li._bnd_node.rend.back_ptr);
		_link_nodes(Li._bnd_node.end.front_ptr, _end_nptr());

		_allocator = Move(Li._allocator);

		_link_nodes(Li._rend_nptr(), Li._end_nptr());

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
				*const my_first = _bnd_node.rend.back_ptr,
				*const my_last = _bnd_node.end.front_ptr,
				*const its_first = Li._bnd_node.rend.back_ptr,
				*const its_last = Li._bnd_node.end.front_ptr;

			_link_nodes(_rend_nptr(), its_first);
			_link_nodes(Li._rend_nptr(), my_first);
	
			_link_nodes(its_last, _end_nptr());
			_link_nodes(my_last, Li._end_nptr());
		}

		Swap(_allocator, Li._allocator);
	}


	auto cbegin() const noexcept-> const_iterator{  return _bnd_node.rend.back_ptr;  }
	auto begin() const noexcept-> SGM_DECLTYPE_AUTO(  cbegin()  )
	auto begin() noexcept-> iterator{  return _bnd_node.rend.back_ptr;  };

	auto cend() const noexcept-> const_iterator{  return _end_nptr();  }
	auto end() const noexcept-> SGM_DECLTYPE_AUTO(  cend()  )
	auto end() noexcept-> iterator{  return _end_nptr();  }

	auto crbegin() const noexcept-> const_reverse_iterator{  return _bnd_node.end.front_ptr;  }
	auto rbegin() const noexcept-> SGM_DECLTYPE_AUTO(  crbegin()  )
	auto rbegin() noexcept-> reverse_iterator{  return _bnd_node.end.front_ptr;  }

	auto crend() const noexcept-> const_reverse_iterator{  return _rend_nptr();  }
	auto rend() const noexcept-> SGM_DECLTYPE_AUTO(  crend()  )
	auto rend() noexcept-> reverse_iterator{  return _rend_nptr();  }


	auto front() const noexcept-> T const&{  return *cbegin();  }
	auto front() noexcept-> T&{  return *begin();  }
	auto back() const noexcept-> T const&{  return *crbegin();  }
	auto back() noexcept-> T&{  return *rbegin();  }


	auto size() const noexcept-> size_t
	{	
		size_t res = 0;

		for(auto itr = cbegin();  itr != cend();  ++res,  ++itr);

		return res;  
	}
	

	auto is_empty() const noexcept-> bool{  return cbegin() == cend();  }


	auto clear() noexcept-> List&
	{
		pop(begin(), end());

		return *this;
	}


	template<class...ARGS>
	auto emplace_back(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	->	List&{  return emplace_prev( end(), Forward<ARGS>(args)... ),  *this;  }

	template<class...ARGS>
	auto emplace_front(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	->	List&{  return emplace_prev( rend(), Forward<ARGS>(args)... ),  *this;  }


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

		_link_itrs(itr, new_itr);
		_link_itrs(new_itr, behind_itr);

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

		_link_itrs(prior_itr, new_itr);
		_link_itrs(new_itr, itr);

		return new_itr;
	}


	auto pop_back() noexcept-> List&{  return pop(rbegin()),  *this;  }
	auto pop_front() noexcept-> List&{  return pop(begin()),  *this;  }


	template<class ITR>
	auto pop(ITR const itr) noexcept-> ITR
	{
		assert( !_itr_hp::is_end_itr(itr) );

		return pop( itr, Next(itr) );  
	}


	template<class ITR>
	auto pop(ITR bi, ITR const ei) noexcept-> ITR
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

			_destroy( _itr_hp::node_ptr(itr) );
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
	union
	{	
		_List_rEnd_Node<T> rend;
		_List_End_Node<T> end;
	}	_bnd_node;

	allocator_t _allocator;


	auto _rend_nptr() const noexcept-> _node_t*
	{
		return const_cast<_node_t*>( reinterpret_cast<_node_t const*>(&_bnd_node) );  
	}

	auto _end_nptr() const noexcept-> _node_t*
	{
		return const_cast<_node_t*>( reinterpret_cast<_node_t const*>(&_bnd_node) );  
	}


	template<class...ARGS>
	auto _alloc(ARGS&&...args)-> _node_t*
	{
		_node_t* const pres = _allocator.allocate( sizeof(_node_t) );

		_allocator.construct( pres, Forward<ARGS>(args)... );

		return pres;
	}

	void _destroy(_node_t* p) noexcept
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
	static void _link_itrs(ITR const itr0, ITR const itr1) noexcept
	{
		_node_t 
			*const _nptr0 = _itr_hp::node_ptr(itr0),  
			*const _nptr1 = _itr_hp::node_ptr(itr1);

		bool constexpr is_fwd_v = Decay_t<ITR>::is_forward_v;	
		
		_node_hp::shift<true, is_fwd_v>(_nptr0) = _nptr1;
		_node_hp::shift<false, is_fwd_v>(_nptr1) = _nptr0;
	}


	template<class ITR>
	static void _unlink_range(ITR const bi, ITR const ei) noexcept
	{
		if(bi != ei)
			_link_itrs( Prev(bi), ei );
	}


	static void _link_nodes(_node_t* nptr0, _node_t* nptr1) noexcept
	{
		assert(nptr0 != nullptr && nptr1 != nullptr);

		nptr0->back_ptr = nptr1,  nptr1->front_ptr = nptr0;
	}


	friend class sgm::List<value_type>;


	template<class A>
	List(_List_by_Tag, A&& alc) : _bnd_node{}, _allocator( Forward<A>(alc) )
	{
		_link_nodes(_rend_nptr(), _end_nptr());
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
		_link_nodes(_rend_nptr(), Li._bnd_node.rend.back_ptr);
		_link_nodes(Li._bnd_node.end.front_ptr, _end_nptr());

		_allocator = Move(Li._allocator);

		_link_nodes(Li._rend_nptr(), Li._end_nptr());
	}

	template<class A>
	List(_List_by_Tag tag, A&& alc, std::initializer_list<T>&& iL) noexcept
	:	List( tag, Forward<A>(alc) ){  _construct_from_iterators(iL.begin(), iL.end());  }

	template<  class RG, class A, class = Enable_if_t< is_iterable<RG>::value >  >
	List(_List_by_Tag tag, A&& alc, RG&& rg) noexcept(is_Rvalue_Reference<RG&&>::value)
	:	List( tag, Forward<A>(alc) ){  _construct_from_iterators( fBegin<RG>(rg), fEnd<RG>(rg) );  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, std::size_t N>
class sgm::_Chunk_Memory
{
private:
	struct _indexed_Memory
	{
		ptrdiff_t from_header = 0;

		union _Mem 
		{
			ptrdiff_t undefined;
			T value;

			_Mem() noexcept{}
			~_Mem() noexcept{}
		}	mem;
	};


	struct _Header{  size_t cur_idx = 0, nof_elem = 0;  };


public:
	auto has_gone() const noexcept-> bool{  return _header.cur_idx == N;  }


	template<class...ARGS>
	auto push(ARGS&&...args)-> void
	{
		assert(!has_gone());

		_indexed_Memory* cur_ptr = &_mem_arr[_header.cur_idx];

		new(cur_ptr) _indexed_Memory{};

		cur_ptr->from_header = _diff_from_header(cur_ptr);
		new(&cur_ptr->mem) T{Forward<ARGS>(args)...};

		++_header.cur_idx,  ++_header.nof_elem;
	}


	static auto Pop(T* p) noexcept-> _Chunk_Memory*
	{
		assert(p);

		auto& from_header = *( reinterpret_cast<ptrdiff_t*>(p) - 1 );

		if(from_header == 0)
			return nullptr;

		auto& chunk = *reinterpret_cast<_Chunk_Memory*>(&from_header - from_header);
		
		Allocator<T>::Destroy(*p);

		from_header = 0;

		if(--chunk._header.nof_elem == 0)
			return chunk._header.cur_idx = N,  &chunk;
		else
			return nullptr;
	}


	auto get() noexcept-> T*{  return &_mem_arr[_header.cur_idx].mem.value;  }


private:
	_Header _header;
	_indexed_Memory _mem_arr[N];


	auto _diff_from_header(_indexed_Memory const* p) const noexcept-> ptrdiff_t
	{
		return 
		(	reinterpret_cast<ptrdiff_t const*>(p)
		-	reinterpret_cast<ptrdiff_t const*>(&_header) 
		);
	}
};


template<class T, std::size_t N>
class sgm::_Allocator_by_Chunk : public Allocator_interface< _Allocator_by_Chunk<T, N> >
{
private:
	using _chunk_t = _Chunk_Memory<T, N>;
	using _chunk_alloc_t = Allocator<_chunk_t>;

public:
	using value_type = T;
	using pointer = value_type*;
	using const_pointer = value_type const*;
	using void_pointer = void*;
	using const_void_pointer = void const*;
	using size_type = size_t;
	using difference_type = ptrdiff_t;


	_Allocator_by_Chunk() noexcept : _cur_chunk(nullptr){}
	_Allocator_by_Chunk(_Allocator_by_Chunk const& rhs) noexcept	 : _Allocator_by_Chunk(){}
	
	_Allocator_by_Chunk(_Allocator_by_Chunk&& rhs) noexcept 
	:	_cur_chunk(rhs._cur_chunk){  rhs._cur_chunk = nullptr;  }


	auto operator=(_Allocator_by_Chunk const& rhs)-> _Allocator_by_Chunk&
	{
		auto temp = rhs;

		return swap(temp),  *this;
	}

	auto operator=(_Allocator_by_Chunk&& rhs) noexcept-> _Allocator_by_Chunk&
	{
		auto temp = Move(rhs);

		return swap(temp),  *this;
	}


	auto allocate(size_type)-> pointer
	{
		if(_cur_chunk == nullptr || _cur_chunk->has_gone())
			_cur_chunk = _chunk_alloc_t::Allocate(1),
			_chunk_alloc_t::Construct(_cur_chunk);

		return _cur_chunk->get();
	}
	

	auto deallocate(pointer, size_type) noexcept-> void{}


	template<class Q, class...ARGS>
	auto construct(Q*, ARGS&&...args)-> void
	{
		SGM_CRTP_OVERRIDE(construct, <Q, ARGS...>);

		_cur_chunk->push( Forward<ARGS>(args)... );
	}


	template<class Q>
	auto destroy(Q* p) noexcept-> void
	{
		SGM_CRTP_OVERRIDE(destroy, <Q>);

		if(!p)
			return;
		
		_chunk_t* chunk_ptr = _chunk_t::Pop(p);

		if(chunk_ptr)
			_chunk_alloc_t::Destroy(*chunk_ptr),
			_chunk_alloc_t::Deallocate(chunk_ptr, 1);

		if(_cur_chunk == chunk_ptr)
			_cur_chunk = nullptr;
	}


	auto swap(_Allocator_by_Chunk& rhs) noexcept
	->	void{  Swap(_cur_chunk, rhs._cur_chunk);  }

private:
	_chunk_t* _cur_chunk;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace std
{

	template<class T>
	static auto swap(sgm::List<T>& L0, sgm::List<T>& L1) noexcept-> void{  L0.swap(L1);  }

	template<class T, size_t N>
	static auto swap(sgm::_Allocator_by_Chunk<T, N>& a0, sgm::_Allocator_by_Chunk<T, N>& a1) 
	noexcept-> void{  a0.swap(a1);  }

}


namespace sgm
{
	
	template<class T>
	static auto Swap(List<T>& L0, List<T>& L1) noexcept-> void{  L0.swap(L1);  }

	template<class T, size_t N>
	static auto Swap(_Allocator_by_Chunk<T, N>& a0, _Allocator_by_Chunk<T, N>& a1) noexcept
	->	void{  a0.swap(a1);  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#

#endif // end of #ifndef _SGM_LIST_

