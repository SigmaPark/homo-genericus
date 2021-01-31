#pragma once

#ifndef _SGM_ABBREVIABLE_
#define _SGM_ABBREVIABLE_

#include "..\Type_Analysis\Type_Analysis.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


#ifndef SGM_ABBREVIABLE_PROXY
	#define SGM_ABBREVIABLE_PROXY(PROXYNAME)	\
	template<class> struct is_##PROXYNAME;	\
	\
	\
	template	\
	<	class T, class M	\
	,	bool IS_NESTED_PROXY = is_##PROXYNAME<T>::value	\
	,	bool IS_IMMUTABLE = is_immutable<T>::value	\
	,	bool IS_REFERENCE = is_Reference<T>::value	\
	>	\
	struct PROXYNAME##_T_Helper;	\
	\
	template< class T, class M, class = PROXYNAME##_T_Helper<T, M> > class PROXYNAME##_t;	\
	template<class T> using PROXYNAME = PROXYNAME##_t<T, Var>;	\
	template<class T> using const##PROXYNAME = PROXYNAME##_t<T, invar>;	\
	\
	\
	template<class T, class M, bool IS_NESTED_PROXY, bool IS_IMMUTABLE, bool IS_REFERENCE>	\
	struct PROXYNAME##_T_Helper		\
	{	\
	public:	\
		using Parent_t	\
		=	Selective_t	\
			<	IS_NESTED_PROXY	\
			,	Selective_t	\
				<	IS_IMMUTABLE	\
				,	PROXYNAME##_t< typename is_##PROXYNAME<T>::Q_type, invar >	\
				,	PROXYNAME##_t	\
					<	typename is_##PROXYNAME<T>::Q_type	\
					,	typename is_##PROXYNAME<T>::M_type	\
					>	\
				>	\
			,	Selective_t	\
				<	IS_IMMUTABLE	\
				,	PROXYNAME##_t< Decay_t<T>, invar >	\
				,	Selective_t	\
					<	IS_REFERENCE	\
					,	PROXYNAME##_t< Decay_t<T>, M >	\
					,	False_t	\
					>	\
				>	\
			>;	\
		\
		PROXYNAME##_T_Helper() = delete;		\
	};	\
	\
	\
	template<class T>	\
	struct is_##PROXYNAME	\
	{	\
	private:	\
		template<class Q> struct Eval : False_t	\
		{	\
			using Q_type = Q;	\
			using M_type = void;	\
		};	\
		\
		template<class Q, class M, class...TYPES>		\
		struct Eval< PROXYNAME##_t<Q, M, TYPES...> > : True_t	\
		{	\
			using Q_type = Q;	\
			using M_type = M;	\
		};	\
		\
		\
	public: \
		enum : bool{  value = Eval< Decay_t<T> >::value  };	\
		\
		using Q_type = typename Eval< Decay_t<T> >::Q_type;	\
		using M_type = typename Eval< Decay_t<T> >::M_type;	\
	};	\
	\
	\
	template< class T, class M, bool...BS>	\
	class PROXYNAME##_t< T, M, PROXYNAME##_T_Helper<T, M, BS...> >	\
	:	public PROXYNAME##_T_Helper<T, M, BS...>::Parent_t	\
	{	\
	public:		\
		template<class...QS>	\
		PROXYNAME##_t(QS&&...qs) \
		:	PROXYNAME##_T_Helper<T, M, BS...>::Parent_t( Forward<QS>(qs)... ){}	\
	};	\
	\
	\
	template<class T>	\
	struct is_const##PROXYNAME	\
	{	\
	private:	\
		template<class Q>	\
		struct PROXYNAME##Mutability	\
		{	\
			using type	\
			=	Selective_t	\
				<	is_immutable<Q>::value	 && is_##PROXYNAME<Q>::value	\
				,	invar, void		\
				>;	\
		};	\
		\
		template<class Q, class M, bool...BS>	\
		struct PROXYNAME##Mutability	\
		<	PROXYNAME##_t< Q, M, PROXYNAME##_T_Helper<Q, M, BS...> >	\
		>	\
		{	\
			using type	\
			=	Selective_t	\
				<	is_Same<M, invar>::value || is_immutable<Q>::value	\
				,	invar	\
				,	Selective_t		\
					<	is_##PROXYNAME<Q>::value	\
					,	typename PROXYNAME##Mutability< Referenceless_t<Q> >::type	\
					,	Var	\
					>	\
				>;	\
		};	\
		\
		\
	public:	\
		enum : bool	\
		{	value	\
			=	is_Same	\
				<	typename PROXYNAME##Mutability< Referenceless_t<T> >::type	\
				,	invar	\
				>::	value	\
		};	\
		\
		is_const##PROXYNAME() = delete;	\
	};	\
	\
	\
	template<class T, class...ARGS>	\
	struct is_immutable< PROXYNAME##_t<T, ARGS...>, false > \
	:	is_const##PROXYNAME< PROXYNAME##_t<T, ARGS...> >	\
	{}
	

#else
	#error SGM_ABBREVIABLE_PROXY was already defined somewhere else.
#endif
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif