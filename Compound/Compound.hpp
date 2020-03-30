#pragma once

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version of language support is required.
#endif

#ifndef _SGM_COMPOUND_
#define _SGM_COMPOUND_

#include "..\Type_Analysis\Type_Analysis.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{
	

	template<class...TYPES> class Compound;

	template<> class Compound<>{};

	template<class CMP, class...TYPES>
	static auto equal_impl(Compound<TYPES...>& cmp1, CMP&& cmp2)-> Compound<TYPES...>&;


#pragma warning(push)
	//	default constructor or operator= might not be defined, but it doesn't matter.
	#pragma warning(disable : 4510 4512)
	template<class T, class...TYPES>
	class Compound<T, TYPES...> : public Compound<TYPES...>
	{
	public:
		Compound() = default;


		template<class _T, class..._TYPES>
		Compound(_T _t, _TYPES..._types) 
		:	_first( static_cast<T>(_t) ), Compound<TYPES...>( static_cast<TYPES>(_types)... )
		{}

		enum{ SIZE = 1 + sizeof...(TYPES) };

		T _first;

		operator T() const = delete;

		auto operator=(Compound const&)-> Compound& = default;


		template<class CMP, class...TYPES>
		friend auto equal_impl(Compound<TYPES...>& cmp1, CMP&& cmp2)-> Compound<TYPES...>&;


		template<class...TYPES2>
		auto operator=(Compound<TYPES2...> const& cmp2)-> Compound&
		{
			return equal_impl(*this, cmp2);
		}
	};
#pragma warning(pop)
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
		

	template<size_t N, class CMP> class Compound_element;

	template<class T, class...TYPES>
	class Compound_element< 0, Compound<T, TYPES...> >
	{	
	public:
		using type = T;
		using compound_type = Compound<T, TYPES...>;
	};

	template<size_t N, class T, class...TYPES>
	class Compound_element< N, Compound<T, TYPES...> > 
	:	public Compound_element< N - 1, Compound<TYPES...> >
	{};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<size_t N, class...TYPES>
	static auto Get(Compound<TYPES...>& cmp)
	->	typename Compound_element< N, Compound<TYPES...> >::type&
	{
		return
		static_cast<  typename Compound_element< N, Compound<TYPES...> >::compound_type&  >
		(cmp)._first;
	}


	template<size_t N, class...TYPES>
	static auto Get(Compound<TYPES...> const& cmp)
	->	typename Compound_element< N, Compound<TYPES...> >::type const&
	{
		return
		static_cast<  typename Compound_element< N, Compound<TYPES...> >::compound_type const&  >
		(cmp)._first;
	}


	//template<size_t N, class...TYPES>
	//static auto Get(Compound<TYPES...>&& cmp)
	//->	typename Compound_element< N, Compound<TYPES...> >::type&&
	//{
	//	return
	//	static_cast<  typename Compound_element< N, Compound<TYPES...> >::compound_type&&  >
	//	(cmp)._first;
	//}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<unsigned N>
	struct _Compound_Eq_Helper : No_Making
	{
		template<class CMP1, class CMP2>
		static auto calc(CMP1& cmp1, CMP2&& cmp2)-> CMP1&
		{
			Get<N-1>(cmp1) = Get<N-1>(cmp2);

			_Compound_Eq_Helper<N-1>::calc( cmp1, std::forward<CMP2>(cmp2) );

			return cmp1;
		}
	};

	template<>
	struct _Compound_Eq_Helper<0> : No_Making
	{
		static auto calc(Compound<>& cmp1, Compound<> const&)-> Compound<>&{  return cmp1;  }
	};


	template<class CMP, class...TYPES>
	static auto equal_impl(Compound<TYPES...>& cmp1, CMP&& cmp2)
	->	Compound<TYPES...>&
	{
		static_assert
		(	sizeof...(TYPES) == std::decay_t<CMP>::SIZE, "Compound sizes are not matched." 
		);

		return _Compound_Eq_Helper<sizeof...(TYPES)>::calc( cmp1, std::forward<CMP>(cmp2) );
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class...TYPES>
	static auto Make_Compound(TYPES...types)-> Compound<TYPES...>
	{
		return Compound<TYPES...>(types...);
	}


	template<class...TYPES>
	static auto Forward_as_Compound(TYPES&&...types)-> Compound<decltype(types)...>
	{
		return Compound<decltype(types)...>( std::forward<TYPES>(types)... );
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
	
	
	template<class...TYPES>
	static auto Bond(TYPES&...types)-> Compound<decltype(types)...>
	{
		return Compound<decltype(types)...>( static_cast<decltype(types)>(types)... );
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


}	// end of namespace sgm

#endif // end of #ifndef _SGM_COMPOUND_