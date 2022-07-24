/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_ALLOCATOR_
#define _SGM_ALLOCATOR_


#include <new>
#include <cstddef>
#include <limits>
#include "../Type_Analysis/Type_Analysis.hpp"


namespace sgm
{
	
	template<class T>
	class Allocator;


	SGM_USER_DEFINED_TYPE_CHECK
	(	class T
	,	Allocator, <T>
	);

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
class sgm::Allocator
{
public:
	using value_type = T;
	using pointer = value_type*;
	using const_pointer = value_type const*;
	using void_pointer = void*;
	using const_void_pointer = void const*;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;


	auto allocate(size_type n)
	->	pointer{  return static_cast<pointer>(  ::operator new( sizeof(value_type)*n )  );  }

	void deallocate(pointer p, size_type){  ::operator delete(p);  }

	auto max_size() const noexcept
	->	size_type{  return std::numeric_limits<size_type>::max() / sizeof(value_type);  }

	template<class Q, class...ARGS>
	void construct(Q* p, ARGS&&...args){  new(p) Q( Forward<ARGS>(args)... );  }

	template<class Q>
	void destroy(Q* p){  p->~Q();  }
};
//========//========//========//========//=======#//========//========//========//========//=======#


#endif // end of #ifndef _SGM_ALLOCATOR_