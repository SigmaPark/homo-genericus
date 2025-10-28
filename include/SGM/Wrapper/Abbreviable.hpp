/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_ABBREVIABLE_
#define _SGM_ABBREVIABLE_


#include "SGM/TMP/Type_Analysis.hpp"


#ifndef SGM_ABBREVIABLE_WIZARD
	#define SGM_ABBREVIABLE_WIZARD(TITLE)		\
		enum class TITLE##_tag{};	\
		\
		template<class T>	\
		using is_##TITLE = is_Abbreviable<TITLE##_tag, T>;	\
		\
		template<class T>	\
		using is_const##TITLE = is_constAbbreviable<TITLE##_tag, T>;		\
		\
		template<class T, class M, bool...BFLAGS>		\
		using TITLE##_Pedigree = Abbreviable_Pedigree<TITLE##_tag, T, M, BFLAGS...>;	\
		\
		template<class T, class M, class PDGR = TITLE##_Pedigree<T, M> >	\
		using TITLE##_t = Abbreviable_t<TITLE##_tag, T, M, PDGR>;		\
		\
		template<class T>	\
		using TITLE = TITLE##_t<T, Variable_t>;	\
		\
		template<class T>	\
		using const##TITLE = TITLE##_t<T, invariable_t>;	\
		\
		template<class T, class M, class PDGR>	\
		struct is_immutable< TITLE##_t<T, M, PDGR>, false >	\
		:	is_const##TITLE< TITLE##_t<T, M, PDGR> >{}


#else
	#error SGM_ABBREVIABLE_WIZARD was already defined somewhere else.
#endif


namespace sgm
{

	template<  class TAG, class T, bool = is_Same< T, Decay_t<T> >::value  >
	struct is_Abbreviable;

	template< class TAG, class T, bool = is_Same< T, Decay_t<T> >::value >
	struct is_constAbbreviable;


	template	
	<	class TAG, class T, class M	
	,	bool = is_Abbreviable<TAG, T>::value, bool = is_immutable<T>::value
	>
	struct Abbreviable_Pedigree;


	template< class TAG, class T, class M, class = Abbreviable_Pedigree<TAG, T, M> >
	class Abbreviable_t;	


	template<class TAG, class T> 
	using Abbreviable = Abbreviable_t<TAG, T, Variable_t>;

	template<class TAG, class T> 
	using constAbbreviable = Abbreviable_t<TAG, T, invariable_t>;


	template<  class TAG, class ABRV, bool = is_Same< ABRV, Decay_t<ABRV> >::value  >
	struct _Abbreviable_Type_Traits;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class TAG, class T>
struct sgm::is_Abbreviable<TAG, T, false> : is_Abbreviable< TAG, Decay_t<T> >{};  

template<class TAG, class T>
struct sgm::is_Abbreviable<TAG, T, true> : False_t{};

template<class TAG, class T, class M, class...TYPES>	
struct sgm::is_Abbreviable< TAG, sgm::Abbreviable_t<TAG, T, M, TYPES...>, true > : True_t{};


template<class TAG, class T>
struct sgm::is_constAbbreviable<TAG, T, false> : is_constAbbreviable< TAG, Decay_t<T> >{};

template<class TAG, class T>
struct sgm::is_constAbbreviable<TAG, T, true> : False_t{};

template<class TAG, class T, class M, class...TYPES>
struct sgm::is_constAbbreviable< TAG, sgm::Abbreviable_t<TAG, T, M, TYPES...>, true >
:	Boolean_Or< is_immutable<T>, is_Same<M, invariable_t>, is_constAbbreviable<TAG, T> >{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class TAG, class ABRV>
struct sgm::_Abbreviable_Type_Traits<TAG, ABRV, false>
:	_Abbreviable_Type_Traits< TAG, Decay_t<ABRV> >{};

template<class TAG, class T>
struct sgm::_Abbreviable_Type_Traits<TAG, T, true> 
:	Guaranteed_t< is_Abbreviable<TAG, T>::value >{};

template<class TAG, class T, class M, class PDGR>
struct sgm::_Abbreviable_Type_Traits< TAG, sgm::Abbreviable_t<TAG, T, M, PDGR>, true >
:	As_type_itself<T>{  using mutability_type = M;  };
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class TAG, class T, class M, bool IS_IMMUTABLE>
struct sgm::Abbreviable_Pedigree<TAG, T, M, true, IS_IMMUTABLE>
:	As_type_itself
	<	Abbreviable_t
		<	TAG
		,	typename _Abbreviable_Type_Traits<TAG, T>::type
		,	Selective_t
			<	IS_IMMUTABLE || is_Same<M, invariable_t>::value
			,	invariable_t
			,	typename _Abbreviable_Type_Traits<TAG, T>::mutability_type
			>
		>	
	>
{};

template<class TAG, class T, class M, bool IS_IMMUTABLE>
struct sgm::Abbreviable_Pedigree<TAG, T, M, false, IS_IMMUTABLE>
:	As_type_itself
	<	Abbreviable_t
		<	TAG
		,	Decay_t<T>
		,	Selective_t<IS_IMMUTABLE, invariable_t, M>
		>
	>
{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class TAG, class T, class M, bool...BS>	
class sgm::Abbreviable_t< TAG, T, M, sgm::Abbreviable_Pedigree<TAG, T, M, BS...> >
:	public Abbreviable_Pedigree<TAG, T, M, BS...>::type
{
public:	
	using Abbreviable_Pedigree<TAG, T, M, BS...>::type::type;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_ABBREVIABLE_