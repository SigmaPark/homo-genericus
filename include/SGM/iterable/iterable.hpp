/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_ITERABLE_
#define _SGM_ITERABLE_


#include "../interface_Traits/interface_Traits.hpp"


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
		static auto calc(_RG&& rg)-> SGM_DECLTYPE_AUTO(  rg.begin()  )
	};

	template<class RG>
	struct _Begin_Helper<RG, 2> : Unconstructible
	{
		template<class A>  
		static auto calc(A&& arr)-> SGM_DECLTYPE_AUTO(  &arr[0]  )
	};

	template<class RG>
	static auto Begin(RG&& rg)
	->	SGM_DECLTYPE_AUTO(  _Begin_Helper<RG>::calc( Forward<RG>(rg) )  )

	template<class RG>
	static auto cBegin(RG const& rg)-> SGM_DECLTYPE_AUTO(  Begin(rg)  )


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
		static auto calc(_RG&& rg)-> SGM_DECLTYPE_AUTO(  rg.end()  )
	};


	template<class>
	struct __Static_Array_Size;

	template<class T, size_t N>
	struct __Static_Array_Size<T[N]>{  static size_t constexpr value = N;  };


	template<class RG>
	struct _End_Helper<RG, 2> : Unconstructible
	{
		template<class A>
		static auto calc(A&& arr)
		->	SGM_DECLTYPE_AUTO(  &arr[__Static_Array_Size< Decay_t<A> >::value]  )
	};

	template<class RG>
	static auto End(RG&& rg)-> SGM_DECLTYPE_AUTO(  _End_Helper<RG>::calc( Forward<RG>(rg) )  )

	template<class RG>
	static auto cEnd(RG const& rg)-> SGM_DECLTYPE_AUTO(  End(rg)  )

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	template<class ITR>
	class Reverse_iterator;

	template<class ITR>
	static auto Reversing(ITR const itr)-> Reverse_iterator<ITR>{  return {itr};  }

	template<class ITR>
	static auto Reversing(Reverse_iterator<ITR> const ritr)-> ITR{  return ritr._itr;  }


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
		static auto calc(_RG&& rg)-> SGM_DECLTYPE_AUTO(  rg.rbegin()  )
	};

	template<class RG>
	struct _rBegin_Helper<RG, 2> : Unconstructible
	{
		template<class A>
		static auto calc(A&& arr)-> SGM_DECLTYPE_AUTO(  Reversing( End(arr) - 1 )  )
	};

	template<class RG>
	static auto rBegin(RG&& rg)
	->	SGM_DECLTYPE_AUTO(  _rBegin_Helper<RG>::calc( Forward<RG>(rg) )  )

	template<class RG>
	static auto crBegin(RG const& rg)-> SGM_DECLTYPE_AUTO(  rBegin(rg)  )


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
		static auto calc(_RG&& rg)-> SGM_DECLTYPE_AUTO(  rg.rend()  ) 
	};

	template<class RG>
	struct _rEnd_Helper<RG, 2> : Unconstructible
	{
		template<class A>
		static auto calc(A&& arr)-> SGM_DECLTYPE_AUTO(  Reversing( Begin(arr) - 1 )  )
	};

	template<class RG>
	static auto rEnd(RG&& rg)-> SGM_DECLTYPE_AUTO(  _rEnd_Helper<RG>::calc( Forward<RG>(rg) )  )

	template<class RG>
	static auto crEnd(RG const& rg)-> SGM_DECLTYPE_AUTO(  rEnd(rg)  )

}


#ifndef _SGM_DETAIL_XXX_OR
	#define _SGM_DETAIL_XXX_OR(XXX)	\
		template<class ITR, class OTHER>		\
		struct XXX##_or	\
		{	\
		private:	\
			template<class Q>	\
			static auto _calc(int)-> SFINAE_t<typename Q::XXX, typename Q::XXX>;	\
			\
			template<class>	\
			static auto _calc(...)-> OTHER;	\
			\
		public:	\
			using type = decltype( _calc<ITR>(0) );	\
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


template<class ITR>
class sgm::Reverse_iterator
{
private: 
	using _itr_t = Reverse_iterator;
	using _deref_t = decltype(*Declval<ITR>());

public:
	using value_type = _detail::value_type_or_t< ITR, Decay_t<_deref_t> >;
	using difference_type = _detail::difference_type_or_t<ITR, ptrdiff_t>;
	using pointer = _detail::pointer_or_t< ITR, Referenceless_t<_deref_t>* >;
	using reference = _detail::reference_or_t< ITR, Referenceless_t<_deref_t>& >;


	Reverse_iterator(ITR const itr) : _itr(itr){}

	auto operator[](difference_type const diff) const-> _deref_t{  return _itr[diff];  }
	auto operator*() const-> _deref_t{  return (*this)[0];  }
	
	auto operator->() const-> pointer const{  return &**this;  }
	auto operator->()-> pointer{  return &**this;  }

	auto operator+(difference_type const diff) const-> _itr_t{  return {_itr - diff};  }
	auto operator-(difference_type const diff) const-> _itr_t{  return {_itr + diff};  }
	auto operator-(_itr_t const itr) const-> difference_type{  return itr._itr - _itr;  }

	auto operator+=(difference_type const diff)-> _itr_t&
	{
		_itr += -diff;

		return *this;
	}

	auto operator-=(difference_type const diff)-> _itr_t&{  return *this += -diff;  }

	auto operator++()-> _itr_t&{  return *this += 1;  }
	auto operator--()-> _itr_t&{  return *this -= 1;  }

	auto operator++(int)-> _itr_t
	{
		auto const itr = *this;

		_itr--;

		return itr;
	}

	auto operator--(int)-> _itr_t
	{
		auto const itr = *this;

		_itr++;

		return itr;
	}

	auto operator==(_itr_t const itr) const-> bool{  return _itr == itr._itr;  }
	auto operator!=(_itr_t const itr) const-> bool{  return !(*this == itr);  }

	auto operator<(_itr_t const itr) const-> bool{  return _itr > itr._itr;  }
	auto operator>(_itr_t const itr) const-> bool{  return _itr < itr._itr;  }

	auto operator<=(_itr_t const itr) const-> bool{  return !(*this > itr);  }
	auto operator>=(_itr_t const itr) const-> bool{  return !(*this < itr);  }


private:
	template<class _ITR>
	friend auto sgm::Reversing(Reverse_iterator<_ITR> const ritr)-> _ITR;


	ITR _itr;
};
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	
	template<class ITR>
	class Move_iterator : public ITR
	{
	public:
		template<class...ARGS>
		Move_iterator(ARGS&&...args) : ITR( Forward<ARGS>(args)... ){}

		auto base() const noexcept-> ITR{  return *this;  }

		auto operator*() const noexcept-> SGM_DECLTYPE_AUTO(  Move(*base())  )

		auto operator[](ptrdiff_t const diff) const noexcept
		->	SGM_DECLTYPE_AUTO(  Move(base()[diff])  )
	};


	SGM_USER_DEFINED_TYPE_CHECK
	(	class ITR
	,	Move_iterator, <ITR>
	);

	
	template
	<	class ITR
	,	class = Enable_if_t< is_iterator<ITR>::value && !is_Move_iterator<ITR>::value >
	>
	static auto To_Move_iterator(ITR&& itr) noexcept
	->	Move_iterator<ITR>{  return Forward<ITR>(itr);  }

	template<class ITR>
	static auto To_Move_iterator(Move_iterator<ITR> itr) noexcept
	->	Move_iterator<ITR>{  return itr;  }


	template<class RG>
	static auto mBegin(RG&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(   To_Move_iterator(  Begin( Forward<RG>(rg) )  )   )

	template<class RG>
	static auto mEnd(RG&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(   To_Move_iterator(  End( Forward<RG>(rg) )  )   )

	template<class RG>
	static auto mrBegin(RG&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(   To_Move_iterator(  rBegin( Forward<RG>(rg) )  )   )

	template<class RG>
	static auto mrEnd(RG&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(   To_Move_iterator(  rEnd( Forward<RG>(rg) )  )   )

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	
	template<class RG, class T = void>
	struct is_iterable;


	template
	<	class RG1, class RG2
	,	class 
		=	Guaranteed_t
			<	Boolean_And
				<	is_iterable<RG1>, is_iterable<RG2>
				,	is_Same
					<	Decay_t< decltype( *Begin(Declval<RG1>()) ) >
					,	Decay_t< decltype( *Begin(Declval<RG2>()) ) >
					>
				>::	value
			>
	>
	static auto iterable_cast(RG2&& rg)-> RG1{  return RG1( Begin(rg), End(rg) );  }

}


template<class RG, class T>
struct sgm::is_iterable : Unconstructible
{
private:
	template<class Q>  /* Declaration Only */
	static auto _begin(int)-> SFINAE_t< decltype( static_cast<T>(*Declval<Q>().begin()) ) >;

	template<class...>  /* Declaration Only */
	static auto _begin(...)-> False_t;

	template<class Q>  /* Declaration Only */
	static auto _end(int)-> SFINAE_t< decltype( static_cast<T>(*Declval<Q>().end()) ) >;

	template<class...>  /* Declaration Only */
	static auto _end(...)-> False_t;

public:
	static bool constexpr value
	=	decltype( _begin<RG>(0) )::value && decltype( _end<RG>(0) )::value;

	using type = Boolean<value>;
};
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
		static auto _next(ITR itr, ptrdiff_t steps)-> ITR
		{
			while(steps-->0)
				itr++;

			return itr;
		}
	};


	template<class ITR>
	struct _Travel<ITR, true, false> : _Travel<ITR, false, false>
	{
		static auto _prev(ITR itr, ptrdiff_t steps)-> ITR
		{
			while(steps-->0)
				itr--;

			return itr;
		}
	};


	template<class ITR, bool IS_BIDIRECTIONAL>
	struct _Travel<ITR, IS_BIDIRECTIONAL, true>
	{
		static auto _next(ITR const itr, ptrdiff_t const steps)-> ITR{  return itr + steps;  }
		static auto _prev(ITR const itr, ptrdiff_t const steps)-> ITR{  return itr - steps;  }
	};


	template<  class ITR, class = Guaranteed_t< is_iterator<ITR>::value >  >
	static auto Next(ITR const itr, ptrdiff_t const steps = 1)
	->	ITR{  return _Travel<ITR>::_next(itr, steps);  }


	template<  class ITR, class = Guaranteed_t< is_bidirectional_iterator<ITR>::value >  >
	static auto Prev(ITR const itr, ptrdiff_t const steps = 1)
	->	ITR{  return _Travel<ITR>::_prev(itr, steps);  }


	struct _Difference_Helper : Unconstructible
	{
		template<class ITR>
		static auto calc(ITR bi, ITR const ei)
		->	Enable_if_t< is_random_access_iterator<ITR>::value, ptrdiff_t >
		{
			return static_cast<ptrdiff_t>(ei - bi);
		}

		template<class ITR>
		static auto calc(ITR bi, ITR const ei)
		->	Enable_if_t< !is_random_access_iterator<ITR>::value, ptrdiff_t >
		{
			ptrdiff_t diff = 0;

			for(;  bi != ei;  bi++,  ++diff);

			return diff;
		}
	};

	template<  class ITR, class = Guaranteed_t< is_iterator<ITR>::value >  >
	static auto constexpr Difference(ITR const bi, ITR const ei)
	->	ptrdiff_t{  return _Difference_Helper::calc(bi, ei);  }


	struct _iterable_Size_Helper : Unconstructible
	{
	private:
		SGM_HAS_MEMFUNC(size);

	public:
		template<class RG>
		static auto calc(RG&& rg)-> Enable_if_t< Has_MemFunc_size<RG>::value, size_t >
		{
			return static_cast<size_t>(rg.size());
		}

		template<class RG>
		static auto calc(RG&& rg)-> Enable_if_t< !Has_MemFunc_size<RG>::value, size_t >
		{
			return static_cast<size_t>(  Difference( Begin(rg), End(rg) )  );	
		}
	};

	template<class RG>
	static auto constexpr Size(RG&& rg)
	->	size_t{  return _iterable_Size_Helper::calc( Forward<RG>(rg) );  }

}
//========//========//========//========//=======#//========//========//========//========//=======#


#endif // end of #ifndef _SGM_ITERABLE_