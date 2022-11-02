/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "../List/List.hpp"


namespace sgm
{

	template<class T, class = _Default_List_Allocator_t<T> >
	class Queue;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, class ALLOC>
class sgm::Queue
{
public:
	Queue() = default;

	auto size() const noexcept-> size_t{  return _list.size();  }

	auto front() const noexcept-> T const&{  return _list.front();  }
	auto back() const noexcept-> T const&{  return _list.back();  }

	template<class...ARGS>
	auto push(ARGS&&...args)
	->	Queue&{  return _list.emplace_back( Forward<ARGS>(args)... ),  *this;  }

	auto pop()-> Queue&{  return _list.pop_front(),  *this;  }

	auto is_empty() const noexcept-> bool{  return _list.is_empty();  }


	template<class A>
	static auto by(A&& allocator)
	->	Queue< T, Decay_t<A> >{  return { _List_by_Tag{}, Forward<A>(allocator) };  }


private:
	List<T, ALLOC> _list;


	friend class sgm::Queue< T, _Default_List_Allocator_t<T> >;

	template<class A>
	Queue(_List_by_Tag, A&& allocator) : _list(  List<T>::by( Forward<A>(allocator) )  ){}
};