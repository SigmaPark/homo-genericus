#pragma once

#ifndef _SGM_ABBREVIABLE_
#define _SGM_ABBREVIABLE_

#include "..\Type_Analysis\Type_Analysis.hpp"


namespace sgm
{

	struct Mutability{  Mutability() = delete;  };
	struct invariable : Mutability{};		using invar = invariable;
	struct Variable :	Mutability{};			using Var = Variable;
	

	template<class T> struct is_Mutability;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
struct sgm::is_Mutability : std::is_base_of<Mutability, T>{  is_Mutability() = delete;  };


#ifndef SGM_ABBREVIABLE_PROXY
	#define SGM_ABBREVIABLE_PROXY(PROXYNAME)	\
	template<class> struct is_##PROXYNAME;	\
	\
	\
	template	\
	<	class T, class M	\
	,	bool IS_NESTED_PROXY = is_##PROXYNAME<T>::value	\
	,	bool IS_IMMUTABLE = is_immutable<T>::value	\
	,	bool IS_REFERENCE = std::is_reference<T>::value	\
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
	private:	\
		template<class Q>	\
		using _No_RC_t = std::remove_const_t< std::remove_reference_t<Q> >;	\
		\
	public:	\
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
				,	PROXYNAME##_t< _No_RC_t<T>, invar >	\
				,	std::conditional_t	\
					<	IS_REFERENCE	\
					,	PROXYNAME##_t< _No_RC_t<T>, M >	\
					,	std::false_type	\
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
		template<class Q> struct Eval : std::false_type	\
		{	\
			using Q_type = Q;	\
			using M_type = void;	\
		};	\
		\
		template<class Q, class M, class...TYPES>		\
		struct Eval< PROXYNAME##_t<Q, M, TYPES...> > : std::true_type	\
		{	\
			using Q_type = Q;	\
			using M_type = M;	\
		};	\
		\
		template<class Q>	\
		using _No_RC_t = std::remove_const_t< std::remove_reference_t<Q> >;	\
		\
	public: \
		enum : bool{  value = Eval< _No_RC_t<T> >::value  };	\
		\
		using Q_type = typename Eval< _No_RC_t<T> >::Q_type;	\
		using M_type = typename Eval< _No_RC_t<T> >::M_type;	\
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
		:	PROXYNAME##_T_Helper<T, M, BS...>::Parent_t( std::forward<QS>(qs)... ){}	\
	};	\
	\
	\
	template<class T>	\
	struct is_const##PROXYNAME	\
	{	\
	private:	\
		\
		template<class Q>	\
		using _is_immutable \
		=	std::is_const<  std::remove_pointer_t< std::remove_reference_t<Q> >  >;	\
		\
		template<class Q>	\
		struct PROXYNAME##Mutability	\
		{	\
			using type	\
			=	std::conditional_t	\
				<	_is_immutable<Q>::value	 && is_##PROXYNAME<Q>::value	\
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
			=	std::conditional_t	\
				<	std::is_same<M, invar>::value || _is_immutable<Q>::value	\
				,	invar	\
				,	std::conditional_t		\
					<	is_##PROXYNAME<Q>::value	\
					,	typename PROXYNAME##Mutability< std::remove_reference_t<Q> >::type	\
					,	Var	\
					>	\
				>;	\
		};	\
		\
		\
	public:	\
		enum : bool	\
		{	value	\
			=	std::is_same	\
				<	typename PROXYNAME##Mutability< std::remove_reference_t<T> >::type	\
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
	{};	\
	\
	\
	template<class T, class...ARGS>	\
	struct std::decay< PROXYNAME##_t<T, ARGS...> > : std::decay<T>{}


#else
	#error SGM_ABBREVIABLE_PROXY was already defined somewhere else.
#endif
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif