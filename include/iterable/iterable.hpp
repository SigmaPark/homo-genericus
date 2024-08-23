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
		static auto calc(_RG&& rg) noexcept-> SGM_DECLTYPE_AUTO(  rg.begin()  )
	};

	template<class RG>
	struct _Begin_Helper<RG, 2> : Unconstructible
	{
		template<class A>  
		static auto calc(A&& arr) noexcept-> decltype( Address_of(arr[0]) ){  return arr;  }
	};

	template<class RG>
	static auto Begin(RG&& rg) noexcept-> SGM_DECLTYPE_AUTO
	(
		_Begin_Helper<RG>::calc( Forward<RG>(rg) )  
	)

	template<class RG>
	static auto cBegin(RG const& rg) noexcept-> SGM_DECLTYPE_AUTO(  Begin(rg)  )


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
		static auto calc(_RG&& rg) noexcept-> SGM_DECLTYPE_AUTO(  rg.end()  )
	};


	template<class>
	struct __Static_Array_Size;

	template<class T, size_t N>
	struct __Static_Array_Size<T[N]>{  static size_t constexpr value = N;  };


	template<class RG>
	struct _End_Helper<RG, 2> : Unconstructible
	{
		template<class A>
		static auto calc(A&& arr) noexcept-> decltype( Address_of(arr[0]) )
		{
			return arr + __Static_Array_Size< Decay_t<A> >::value;
		}
	};

	template<class RG>
	static auto End(RG&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  _End_Helper<RG>::calc( Forward<RG>(rg) )  )

	template<class RG>
	static auto cEnd(RG const& rg) noexcept-> SGM_DECLTYPE_AUTO(  End(rg)  )

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
	static auto Reversing(ITR const itr) noexcept-> Reverse_iterator<ITR>{  return {itr};  }

	template<class ITR>
	static auto Reversing(Reverse_iterator<ITR> const ritr) noexcept
	->	ITR{  return ritr.base();  }


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
		static auto calc(_RG&& rg) noexcept-> SGM_DECLTYPE_AUTO(  rg.rbegin()  )
	};

	template<class RG>
	struct _rBegin_Helper<RG, 2> : Unconstructible
	{
		template<class A>
		static auto calc(A&& arr) noexcept-> SGM_DECLTYPE_AUTO(  Reversing( End(arr) - 1 )  )
	};

	template<class RG>
	static auto rBegin(RG&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  _rBegin_Helper<RG>::calc( Forward<RG>(rg) )  )

	template<class RG>
	static auto crBegin(RG const& rg) noexcept-> SGM_DECLTYPE_AUTO(  rBegin(rg)  )


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
		static auto calc(_RG&& rg) noexcept-> SGM_DECLTYPE_AUTO(  rg.rend()  ) 
	};

	template<class RG>
	struct _rEnd_Helper<RG, 2> : Unconstructible
	{
		template<class A>
		static auto calc(A&& arr) noexcept
		->	SGM_DECLTYPE_AUTO(  Reversing( Begin(arr) - 1 )  )
	};

	template<class RG>
	static auto rEnd(RG&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  _rEnd_Helper<RG>::calc( Forward<RG>(rg) )  )

	template<class RG>
	static auto crEnd(RG const& rg) noexcept-> SGM_DECLTYPE_AUTO(  rEnd(rg)  )

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


public:
	using value_type = _detail::value_type_or_t< ITR, Decay_t<_deref_t> >;
	using difference_type = _detail::difference_type_or_t<ITR, ptrdiff_t>;
	using pointer = _detail::pointer_or_t< ITR, Referenceless_t<_deref_t>* >;
	using reference = _detail::reference_or_t< ITR, Referenceless_t<_deref_t>& >;	


	Reverse_iterator(ITR const itr) noexcept : _itr(itr){}	

	auto base() const noexcept-> ITR const&{  return _itr;  }
	auto base() noexcept-> ITR&{  return _itr;  }

	auto operator*() const noexcept-> SGM_DECLTYPE_AUTO(  *base()  )
	auto operator->() const noexcept-> pointer{  return Address_of(**this);  }	

	auto operator==(_itr_t const itr) const noexcept
	->	SGM_DECLTYPE_AUTO(  base() == itr.base()  )

	auto operator!=(_itr_t const itr) const noexcept-> bool{  return !(*this == itr);  }


	auto operator--() noexcept-> _itr_t&{  return ++base(),  *this;  }

	auto operator--(int) noexcept-> _itr_t
	{
		auto const res = *this;

		--*this;

		return res;
	}


private:
	ITR _itr;
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


	auto operator++() noexcept-> _itr_t&{  return --_1st_t::base(),  *this;  }

	auto operator++(int) noexcept
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


public:
	using typename _1st_t::difference_type;

	using _2nd_t::_2nd_t;

	using _2nd_t::operator++;
	using _1st_t::operator--;

	using _1st_t::base;

	auto operator[](difference_type const diff) const noexcept
	->	decltype(*Mock<ITR>()){  return *(*this + diff);  }

	auto operator+(difference_type const diff) const noexcept
	->	_itr_t{  return {base() - diff};  }
	
	auto operator-(difference_type const diff) const noexcept
	->	_itr_t{  return {base() + diff};  }
	
	auto operator-(_itr_t const itr) const noexcept
	->	SGM_DECLTYPE_AUTO(  itr.base() - base()  )
	

	auto operator+=(difference_type const diff) noexcept
	->	_itr_t&{  return base() -= diff,  *this;  }

	auto operator-=(difference_type const diff) noexcept
	->	_itr_t&{  return base() += diff,  *this;  }


	auto operator<(_itr_t const itr) const noexcept
	->	SGM_DECLTYPE_AUTO(  base() > itr.base()  )
	
	auto operator>(_itr_t const itr) const noexcept
	->	SGM_DECLTYPE_AUTO(  base() < itr.base()  )

	auto operator<=(_itr_t const itr) const noexcept
	->	bool{  return !(*this > itr);  }

	auto operator>=(_itr_t const itr) const noexcept
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


	Move_iterator(ITR const itr) noexcept : _itr(itr){}

	auto base() const noexcept-> ITR const&{  return _itr;  }
	auto base() noexcept-> ITR&{  return _itr;  }

	auto operator*() const noexcept-> value_type{  return Move(*base());  }
	auto operator->() const noexcept-> pointer{  return Address_of(*base());  }

	auto operator==(_itr_t const itr) const noexcept-> bool{  return base() == itr.base();  }
	auto operator!=(_itr_t const itr) const noexcept-> bool{  return !(*this == itr);  }


	auto operator++() noexcept-> _itr_t&{  return ++base(),  *this;  }

	auto operator++(int) noexcept-> _itr_t
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


	auto operator--() noexcept-> _itr_t&{  return --_1st_t::base(),  *this;  }

	auto operator--(int) noexcept-> _itr_t
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

public:
	using _2nd_t::_2nd_t;

	using _1st_t::operator++;
	using _2nd_t::operator--;


	auto operator+(ptrdiff_t const diff) const noexcept
	->	_itr_t{  return {_1st_t::base() + diff};  }
	
	auto operator-(ptrdiff_t const diff) const noexcept
	->	_itr_t{  return {_1st_t::base() - diff};  }


	auto operator-(_itr_t const itr) const noexcept
	->	ptrdiff_t{  return _1st_t::base() - itr.base();  }


	auto operator+=(ptrdiff_t const diff) noexcept
	->	_itr_t&{  return _1st_t::base() += diff,  *this;  }
	
	auto operator-=(ptrdiff_t const diff) noexcept
	->	_itr_t&{  return _1st_t::base() -= diff,  *this;  }


	auto operator[](ptrdiff_t const diff) const noexcept
	->	typename _1st_t::value_type{  return Move(_1st_t::base()[diff]);  }
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
	static auto To_Move_iterator(ITR itr) noexcept-> Move_iterator<ITR>{  return itr;  }

	template<class ITR>
	static auto To_Move_iterator(Move_iterator<ITR> itr) noexcept
	->	Move_iterator<ITR>{  return itr;  }


	template<class RG>
	static auto mBegin(RG&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  To_Move_iterator( Begin(rg) )  )

	template<class RG>
	static auto mEnd(RG&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  To_Move_iterator( End(rg) )  )

	template<class RG>
	static auto mrBegin(RG&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  To_Move_iterator( rBegin(rg) )  )

	template<class RG>
	static auto mrEnd(RG&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  To_Move_iterator( rEnd(rg) )  )


	template<  class RG, class ITR = Decay_t< decltype( Begin(Mock<RG>()) ) >  >
	static auto fBegin(Referenceless_t<RG>& rg) noexcept
	->	Selective_t< is_Rvalue_Reference<RG&&>::value, Move_iterator<ITR>, ITR >  
	{
		return Begin(rg);
	}
	
	template<class RG>
	static auto fBegin(Referenceless_t<RG>&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  mBegin( Move(rg) )  )

	template<  class RG, class ITR = Decay_t< decltype( End(Mock<RG>()) ) >  >
	static auto fEnd(Referenceless_t<RG>& rg) noexcept
	->	Selective_t< is_Rvalue_Reference<RG&&>::value, Move_iterator<ITR>, ITR >  
	{
		return End(rg);
	}
	
	template<class RG>
	static auto fEnd(Referenceless_t<RG>&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  mEnd( Move(rg) )  )

	template<  class RG, class ITR = Decay_t< decltype( rBegin(Mock<RG>()) ) >  >
	static auto frBegin(Referenceless_t<RG>& rg) noexcept
	->	Selective_t< is_Rvalue_Reference<RG&&>::value, Move_iterator<ITR>, ITR >  
	{
		return rBegin(rg);
	}
	
	template<class RG>
	static auto frBegin(Referenceless_t<RG>&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  mrBegin( Move(rg) )  )

	template<  class RG, class ITR = Decay_t< decltype( rEnd(Mock<RG>()) ) >  >
	static auto frEnd(Referenceless_t<RG>& rg) noexcept
	->	Selective_t< is_Rvalue_Reference<RG&&>::value, Move_iterator<ITR>, ITR >  
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


	template
	<	class RG1, class RG2
	,	class 
		=	Guaranteed_t
			<	Boolean_And
				<	is_iterable<RG1>, is_iterable<RG2>
				,	is_Same
					<	Decay_t< decltype( *Begin(Mock<RG1>()) ) >
					,	Decay_t< decltype( *Begin(Mock<RG2>()) ) >
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
	static auto _begin(int)-> SFINAE_t< decltype( static_cast<T>(*Mock<Q>().begin()) ) >;

	template<class Q>  /* Declaration Only */
	static auto _begin(...)-> is_Bounded_Array< Referenceless_t<Q> >;

	template<class Q>  /* Declaration Only */
	static auto _end(int)-> SFINAE_t< decltype( static_cast<T>(*Mock<Q>().end()) ) >;

	template<class Q>  /* Declaration Only */
	static auto _end(...)-> is_Bounded_Array< Referenceless_t<Q> >;

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
		static auto _next(ITR itr, ptrdiff_t steps) noexcept-> ITR
		{
			while(steps-->0)
				itr++;

			return itr;
		}
	};


	template<class ITR>
	struct _Travel<ITR, true, false> : _Travel<ITR, false, false>
	{
		static auto _prev(ITR itr, ptrdiff_t steps) noexcept-> ITR
		{
			while(steps-->0)
				itr--;

			return itr;
		}
	};


	template<class ITR, bool IS_BIDIRECTIONAL>
	struct _Travel<ITR, IS_BIDIRECTIONAL, true>
	{
		static auto _next(ITR const itr, ptrdiff_t const steps) noexcept
		->	SGM_DECLTYPE_AUTO(  itr + steps  )

		static auto _prev(ITR const itr, ptrdiff_t const steps) noexcept
		->	SGM_DECLTYPE_AUTO(  itr - steps  )
	};


	template<  class ITR, class = Guaranteed_t< is_iterator<ITR>::value >  >
	static auto Next(ITR const itr, ptrdiff_t const steps = 1) noexcept
	->	SGM_DECLTYPE_AUTO(  _Travel<ITR>::_next(itr, steps)  )


	template<  class ITR, class = Guaranteed_t< is_bidirectional_iterator<ITR>::value >  >
	static auto Prev(ITR const itr, ptrdiff_t const steps = 1) noexcept
	->	SGM_DECLTYPE_AUTO(  _Travel<ITR>::_prev(itr, steps)  )


	struct _Difference_Helper : Unconstructible
	{
		template<class ITR>
		static auto calc(ITR bi, ITR const ei) noexcept
		->	Enable_if_t< is_random_access_iterator<ITR>::value, ptrdiff_t >
		{
			return static_cast<ptrdiff_t>(ei - bi);
		}

		template<class ITR>
		static auto calc(ITR bi, ITR const ei) noexcept
		->	Enable_if_t< !is_random_access_iterator<ITR>::value, ptrdiff_t >
		{
			ptrdiff_t diff = 0;

			for(;  bi != ei;  bi++,  ++diff);

			return diff;
		}
	};

	template<  class ITR, class = Guaranteed_t< is_iterator<ITR>::value >  >
	static auto constexpr Difference(ITR const bi, ITR const ei) noexcept
	->	SGM_DECLTYPE_AUTO(  _Difference_Helper::calc(bi, ei)  )


	struct _iterable_Size_Helper : Unconstructible
	{
	private:
		SGM_HAS_MEMFUNC(size);

	public:
		template<class RG>
		static auto calc(RG&& rg) noexcept
		->	Enable_if_t< Has_MemFunc_size<RG>::value, size_t >
		{
			return static_cast<size_t>(rg.size());
		}

		template<class RG>
		static auto calc(RG&& rg) noexcept
		->	Enable_if_t< !Has_MemFunc_size<RG>::value, size_t >
		{
			return static_cast<size_t>(  Difference( Begin(rg), End(rg) )  );	
		}
	};

	template<class RG>
	static auto constexpr Size(RG&& rg) noexcept
	->	SGM_DECLTYPE_AUTO(  _iterable_Size_Helper::calc( Forward<RG>(rg) )  )

}
//========//========//========//========//=======#//========//========//========//========//=======#


#endif // end of #ifndef _SGM_ITERABLE_