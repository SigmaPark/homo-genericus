#pragma once

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version of language support is required.
#endif

#ifndef _SGM_COMPOUND_
#define _SGM_COMPOUND_


namespace sgm
{
	

	template<class...TYPES> class Compound;

	template<> class Compound<>{};

#pragma warning(push)
	//	default constructor or operator= might not be defined, but it doesn't matter.
	#pragma warning(disable : 4510 4512)
	template<class T, class...TYPES>
	class Compound<T, TYPES...> : public Compound<TYPES...>
	{
	public:
		Compound() = default;

		template<class _T, class..._TYPES>
		Compound(_T&& _t, _TYPES&&..._types) 
		:	_first( static_cast<T>(_t) ), Compound<TYPES...>( static_cast<TYPES>(_types)... )
		{}

		enum{ SIZE = 1 + sizeof...(TYPES) };

		T _first;
	};
#pragma warning(pop)
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
	


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


	template<size_t N, class...TYPES>
	static auto Get(Compound<TYPES...>&& cmp)
	->	typename Compound_element< N, Compound<TYPES...> >::type&&
	{
		return
		static_cast<  typename Compound_element< N, Compound<TYPES...> >::compound_type&&  >
		(cmp)._first;
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
		return Compound<decltype(types)...>( static_cast<decltype(types)>(types)... );
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


}	// end of namespace sgm

#endif // end of #ifndef _SGM_COMPOUND_