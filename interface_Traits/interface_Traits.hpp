#pragma once

#ifndef _SGM_INTERFACE_TRAITS_
#define _SGM_INTERFACE_TRAITS_

#include "..\Type_Analysis\Type_Analysis.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	template<class...> using Void_t = void;


#ifndef SGM_INTERFACE_CHECK
	#define SGM_INTERFACE_CHECK(TEMPLATE_ARGS, NAME, ...)	\
		template<class _CLASS, TEMPLATE_ARGS>	\
		struct NAME	\
		{	\
		private:	\
			template<class _QCLASS>	\
			static auto _calc(_QCLASS*)	\
			->	typename std::is_convertible	\
				<	std::add_pointer_t	\
					<	std::decay_t<decltype(__VA_ARGS__)>	\
					>	\
				,	void*	\
				>::	type;	\
		\
			template<class> static auto _calc(...)-> std::false_type;	\
		\
		public:	\
			NAME() = delete;	\
		\
			enum : bool{value = decltype( _calc< std::decay_t<_CLASS> >(nullptr) )::value};	\
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
	_BINARY_OPERATOR_INTERFACE(BitXorAlloc, ^=);
	_BINARY_OPERATOR_INTERFACE(LShiftAlloc, <<=);
	_BINARY_OPERATOR_INTERFACE(RShiftAlloc, >>=);

	
	#undef _BINARY_OPERATOR_INTERFACE
#else
	#error _BINARY_OPERATOR_INTERFACE was already defined somewhere else.
#endif
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
	:	std::bool_constant< Has_Operator_Same<T>::value && Has_Operator_NotSame<T>::value >
	{};


	template<class T> 
	struct is_Ordered 
	:	std::bool_constant
		<	Has_Operator_Less<T>::value && Has_Operator_Greater<T>::value 
		&&	Has_Operator_NotLess<T>::value && Has_Operator_NotGreater<T>::value
		>
	{};


	template<class T> 
	struct is_Logical 
	:	std::bool_constant
		<	Has_Operator_And<T>::value && Has_Operator_Or<T>::value && Has_Operator_Not<T>::value
		>
	{};


	template<class T>
	struct is_Digital
	:	std::bool_constant
		<	Has_Operator_BitAnd<T>::value && Has_Operator_BitOr<T>::value
		&&	Has_Operator_BitNot<T>::value && Has_Operator_Xor<T>::value
		&&	Has_Operator_LShift<T>::value && Has_Operator_RShift<T>::value
		>
	{};


	template<class T> 
	struct is_Algebraic 
	:	std::bool_constant
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


	template<class T, class HOST>
	struct _AllocOp_Processor
	{
		T const* p;
		HOST* pHost;
		void(HOST::*pre_func)();
		void(HOST::*post_func)();
	};

	//template<class T, class HOST>
	//static auto AllocOp_Process
	//(	T const* p, HOST* pHost, void(HOST::* pre_func)(), void(HOST::* post_func)() 
	//)->	_AllocOp_Processor<T, HOST>
	//{
	//	return {p, pHost, pre_func, post_func};
	//}


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

		template<class HOST>
		auto update_by_operator
		(	T const* p, HOST* pHost
		,	void(Operation_Decorator::* pre_func)(), void(Operation_Decorator::* post_func)()
		)->	T const*
		{
			return
			(	static_cast< Comparable<T>& >(*this)
			=	static_cast< Ordered<T>& >(*this)
			=	static_cast< Logical<T>& >(*this)
			=	static_cast< Digital<T>& >(*this)
			=	static_cast< Algebraic<T>& >(*this)
			=	_AllocOp_Processor<T, HOST>{p, pHost, pre_func, post_func}
			);
		}
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T> 
	struct is_iterator
	{
		enum : bool
		{	value 
			=	(	(	Has_Operator_Deref<T>::value 
					&&	Has_Operator_Post_increase<T>::value
					&&	Has_Operator_NotSame<T>::value
					)
				||	std::is_pointer<T>::value
				)
		};

		is_iterator() = delete;
	};


	template<class T>
	struct is_random_access_iterator 
	:	std::conditional_t< Has_Operator_index<T>::value, is_iterator<T>, Has_Operator_index<T> >
	{};


	template<class CON, class T = void> 
	struct is_iterable
	{
	private:
	#ifndef _HAS_ITERABLE_METHOD_WHOSE_NAME_IS
		#define _HAS_ITERABLE_METHOD_WHOSE_NAME_IS(METHOD, TYPE, DEREF)	\
			template<class Q>	\
			static auto _has_##METHOD(Q*)	\
			->	typename std::is_convertible	\
				<	std::add_pointer_t	\
					<	std::decay_t< decltype( DEREF Declval<Q>().METHOD() ) >  \
					>	\
				,	TYPE* \
				>::	type;	\
			\
			template<class> static auto _has_##METHOD(...)-> std::false_type;	\
			\
			using has_##METHOD = decltype( _has_##METHOD< std::decay_t<CON> >(nullptr) )

		_HAS_ITERABLE_METHOD_WHOSE_NAME_IS(begin, T, *);
		_HAS_ITERABLE_METHOD_WHOSE_NAME_IS(end, T, *);
		_HAS_ITERABLE_METHOD_WHOSE_NAME_IS(size, size_t, );

		#undef _HAS_ITERABLE_METHOD_WHOSE_NAME_IS
	#else
		#error _HAS_ITERABLE_METHOD_WHOSE_NAME_IS was already defined somewhere else.
	#endif

	public: 
		enum : bool{value = has_begin::value && has_end::value && has_size::value};

		is_iterable() = delete;
	};


	template
	<	class CON1, class CON2
	,	class 
		=	std::enable_if_t
			<	is_iterable<CON1>::value && is_iterable<CON2>::value
			&&	std::is_same
				<	std::decay_t< Deref_t<CON1> >, std::decay_t< Deref_t<CON2> >  
				>::	value
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

#endif // end of #ifndef _SGM_INTERFACE_TRAITS_