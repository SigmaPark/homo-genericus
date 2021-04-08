#pragma once

#ifndef _SGM_INTERFACE_TRAITS_
#define _SGM_INTERFACE_TRAITS_

#include "..\Type_Analysis\Type_Analysis.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{


#ifndef SGM_INTERFACE_CHECK
	#define SGM_INTERFACE_CHECK(TEMPLATE_ARGS, NAME, ...)	\
		template<class _CLASS, TEMPLATE_ARGS>	\
		struct NAME	\
		{	\
		private:	\
			template<class _QCLASS>	 /* Declaration Only */		\
			static auto _calc(int)-> SFINAE_t< decltype(__VA_ARGS__) >;\
			\
			template<class> /* Declaration Only */ static auto _calc(...)-> False_t;	\
		\
		public:	\
			NAME() = delete;	\
		\
			static bool constexpr value = decltype( _calc< Decay_t<_CLASS> >(0) )::value;	\
			using type = Boolean_type<value>;	\
		}

#else
	#error SGM_INTERFACE_CHECK was already defined somewhere else.
#endif


	SGM_INTERFACE_CHECK
	(	class _INDEX_TYPE = int
	,	Has_Operator_index, sgm::Declval<_QCLASS>()[sgm::Declval<_INDEX_TYPE>()]
	);
	//========//========//========//========//=======#//========//========//========//========//===

	
#ifndef _BINARY_OPERATOR_INTERFACE
	#define _BINARY_OPERATOR_INTERFACE(NAME, OP) \
		SGM_INTERFACE_CHECK	\
		(	class _RIGHT_HAND_SIDE = _CLASS, Has_Operator_##NAME	\
		,	sgm::Declval<_QCLASS>() OP sgm::Declval<_RIGHT_HAND_SIDE>()	\
		)


	_BINARY_OPERATOR_INTERFACE(Same, ==);
	_BINARY_OPERATOR_INTERFACE(NotSame, !=);
	_BINARY_OPERATOR_INTERFACE(Less, <);
	_BINARY_OPERATOR_INTERFACE(NotGreater, <=);
	_BINARY_OPERATOR_INTERFACE(Greater, >);
	_BINARY_OPERATOR_INTERFACE(NotLess, >=);

	_BINARY_OPERATOR_INTERFACE(Plus, +);
	_BINARY_OPERATOR_INTERFACE(Minus, -);
	_BINARY_OPERATOR_INTERFACE(Multiply, *);
	_BINARY_OPERATOR_INTERFACE(Divide, /);
	_BINARY_OPERATOR_INTERFACE(Mod, %);

	_BINARY_OPERATOR_INTERFACE(And, &&);
	_BINARY_OPERATOR_INTERFACE(Or, ||);
	_BINARY_OPERATOR_INTERFACE(BitAnd, &);
	_BINARY_OPERATOR_INTERFACE(BitOr, |);
	_BINARY_OPERATOR_INTERFACE(Xor, ^);

	_BINARY_OPERATOR_INTERFACE(LShift, <<);
	_BINARY_OPERATOR_INTERFACE(RShift, >>);

	_BINARY_OPERATOR_INTERFACE(PlusAlloc, +=);
	_BINARY_OPERATOR_INTERFACE(MinusAlloc, -=);
	_BINARY_OPERATOR_INTERFACE(MultiplyAlloc, *=);
	_BINARY_OPERATOR_INTERFACE(DivideAlloc, /=);
	_BINARY_OPERATOR_INTERFACE(ModAlloc, %=);
	_BINARY_OPERATOR_INTERFACE(BitAndAlloc, &=);
	_BINARY_OPERATOR_INTERFACE(BitOrAlloc, |=);
	_BINARY_OPERATOR_INTERFACE(XorAlloc, ^=);
	_BINARY_OPERATOR_INTERFACE(LShiftAlloc, <<=);
	_BINARY_OPERATOR_INTERFACE(RShiftAlloc, >>=);

	_BINARY_OPERATOR_INTERFACE(Ptr2Mem, ->*);

	
	#undef _BINARY_OPERATOR_INTERFACE
#else
	#error _BINARY_OPERATOR_INTERFACE was already defined somewhere else.
#endif

	template<class T1, class T2 = T1, class = void>
	struct Has_Operator_Comma : False_t{};

	template<class T1, class T2>
	struct Has_Operator_Comma
	<	T1, T2, Void_t< decltype( Declval<T1>().operator,(Declval<T2>()) ) >  
	>
	:	True_t
	{};
	//========//========//========//========//=======#//========//========//========//========//===


#ifndef _UNARY_OPERATOR_INTERFACE
	#define _UNARY_OPERATOR_INTERFACE(PRE, NAME, POST)	\
		SGM_INTERFACE_CHECK(class..., Has_Operator_##NAME, PRE sgm::Declval<_QCLASS>() POST)


	_UNARY_OPERATOR_INTERFACE(+, Posit,/**/);
	_UNARY_OPERATOR_INTERFACE(-, Negate, /**/);
	
	_UNARY_OPERATOR_INTERFACE(++, Pre_increase, /**/);
	_UNARY_OPERATOR_INTERFACE(--, Pre_Decrease, /**/);

	_UNARY_OPERATOR_INTERFACE(/**/, Post_increase, ++);
	_UNARY_OPERATOR_INTERFACE(/**/, Post_Decrease, --);

	_UNARY_OPERATOR_INTERFACE(~, BitNot, /**/);
	_UNARY_OPERATOR_INTERFACE(!, Not, /**/);

	_UNARY_OPERATOR_INTERFACE(*, Deref, /**/);
	_UNARY_OPERATOR_INTERFACE(&, Ref, /**/);

	
	#undef _UNARY_OPERATOR_INTERFACE
#else
	#error _UNARY_OPERATOR_INTERFACE was already defined somewhere else.
#endif
	//========//========//========//========//=======#//========//========//========//========//===

	
#ifndef SGM_HAS_MEMFUNC
	#define SGM_HAS_MEMFUNC(MEMFUNC)	\
		SGM_INTERFACE_CHECK	\
		(	class..._ARGUMENTS, Has_MemFunc_##MEMFUNC		\
		,	sgm::Declval<_QCLASS>().MEMFUNC(sgm::Declval<_ARGUMENTS>()...) \
		)

#else
	#error SGM_HAS_MEMFUNC was already defined somewhere else.
#endif


#ifndef SGM_HAS_MEMBER
	/**	Not only member data but also 
	*	1)	static member function 
	*	2)	static member data
	*	3)	unscoped enumerator
	*	can be substituted to MEMBER.
	*/
	#define SGM_HAS_MEMBER(MEMBER)	\
		SGM_INTERFACE_CHECK(class..., Has_Member_##MEMBER, sgm::Declval<_QCLASS>().MEMBER)
	
#else
	#error SGM_HAS_MEMBER was already defined somewhere else.
#endif


#ifndef SGM_HAS_NESTED_TYPE
	#define SGM_HAS_NESTED_TYPE(TYPE)	\
		SGM_INTERFACE_CHECK	\
		(	class..., Has_NestedType_##TYPE, sgm::Declval<typename _QCLASS::TYPE>()		\
		)

#else
	#error SGM_HAS_NESTED_TYPE was already defined somewhere else.
#endif
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T> 
	struct is_Comparable 
	:	Boolean_type< Has_Operator_Same<T>::value && Has_Operator_NotSame<T>::value >
	{};


	template<class T> 
	struct is_Ordered 
	:	Boolean_type
		<	Has_Operator_Less<T>::value && Has_Operator_Greater<T>::value 
		&&	Has_Operator_NotLess<T>::value && Has_Operator_NotGreater<T>::value
		>
	{};


	template<class T> 
	struct is_Logical 
	:	Boolean_type
		<	Has_Operator_And<T>::value && Has_Operator_Or<T>::value && Has_Operator_Not<T>::value
		>
	{};


	template<class T>
	struct is_Digital
	:	Boolean_type
		<	Has_Operator_BitAnd<T>::value && Has_Operator_BitOr<T>::value
		&&	Has_Operator_BitNot<T>::value && Has_Operator_Xor<T>::value
		&&	Has_Operator_LShift<T>::value && Has_Operator_RShift<T>::value
		>
	{};


	template<class T> 
	struct is_Algebraic 
	:	Boolean_type
		<	Has_Operator_Plus<T>::value && Has_Operator_Minus<T>::value
		&&	Has_Operator_Multiply<T>::value && Has_Operator_Divide<T>::value
		>
	{};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template< class T, bool = is_Comparable<T>::value > struct Comparable;
	template< class T, bool = is_Ordered<T>::value > struct Ordered;
	template< class T, bool = is_Logical<T>::value > struct Logical;
	template< class T, bool = is_Digital<T>::value > struct Digital;
	template< class T, bool = is_Algebraic<T>::value > struct Algebraic;


#if !defined(_SGM_OPERATION_DECORATOR0) || !defined(_SGM_OPERATOR)
	#define _SGM_OPERATOR(DECO_NAME, _OP)	\
		template<class Q>	\
		auto operator _OP(Q const& q) const	\
		->	decltype(Declval<T>() _OP q){  return *_p _OP q;  }	\
		\
		auto operator _OP(DECO_NAME const& rhs) const	\
		->	decltype(Declval<T>() _OP Declval<T>()){  return *_p _OP *rhs._p;  }
	
	#define _SGM_OPERATION_DECORATOR(DECO_NAME, ...)	\
		template<class T>	\
		struct DECO_NAME<T, true>	\
		{	\
			DECO_NAME(T const* p = nullptr) : _p(p){}	\
		\
			auto operator=(T const* p)-> T const*{  return _p = p;  }	\
		\
			__VA_ARGS__ \
		\
		private:	\
			T const* _p;	\
		};	\
		\
		template<class T> struct DECO_NAME<T, false>	\
		{	\
			DECO_NAME(T const* = nullptr){}	\
		\
			auto operator=(T const* p)-> T const*{  return p;  }	\
		}


	_SGM_OPERATION_DECORATOR
	(	Comparable
	,	_SGM_OPERATOR(Comparable, ==)  _SGM_OPERATOR(Comparable, !=)
	);

	_SGM_OPERATION_DECORATOR
	(	Ordered
	,	_SGM_OPERATOR(Ordered, <)  _SGM_OPERATOR(Ordered, >)
		_SGM_OPERATOR(Ordered, <=)  _SGM_OPERATOR(Ordered, >=)
	);

	_SGM_OPERATION_DECORATOR
	(	Logical
	,	_SGM_OPERATOR(Logical, &&)  _SGM_OPERATOR(Logical, ||)
		bool operator!() const{  return !*_p;  }
	);

	_SGM_OPERATION_DECORATOR
	(	Digital
	,	_SGM_OPERATOR(Digital, &)  _SGM_OPERATOR(Digital, |)
		_SGM_OPERATOR(Digital, ^)  _SGM_OPERATOR(Digital, <<)  _SGM_OPERATOR(Digital, >>)
		T operator~() const{  return ~*_p;  }
	);

	_SGM_OPERATION_DECORATOR
	(	Algebraic
	,	_SGM_OPERATOR(Algebraic, +)  _SGM_OPERATOR(Algebraic, -)
		_SGM_OPERATOR(Algebraic, *)  _SGM_OPERATOR(Algebraic, /)
	);
	

	#undef _SGM_OPERATION_DECORATOR
	#undef _SGM_OPERATOR

#else
	#error _SGM_OPERATION_DECORATOR or _SGM_OPERATOR were already defined.
#endif


	template<class T>
	struct Operation_Decorator : Comparable<T>, Ordered<T>, Logical<T>, Digital<T>, Algebraic<T>
	{	
		auto update_by_operator(T const* p)-> T const*
		{
			return
			(	static_cast< Comparable<T>& >(*this)
			=	static_cast< Ordered<T>& >(*this)
			=	static_cast< Logical<T>& >(*this)
			=	static_cast< Digital<T>& >(*this)
			=	static_cast< Algebraic<T>& >(*this)
			=	p
			);
		}
	};


	template<class T>
	class Operator_interface;

	SGM_USER_DEFINED_TYPE_CHECK(_, Operator_interface, class T, T);
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T> 
	struct is_iterator
	:	Boolean_type
		<	is_Pointer<T>::value
		||	(	Has_Operator_Deref<T>::value 
			&&	Has_Operator_Post_increase<T>::value
			&&	Has_Operator_NotSame<T>::value
			)
		>
	{};


	template<class T>
	struct is_random_access_iterator 
	:	Selective_t< Has_Operator_index<T>::value, is_iterator<T>, Has_Operator_index<T> >
	{};


	template<class CON, class T = void>
	struct is_iterable
	{
	private:
		template<class Q> /* Declaration Only */ 
		static auto _has_begin(int)
		->	SFINAE_t< decltype( static_cast<T>(*Declval<Q>().begin()) ) >;

		template<class> /* Declaration Only */ static auto _has_begin(...)-> False_t;


		template<class Q> /* Declaration Only */ 
		static auto _has_end(int)
		->	SFINAE_t< decltype( static_cast<T>(*Declval<Q>().end()) ) >;

		 template<class> /* Declaration Only */ static auto _has_end(...)-> False_t;


		template<class Q> /* Declaration Only */
		static auto _has_size(int)
		->	SFINAE_t< decltype( static_cast<size_t>(Declval<Q>().size()) ) >;

		template<class> /* Declaration Only */ static auto _has_size(...)-> False_t;


	public:
		static bool constexpr value
		=	(	decltype( _has_begin< Decay_t<CON> >(0) )::value
			&&	decltype( _has_end< Decay_t<CON> >(0) )::value
			&&	decltype( _has_size< Decay_t<CON> >(0) )::value
			);
	};	


	template<class T, size_t N> struct is_iterable<T[N], void> : True_t{};
	template<class T, size_t N> struct is_iterable<T[N], T> : True_t{};
	

	template
	<	class CON1, class CON2
	,	class 
		=	Guaranteed_t
			<	is_iterable<CON1>::value && is_iterable<CON2>::value
			&&	is_Same<  Decay_t< Deref_t<CON1> >, Decay_t< Deref_t<CON2> >  >::value
			>
	>
	static auto iterable_cast(CON2&& con)-> CON1{  return CON1(con.begin(), con.end());  }
	//========//========//========//========//=======#//========//========//========//========//===


	template
	<	class ITR
	,	bool = Has_Operator_Post_Decrease<ITR>::value
	,	bool = is_random_access_iterator<ITR>::value		
	>
	struct _Travel;


	template<class ITR>
	struct _Travel<ITR, false, false>
	{
		static auto next(ITR itr, size_t steps = 1)-> ITR
		{
			while(steps-->0)
				itr++;

			return itr;
		}
	};


	template<class ITR>
	struct _Travel<ITR, true, false> : _Travel<ITR, false, false>
	{
		static auto prev(ITR itr, size_t steps = 1)-> ITR
		{
			while(steps-->0)
				itr--;

			return itr;
		}
	};


	template<class ITR, bool HAS_POST_DECREASE>
	struct _Travel<ITR, HAS_POST_DECREASE, true>
	{
		static auto next(ITR itr, long long steps = 1)-> ITR{  return itr + steps;  }
		static auto prev(ITR itr, long long steps = 1)-> ITR{  return itr - steps;  }
	};


	template<  class ITR, class = Guaranteed_t< is_iterator<ITR>::value >  >
	static auto Next(ITR const itr, long long steps = 1)-> ITR
	{
		return _Travel<ITR>::next(itr, steps);  
	}

	template
	<	class ITR
	,	class = Guaranteed_t< is_iterator<ITR>::value && Has_Operator_Post_Decrease<ITR>::value >
	>
	static auto Prev(ITR const itr, long long steps = 1)-> ITR
	{
		return _Travel<ITR>::prev(itr, steps);  
	}
	//========//========//========//========//=======#//========//========//========//========//===


} // end of namespace sgm
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
class sgm::Operator_interface
{
private:
	T *_p;


public:
	using value_type = T;

	Operator_interface(T *p = nullptr) : _p(p){}

	auto operator=(T *p)-> T*{  return _p = p;  }
	
	operator T const&() const&{  return *_p;  }
	operator T&() &{  return *_p;  }
	operator T&&() &&{  return *_p;  }


#if !defined(_SGM_UNARY_OPERATOR_WITH_PARAM) && !defined(_SGM_UNARY_OPERATOR)
	#define _SGM_UNARY_OPERATOR_WITH_PARAM(SYM, NAME, PARAM_TYPE, PARAM)	\
		private:	\
			template<class, class = void> struct _Has##NAME : False_t{};	\
			\
			template<class Q> \
			struct _Has##NAME<  Q, Void_t< decltype( Declval<Q>().operator SYM(PARAM) ) >  >	\
			:	True_t	\
			{};	\
			\
			template	\
			<	class Q, bool IS_MUT_METHOD		\
			,	int	\
				=	_Has##NAME<Q>::value && IS_MUT_METHOD ? 0		\
				:	_Has##NAME<Q const>::value ? 1	\
				:	-1	\
			>	\
			struct _op##NAME##Helper;	\
			\
			template<class Q, bool M>	\
			struct _op##NAME##Helper<Q, M, 0>	\
			{	\
				static auto calc(Constless_t<Q> *p)	\
				->	SGM_DECLTYPE_AUTO( p->operator SYM(PARAM) )	\
			};	\
			\
			template<class Q, bool M>	\
			struct _op##NAME##Helper<Q, M, 1>	\
			{	\
				static auto calc(Q const *p)-> SGM_DECLTYPE_AUTO( p->operator SYM(PARAM) )\
			};	\
			\
			template<class Q, bool M>	\
			struct _op##NAME##Helper<Q, M, -1>	\
			{	\
				static auto calc(...)-> None = delete;	\
			};	\
			\
		public:	\
			template< class Q = _op##NAME##Helper<T, true> >	\
			auto operator SYM(PARAM_TYPE)-> SGM_DECLTYPE_AUTO( Q::calc(_p) )	\
			\
			template< class Q = _op##NAME##Helper<T, false> >	\
			auto operator SYM(PARAM_TYPE) const-> SGM_DECLTYPE_AUTO( Q::calc(_p) )


	_SGM_UNARY_OPERATOR_WITH_PARAM(++, Post_increase, int, 0)
	_SGM_UNARY_OPERATOR_WITH_PARAM(--, Post_Decrease, int, 0)
	

	#define _SGM_UNARY_OPERATOR(SYM, NAME) \
		_SGM_UNARY_OPERATOR_WITH_PARAM(SYM, NAME, /**/, /**/)


	_SGM_UNARY_OPERATOR(+, Posit)
	_SGM_UNARY_OPERATOR(-, Negate)
	_SGM_UNARY_OPERATOR(++, Pre_increase)
	_SGM_UNARY_OPERATOR(--, Pre_Decrease)
	_SGM_UNARY_OPERATOR(*, Deref)
	_SGM_UNARY_OPERATOR(&, Ref)
	_SGM_UNARY_OPERATOR(!, Not)
	_SGM_UNARY_OPERATOR(~, BitNot)
	_SGM_UNARY_OPERATOR(->, Arrow)

	#undef _SGM_UNARY_OPERATOR
	#undef _SGM_UNARY_OPERATOR_WITH_PARAM
#else
	#error _SGM_UNARY_OPERATOR_WITH_PARAM was already defined somewhere else.
#endif // end of #if !defined(_SGM_UNARY_OPERATOR_WITH_PARAM) && !defined(_SGM_UNARY_OPERATOR)


#ifndef _SGM_BINARY_OPERATOR
	#define _SGM_BINARY_OPERATOR(SYM, NAME)	\
	private:	\
		template<class, class, class = void> struct _Has##NAME : False_t{};	\
		\
		template<class Q, class R>	\
		struct _Has##NAME	\
		<	Q, R, Void_t< decltype( Declref<Q>().operator SYM(Declref<R>()) ) >	\
		>	\
		:	True_t	\
		{};	\
		\
		template	\
		<	class Q, class R, bool IS_MUT_METHOD		\
		,	int	\
			=	_Has##NAME<Q, R>::value && IS_MUT_METHOD ? 0	\
			:	_Has##NAME<Q const, R>::value ? 1	\
			:	-1	\
		>	\
		struct _op##NAME##Helper;	\
		\
		template<class Q, class R, bool M>	\
		struct _op##NAME##Helper<Q, R, M, 0>	\
		{	\
			static auto calc(Constless_t<Q> *p, R r)	\
			->	SGM_DECLTYPE_AUTO(  p->operator SYM( static_cast<R>(r) )  )	\
		};	\
		\
		template<class Q, class R, bool M>	\
		struct _op##NAME##Helper<Q, R, M, 1>	\
		{	\
			static auto calc(Q const *p, R r)	\
			->	SGM_DECLTYPE_AUTO(  p->operator SYM( static_cast<R>(r) )  )	\
		};	\
		\
		template<class Q, class R, bool M>	\
		struct _op##NAME##Helper<Q, R, M, -1>	\
		{	\
			static auto calc(...)-> None = delete;	\
		};	\
		\
	public:	\
		template< class R, class Q = _op##NAME##Helper<T, R&&, true> >	\
		auto operator SYM(R&& r)-> SGM_DECLTYPE_AUTO(  Q::calc( _p, Forward<R>(r) )  )	\
		\
		template< class R, class Q = _op##NAME##Helper<T, R&&, false> >	\
		auto operator SYM(R&& r) const-> SGM_DECLTYPE_AUTO(  Q::calc( _p, Forward<R>(r) )  )
	

	_SGM_BINARY_OPERATOR([], index)	
	_SGM_BINARY_OPERATOR(+, Plus)
	_SGM_BINARY_OPERATOR(-, Minus)
	_SGM_BINARY_OPERATOR(*, Multiply)
	_SGM_BINARY_OPERATOR(/, Divide)
	_SGM_BINARY_OPERATOR(%, Mod)
	_SGM_BINARY_OPERATOR(==, Equal)
	_SGM_BINARY_OPERATOR(!=, NotEqual)
	_SGM_BINARY_OPERATOR(<, Less)
	_SGM_BINARY_OPERATOR(>, Greater)
	_SGM_BINARY_OPERATOR(<=, NotGreater)
	_SGM_BINARY_OPERATOR(>=, NotLess)
	_SGM_BINARY_OPERATOR(&&, And)
	_SGM_BINARY_OPERATOR(||, Or)
	_SGM_BINARY_OPERATOR(&, BitAnd)
	_SGM_BINARY_OPERATOR(|, BitOr)
	_SGM_BINARY_OPERATOR(^, Xor)
	_SGM_BINARY_OPERATOR(<<, LShift)
	_SGM_BINARY_OPERATOR(>>, RShift)
	_SGM_BINARY_OPERATOR(->*, ArrowStar)
	_SGM_BINARY_OPERATOR(+=, PlusAlloc)
	_SGM_BINARY_OPERATOR(-=, MinusAlloc)
	_SGM_BINARY_OPERATOR(*=, MultiplyAlloc)
	_SGM_BINARY_OPERATOR(/=, DivideAlloc)
	_SGM_BINARY_OPERATOR(%=, ModAlloc)
	_SGM_BINARY_OPERATOR(&=, BitAndAlloc)
	_SGM_BINARY_OPERATOR(|=, BitOrAlloc)
	_SGM_BINARY_OPERATOR(^=, XorAlloc)
	_SGM_BINARY_OPERATOR(<<=, LShiftAlloc)
	_SGM_BINARY_OPERATOR(>>=, RShiftAlloc)

	#undef _SGM_BINARY_OPERATOR
#else
	#error _SGM_BINARY_OPERATOR was already defined somewhere else.
#endif // end of #ifndef _SGM_BINARY_OPERATOR


private:
	template<class Q, class...ARGS>	/* Declaration Only */
	static auto _hasBracket(int)
	->	SFINAE_t< decltype( Declval<Q>().operator()(Declref<ARGS>()...) ) >;

	template<class...>	/* Declaration Only */
	static auto _hasBracket(...)-> False_t;

	template<bool IS_MUT_METHOD, class Q, class...ARGS>
	struct _opBracketCase
	{
		static int constexpr value 
		=	decltype( _hasBracket<Q, ARGS...>(0) )::value && IS_MUT_METHOD ? 0
		:	decltype( _hasBracket<Q const, ARGS...>(0) )::value ? 1
		:	-1;
	};

	template<int>	struct _opBracketHelper;

	template<>
	struct _opBracketHelper<0>
	{
		template<class Q, class...ARGS>
		static auto calc(Q *p, ARGS&&...args)
		->	SGM_DECLTYPE_AUTO(  p->operator()( Forward<ARGS>(args)... )  )
	};

	template<>
	struct _opBracketHelper<1>
	{
		template<class Q, class...ARGS>
		static auto calc(Q const *p, ARGS&&...args)
		->	SGM_DECLTYPE_AUTO(  p->operator()( Forward<ARGS>(args)... )  )
	};

	template<>
	struct _opBracketHelper<-1>
	{
		template<class...>
		static auto calc(...)-> None = delete;
	};

public:
	template
	<	class...ARGS, class Q = _opBracketHelper< _opBracketCase<true, T, ARGS&&...>::value >
	>
	auto operator()(ARGS&&...args)
	->	SGM_DECLTYPE_AUTO(  Q::calc( _p, Forward<ARGS>(args)... )  )

	template
	<	class...ARGS, class Q = _opBracketHelper< _opBracketCase<false, T, ARGS&&...>::value > 
	>
	auto operator()(ARGS&&...args) const
	->	SGM_DECLTYPE_AUTO(  Q::calc( _p, Forward<ARGS>(args)... )  )

};


#endif // end of #ifndef _SGM_INTERFACE_TRAITS_