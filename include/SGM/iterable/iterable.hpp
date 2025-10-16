/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_ITERABLE_
#define _SGM_ITERABLE_


#include "SGM/interface_Traits/interface_Traits.hpp"


namespace sgm
{

	template<class T>
	struct is_iterator
	:	Boolean_Or
		<	is_Pointer<T>
		,	Boolean_And
			<	Has_Operator_Deref<T>, Has_Operator_Post_increase<T>, Has_Operator_NotSame<T>
			>
		>
	{};


	template<class T>
	struct is_bidirectional_iterator
	:	Boolean_And
		<	is_iterator<T>
		,	Boolean_Or< Has_Operator_Post_Decrease<T>, is_Pointer<T> >
		>
	{};


	template<class T>
	struct is_random_access_iterator 
	:	Boolean_And
		<	is_bidirectional_iterator<T>
		,	Boolean_Or< Has_Operator_index<T>, is_Pointer<T> >
		>
	{};

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	SGM_HAS_NESTED_TYPE(value_type);

	SGM_HAS_MEMFUNC(begin);
	SGM_HAS_MEMFUNC(end);


	template
	<	class RG
	,	int MODE
		=	Has_MemFunc_begin< Referenceless_t<RG> >::value ? 1
		:	is_Primitive_Array< Referenceless_t<RG> >::value ? 2
		:	/* otherwise */ 0
	>
	struct _Begin_Helper
	{
		static_assert(MODE != 0, "No suitable action for sgm::Begin ");
	};

	template<class RG>
	struct _Begin_Helper<RG, 1> : Unconstructible
	{
		template<class _RG>  
		static auto Calc(_RG&& rg) SGM_NOEXCEPT_DECLTYPE_AUTO(  rg.begin()  )
	};

	template<class RG>
	struct _Begin_Helper<RG, 2> : Unconstructible
	{
		template<class A>  
		static auto Calc(A&& arr) noexcept-> decltype( Address_of(arr[0]) ){  return arr;  }
	};

	template<class RG>
	static auto Begin(RG&& rg) 
	SGM_NOEXCEPT_DECLTYPE_AUTO(  _Begin_Helper<RG>::Calc( Forward<RG>(rg) )  )

	template<class RG>
	static auto cBegin(RG const& rg) SGM_NOEXCEPT_DECLTYPE_AUTO(  Begin(rg)  )


	template
	<	class RG
	,	int MODE
		=	Has_MemFunc_end< Referenceless_t<RG> >::value ? 1
		:	is_Bounded_Array< Referenceless_t<RG> >::value ? 2
		:	/* otherwise */ 0
	>
	struct _End_Helper
	{
		static_assert(MODE != 0, "No suitable action for sgm::End ");		
	};

	template<class RG>
	struct _End_Helper<RG, 1> : Unconstructible
	{
		template<class _RG>  
		static auto Calc(_RG&& rg) SGM_NOEXCEPT_DECLTYPE_AUTO(  rg.end()  )
	};


	template<class>
	struct __Static_Array_Size;

	template<class T, size_t N>
	struct __Static_Array_Size<T[N]>{  static size_t constexpr value = N;  };


	template<class RG>
	struct _End_Helper<RG, 2> : Unconstructible
	{
		template<class A>
		static auto Calc(A&& arr) noexcept-> decltype( Address_of(arr[0]) )
		{
			return arr + __Static_Array_Size< Decay_t<A> >::value;
		}
	};

	template<class RG>
	static auto End(RG&& rg) 
	SGM_NOEXCEPT_DECLTYPE_AUTO(  _End_Helper<RG>::Calc( Forward<RG>(rg) )  )

	template<class RG>
	static auto cEnd(RG const& rg) SGM_NOEXCEPT_DECLTYPE_AUTO(  End(rg)  )

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	template
	<	class ITR
	,	int 
		=	is_random_access_iterator<ITR>::value ? 3
		:	is_bidirectional_iterator<ITR>::value ? 2
		:	is_iterator<ITR>::value ? 1
		:	/* otherwise */ 0
	>
	class Reverse_iterator;


	template<class ITR>
	static auto Reversing(ITR const& itr) 
	SGM_NOEXCEPT_DECLTYPE_AUTO(  Reverse_iterator<ITR>(itr)  )

	template<class ITR>
	static auto Reversing(Reverse_iterator<ITR> const& ritr) 
	SGM_NOEXCEPT_DECLTYPE_AUTO(  ritr.base()  )


	SGM_HAS_MEMFUNC(rbegin);
	SGM_HAS_MEMFUNC(rend);


	template
	<	class RG
	,	int MODE
		=	Has_MemFunc_rbegin< Referenceless_t<RG> >::value ? 1
		:	Boolean_Or
			<	Has_MemFunc_end< Referenceless_t<RG> >
			,	is_Bounded_Array< Referenceless_t<RG> >
			>::	value ? 2
		:	/* otherwise */ 0
	>
	struct _rBegin_Helper
	{
		static_assert(MODE != 0, "No suitable action for sgm::rBegin ");
	};

	template<class RG>
	struct _rBegin_Helper<RG, 1> : Unconstructible
	{
		template<class _RG>
		static auto Calc(_RG&& rg) SGM_NOEXCEPT_DECLTYPE_AUTO(  rg.rbegin()  )
	};

	template<class RG>
	struct _rBegin_Helper<RG, 2> : Unconstructible
	{
		template<class A>
		static auto Calc(A&& arr) SGM_NOEXCEPT_DECLTYPE_AUTO(  Reversing( End(arr) - 1 )  )
	};

	template<class RG>
	static auto rBegin(RG&& rg)
	SGM_NOEXCEPT_DECLTYPE_AUTO(  _rBegin_Helper<RG>::Calc( Forward<RG>(rg) )  )

	template<class RG>
	static auto crBegin(RG const& rg) SGM_NOEXCEPT_DECLTYPE_AUTO(  rBegin(rg)  )


	template
	<	class RG
	,	int MODE
		=	Has_MemFunc_rend< Referenceless_t<RG> >::value ? 1
		:	Boolean_Or
			<	Has_MemFunc_begin< Referenceless_t<RG> >
			,	is_Primitive_Array< Referenceless_t<RG> >
			>::	value ? 2
		:	/* otherwise */ 0
	>
	struct _rEnd_Helper
	{
		static_assert(MODE != 0, "No suitable action for sgm::rEnd ");
	};

	template<class RG>
	struct _rEnd_Helper<RG, 1> : Unconstructible
	{
		template<class _RG>
		static auto Calc(_RG&& rg) SGM_NOEXCEPT_DECLTYPE_AUTO(  rg.rend()  ) 
	};

	template<class RG>
	struct _rEnd_Helper<RG, 2> : Unconstructible
	{
		template<class A>
		static auto Calc(A&& arr) SGM_NOEXCEPT_DECLTYPE_AUTO(  Reversing( Begin(arr) - 1 )  )
	};

	template<class RG>
	static auto rEnd(RG&& rg) 
	SGM_NOEXCEPT_DECLTYPE_AUTO(  _rEnd_Helper<RG>::Calc( Forward<RG>(rg) )  )

	template<class RG>
	static auto crEnd(RG const& rg) SGM_NOEXCEPT_DECLTYPE_AUTO(  rEnd(rg)  )

}
//========//========//========//========//=======#//========//========//========//========//=======#


#ifndef _SGM_DETAIL_XXX_OR
	#define _SGM_DETAIL_XXX_OR(XXX)	\
		template<class ITR, class OTHER>		\
		struct XXX##_or	\
		{	\
		private:	\
			template<class Q>	\
			static auto _Calc(int) noexcept-> SFINAE_t<typename Q::XXX, typename Q::XXX>;	\
			\
			template<class>	\
			static auto _Calc(...) noexcept-> OTHER;	\
			\
		public:	\
			using type = decltype( _Calc<ITR>(0) );	\
		};	\
		\
		template<class ITR, class OTHER>		\
		using XXX##_or_t = typename XXX##_or<ITR, OTHER>::type


namespace sgm
{
	namespace _detail
	{

		_SGM_DETAIL_XXX_OR(value_type);
		_SGM_DETAIL_XXX_OR(difference_type);
		_SGM_DETAIL_XXX_OR(pointer);
		_SGM_DETAIL_XXX_OR(reference);

	}
}

	#undef _SGM_DETAIL_XXX_OR
#else
	#error _SGM_DETAIL_XXX_OR was already defined somewhere else.
#endif


template<class ITR, int ITR_TRAITS>
class sgm::Reverse_iterator
{
public:
	static_assert(ITR_TRAITS != 0, "sgm::Reverse_iterator expects ITR to be an iterator type.");
};


template<class ITR>
class sgm::Reverse_iterator<ITR, 1> 
{
private:
	using _itr_t = Reverse_iterator;
	using _deref_t = decltype(*Mock<ITR>());	

protected:
	static bool constexpr _is_Nxct_constructible_v = noexcept( ITR(Mock<ITR const&>()) );

public:
	ITR base_itr;


	using value_type = _detail::value_type_or_t< ITR, Decay_t<_deref_t> >;
	using difference_type = _detail::difference_type_or_t<ITR, ptrdiff_t>;
	using pointer = _detail::pointer_or_t< ITR, Referenceless_t<_deref_t>* >;
	using reference = _detail::reference_or_t< ITR, Referenceless_t<_deref_t>& >;	


	Reverse_iterator(ITR const& base_itr_) noexcept(_is_Nxct_constructible_v)
	:	base_itr(base_itr_){}

	auto operator*() const noexcept( noexcept(*Mock<ITR const>()) )
	->	SGM_DECLTYPE_AUTO(  *base_itr  )

	auto operator->() const noexcept( noexcept(*Mock<ITR const>()) )
	->	SGM_DECLTYPE_AUTO(  Address_of(**this)  )

	auto operator==(_itr_t const& itr) const
	noexcept( noexcept(Mock<ITR const>() == Mock<ITR const&>()) )
	->	SGM_DECLTYPE_AUTO(  base_itr == itr.base_itr  )

	auto operator!=(_itr_t const& itr) const
	noexcept( noexcept(Mock<ITR const>() == Mock<ITR const&>()) )
	->	SGM_DECLTYPE_AUTO(  !(*this == itr)  )


	auto operator--() noexcept( noexcept(++Mock<ITR&>()) )
	->	SGM_DECLTYPE_AUTO(  (++base_itr,  *this)  )

	auto operator--(int) noexcept( _is_Nxct_constructible_v && noexcept(++Mock<ITR&>()) )
	->	_itr_t
	{
		auto const res = *this;

		--*this;

		return res;
	}
};


template<class ITR>
class sgm::Reverse_iterator<ITR, 2> : public Reverse_iterator<ITR, 1>
{
private:
	using _1st_t = Reverse_iterator<ITR, 1>;
	using _itr_t = Reverse_iterator;


public:
	using _1st_t::_1st_t;

	using _1st_t::operator--;


	auto operator++() noexcept( noexcept(--Mock<ITR&>()) )
	->	SGM_DECLTYPE_AUTO(  (--_1st_t::base_itr,  *this)  )

	auto operator++(int)
	noexcept( _1st_t::_is_Nxct_constructible_v && noexcept(--Mock<ITR&>()) )-> _itr_t
	{
		auto const res = *this;

		++*this;

		return res;
	}
};


template<class ITR>
class sgm::Reverse_iterator<ITR, 3> : public Reverse_iterator<ITR, 2>
{
private: 
	using _1st_t = Reverse_iterator<ITR, 1>;
	using _2nd_t = Reverse_iterator<ITR, 2>;
	using _itr_t = Reverse_iterator;

	using _1st_t::base_itr;

public:
	using typename _1st_t::difference_type;

	using _2nd_t::_2nd_t;

	using _2nd_t::operator++;
	using _1st_t::operator--;


	auto operator+(difference_type const diff) const 
	noexcept
	(	noexcept(Mock<ITR const>() - Mock<difference_type const>())
	&&	_1st_t::_is_Nxct_constructible_v
	)->	_itr_t{  return {base_itr - diff};  }
	
	auto operator-(difference_type const diff) const 
	noexcept
	(	noexcept(Mock<ITR const>() + Mock<difference_type const>()) 
	&&	_1st_t::_is_Nxct_constructible_v
	)->	_itr_t{  return {base_itr + diff};  }

	auto operator[](difference_type const diff) const 
	noexcept( noexcept(Mock<ITR const>() - Mock<difference_type const>()) )
	->	decltype(*Mock<ITR const>()){  return *(*this + diff);  }

	
	auto operator-(_itr_t const& itr) const 
	noexcept( noexcept(Mock<ITR const&>() - Mock<ITR const>()) )
	->	SGM_DECLTYPE_AUTO(  itr.base_itr - base_itr  )
	

	auto operator+=(difference_type const diff)
	noexcept( noexcept(Mock<ITR&>() -= Mock<difference_type const>()) )
	->	_itr_t&{  return base_itr -= diff,  *this;  }

	auto operator-=(difference_type const diff)
	noexcept( noexcept(Mock<ITR&>() += Mock<difference_type const>()) )
	->	_itr_t&{  return base_itr += diff,  *this;  }


	auto operator<(_itr_t const& itr) const 
	noexcept( noexcept(Mock<ITR const>() > Mock<ITR const&>()) )
	->	SGM_DECLTYPE_AUTO(  base_itr > itr.base_itr  )
	
	auto operator>(_itr_t const& itr) const
	noexcept( noexcept(Mock<ITR const>() < Mock<ITR const&>()) )
	->	SGM_DECLTYPE_AUTO(  base_itr < itr.base_itr  )

	auto operator<=(_itr_t const& itr) const
	noexcept( noexcept(Mock<ITR const>() < Mock<ITR const&>()) )
	->	bool{  return !(*this > itr);  }

	auto operator>=(_itr_t const& itr) const
	noexcept( noexcept(Mock<ITR const>() > Mock<ITR const&>()) )
	->	bool{  return !(*this < itr);  }
};
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	template
	<	class ITR
	,	int 
		=	is_random_access_iterator<ITR>::value ? 3
		:	is_bidirectional_iterator<ITR>::value ? 2
		:	is_iterator<ITR>::value ? 1
		:	/* otherwise */ 0
	>
	class Move_iterator;

}


template<class ITR, int ITR_TRAITS>
class sgm::Move_iterator 
{
public:
	static_assert(ITR_TRAITS != 0, "sgm:Move_iterator expects ITR to be an iterator type ");
};


template<class ITR>
class sgm::Move_iterator<ITR, 1> 
{
private:
	using _itr_t = Move_iterator;
	using _deref_t = decltype(*Mock<ITR>());	

public:
	using value_type = _detail::value_type_or_t< ITR, Decay_t<_deref_t> >;
	using difference_type = _detail::difference_type_or_t<ITR, ptrdiff_t>;
	using pointer = _detail::pointer_or_t< ITR, Referenceless_t<_deref_t>* >;
	using reference = _detail::reference_or_t< ITR, Referenceless_t<_deref_t>& >;


	Move_iterator(ITR const& itr) noexcept(  noexcept( ITR(Mock<ITR const&>()) )  )
	:	_itr(itr){}

	auto base() const noexcept-> ITR const&{  return _itr;  }
	auto base() noexcept-> ITR&{  return _itr;  }


	auto operator*() const
	noexcept
	(	noexcept(*Mock<ITR const&>()) 
	&&	(	is_Reference< decltype(*Mock<ITR const&>()) >::value
		||	noexcept( value_type(Mock<value_type&&>()) )
		)
	)->	Selective_t
		<	is_Lvalue_Reference< decltype(*Mock<ITR const&>()) >::value
		,	value_type&&
		,	decltype(*Mock<ITR const&>())
		>
	{  
		return 
		Move_if<  is_Lvalue_Reference< decltype(*Mock<ITR const&>()) >::value  >(*base()); 
	}


	auto operator->() const noexcept( noexcept(*Mock<ITR const&>()) )
	->	pointer{  return Address_of(*base());  }

	auto operator==(_itr_t const& itr) const 
	noexcept( noexcept(Mock<ITR const&>() == Mock<ITR const&>()) )
	->	bool{  return base() == itr.base();  }
	
	auto operator!=(_itr_t const& itr) const 
	noexcept( noexcept(Mock<_itr_t const>() == Mock<_itr_t const&>()) )
	->	bool{  return !(*this == itr);  }


	auto operator++() noexcept( noexcept(++Mock<ITR&>()) )
	->	_itr_t&{  return ++base(),  *this;  }

	auto operator++(int) 
	noexcept(  noexcept( ITR(Mock<ITR const&>()) ) && noexcept(++Mock<_itr_t&>())  )-> _itr_t
	{
		auto const res = *this;

		++*this;

		return res;
	}


private:
	ITR _itr;
};


template<class ITR>
class sgm::Move_iterator<ITR, 2> : public Move_iterator<ITR, 1> 
{
private:
	using _1st_t = Move_iterator<ITR, 1>;
	using _itr_t = Move_iterator;

public:
	using _1st_t::_1st_t;

	using _1st_t::operator++;


	auto operator--() noexcept( noexcept(--Mock<_1st_t&>().base()) )
	->	_itr_t&{  return --_1st_t::base(),  *this;  }

	auto operator--(int) 
	noexcept(  noexcept( ITR(Mock<ITR const&>()) ) && noexcept(--Mock<_itr_t&>())  )-> _itr_t
	{
		auto const res = *this;

		--*this;

		return res;
	}
};

	
template<class ITR>
class sgm::Move_iterator<ITR, 3> : public Move_iterator<ITR, 2>
{
private:
	using _1st_t = Move_iterator<ITR, 1>;
	using _2nd_t = Move_iterator<ITR, 2>;
	using _itr_t = Move_iterator;

	using _1st_t::base;

public:
	using typename _1st_t::difference_type;

	using _2nd_t::_2nd_t;

	using _1st_t::operator++;
	using _2nd_t::operator--;


	auto operator+(difference_type const diff) const
	noexcept
	(	noexcept(Mock<_1st_t const>().base() + Mock<difference_type const>())
	&&	noexcept( _itr_t(Mock<ITR const&>()) )
	)->	_itr_t{  return {base() + diff};  }
	
	auto operator-(difference_type const diff) const
	noexcept	
	(	noexcept(Mock<_1st_t const>().base() - Mock<difference_type const>())
	&&	noexcept( _itr_t(Mock<ITR const&>()) )
	)->	_itr_t{  return {base() - diff};  }


	auto operator-(_itr_t const itr) const 
	noexcept( noexcept(Mock<ITR const&>() - Mock<ITR const&>()) )
	->	difference_type{  return base() - itr.base();  }


	auto operator+=(difference_type const diff)
	noexcept( noexcept(Mock<ITR&>() += Mock<difference_type const>()) )
	->	_itr_t&{  return base() += diff,  *this;  }
	
	auto operator-=(difference_type const diff)
	noexcept( noexcept(Mock<ITR&>() -= Mock<difference_type const>()) )
	->	_itr_t&{  return base() -= diff,  *this;  }


	auto operator[](difference_type const diff) const
	noexcept(  noexcept( *(Mock<_itr_t const>() + Mock<difference_type const>()) )  )
	->	typename _1st_t::value_type{  return Move(base()[diff]);  }
};


namespace sgm
{

	SGM_USER_DEFINED_TYPE_CHECK
	(	class ITR
	,	Move_iterator, <ITR>
	);

	
	template
	<	class ITR
	,	class = Enable_if_t< is_iterator<ITR>::value && !is_Move_iterator<ITR>::value >
	>
	static auto To_Move_iterator(ITR const& itr) 
	noexcept(  noexcept( Move_iterator<ITR>(Mock<ITR const&>()) )  )
	->	Move_iterator<ITR>{  return itr;  }

	template<class ITR>
	static auto To_Move_iterator(Move_iterator<ITR> const& itr) noexcept
	->	Move_iterator<ITR> const&{  return itr;  }


	template<class RG>
	static auto mBegin(RG&& rg) 
	noexcept(   noexcept(  To_Move_iterator( Begin(Mock<RG&>()) )  )   )
	->	SGM_DECLTYPE_AUTO(  To_Move_iterator( Begin(rg) )  )

	template<class RG>
	static auto mEnd(RG&& rg)
	noexcept(   noexcept(  To_Move_iterator( End(Mock<RG&>()) )  )   )
	->	SGM_DECLTYPE_AUTO(  To_Move_iterator( End(rg) )  )

	template<class RG>
	static auto mrBegin(RG&& rg)
	noexcept(   noexcept(  To_Move_iterator( rBegin(Mock<RG&>()) )  )   )
	->	SGM_DECLTYPE_AUTO(  To_Move_iterator( rBegin(rg) )  )

	template<class RG>
	static auto mrEnd(RG&& rg)
	noexcept(   noexcept(  To_Move_iterator( rEnd(Mock<RG&>()) )  )   )
	->	SGM_DECLTYPE_AUTO(  To_Move_iterator( rEnd(rg) )  )


	template<  class RG, class ITR = Decay_t< decltype( Begin(Mock<RG>()) ) >  >
	static auto fBegin(Referenceless_t<RG>& rg)
	noexcept
	(	is_Rvalue_Reference<RG&&>::value
		?	noexcept( Begin(Mock< Referenceless_t<RG>& >()) )
		:	noexcept(  Move_iterator<ITR>( Begin(Mock< Referenceless_t<RG>& >()) )  )
	)->	Selective_t< is_Rvalue_Reference<RG&&>::value, Move_iterator<ITR>, ITR >  
	{
		return Begin(rg);
	}
	
	template<class RG>
	static auto fBegin(Referenceless_t<RG>&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  mBegin( Move(rg) )  )

	template<  class RG, class ITR = Decay_t< decltype( End(Mock<RG>()) ) >  >
	static auto fEnd(Referenceless_t<RG>& rg)
	noexcept
	(	is_Rvalue_Reference<RG&&>::value
		?	noexcept( End(Mock< Referenceless_t<RG>& >()) )
		:	noexcept(  Move_iterator<ITR>( End(Mock< Referenceless_t<RG>& >()) )  )
	)->	Selective_t< is_Rvalue_Reference<RG&&>::value, Move_iterator<ITR>, ITR >  
	{
		return End(rg);
	}
	
	template<class RG>
	static auto fEnd(Referenceless_t<RG>&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  mEnd( Move(rg) )  )

	template<  class RG, class ITR = Decay_t< decltype( rBegin(Mock<RG>()) ) >  >
	static auto frBegin(Referenceless_t<RG>& rg)
	noexcept
	(	is_Rvalue_Reference<RG&&>::value
		?	noexcept( rBegin(Mock< Referenceless_t<RG>& >()) )
		:	noexcept(  Move_iterator<ITR>( rBegin(Mock< Referenceless_t<RG>& >()) )  )
	)->	Selective_t< is_Rvalue_Reference<RG&&>::value, Move_iterator<ITR>, ITR >  
	{
		return rBegin(rg);
	}
	
	template<class RG>
	static auto frBegin(Referenceless_t<RG>&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  mrBegin( Move(rg) )  )

	template<  class RG, class ITR = Decay_t< decltype( rEnd(Mock<RG>()) ) >  >
	static auto frEnd(Referenceless_t<RG>& rg)
	noexcept
	(	is_Rvalue_Reference<RG&&>::value
		?	noexcept( rEnd(Mock< Referenceless_t<RG>& >()) )
		:	noexcept(  Move_iterator<ITR>( rEnd(Mock< Referenceless_t<RG>& >()) )  )
	)->	Selective_t< is_Rvalue_Reference<RG&&>::value, Move_iterator<ITR>, ITR >  
	{
		return rEnd(rg);
	}
	
	template<class RG>
	static auto frEnd(Referenceless_t<RG>&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  mrEnd( Move(rg) )  )

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	template<class RG, class T = void>
	struct is_iterable;	

}


template<class RG, class T>
struct sgm::is_iterable : Unconstructible
{
private:
	template<class Q>  /* Declaration Only */
	static auto _Begin(int) noexcept-> SFINAE_t< decltype( static_cast<T>(*Mock<Q>().begin()) ) >;

	template<class Q>  /* Declaration Only */
	static auto _Begin(...) noexcept-> is_Bounded_Array< Referenceless_t<Q> >;

	template<class Q>  /* Declaration Only */
	static auto _End(int) noexcept-> SFINAE_t< decltype( static_cast<T>(*Mock<Q>().end()) ) >;

	template<class Q>  /* Declaration Only */
	static auto _End(...) noexcept-> is_Bounded_Array< Referenceless_t<Q> >;

public:
	static bool constexpr value
	=	decltype( _Begin<RG>(0) )::value && decltype( _End<RG>(0) )::value;

	using type = Boolean<value>;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template<int NXCT_FLAG, class RES_CON, class RG, class...ARGS>
	struct Conversion_to_Container_impl;
	
}


template<int NXCT_FLAG, class RES_CON, class RG, class...ARGS>
struct sgm::Conversion_to_Container_impl
{
	static bool constexpr is_NXCT = true;

	static_assert(NXCT_FLAG != 0, "Error : No suitable convertible container .");

	static auto Calc(RG&&, ARGS&&...) noexcept-> RES_CON;
};

template<class RES_CON, class RG, class...ARGS>
struct sgm::Conversion_to_Container_impl<1, RES_CON, RG, ARGS...> : Unconstructible
{
	static_assert
	(	(	is_Reference<RG>::value 
		&&	Check_if_All<is_Reference, ARGS...>::value
		)
	, 	""
	);

	static bool constexpr is_NXCT 
	=	noexcept(  RES_CON( fBegin<RG>(Mock<RG>()), fEnd<RG>(Mock<RG>()), Mock<ARGS>()... )  );

	static auto Calc(RG&& rg, ARGS&&...args) noexcept(is_NXCT)-> RES_CON
	{
		return 
		RES_CON
		(	fBegin<RG>( Forward<RG>(rg) ), fEnd<RG>( Forward<RG>(rg) ), Forward<ARGS>(args)... 
		);
	}
};

template<class RES_CON, class RG, class...ARGS>
struct sgm::Conversion_to_Container_impl<2, RES_CON, RG, ARGS...> : Unconstructible
{
	static_assert
	(	(	is_Reference<RG>::value 
		&&	Check_if_All<is_Reference, ARGS...>::value
		)
	, 	""
	);

	static bool constexpr is_NXCT 
	=	noexcept(  RES_CON( Mock<ARGS>()..., fBegin<RG>(Mock<RG>()), fEnd<RG>(Mock<RG>()) )  );

	static auto Calc(RG&& rg, ARGS&&...args) noexcept(is_NXCT)-> RES_CON
	{
		return 
		RES_CON
		(	Forward<ARGS>(args)..., fBegin<RG>( Forward<RG>(rg) ), fEnd<RG>( Forward<RG>(rg) )
		);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{
	
	template
	<	class RES_CON
	, 	class RG, class...ARGS
	,	int NXCT_FLAG
		=	(	Has_Operator_New
				<	RES_CON
				, 	decltype( fBegin<RG>(Mock<RG&&>()) ), decltype( fEnd<RG>(Mock<RG&&>()) )
				, 	ARGS&&...
				>::	value 
			? 	1
			:	(	Has_Operator_New
					<	RES_CON
					, 	ARGS&&...
					, 	decltype( fBegin<RG>(Mock<RG&&>()) ), decltype( fEnd<RG>(Mock<RG&&>()) )
					>::	value
				?	2
				:	0
				)
			)
	,	class = Enable_if_t< is_iterable<RG>::value && NXCT_FLAG != 0 >
	>
	static auto iterable_cast(RG&& rg, ARGS&&...args)
	noexcept(Conversion_to_Container_impl<NXCT_FLAG, RES_CON, RG&&, ARGS&&...>::is_NXCT)
	-> 	RES_CON
	{
		return 
		Conversion_to_Container_impl<NXCT_FLAG, RES_CON, RG&&, ARGS&&...>
		::	Calc( Forward<RG>(rg), Forward<ARGS>(args)... );
	}

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	
	template
	<	class ITR
	,	bool = is_bidirectional_iterator<ITR>::value
	,	bool = is_random_access_iterator<ITR>::value
	>
	struct _Travel;


	template<class ITR>
	struct _Travel<ITR, false, false>
	{
		static auto _Next(ITR itr, ptrdiff_t steps) noexcept( noexcept(Mock<ITR&>()++) )-> ITR
		{
			while(steps-->0)
				itr++;

			return itr;
		}
	};


	template<class ITR>
	struct _Travel<ITR, true, false> : _Travel<ITR, false, false>
	{
		static auto _Prev(ITR itr, ptrdiff_t steps) noexcept( noexcept(Mock<ITR&>()--) )-> ITR
		{
			while(steps-->0)
				itr--;

			return itr;
		}
	};


	template<class ITR, bool IS_BIDIRECTIONAL>
	struct _Travel<ITR, IS_BIDIRECTIONAL, true>
	{
		static auto _Next(ITR const& itr, ptrdiff_t const steps) 
		noexcept( noexcept(Mock<ITR const&>() + Mock<ptrdiff_t const>()) )
		->	SGM_DECLTYPE_AUTO(  itr + steps  )

		static auto _Prev(ITR const& itr, ptrdiff_t const steps) 
		noexcept( noexcept(Mock<ITR const&>() - Mock<ptrdiff_t const>()) )
		->	SGM_DECLTYPE_AUTO(  itr - steps  )
	};


	template<  class ITR, class = Guaranteed_t< is_iterator<ITR>::value >  >
	static auto Next(ITR const& itr, ptrdiff_t const steps = 1)
	noexcept(  noexcept( _Travel<ITR>::_Next(Mock<ITR const&>(), Mock<ptrdiff_t const>()) )  )
	->	SGM_DECLTYPE_AUTO(  _Travel<ITR>::_Next(itr, steps)  )


	template<  class ITR, class = Guaranteed_t< is_bidirectional_iterator<ITR>::value >  >
	static auto Prev(ITR const& itr, ptrdiff_t const steps = 1)
	noexcept(  noexcept( _Travel<ITR>::_Prev(Mock<ITR const&>(), Mock<ptrdiff_t const>()) )  )
	->	SGM_DECLTYPE_AUTO(  _Travel<ITR>::_Prev(itr, steps)  )


	struct _Difference_Helper : Unconstructible
	{
		template<class ITR>
		static auto Calc(ITR bi, ITR const& ei)
		noexcept(  noexcept( static_cast<ptrdiff_t>(Mock<ITR>() - Mock<ITR const>()) )  )
		->	Enable_if_t< is_random_access_iterator<ITR>::value, ptrdiff_t >
		{
			return static_cast<ptrdiff_t>(ei - bi);
		}

		template<class ITR>
		static auto Calc(ITR bi, ITR const& ei)
		noexcept( noexcept(Mock<ITR>() != Mock<ITR const&>()) && noexcept(++Mock<ITR>()) )
		->	Enable_if_t< !is_random_access_iterator<ITR>::value, ptrdiff_t >
		{
			ptrdiff_t diff = 0;

			for(;  bi != ei;  bi++,  ++diff);

			return diff;
		}
	};

	template<  class ITR, class = Guaranteed_t< is_iterator<ITR>::value >  >
	static auto constexpr Difference(ITR const& bi, ITR const& ei)
	noexcept(  noexcept( _Difference_Helper::Calc(Mock<ITR const&>(), Mock<ITR const&>()) )  )
	->	SGM_DECLTYPE_AUTO(  _Difference_Helper::Calc(bi, ei)  )


	struct _iterable_Size_Helper : Unconstructible
	{
	private:
		SGM_HAS_MEMFUNC(size);

	public:
		template<class RG>
		static auto Calc(RG&& rg)
		noexcept(  noexcept( static_cast<size_t>(Mock<RG&>().size()) )  )
		->	Enable_if_t< Has_MemFunc_size<RG>::value, size_t >
		{
			return static_cast<size_t>(rg.size());
		}

		template<class RG>
		static auto Calc(RG&& rg)
		noexcept
		(	noexcept
			(	static_cast<size_t>(  Difference( Begin(Mock<RG&>()), End(Mock<RG&>()) )  )
			)
		)->	Enable_if_t< !Has_MemFunc_size<RG>::value, size_t >
		{
			return static_cast<size_t>(  Difference( Begin(rg), End(rg) )  );	
		}
	};

	template<class RG>
	static auto constexpr Size(RG&& rg)
	noexcept(  noexcept( _iterable_Size_Helper::Calc(Mock<RG&&>()) )  )
	->	SGM_DECLTYPE_AUTO(  _iterable_Size_Helper::Calc( Forward<RG>(rg) )  )

}
//========//========//========//========//=======#//========//========//========//========//=======#


#endif // end of #ifndef _SGM_ITERABLE_