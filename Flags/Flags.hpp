#pragma once

#ifndef _SGM_FLAGS_
#define _SGM_FLAGS_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

#include "..\Type_Analysis\Type_Analysis.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	template<class> struct is_Flag;
	template<class> struct is_FlagSet;


	template<class...FLAGS> 
	struct Flag
	{
		template<class F>
		auto operator&(F) const-> Flag<FLAGS..., F>
		{
			static_assert(is_Flag<F>::value, "expected type is sgm::Flag");

			return Flag<FLAGS..., F>();
		}


		template<class F>
		struct has : No_Making
		{
			enum : bool{value = Has_Type<F>::template among<FLAGS...>::value};
		};


		template<class> struct is_superset_of;
		template<class> struct is_subset_of;

		template<class FS> struct has_same_flags_to : std::false_type
		{
			static_assert(is_FlagSet<FS>::value, "not a set of sgm::Flag");
		};

		template<class..._FS>
		struct has_same_flags_to< Flag<_FS...> >
		{
			enum : bool
			{	value 
				=	(	is_superset_of< Flag<_FS...> >::value
					&&	is_subset_of< Flag<_FS...> >::value
					)
			};
		};


	private:
		template<> struct is_superset_of<Flag<>> : std::true_type{};
		
		template<class F, class..._FS>
		struct is_superset_of< Flag<F, _FS...> >
		{
			enum : bool
			{	value 
				=	Has_Type<F>::template among<FLAGS...>::value
					?	is_superset_of< Flag<_FS...> >::value
					:	false 
			};
		};


		template<class..._FS>
		struct is_subset_of< Flag<_FS...> >
		{
			enum : bool
			{	value = Flag<_FS...>::template is_superset_of< Flag<FLAGS...> >::value
			};
		};
	};


	static Flag<> const NoFlag;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
	

	template<class FLAG>
	struct is_Flag : No_Making
	{
		enum : bool{value = std::is_convertible< FLAG, Flag<FLAG> >::value};
	};

	template<class> struct is_FlagSet : std::false_type{};
	template<class...FLAGS> struct is_FlagSet< Flag<FLAGS...> > : std::true_type{};
	//========//========//========//========//=======#//========//========//========//========//===


	template<class, class...> struct FlagMatching;

	template<class Q> struct FlagMatching<Q> : No_Making{  enum{number = 0};  };
	
	template<class Q, class F , class...FLAGS>
	struct FlagMatching<Q, F, FLAGS...> : No_Making
	{
		enum
		{	number 
			=	Q::template has_same_flags_to<F>::value 
				?	sizeof...(FLAGS) + 1 
				:	FlagMatching<Q, FLAGS...>::number
		};
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	
	template< template<class> class FM, class BRANCH >
	struct Branch : No_Making
	{
	protected: 
		template<class Q> using KeyFlag = FM<Q>;

		template<class...FLAGS> struct Case
		{
			enum{ON = KeyFlag< Flag<FLAGS...> >::number};  
		};


	public:
		template
		<	class...TYPES, class FLAG_PARAM, class...ARGS
		,	class = std::enable_if_t< is_FlagSet<FLAG_PARAM>::value > 
		>
		static auto calc(FLAG_PARAM, ARGS&&...args) SGM_DECLTYPE_AUTO
		(
			BRANCH::template Switch<FLAG_PARAM>::calc<TYPES...>( std::forward<ARGS>(args)... )
		)


		template<class...FLAGS, class...ARGS>
		static auto calc(ARGS&&...args) SGM_DECLTYPE_AUTO
		( 
			BRANCH::template Switch< Flag<FLAGS...> >::calc( std::forward<ARGS>(args)... )
		)
	};


#ifndef SGM_FLAG_SWITCH
	#define SGM_FLAG_SWITCH template< class FLAG, int = KeyFlag<FLAG>::number > struct Switch
#else
	#error SGM_FLAG_SWITCH was already defined somewhere else.
#endif

#ifndef SGM_FLAG_CASE
	#define SGM_FLAG_CASE(...) template<class F> struct Switch< F, Case<__VA_ARGS__>::ON >
#else
	#error SGM_FLAG_CASE was already defined somewhere else.
#endif


}


#endif