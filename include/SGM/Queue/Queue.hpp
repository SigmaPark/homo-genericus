/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "../List/List.hpp"


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

	auto front() noexcept-> T&{  return _list.front();  }
	auto front() const noexcept-> T const&{  return _list.front();  }

	auto back() const noexcept-> T const&{  return _list.back();  }

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
	,	_iptr(_begin), _optr(_begin){}


	auto size() const noexcept-> size_t;

	auto max_size() const noexcept-> size_t{  return Difference(_begin, _end) - 1;  }

	auto front() noexcept-> T&;
	auto front() const noexcept-> T const&;

	auto back() const noexcept-> T const&;


	template<class...ARGS>
	auto push(ARGS&&...args)-> Circular_Queue&;

	template<class...ARGS>
	auto try_push(ARGS&&...args)-> Circular_Queue&
	{
		return is_full() ? *this : push( Forward<ARGS>(args)... );
	}

	auto pop()-> Circular_Queue&;

	auto try_pop()-> Circular_Queue&
	{
		return is_empty() ? *this : pop();
	}


	auto is_empty() const noexcept-> bool{  return _iptr == _optr;  }
	auto is_full() const noexcept-> bool;


	auto clear()-> Circular_Queue&
	{
		while(!is_empty())
			return pop();

		return *this;
	}


	template<  class A, class = Guaranteed_t< is_Allocator<A>::value >  >
	static auto by(A&& allocator, std::size_t const s)-> Circular_Queue< T, Decay_t<A> >
	{
		return { _Circular_Queue_by_Tag{}, Forward<A>(allocator), s };  
	}


private:
	ALLOC _allocator;
	T *_begin, *_end, *_iptr, *_optr;


	friend class sgm::Circular_Queue< T, Allocator<T> >;

	template<class A>
	Circular_Queue(_Circular_Queue_by_Tag, A&& allocator, std::size_t const s);
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#
