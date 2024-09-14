/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_ALLOCATOR_
#define _SGM_ALLOCATOR_


#include <new>
#include <limits>
#include "SGM/interface_Traits/interface_Traits.hpp"


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
class sgm::Allocator_interface : public CRTP_of< Allocator_interface<ALLOC> >
{
private:
	SGM_HAS_NESTED_TYPE(value_type);
	SGM_HAS_NESTED_TYPE(size_type);
	SGM_HAS_NESTED_TYPE(pointer);

	SGM_HAS_MEMFUNC(allocate);
	SGM_HAS_MEMFUNC(deallocate);


public:
	template<class Q, class...ARGS>
	SGM_CRTP_TEMPLATE_INTERFACE
	(	construct, (Has_Operator_New<Q, ARGS...>::value)
	);


	template<class Q>
	SGM_CRTP_TEMPLATE_INTERFACE
	(	destroy, (Has_Operator_Delete<Q>::value)
	);


	auto constexpr check_type_definitions() const noexcept-> void
	{
		static_assert
		(	Boolean_And
			<	Has_NestedType_value_type<ALLOC>
			,	Has_NestedType_size_type<ALLOC>
			,	Has_NestedType_pointer<ALLOC>
			>::	value
		,	"sgm::Allocator_interface::check_type_definitions Failed ."
		);
	}


	template<class SIZE, class PTR>
	auto constexpr check_nontemplate_overridings() const noexcept-> void
	{
		static_assert
		(	Boolean_And
			<	Has_MemFunc_allocate_Returning<PTR, ALLOC, SIZE>
			,	Has_MemFunc_deallocate<ALLOC, PTR, SIZE>
			>::	value
		,	"sgm::Allocator_interface::check_nontemplate_overridings Failed ."
		);
	}


	constexpr Allocator_interface() noexcept
	{
		check_type_definitions(),
		check_nontemplate_overridings<typename ALLOC::size_type, typename ALLOC::pointer>();
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#

template<class T>
struct sgm::is_Allocator : Unconstructible
{
private:
	template<class...> /* Declaration Only */ 
	static auto _calc(...) noexcept-> False_t;

	template<class Q> /* Declaration Only */ 
	static auto _calc(Allocator_interface<Q>) noexcept-> True_t;


public:
	using type = decltype( _calc(Mock<T>()) );
	
	static bool constexpr value = type::value;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


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


	auto allocate(size_type n) const-> pointer{  return Allocate(n);  }

	auto deallocate(pointer p, size_type n) const noexcept-> void{  Deallocate(p, n);  }

	template<class Q, class...ARGS>
	auto construct(Q* p, ARGS&&...args) const-> void
	{
		SGM_CRTP_OVERRIDE(construct, <Q, ARGS...>);

		Construct( p, Forward<ARGS>(args)... );
	}

	template<class Q>
	auto destroy(Q* p) const noexcept-> void
	{
		SGM_CRTP_OVERRIDE(destroy, <Q>);

		Destroy(*p);
	}


	auto max_size() const noexcept-> size_type{  return Max_size();  }

public:
	static auto Allocate(size_type n)
	->	pointer{  return static_cast<pointer>(  ::operator new( sizeof(value_type)*n )  );  }

	static auto Deallocate(pointer p, size_type) noexcept
	->	void{  ::operator delete(p);  }

	template<class Q, class...ARGS>
	static auto Construct(Q* p, ARGS&&...args)
	->	void{  new(p) Q{ Forward<ARGS>(args)... };  }

	template< class Q, class _Q = Decay_t<Q> >
	static auto Destroy(Q& q) noexcept
	->	Enable_if_t< is_Class_or_Union<_Q>::value >{  q.~_Q();  }
	
	template< class Q, class _Q = Decay_t<Q> >
	static auto Destroy(Q&) noexcept
	->	Enable_if_t< !is_Class_or_Union<_Q>::value >{}

	static auto Max_size() noexcept
	->	size_type{  return std::numeric_limits<size_type>::max() / sizeof(value_type);  }

};
//========//========//========//========//=======#//========//========//========//========//=======#


#endif // end of #ifndef _SGM_ALLOCATOR_