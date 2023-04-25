/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_FLAG_SET_
#define _SGM_FLAG_SET_


#include "../Family/Family.hpp"


namespace sgm
{
	
	template<class...FLAGS>  
	class Flag_Set;

	template<class...FLAGS>
	static auto Flags(FLAGS...args)-> Flag_Set<FLAGS...>;


	SGM_USER_DEFINED_TYPE_CHECK
	(	class...ARGS
	,	Flag_Set, <ARGS...>
	);

	template<class FLAG, class FS>	
	struct Has_Flag;

	template< template<class> class CONDITION, class FS >  
	struct Has_Satisfying_Flag;


	template<class FLAG, class FS>  
	struct _Flag_Pick_Helper;

	template<  class FLAG, class FS, class = Enable_if_t< is_Flag_Set<FS>::value >  >
	using Picked_Flag_t = typename _Flag_Pick_Helper<FLAG, FS>::type;

	template<  class FLAG, class FS, class = Enable_if_t< is_Flag_Set<FS>::value >  >
	static auto Pick_Flag(FS&& flagset) noexcept(Aleph_Check<FS&&>::value)
	->	Picked_Flag_t<FLAG, FS>;


	template
	<	template<class> class CONDITION, class FS
	,	class = Enable_if_t< is_Flag_Set<FS>::value >
	>
	using Satisfying_Flag_t = typename Decay_t<FS>::template Satisfying_t<CONDITION>;

	template
	<	template<class> class CONDITION, class FS
	,	class = Enable_if_t< is_Flag_Set<FS>::value >
	>
	static auto Satisfying_Flag(FS&& flagset) noexcept(Aleph_Check<FS&&>::value)
	->	Satisfying_Flag_t<CONDITION, FS>;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class...FLAGS>
class sgm::Flag_Set
{
public:
	Flag_Set() = default;
	Flag_Set(FLAGS...args) : _fs(args...){}

	template<class F>
	struct Has : Has_Type<F>::template among<FLAGS...>{};

	template< template<class> class CONDITION >
	using Satisfying_t = typename Satisfying_Among<CONDITION, FLAGS...>::type;

private:
	template<class FLAG, class FS>  
	friend struct sgm::_Flag_Pick_Helper;


	template<class FLAG, class FS, class>
	friend auto sgm::Pick_Flag(FS&&) noexcept(Aleph_Check<FS&&>::value)
	->	Picked_Flag_t<FLAG, FS>;


	Family<FLAGS...> _fs;
};


template<>
class sgm::Flag_Set<>
{
public:
	template<class>  
	struct Has : False_t{};

	template< template<class> class  > 
	using Satisfying_t = None;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FLAG, class FS>	
struct sgm::Has_Flag : Decay_t<FS>::template Has<FLAG>{};


template< template<class> class CONDITION, class FS >  
struct sgm::Has_Satisfying_Flag
:	Boolean_Not<  is_None< typename Decay_t<FS>::template Satisfying_t<CONDITION> >  >{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FLAG, class FS>
struct sgm::_Flag_Pick_Helper : Unconstructible
{
	using type = Referenceless_t< decltype( std::get<FLAG>(Mock<FS>()._fs) ) >;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template<class...FLAGS>
	auto Flags(FLAGS...args)-> Flag_Set<FLAGS...>{  return{args...};  }


	template<class FLAG, class FS, class>
	auto Pick_Flag(FS&& flagset) noexcept(Aleph_Check<FS&&>::value)-> Picked_Flag_t<FLAG, FS>
	{
		return 
		std::get< Picked_Flag_t<FLAG, FS> >
		(	Move_if< is_Rvalue_Reference<FS&&>::value >(flagset._fs)
		);
	}
	
	
	template< template<class> class CONDITION, class FS, class >
	auto Satisfying_Flag(FS&& flagset) noexcept(Aleph_Check<FS&&>::value)
	->	Satisfying_Flag_t<CONDITION, FS>
	{
		using flag_t = Satisfying_Flag_t<CONDITION, FS>;
	
		return Pick_Flag<flag_t>( Forward<FS>(flagset) );
	}

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_FLAG_SET_
