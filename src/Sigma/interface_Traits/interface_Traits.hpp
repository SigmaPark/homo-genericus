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
			static auto _calc(int)-> sgm::SFINAE_t< decltype(__VA_ARGS__) >;\
			\
			template<class> /* Declaration Only */ static auto _calc(...)-> sgm::False_t;	\
		\
		public:	\
			NAME() = delete;	\
		\
			static bool constexpr value = decltype( _calc< sgm::Decay_t<_CLASS> >(0) )::value;	\
			using type = sgm::Boolean_type<value>;	\
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
	:	True_t{};
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
	:	Boolean_type< Has_Operator_Same<T>::value && Has_Operator_NotSame<T>::value >{};


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


	template<class T>  class Operator_interface;

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
	:	Selective_t< Has_Operator_index<T>::value, is_iterator<T>, Has_Operator_index<T> >{};


	template<class T>
	struct is_bidirectional_iterator
	:	Boolean_type
		<	is_random_access_iterator<T>::value
		||	(is_iterator<T>::value && Has_Operator_Post_Decrease<T>::value)
		>
	{};
	//========//========//========//========//=======#//========//========//========//========//===


	template<class RG, class T = void>
	struct Has_begin : No_Making
	{
	private:
		template<class Q> /* Declaration Only */ 
		static auto _has_begin(int)
		->	SFINAE_t< decltype( static_cast<T>(*Declval<Q>().begin()) ) >;

		template<class...>  static auto _has_begin(...)-> False_t;

	public:
		static bool constexpr value = decltype( _has_begin< Decay_t<RG> >(0) )::value;
	};


	template<class RG, class T = void>
	struct Has_end : No_Making
	{
	private:
		template<class Q> /* Declaration Only */ 
		static auto _has_end(int)
		->	SFINAE_t< decltype( static_cast<T>(*Declval<Q>().end()) ) >;

		template<class...>  static auto _has_end(...)-> False_t;

	public:
		static bool constexpr value = decltype( _has_end< Decay_t<RG> >(0) )::value;
	};


	template
	<	class RG
	,	int 
		=	Has_begin< Referenceless_t<RG> >::value ? 1
		:	is_Array< Referenceless_t<RG> >::value ? 2
		:	/* otherwise */ 0
	>
	struct _Begin_Helper;

	template<class RG>
	struct _Begin_Helper<RG, 1> : No_Making
	{
		template<class _RG>  static auto get(_RG &&rg)-> SGM_DECLTYPE_AUTO(  rg.begin()  )
	};

	template<class RG>
	struct _Begin_Helper<RG, 2> : No_Making
	{
		template<class A>  static auto get(A &&arr)-> SGM_DECLTYPE_AUTO(  &arr[0]  )
	};

	template<class RG>
	static auto Begin(RG &&rg)-> SGM_DECLTYPE_AUTO(  _Begin_Helper<RG>::get( Forward<RG>(rg) )  )


	template
	<	class RG
	,	int 
		=	Has_end< Referenceless_t<RG> >::value ? 1
		:	is_BoundedArray< Referenceless_t<RG> >::value ? 2
		:	/* otherwise */ 0
	>
	struct _End_Helper;

	template<class RG>
	struct _End_Helper<RG, 1> : No_Making
	{
		template<class _RG>  static auto get(_RG &&rg)-> SGM_DECLTYPE_AUTO(  rg.end()  )
	};

	template<class RG>
	struct _End_Helper<RG, 2> : No_Making
	{
	private:
		template<class ARR>  struct _Size;
		template<class T, size_t S>  struct _Size<T[S]>{  static size_t constexpr value = S;  };

	public:
		template<class A>
		static auto get(A &&arr)-> SGM_DECLTYPE_AUTO(  &arr[_Size< Decay_t<A> >::value]  )
	};

	template<class RG>
	static auto End(RG &&rg)-> SGM_DECLTYPE_AUTO(  _End_Helper<RG>::get( Forward<RG>(rg) )  )
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class RG, class T = void>
	struct Has_rbegin : No_Making
	{
	private:
		template<class Q> /* Declaration Only */ 
		static auto _has_rbegin(int)
		->	SFINAE_t< decltype( static_cast<T>(*Declval<Q>().rbegin()) ) >;

		template<class...>  static auto _has_rbegin(...)-> False_t;

	public:
		static bool constexpr value = decltype( _has_rbegin< Decay_t<RG> >(0) )::value;
	};


	template<class RG, class T = void>
	struct Has_rend : No_Making
	{
	private:
		template<class Q> /* Declaration Only */ 
		static auto _has_rend(int)
		->	SFINAE_t< decltype( static_cast<T>(*Declval<Q>().rend()) ) >;

		template<class...>  static auto _has_rend(...)-> False_t;

	public:
		static bool constexpr value = decltype( _has_rend< Decay_t<RG> >(0) )::value;
	};


	template
	<	class RG
	,	int 
		=	Has_rbegin< Referenceless_t<RG> >::value ? 1
		:	is_Array< Referenceless_t<RG> >::value ? 2
		:	/* otherwise */ 0
	>
	struct _rBegin_Helper;

	template<class RG>
	struct _rBegin_Helper<RG, 1> : No_Making
	{
		template<class _RG>  static auto get(_RG &&rg)-> SGM_DECLTYPE_AUTO(  rg.rbegin()  )
	};

	template<class RG>
	struct _rBegin_Helper<RG, 2> : No_Making
	{
		template<class A>  static auto get(A &&arr)-> SGM_DECLTYPE_AUTO(  --End(arr)  )
	};

	template<class RG>
	static auto rBegin(RG &&rg)-> SGM_DECLTYPE_AUTO(  _rBegin_Helper<RG>::get( Forward<RG>(rg) )  )


	template
	<	class RG
	,	int 
		=	Has_rend< Referenceless_t<RG> >::value ? 1
		:	is_BoundedArray< Referenceless_t<RG> >::value ? 2
		:	/* otherwise */ 0
	>
	struct _rEnd_Helper;

	template<class RG>
	struct _rEnd_Helper<RG, 1> : No_Making
	{
		template<class _RG>  static auto get(_RG &&rg)-> SGM_DECLTYPE_AUTO(  rg.rend()  )
	};

	template<class RG>
	struct _rEnd_Helper<RG, 2> : No_Making
	{
		template<class A>
		static auto get(A &&arr)-> SGM_DECLTYPE_AUTO(  --Begin(arr)  )
	};

	template<class RG>
	static auto rEnd(RG &&rg)-> SGM_DECLTYPE_AUTO(  _rEnd_Helper<RG>::get( Forward<RG>(rg) )  )
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class RG, class T = void>
	struct is_iterable : Boolean_type< Has_begin<RG, T>::value && Has_end<RG, T>::value >{};
	

	template
	<	class RG1, class RG2
	,	class 
		=	Guaranteed_t
			<	is_iterable<RG1>::value && is_iterable<RG2>::value
			&&	is_Same<  Decay_t< Deref_t<RG1> >, Decay_t< Deref_t<RG2> >  >::value
			>
	>
	static auto iterable_cast(RG2 &&rg)-> RG1{  return RG1( Begin(rg), End(rg) );  }
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
	static auto Next(ITR const itr, long long steps = 1)
	->	ITR{  return _Travel<ITR>::next(itr, steps);  }


	template
	<	class ITR
	,	class = Guaranteed_t< is_iterator<ITR>::value && Has_Operator_Post_Decrease<ITR>::value >
	>
	static auto Prev(ITR const itr, long long steps = 1)
	->	ITR{  return _Travel<ITR>::prev(itr, steps);  }


	template< class ITR, bool = is_random_access_iterator<ITR>::value >
	struct _Difference;

	template<class ITR>
	struct _Difference<ITR, true> : No_Making
	{
		static auto calc(ITR bi, ITR const ei)
		->	long long{  return static_cast<long long>(ei - bi);  }
	};

	template<class ITR>
	struct _Difference<ITR, false> : No_Making
	{
		static auto calc(ITR bi, ITR const ei)-> long long
		{
			long long diff = 0;

			for(;  bi != ei;  bi++,  ++diff);

			return diff;
		}
	};

	template<  class ITR, class = Guaranteed_t< is_iterator<ITR>::value >  >
	static auto Difference(ITR const bi, ITR const ei)
	->	long long{  return _Difference<ITR>::calc(bi, ei);  }


	template<class RG>
	struct Has_size : No_Making
	{
	private:
		template<class Q>	/* Declaration Only */
		static auto _has_size(int)-> SFINAE_t< decltype(Declval<Q>().size()) >;

		template<class...> /* Declaration Only */ static auto _has_size(...)-> False_t;

	public:
		static bool constexpr value = decltype( _has_size< Decay_t<RG> >(0) )::value;
	};


	template< class RG, bool = Has_size<RG>::value >  struct _Size_Helper;

	template<class RG>
	struct _Size_Helper<RG, true> : No_Making
	{
		template<class _RG>  static auto get(_RG &&rg)-> size_t{  return rg.size();  }
	};

	template<class RG>
	struct _Size_Helper<RG, false> : No_Making
	{
		static_assert(is_iterable<RG>::value, "RG should be an iterable type.");

		template<class _RG>
		static auto get(_RG &&rg)-> size_t{  return Difference( Begin(rg), End(rg) );  }
	};

	template<class RG>
	static auto Size(RG &&rg)-> SGM_DECLTYPE_AUTO(  _Size_Helper<RG>::get(rg)  )
	//========//========//========//========//=======#//========//========//========//========//===


} // end of namespace sgm
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
class sgm::Operator_interface
{
protected:
	T *_p;


public:
	using value_type = T;

	Operator_interface(T *p = nullptr) : _p(p){}

	auto operator=(T *p)-> T*{  return _p = p;  }
	
	operator T const&() const{  return *_p;  }
	operator T&(){  return *_p;  }


#if	!defined(_SGM_OPERATOR_HELPER) && !defined(_SGM_GENERIC_OPERATOR) &&	\
	!defined(_SGM_UNARY_OPERATOR) && !defined(_SGM_PARAMETRIC_OPERATOR)

	#define _SGM_OPERATOR_HELPER(SYM, NAME)	\
		private:	\
			template<class Q, class...ARGS>	/* Declaration Only */	\
			static auto _has##NAME(int)	\
			->	SFINAE_t< decltype( Declval<Q>().operator SYM(Declval<ARGS>()...) ) >;	\
			\
			template<class...> /* Declaration Only */	\
			static auto _has##NAME(...)-> False_t;	\
			\
			template<bool IS_MUT_METHOD, class Q, class...ARGS>	\
			struct _op##NAME##Case		\
			{	\
				static int constexpr value	\
				=	decltype( _has##NAME<Q, ARGS...>(0) )::value && IS_MUT_METHOD ? 1	\
				:	decltype( _has##NAME<Q const, ARGS...>(0) )::value ? 0	\
				:	-1;	\
			};	\
			\
			template<int> struct _op##NAME##Helper;	\
			\
			template<>	\
			struct _op##NAME##Helper<1>	\
			{	\
				template<class Q, class...ARGS>	\
				static auto calc(Q *p, ARGS&&...args)	\
				->	SGM_DECLTYPE_AUTO(  p->operator SYM( Forward<ARGS>(args)... )  )	\
			};	\
			\
			template<>	\
			struct _op##NAME##Helper<0>	\
			{	\
				template<class Q, class...ARGS>	\
				static auto calc(Q const *p, ARGS&&...args)	\
				->	SGM_DECLTYPE_AUTO(  p->operator SYM( Forward<ARGS>(args)... )  )	\
			};	\
			\
			template<>	\
			struct _op##NAME##Helper<-1>	\
			{	\
				template<class...> static void calc(...) = delete;	\
			};	\
			\
		public:


	#define _SGM_GENERIC_OPERATOR(ARG1, ARG2, SYM, NAME, PARAM1, PARAM2)	\
		template	\
		<	ARG1  class Q = _op##NAME##Helper< _op##NAME##Case<true, T  ARG2>::value >	\
		>	\
		auto operator SYM(PARAM1)-> SGM_DECLTYPE_AUTO(  Q::calc(_p  PARAM2)  )	\
		\
		template	\
		<	ARG1  class Q = _op##NAME##Helper< _op##NAME##Case<false, T  ARG2>::value >	\
		>	\
		auto operator SYM(PARAM1) const-> SGM_DECLTYPE_AUTO(  Q::calc(_p  PARAM2)  )


	#define _SGM_PARAMETRIC_OPERATOR(SYM, NAME)	\
		_SGM_OPERATOR_HELPER(SYM, NAME)	\
		_SGM_GENERIC_OPERATOR	\
		(	SGM_MACROPACK(class...ARGS,), SGM_MACROPACK(, ARGS&&...)	\
		,	SYM, NAME	\
		,	ARGS&&...args, SGM_MACROPACK(, Forward<ARGS>(args)...)	\
		)

	#define _SGM_UNARY_OPERATOR(SYM, NAME)	\
		_SGM_OPERATOR_HELPER(SYM, NAME)	\
		_SGM_GENERIC_OPERATOR(/**/, /**/, SYM, NAME, /**/, /**/)


	_SGM_UNARY_OPERATOR(!, Not)
	_SGM_UNARY_OPERATOR(~, BitNot)
	_SGM_UNARY_OPERATOR(->, Arrow)
	_SGM_UNARY_OPERATOR(++, Pre_increase)
	_SGM_UNARY_OPERATOR(--, Pre_Decrease)


	_SGM_OPERATOR_HELPER(++, Post_increase)

	_SGM_GENERIC_OPERATOR
	(	/**/, SGM_MACROPACK(, int), ++, Post_increase, int, SGM_MACROPACK(, 0) 
	)

	_SGM_OPERATOR_HELPER(--, Post_Decrease)
	
	_SGM_GENERIC_OPERATOR
	(	/**/, SGM_MACROPACK(, int), --, Post_Decrease, int, SGM_MACROPACK(, 0) 
	)


	_SGM_PARAMETRIC_OPERATOR(+, Plus)
	_SGM_PARAMETRIC_OPERATOR(-, Minus)
	_SGM_PARAMETRIC_OPERATOR(*, Star)
	_SGM_PARAMETRIC_OPERATOR(&, Ampersand)
	_SGM_PARAMETRIC_OPERATOR([], index)
	_SGM_PARAMETRIC_OPERATOR((), invoke)
	_SGM_PARAMETRIC_OPERATOR(/, Divide)
	_SGM_PARAMETRIC_OPERATOR(%, Mod)
	_SGM_PARAMETRIC_OPERATOR(==, Equal)
	_SGM_PARAMETRIC_OPERATOR(!=, NotEqual)
	_SGM_PARAMETRIC_OPERATOR(<, Less)
	_SGM_PARAMETRIC_OPERATOR(>, Greater)
	_SGM_PARAMETRIC_OPERATOR(<=, NotGreater)
	_SGM_PARAMETRIC_OPERATOR(>=, NotLess)
	_SGM_PARAMETRIC_OPERATOR(&&, And)
	_SGM_PARAMETRIC_OPERATOR(||, Or)
	_SGM_PARAMETRIC_OPERATOR(|, BitOr)
	_SGM_PARAMETRIC_OPERATOR(^, Xor)
	_SGM_PARAMETRIC_OPERATOR(<<, LShift)
	_SGM_PARAMETRIC_OPERATOR(>>, RShift)
	_SGM_PARAMETRIC_OPERATOR(->*, ArrowStar)
	_SGM_PARAMETRIC_OPERATOR(+=, PlusAlloc)
	_SGM_PARAMETRIC_OPERATOR(-=, MinusAlloc)
	_SGM_PARAMETRIC_OPERATOR(*=, MultiplyAlloc)
	_SGM_PARAMETRIC_OPERATOR(/=, DivideAlloc)
	_SGM_PARAMETRIC_OPERATOR(%=, ModAlloc)
	_SGM_PARAMETRIC_OPERATOR(&=, BitAndAlloc)
	_SGM_PARAMETRIC_OPERATOR(|=, BitOrAlloc)
	_SGM_PARAMETRIC_OPERATOR(^=, XorAlloc)
	_SGM_PARAMETRIC_OPERATOR(<<=, LShiftAlloc)
	_SGM_PARAMETRIC_OPERATOR(>>=, RShiftAlloc)
	_SGM_PARAMETRIC_OPERATOR( SGM_MACROPACK(,), Comma )


	#undef _SGM_UNARY_OPERATOR
	#undef _SGM_PARAMETRIC_OPERATOR
	#undef _SGM_GENERIC_OPERATOR
	#undef _SGM_OPERATOR_HELPER
#else
	#error The macro was already defined somewhere else
#endif
/**	end of
	#if	!defined(_SGM_OPERATOR_HELPER) && !defined(_SGM_GENERIC_OPERATOR) &&	\
		!defined(_SGM_UNARY_OPERATOR) && !defined(_SGM_PARAMETRIC_OPERATOR)
*/

};


#endif // end of #ifndef _SGM_INTERFACE_TRAITS_