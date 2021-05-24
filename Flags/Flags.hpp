#pragma once

#ifndef _SGM_FLAGS_
#define _SGM_FLAGS_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

#include "..\Type_Analysis\Type_Analysis.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


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
		struct has : Has_Type<F>::template among<FLAGS...>{};


		template<class> struct is_superset_of;
		template<class> struct is_subset_of;

		template<class FS> struct has_same_flags_to : False_t
		{
			static_assert(is_FlagSet<FS>::value, "not a set of sgm::Flag");
		};

		template<class..._FS>
		struct has_same_flags_to< Flag<_FS...> >
		:	Boolean_type
			<	is_superset_of< Flag<_FS...> >::value && is_subset_of< Flag<_FS...> >::value
			>
		{};


	private:
		template<> struct is_superset_of<Flag<>> : True_t{};
		
		template<class F, class..._FS>
		struct is_superset_of< Flag<F, _FS...> >
		:	Selective_t
			<	Has_Type<F>::template among<FLAGS...>::value
			,	is_superset_of< Flag<_FS...> >
			,	False_t
			>
		{};


		template<class..._FS>
		struct is_subset_of< Flag<_FS...> >
		:	Flag<_FS...>::template is_superset_of< Flag<FLAGS...> >
		{};
	};


	static Flag<> const NoFlag;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
	

	template<class FLAG> struct is_Flag : is_Convertible< FLAG, Flag<FLAG> >{};

	template<class> struct is_FlagSet : False_t{};
	template<class...FLAGS> struct is_FlagSet< Flag<FLAGS...> > : True_t{};
	//========//========//========//========//=======#//========//========//========//========//===


	template< template<class...> class, class >
	struct _Satisfying{  using flag = void;  };

	template< template<class...> class CONDITION, class F, class...FLAGS >
	struct _Satisfying< CONDITION, Flag<F, FLAGS...> >
	{
		using flag 
		=	Selective_t
			<	CONDITION<F>::value
			,	F
			,	typename _Satisfying< CONDITION, Flag<FLAGS...> >::flag
			>;
	};

	template< template<class...> class CONDITION, class FLAGSET >
	using Satisfying_flag = typename _Satisfying<CONDITION, FLAGSET>::flag;
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


}


#endif