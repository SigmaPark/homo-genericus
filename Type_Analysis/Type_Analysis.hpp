#pragma once

#ifndef _SGM_TYPE_ANALYSIS_
#define _SGM_TYPE_ANALYSIS_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version of language support is required.
#endif

#include <type_traits>

namespace sgm
{
	

#ifndef SGM_DECLTYPE_AUTO
	#define SGM_DECLTYPE_AUTO(...)	-> decltype(__VA_ARGS__){  return __VA_ARGS__;  }
#else
	#error SGM_DECLTYPE_AUTO was already defined somewhere else.
#endif


	struct No_Making{  No_Making() = delete;  };
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	//	same to std::declval in <utility>
	template<class T> 
	static auto Declval()-> std::decay_t<T>{  return *(std::decay_t<T>*)nullptr;  }


	template<class CON>
	using Elem_t = decltype(*Declval<CON>().begin());
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<  class T, class = std::enable_if_t< std::is_integral<T>::value >  >
	struct Size_info : No_Making
	{
	private:
	#pragma warning(push)
	#pragma warning(disable : 4348)
		template< class Q, unsigned = sizeof(Q), bool = std::is_unsigned<Q>::value > 
		struct _info;
	#pragma warning(pop)

		template<class Q> struct _info<Q, 8, true>
		{
			enum : T{MAXIMUM = 0xffffffffffffffffui64, MINIMUM = 0};
		};

		template<class Q> struct _info<Q, 4, true>
		{
			enum : T{MAXIMUM = 0xffffffffUL, MINIMUM = 0};
		};

		template<class Q> struct _info<Q, 2, true>
		{
			enum : T{MAXIMUM = 0xffffui16, MINIMUM = 0}; 
		};

		template<class Q> struct _info<Q, 1, true>
		{
			enum : T{MAXIMUM = 0xffui8, MINIMUM = 0}; 
		};

		template<class Q> struct _info<Q, 8, false>
		{  
			enum : T{MAXIMUM = 0x7fffffffffffffffi64, MINIMUM = -0x8000000000000000i64}; 
		};

		template<class Q> struct _info<Q, 4, false>
		{
			enum : T{MAXIMUM = 0x7fffffffi32, MINIMUM = -0x80000000i32};
		};

		template<class Q> struct _info<Q, 2, false>
		{
			enum : T{MAXIMUM = 0x7fffi16, MINIMUM = -0x8000i16}; 
		};

		template<class Q> struct _info<Q, 1, false>
		{
			enum : T{MAXIMUM = 0x7fi8, MINIMUM = -0x80i8}; 
		};


	public:
		enum : T{MAXIMUM = _info<T>::MAXIMUM, MINIMUM = _info<T>::MINIMUM};
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<int, class...TYPES> struct Nth_Type;

	template<int N> struct Nth_Type<N> : No_Making{  using type = void;  };

	template<int N, class T, class...TYPES>
	struct Nth_Type<N, T, TYPES...> : No_Making
	{
		using type = std::conditional_t< N == 0, T, typename Nth_Type<N - 1, TYPES...>::type >;
	};


	template<int N, class...TYPES>
	using Nth_t = typename Nth_Type<N, TYPES...>::type;

	template<class...TYPES> 
	using First_t = Nth_t<0, TYPES...>;
	
	template<class...TYPES> 
	using Last_t = Nth_t<sizeof...(TYPES) - 1, TYPES...>;
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
	struct _Guaranteed : No_Making
	{
		static_assert(B, "type guarantee failed.");

		using type = T;
	};

	template<bool B, class T = void> using Guaranteed_t = typename _Guaranteed<B, T>::type;


#ifndef SGM_COMPILE_FAILED
	#define SGM_COMPILE_FAILED(...) \
		static_assert([]() constexpr{  return false;  }(), #__VA_ARGS__)
#else
	#error SGM_COMPILE_FAILED was already defined somewhere else.
#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class CON, class T = void> 
	struct is_iterable : No_Making
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

	public: enum : bool{value = has_begin::value && has_end::value && has_size::value};
	};


	template
	<	class CON1, class CON2
	,	class 
		=	std::enable_if_t
			<	is_iterable<CON1>::value && is_iterable<CON2>::value
			&&	std::is_same
				<	std::decay_t< Elem_t<CON1> >, std::decay_t< Elem_t<CON2> >  
				>::	value
			>
	>
	static auto iterable_cast(CON2&& con)-> CON1{  return CON1(con.begin(), con.end());  }
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
				>::	value
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


	template<class T>
	struct Has_Type : No_Making
	{
		template<class...> class among;

		template<class Q, class...TYPES>
		class among<Q, TYPES...>
		{
		public: 
			enum : bool
			{	value 
				=	std::is_base_of<T, Q>::value || std::is_same<T, Q>::value 
					?	true 
					:	among<TYPES...>::value
			};
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
	//========//========//========//========//=======#//========//========//========//========//===


	template<bool TEMP_HOST> struct Move_if;

	template<>
	struct Move_if<false> : No_Making
	{
		template<class T>
		static auto cast(T&& t) SGM_DECLTYPE_AUTO( std::forward<T>(t) )
	};

	template<>
	struct Move_if<true> : No_Making
	{
		template<class T>
		static auto cast(T&& t) SGM_DECLTYPE_AUTO( std::move(t) )
	};
	//========//========//========//========//=======#//========//========//========//========//===


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
		template<class...QS>	\
		PROXYNAME##_t(QS&&...qs) \
		:	PROXYNAME##_T_Helper<T, M, BS...>::Parent_t( std::forward<QS>(qs)... ){}	\
	}

#else
	#error SGM_DECL_PROXY_TEMPLATE_CLASS was already defined somewhere else.
#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---



}

#endif