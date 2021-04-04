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


#ifndef _SGM_GLOBAL_BINARY_OP_DECL
	#define _SGM_GLOBAL_BINARY_OP_DECL(SYM, SPEC)	\
		template	\
		<	class LHS, class Q		\
		,	class _rawL = sgm::Decay_t<LHS>, class _rawQ = sgm::Decay_t<Q>	\
		,	class \
			=	sgm::Enable_if_t	\
				<	!sgm::is_Same<_rawL, _rawQ>::value	\
				&&	!sgm::is_Same< _rawL, sgm::Operator_interface<_rawQ> >::value	\
				>	\
		>	\
		static auto operator SYM(LHS SPEC& lhs, sgm::Operator_interface<Q> const& opi)	\
		->	decltype( lhs SYM static_cast<Q const&>(opi) );	\
		\
		template<class Q, class RHS>	\
		static auto operator SYM(sgm::Operator_interface<Q> SPEC& opi, RHS const& rhs)	\
		->	decltype( static_cast<Q SPEC&>(opi) SYM rhs )


	_SGM_GLOBAL_BINARY_OP_DECL(+, const);
	_SGM_GLOBAL_BINARY_OP_DECL(-, const);
	_SGM_GLOBAL_BINARY_OP_DECL(*, const);
	_SGM_GLOBAL_BINARY_OP_DECL(/, const);
	_SGM_GLOBAL_BINARY_OP_DECL(%, const);
	_SGM_GLOBAL_BINARY_OP_DECL(==, const);
	_SGM_GLOBAL_BINARY_OP_DECL(!=, const);
	_SGM_GLOBAL_BINARY_OP_DECL(<, const);
	_SGM_GLOBAL_BINARY_OP_DECL(>, const);
	_SGM_GLOBAL_BINARY_OP_DECL(<=, const);
	_SGM_GLOBAL_BINARY_OP_DECL(>=, const);
	_SGM_GLOBAL_BINARY_OP_DECL(&&, const);
	_SGM_GLOBAL_BINARY_OP_DECL(||, const);
	_SGM_GLOBAL_BINARY_OP_DECL(&, const);
	_SGM_GLOBAL_BINARY_OP_DECL(|, const);
	_SGM_GLOBAL_BINARY_OP_DECL(^, const);
	_SGM_GLOBAL_BINARY_OP_DECL(<<, const);
	_SGM_GLOBAL_BINARY_OP_DECL(>>, const);
	_SGM_GLOBAL_BINARY_OP_DECL(->*, const);
	_SGM_GLOBAL_BINARY_OP_DECL(->*, /**/);
	_SGM_GLOBAL_BINARY_OP_DECL(+=, /**/);
	_SGM_GLOBAL_BINARY_OP_DECL(-=, /**/);
	_SGM_GLOBAL_BINARY_OP_DECL(*=, /**/);
	_SGM_GLOBAL_BINARY_OP_DECL(/=, /**/);
	_SGM_GLOBAL_BINARY_OP_DECL(%=, /**/);
	_SGM_GLOBAL_BINARY_OP_DECL(&=, /**/);
	_SGM_GLOBAL_BINARY_OP_DECL(|=, /**/);
	_SGM_GLOBAL_BINARY_OP_DECL(^=, /**/);
	_SGM_GLOBAL_BINARY_OP_DECL(<<=, /**/);
	_SGM_GLOBAL_BINARY_OP_DECL(>>=, /**/);

	#undef _SGM_GLOBAL_BINARY_OP_DECL
#else
	#error _SGM_GLOBAL_BINARY_OP_DECL was already defined somewhere else.
#endif


template<class T>
class sgm::Operator_interface
{
private:
	using _self = Operator_interface;

	T *_p;

public:
	using value_type = T;


	Operator_interface(T *p = nullptr) : _p(p){}

	auto operator=(T *p)-> T*{  return _p = p;  }
	
	operator T const&() const{  return *_p;  }
	operator T&(){  return *_p;  }


#ifndef _SGM_UNARY_OP
	#define _SGM_UNARY_OP(SYM, NAME, SPEC)	\
		private:		\
			template<class Q>	\
			struct _op_##SPEC##NAME##_Helper	\
			{	\
				static auto calc(Q *p) SGM_DECLTYPE_AUTO(SYM *p)	\
			};	\
			\
		public:	\
			template< class Q = _op_##SPEC##NAME##_Helper<T SPEC> >	\
			auto operator SYM() SPEC SGM_DECLTYPE_AUTO( Q::calc(_p) )


	_SGM_UNARY_OP(+, Posit, const)
	_SGM_UNARY_OP(-, Negate, const)
	_SGM_UNARY_OP(++, Pre_increase, /**/)
	_SGM_UNARY_OP(--, Pre_Decrease, /**/)
	_SGM_UNARY_OP(*, Deref, const)
	_SGM_UNARY_OP(*, Deref, /**/)
	_SGM_UNARY_OP(&, Ref, const)
	_SGM_UNARY_OP(&, Ref, /**/)
	_SGM_UNARY_OP(!, Not, const)
	_SGM_UNARY_OP(~, BitNot, const)

	#undef _SGM_UNARY_OP
#else
	#error _SGM_UNARY_OP was already defined somewhere else.
#endif

private:
	template<class Q, int>
	struct _op_Post_step_Helper;

	template<class Q>
	struct _op_Post_step_Helper<Q, +1>{  static auto calc(Q *p) SGM_DECLTYPE_AUTO( (*p)++ )  };

	template<class Q>
	struct _op_Post_step_Helper<Q, -1>{  static auto calc(Q *p) SGM_DECLTYPE_AUTO( (*p)-- )  };

public:
	template< class Q = _op_Post_step_Helper<T, +1> >
	auto operator++(int) SGM_DECLTYPE_AUTO( Q::calc(_p) )

	template< class Q = _op_Post_step_Helper<T, -1> >
	auto operator--(int) SGM_DECLTYPE_AUTO( Q::calc(_p) )


private:
	template<class Q>
	struct _op_Arrow_Helper{  static auto calc(Q *p) SGM_DECLTYPE_AUTO(p)  };
	
public:
	template< class Q = _op_Arrow_Helper<T> >
	auto operator->() SGM_DECLTYPE_AUTO( Q::calc(_p) )

	template< class Q = _op_Arrow_Helper<T const> >
	auto operator->() const SGM_DECLTYPE_AUTO( Q::calc(_p) )


private:
	template<class Q>
	struct _op_index_Helper
	{
		template<class RHS>
		static auto calc(Q *p, RHS &&rhs) SGM_DECLTYPE_AUTO( (*p)[Forward<RHS>(rhs)] )
	};

public:
	template< class RHS, class Q = _op_index_Helper<T> >
	auto operator[](RHS&& rhs) SGM_DECLTYPE_AUTO(  Q::calc( _p, Forward<RHS>(rhs) )  )
	
	template< class RHS, class Q = _op_index_Helper<T const> >
	auto operator[](RHS&& rhs) const SGM_DECLTYPE_AUTO(  Q::calc( _p, Forward<RHS>(rhs) )  )


private:
	template<class Q>
	struct _op_Bracket_Helper
	{
		template<class...ARGS>
		static auto calc(Q *p, ARGS&&...args) SGM_DECLTYPE_AUTO
		(
			(*p)( Forward<ARGS>(args)... )
		)
	};

public:
	template< class...ARGS, class Q = _op_Bracket_Helper<T> >
	auto operator()(ARGS&&...args) SGM_DECLTYPE_AUTO(  Q::calc( _p, Forward<ARGS>(args)... )  )

	template< class...ARGS, class Q = _op_Bracket_Helper<T const> >
	auto operator()(ARGS&&...args) const
	SGM_DECLTYPE_AUTO(  Q::calc( _p, Forward<ARGS>(args)... )  )

};


#ifndef _SGM_GLOBAL_BINARY_OP_IMPL
	#define _SGM_GLOBAL_BINARY_OP_IMPL(SYM, SPEC)	\
		template<class LHS, class Q, class, class, class>	\
		auto operator SYM(LHS SPEC& lhs, sgm::Operator_interface<Q> const& opi)	\
		SGM_DECLTYPE_AUTO( lhs SYM static_cast<Q const&>(opi) )		\
		\
		template<class Q, class RHS>	\
		auto operator SYM(sgm::Operator_interface<Q> SPEC& opi, RHS const& rhs)	\
		SGM_DECLTYPE_AUTO( static_cast<Q SPEC&>(opi) SYM rhs )


	_SGM_GLOBAL_BINARY_OP_IMPL(+, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(-, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(*, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(/, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(%, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(==, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(!=, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(<, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(>, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(<=, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(>=, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(&&, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(||, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(&, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(|, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(^, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(<<, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(>>, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(->*, const);
	_SGM_GLOBAL_BINARY_OP_IMPL(->*, /**/);
	_SGM_GLOBAL_BINARY_OP_IMPL(+=, /**/);
	_SGM_GLOBAL_BINARY_OP_IMPL(-=, /**/);
	_SGM_GLOBAL_BINARY_OP_IMPL(*=, /**/);
	_SGM_GLOBAL_BINARY_OP_IMPL(/=, /**/);
	_SGM_GLOBAL_BINARY_OP_IMPL(%=, /**/);
	_SGM_GLOBAL_BINARY_OP_IMPL(&=, /**/);
	_SGM_GLOBAL_BINARY_OP_IMPL(|=, /**/);
	_SGM_GLOBAL_BINARY_OP_IMPL(^=, /**/);
	_SGM_GLOBAL_BINARY_OP_IMPL(<<=, /**/);
	_SGM_GLOBAL_BINARY_OP_IMPL(>>=, /**/);

	#undef _SGM_GLOBAL_BINARY_OP_IMPL
#else
#endif




#endif // end of #ifndef _SGM_INTERFACE_TRAITS_