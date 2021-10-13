#pragma once

#ifndef _SGM_FLAGS_
#define _SGM_FLAGS_


#include "..\Family\Family.hpp"


namespace sgm
{
	
	template<class...FLAGS>  class FlagSet;

#ifdef SGM_SYNTEX_VERSION_CPP14
	SGM_USER_DEFINED_TYPE_CHECK14(_, FlagSet, class...FLAGS, FLAGS...);
#else
	SGM_USER_DEFINED_TYPE_CHECK(_, FlagSet, class...FLAGS, FLAGS...);
#endif

	template<class...FLAGS>
	static auto Flags(FLAGS...args)-> FlagSet<FLAGS...>;


	template<class FLAG, class FS>  struct _Flag_select_Helper;

	template<  class FLAG, class FS, class = Enable_if_t< is_FlagSet<FS>::value >  >
	using Selected_Flag_t = typename _Flag_select_Helper<FLAG, FS>::type;

	template<  class FLAG, class FS, class = Enable_if_t< is_FlagSet<FS>::value >  >
	static auto Select_Flag(FS&&) noexcept(is_RvalueReference<FS&&>::value)
	->	Selected_Flag_t<FLAG, FS>;


	template
	<	template<class> class CONDITION, class FS, class = Enable_if_t< is_FlagSet<FS>::value >
	>
	using Satisfying_Flag_t = typename Decay_t<FS>::template Satisfying_t<CONDITION>;

	template
	<	template<class> class CONDITION, class FS, class = Enable_if_t< is_FlagSet<FS>::value >
	>
	static auto Satisfying_Flag(FS&&) noexcept(is_RvalueReference<FS&&>::value)
	->	Satisfying_Flag_t<CONDITION, FS>;


	template<class FLAG, class FS>	struct is_Selected_Flag;
	template< template<class> class CONDITION, class FS >  struct has_Satisfying_Flag;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class...FLAGS>
class sgm::FlagSet
{
public:
	FlagSet() = default;
	FlagSet(FLAGS...args) : _fs(args...){}

	template<class F>  struct has : Has_Type<F>::template among<FLAGS...>{};

	template< template<class> class CONDITION >
	using Satisfying_t = typename Satisfying<CONDITION>::template among<FLAGS...>::type;

private:
	template<class FLAG, class FS>  friend struct _Flag_select_Helper;

	template<class FLAG, class FS, class>
	friend auto sgm::Select_Flag(FS &&) noexcept(is_RvalueReference<FS&&>::value)
	->	Selected_Flag_t<FLAG, FS>;

	Family<FLAGS...> _fs;
};


template<>
class sgm::FlagSet<>
{
public:
	template<class>  struct has : False_t{};
	template< template<class> class  > using Satisfying_t = None;

private:
	template<class FLAG, class FS>  friend struct _Flag_select_Helper;

	template<class FLAG, class FS, class>
	friend auto sgm::Select_Flag(FS &&) noexcept(is_RvalueReference<FS&&>::value)
	->	Selected_Flag_t<FLAG, FS>;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FLAG, class FS>	struct sgm::is_Selected_Flag : Decay_t<FS>::template has<FLAG>{};


template< template<class> class CONDITION, class FS >  
struct sgm::has_Satisfying_Flag
:	Boolean_type<  !is_None< typename Decay_t<FS>::template Satisfying_t<CONDITION> >::value  >{};


template<class...FLAGS>
static auto sgm::Flags(FLAGS...args)-> FlagSet<FLAGS...>{  return{args...};  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class FLAG, class FS>
struct sgm::_Flag_select_Helper
{
	using type = Referenceless_t< decltype( std::get<FLAG>(Declval<FS>()._fs) ) >;
};


template<class FLAG, class FS, class>
auto sgm::Select_Flag(FS &&flagset) noexcept(is_RvalueReference<FS&&>::value)
->	Selected_Flag_t<FLAG, FS>
{
	using fam_t
	=	Selective_t
		<	is_RvalueReference<FS&&>::value
		,	decltype( Move(flagset._fs) ), decltype(flagset._fs)
		>;

	return std::get< Selected_Flag_t<FLAG, FS> >( static_cast<fam_t>(flagset._fs) );
}


template< template<class> class CONDITION, class FS, class >
auto sgm::Satisfying_Flag(FS &&flagset) noexcept(is_RvalueReference<FS&&>::value)
->	Satisfying_Flag_t<CONDITION, FS>
{
	return Select_Flag< Satisfying_Flag_t<CONDITION, FS> >( Forward<FS>(flagset) );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_FLAGS_