#pragma once

#ifndef _SGM_TYPE_ANALYSIS_
#define _SGM_TYPE_ANALYSIS_

#include <type_traits>

namespace sgm
{


	#ifndef SGM_DECLTYPE_AUTO
		#define SGM_DECLTYPE_AUTO(...)	-> decltype(__VA_ARGS__){  return __VA_ARGS__;  }
	#endif

	struct No_Making{  No_Making() = delete;  };
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	struct _Pointerless : No_Making
	{
	private:
		template<class Q> class rmv{  public: using type = Q;  };
		template<class Q> class rmv<Q*>{  public: using type = typename rmv<Q>::type;  };
		template<class Q> class rmv<Q* const>{  public: using type = typename rmv<Q>::type;  };

	public: using type = typename rmv<T>::type;
	};

	template<class T> using Pointerless_t = typename _Pointerless<T>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<bool B, class T> 
	struct _Provided : No_Making
	{
		static_assert(B, "type condition failed.");

		using type = T;
	};

	template<bool B, class T> using Provided_t = typename _Provided<B, T>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


#ifdef _ITERATOR_
	template<class> using _Void_t = void;

	template <class T, class = void> struct is_iterator : public std::false_type{};

	template <class T>
	struct is_iterator<  T, _Void_t< typename std::iterator_traits<T>::iterator_category >  > 
	:	public std::true_type 
	{};
#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	struct is_immutable : No_Making
	{
		enum : bool
		{	value 
			=	std::is_const
				<	std::conditional_t
					<	std::is_pointer<T>::value
					,	std::remove_pointer_t<T>
					,	std::remove_reference_t<T> 
					>
				>::value
		};
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T1, class T2>
	struct are_mutually_convertible : No_Making
	{
		enum : bool
		{	value = std::is_convertible<T1, T2>::value && std::is_convertible<T2, T1>::value
		};
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	//	same to std::declval in <utility>
	template<class T> static auto Declval()
	->	std::decay_t<T>{  return *(std::decay_t<T>*)nullptr;  }


	template<class CON>
	using Elem_t = decltype(*Declval<CON>().begin());

	template<class CON>
	using RawElem_t = std::decay_t< Elem_t<CON> >;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	struct Has_Type : No_Making
	{
		template<class...> class among;

		template<class Q, class...TYPES>
		class among<Q, TYPES...>
		{
		public: enum : bool{value = std::is_same<T, Q>::value ? true : among<TYPES...>::value};
		};

		template<> class among<> {  public: enum : bool{value = false}; };
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template< template<class...> class FUNCTOR >
	struct Check_All : No_Making
	{
		template<class...> struct for_all;

		template<class Q, class...TYPES>
		struct for_all<Q, TYPES...>
		{
			enum : bool{value = FUNCTOR<Q>::value ? for_all<TYPES...>::value : false};
		};

		template<> struct for_all<>{  enum : bool{value = true};  };
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<class...> struct for_any;

		template<class Q, class...TYPES>
		struct for_any<Q, TYPES...>
		{
			enum : bool{value = FUNCTOR<Q>::value ? true : for_any<TYPES...>::value};
		};

		template<> struct for_any<>{  enum : bool{value = false};  };
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	struct Mutability : No_Making{};
	struct invariable : Mutability{};		using invar = invariable;
	struct Variable :	Mutability{};			using Var = Variable;

	template<class T>
	struct is_Mutability : No_Making
	{
		enum : bool{value = std::is_base_of<Mutability, T>::value};
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
	


#ifndef SGM_DECL_PROXY_TEMPLATE_CLASS
	#define SGM_DECL_PROXY_TEMPLATE_CLASS(PROXYNAME)	\
	template<class> struct is_##PROXYNAME;	\
	\
	template	\
	<	class T, class M	\
	,	bool IS_NESTED_PROXY = is_##PROXYNAME<T>::value	\
	,	bool IS_IMMUTABLE	\
		=	is_immutable<T>::value	\
			||	(	IS_NESTED_PROXY \
				&&	std::is_same< typename is_##PROXYNAME<T>::M_type, invar >::value	\
				)	\
	,	bool IS_REFERENCE = std::is_reference<T>::value	\
	>	\
	struct PROXYNAME##_T_Helper;	\
	\
	template< class T, class M, class = PROXYNAME##_T_Helper<T, M> > class PROXYNAME##_t;	\
	template<class T> using PROXYNAME = PROXYNAME##_t<T, Var>;	\
	template<class T> using const##PROXYNAME = PROXYNAME##_t<T, invar>;	\
	\
	template<class T, class M, bool IS_NESTED_PROXY, bool IS_IMMUTABLE, bool IS_REFERENCE>	\
	struct PROXYNAME##_T_Helper : No_Making	\
	{	\
		using Parent_t	\
		=	std::conditional_t	\
			<	IS_NESTED_PROXY	\
			,	std::conditional_t	\
				<	IS_IMMUTABLE	\
				,	PROXYNAME##_t< typename is_##PROXYNAME<T>::Q_type, invar >	\
				,	PROXYNAME##_t	\
					<	typename is_##PROXYNAME<T>::Q_type	\
					,	typename is_##PROXYNAME<T>::M_type	\
					>	\
				>	\
			,	std::conditional_t	\
				<	IS_IMMUTABLE	\
				,	PROXYNAME##_t< std::decay_t<T>, invar >	\
				,	std::conditional_t	\
					<	IS_REFERENCE	\
					,	PROXYNAME##_t< std::decay_t<T>, M >	\
					,	std::false_type	\
					>	\
				>	\
			>;	\
	};	\
	\
	template<class T>	\
	struct is_##PROXYNAME	\
	{	\
	private:	\
		template<class Q> struct Eval : public std::false_type	\
		{	\
			using Q_type = Q;	\
			using M_type = void;	\
		};	\
		\
		template<class Q, class M, class...TYPES>		\
		struct Eval< PROXYNAME##_t<Q, M, TYPES...> > : public std::true_type	\
		{	\
			using Q_type = Q;	\
			using M_type = M;	\
		};	\
		\
	public: \
		enum : bool{  value = Eval< std::decay_t<T> >::value  };	\
		\
		using Q_type = typename Eval< std::decay_t<T> >::Q_type;	\
		using M_type = typename Eval< std::decay_t<T> >::M_type;	\
	};	\
	\
	template< class T, class M, bool...BS>	\
	class PROXYNAME##_t< T, M, PROXYNAME##_T_Helper<T, M, BS...> >	\
	:	public PROXYNAME##_T_Helper<T, M, BS...>::Parent_t	\
	{	\
	public:		\
		template<class Q>	\
		PROXYNAME##_t(Q&& q) \
		:	PROXYNAME##_T_Helper<T, M, BS...>::Parent_t( std::forward<Q>(q) ){}	\
	}

#else
	#error SGM_DECL_PROXY_TEMPLATE_CLASS was already defined somewhere else.
#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class CON, class T> 
	struct is_iterable
	{
	private:
	#ifndef _HAS_ITERABLE_METHOD_WHOSE_NAME_IS
		#define _HAS_ITERABLE_METHOD_WHOSE_NAME_IS(METHOD, TYPE, MARK)	\
			template<class Q>	\
			static auto _has_##METHOD(Q*)	\
			->	typename std::is_convertible	\
				<	std::decay_t< decltype( MARK Declval<Q>().METHOD() ) >, TYPE	\
				>::	type;	\
			\
			template<class> static auto _has_##METHOD(...)-> std::false_type;	\
			\
			using has_##METHOD = decltype( _has_##METHOD<CON>(nullptr) )

		_HAS_ITERABLE_METHOD_WHOSE_NAME_IS(begin, T, *);
		_HAS_ITERABLE_METHOD_WHOSE_NAME_IS(end, T, *);
		_HAS_ITERABLE_METHOD_WHOSE_NAME_IS(size, size_t, );

		#undef _HAS_ITERABLE_METHOD_WHOSE_NAME_IS
	#else
		#error _HAS_ITERABLE_METHOD_WHOSE_NAME_IS was already defined somewhere else.
	#endif

	public: enum : bool{value = has_begin::value && has_end::value && has_size::value};
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


}

#endif