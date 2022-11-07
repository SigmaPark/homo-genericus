/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_ALLOCATOR_
#define _SGM_ALLOCATOR_


#include <new>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include "../interface_Traits/interface_Traits.hpp"


namespace sgm
{
	
	template<class ALLOC>
	class Allocator_interface;

	template<class T>
	struct is_Allocator;

	template<class T>
	class Allocator;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class ALLOC>
class sgm::Allocator_interface
{
private:
	SGM_HAS_NESTED_TYPE(value_type);
	SGM_HAS_NESTED_TYPE(size_type);
	SGM_HAS_NESTED_TYPE(pointer);


	enum class _Abstract{};


public:
	template<class...>
	_Abstract allocate(...) = delete;

	template<class...>
	_Abstract deallocate(...) = delete;

	template<class...>
	_Abstract construct(...) = delete;

	template<class...>
	_Abstract destroy(...) = delete;


	void constexpr type_interface_check()
	{
		static_assert
		(	Boolean_And
			<	Has_NestedType_value_type<ALLOC>
			,	Has_NestedType_size_type<ALLOC>
			,	Has_NestedType_pointer<ALLOC>
			>::	value
		,	"sgm::Allocator_interface::_type_interface_check Failed ."
		);
	}


	template<class SIZE, class PTR>
	void constexpr overriding_interface_check()
	{
		static_assert
		(	Boolean_And
			<	is_Same
				<	decltype( Declval<ALLOC>().allocate(SIZE{}) )
				,	PTR
				>
			,	is_Void< decltype( Declval<ALLOC>().deallocate(PTR{}, SIZE{}) ) >
			,	is_Void< decltype( Declval<ALLOC>().construct(PTR{}, "Any Types") ) >
			,	is_Void< decltype( Declval<ALLOC>().destroy(PTR{}) ) >
			>::	value
		,	"sgm::Allocator_interface::_overriding_interface_check Failed ."
		);
	}


	constexpr Allocator_interface()
	{
		type_interface_check(),
		overriding_interface_check<typename ALLOC::size_type, typename ALLOC::pointer>();
	}
};


template<class T>
struct sgm::is_Allocator : Unconstructible
{
private:
	template<class...> /* Declaration Only */ 
	static auto _calc(...) noexcept-> False_t;

	template<class Q> /* Declaration Only */ 
	static auto _calc(Allocator_interface<Q>) noexcept-> True_t;


public:
	using type = decltype( _calc(Declval<T>()) );
	
	static bool constexpr value = type::value;
};


template<class T>
class sgm::Allocator : public Allocator_interface< Allocator<T> >
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

	void deallocate(pointer p, size_type) noexcept{  ::operator delete(p);  }

	auto reallocate(pointer p, size_type n)
	->	pointer{  return static_cast<pointer>( std::realloc(p, n) );  }

	auto max_size() const noexcept
	->	size_type{  return std::numeric_limits<size_type>::max() / sizeof(value_type);  }

	template<class Q, class...ARGS>
	void construct(Q* p, ARGS&&...args){  new(p) Q( Forward<ARGS>(args)... );  }

	template<class Q>
	void destroy(Q* p) noexcept{  p->~Q();  }
};
//========//========//========//========//=======#//========//========//========//========//=======#


#endif // end of #ifndef _SGM_ALLOCATOR_