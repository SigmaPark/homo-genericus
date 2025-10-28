/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Container/List.hpp"


namespace sgm
{

	template<class T, class = _Default_List_Allocator_t<T> >
	class Queue;


	enum class _Circular_Queue_by_Tag{};

	template< class T, class = Allocator<T> >
	class Circular_Queue;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, class ALLOC>
class sgm::Queue
{
public:
	Queue() = default;

	auto size() const noexcept-> size_t{  return _list.size();  }

	auto front() const noexcept-> T const&{  return _list.front();  }
	auto front() noexcept-> T&{  return _list.front();  }

	auto back() const noexcept-> T const&{  return _list.back();  }
	auto back() noexcept-> T&{  return _list.back();  }

	template<class...ARGS>
	auto push(ARGS&&...args)
	->	Queue&{  return _list.emplace_back( Forward<ARGS>(args)... ),  *this;  }

	auto pop()-> Queue&{  return _list.pop_front(),  *this;  }

	auto is_empty() const noexcept-> bool{  return _list.is_empty();  }

	auto clear()-> Queue&{  return _list.clear(),  *this;  }


	template<  class A, class = Guaranteed_t< is_Allocator<A>::value >  >
	static auto by(A&& allocator)
	->	Queue< T, Decay_t<A> >{  return { _List_by_Tag{}, Forward<A>(allocator) };  }


private:
	List<T, ALLOC> _list;


	friend class sgm::Queue< T, _Default_List_Allocator_t<T> >;

	template<class A>
	Queue(_List_by_Tag, A&& allocator) : _list(  List<T>::by( Forward<A>(allocator) )  ){}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, class ALLOC>
class sgm::Circular_Queue
{
public:
	Circular_Queue(std::size_t const s) 
	:	_allocator(), _begin( _allocator.allocate(s + 1) ), _end(_begin + s + 1)
	,	_optr(_begin), _iptr(_begin){}


	Circular_Queue(Circular_Queue const& qu)
	:	_allocator(qu._allocator)
	,	_begin( _allocator.allocate(qu.max_size() + 1) ), _end(_begin + qu.max_size() + 1)
	,	_optr( _begin + Difference(qu._begin, qu._optr) ), _iptr(_optr)
	{
		for( auto p = qu._optr;  p != qu._iptr;  p = qu._next(p) )
			push(*p);
	}


	Circular_Queue(Circular_Queue&& qu) noexcept
	:	_allocator( Move(qu._allocator) )
	,	_begin(qu._begin), _end(qu._end), _optr(qu._optr), _iptr(qu._iptr)
	{
		qu._end = qu._iptr = qu._optr = qu._begin = nullptr;
	}


	~Circular_Queue()
	{
		if(_begin == nullptr)
			return;

		clear();

		_allocator.deallocate(_begin, max_size() + 1);

		_end = _iptr = _optr = _begin = nullptr;
	}


	auto operator=(Circular_Queue const& qu)-> Circular_Queue&
	{
		auto temp = qu;

		this->swap(temp);

		return *this;
	}

	auto operator=(Circular_Queue&& qu) noexcept-> Circular_Queue&
	{
		auto temp = Move(qu);

		this->swap(temp);

		return *this;
	}


	void swap(Circular_Queue& qu) noexcept
	{
		Swap(_allocator, qu._allocator);
		Swap(_begin, qu._begin);
		Swap(_end, qu._end);
		Swap(_optr, qu._optr);
		Swap(_iptr, qu._iptr);
	}


	auto size() const noexcept-> size_t
	{
		auto const d = Difference(_optr, _iptr);

		return d >= 0 ? d : max_size() + (d+1);
	}

	auto max_size() const noexcept-> size_t
	{
		return _begin == nullptr ? 0 : Difference(_begin, _end) - 1;
	}


	auto front() noexcept-> T&{  return *_optr;  }
	auto front() const noexcept-> T const&{  return *_optr;  }

	auto back() noexcept-> T&{  return *_prev(_iptr);  }
	auto back() const noexcept-> T const&{  return *_prev(_iptr);  }

	template<class...ARGS>
	auto push(ARGS&&...args)-> Circular_Queue&
	{
		assert(!is_full());

		_allocator.construct( _iptr, Forward<ARGS>(args)... );
		
		_iptr = _next(_iptr);

		return *this;
	}

	template<class...ARGS>
	auto try_push(ARGS&&...args)-> Circular_Queue&
	{
		return is_full() ? *this : push( Forward<ARGS>(args)... );
	}


	auto pop()-> Circular_Queue&
	{
		assert(!is_empty());

		_allocator.destroy(_optr);
		
		_optr = _next(_optr);

		return *this;
	}

	auto try_pop()-> Circular_Queue&
	{
		return is_empty() ? *this : pop();
	}


	auto is_empty() const noexcept-> bool{  return _iptr == _optr;  }
	auto is_full() const noexcept-> bool{  return _next(_iptr) == _optr;  }


	auto clear()-> Circular_Queue&
	{
		while(!is_empty())
			pop();

		return *this;
	}


	template<  class A, class = Guaranteed_t< is_Allocator<A>::value >  >
	static auto by(A&& allocator, std::size_t const s)-> Circular_Queue< T, Decay_t<A> >
	{
		return { _Circular_Queue_by_Tag{}, Forward<A>(allocator), s };  
	}


private:
	ALLOC _allocator;
	T *_begin, *_end, *_optr, *_iptr;


	auto _next(T* const p) const noexcept-> T* 
	{
		auto const last = Prev(_end);

		return p == last ? _begin : p+1;
	}

	auto _prev(T* const p) const noexcept-> T*
	{
		return Prev(p == _begin ? _end : p);
	}


	friend class sgm::Circular_Queue< T, Allocator<T> >;

	template<class A>
	Circular_Queue(_Circular_Queue_by_Tag, A&& allocator, std::size_t const s)
	:	_allocator( Forward<A>(allocator) )
	,	_begin( _allocator.allocate(s + 1) ), _end(_begin + s + 1)
	,	_optr(_begin), _iptr(_begin){}
};


namespace sgm
{

	template<class T>
	static void Swap(Circular_Queue<T>& a, Circular_Queue<T>& b) noexcept{  a.swap(b);  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#
