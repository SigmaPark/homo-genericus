#pragma once

#if defined (_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version of language support is required.
#endif

#ifndef _SGM_FAMILY_
#define _SGM_FAMILY_

#include <type_traits>
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	
	template<class...TYPES>
	class Family;


	template<size_t N, class...FAM>
	struct Family_member;


	template<class...TYPES>
	static auto Make_Family(TYPES...types)-> Family<TYPES...>;

	template<class...TYPES>
	static auto Forward_as_Family(TYPES&&...types)-> Family<TYPES&&...>;

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace std
{
	
	template<size_t N, class...TYPES>
	static auto get(sgm::Family<TYPES...>& fam)
	->	typename sgm::Family_member< N, sgm::Family<TYPES...> >::type&
	{
		return
		static_cast
		<	typename sgm::Family_member< N, sgm::Family<TYPES...> >::family_type&  
		>
		(fam)._val;
	}

	template<size_t N, class...TYPES>
	static auto get(sgm::Family<TYPES...> const& fam)
	->	typename sgm::Family_member< N, sgm::Family<TYPES...> const >::type const&
	{
		return
		static_cast
		<	typename sgm::Family_member< N, sgm::Family<TYPES...> const >::family_type const&
		>
		(fam)._val;
	}

	template<size_t N, class...TYPES>
	static auto get(sgm::Family<TYPES...>&& fam) throw()
	->	typename sgm::Family_member< N, sgm::Family<TYPES...> >::type&&
	{
		return
		static_cast
		<	typename sgm::Family_member< N, sgm::Family<TYPES...> >::family_type&&  
		>
		(fam)._val;
	}


#ifndef _UTILITY_
	template<class>
	struct tuple_size;

	template<size_t, class>
	struct tuple_element;
#endif

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<>
class sgm::Family<>{};


#pragma warning(push)
//	default construct may not be generated depending on types of members
#pragma warning(disable : 4510)

template<class T, class...TYPES>
class sgm::Family<T, TYPES...> : public Family<TYPES...>
{
	using _upper_t = Family<TYPES...>;


public:
	enum{SIZE = sizeof...(TYPES) + 1};

	T _val;


	Family() = default;

	Family(T t, TYPES...types)
	:	_val( static_cast<T>(t) ), _upper_t( static_cast<TYPES>(types)... )
	{}


	Family(Family const& fam)
	:	_val( static_cast<T>(fam._val) ), _upper_t( static_cast<_upper_t const&>(fam) )
	{}

	Family(Family&& fam) throw()
	:	_val( std::forward<T>(fam._val) ), _upper_t( static_cast<_upper_t&&>(fam) )
	{}


	auto operator=(Family const& fam)-> Family&
	{
		_val = static_cast<T>(fam._val),
		static_cast<_upper_t&>(*this) = static_cast<_upper_t const&>(fam);

		return *this;
	}

	auto operator=(Family&& fam) throw()-> Family&
	{
		_val = std::forward<T>(fam._val),
		static_cast<_upper_t&>(*this) = static_cast<_upper_t&&>(fam);

		return *this;
	}

};
#pragma warning(pop)
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, class...TYPES>
struct sgm::Family_member< 0, sgm::Family<T, TYPES...> >
{
	using type = T;
	using family_type = Family<T, TYPES...>;

	Family_member() = delete;
};

template<size_t N, class T, class...TYPES>
struct sgm::Family_member< N, sgm::Family<T, TYPES...> > 
:	Family_member< N - 1, Family<TYPES...> >
{};


template<class T, class...TYPES>
struct sgm::Family_member< 0, sgm::Family<T, TYPES...> const >
{
	using type = T const;
	using family_type = Family<T, TYPES...> const;

	Family_member() = delete;
};

template<size_t N, class T, class...TYPES>
struct sgm::Family_member< N, sgm::Family<T, TYPES...> const > 
:	Family_member< N - 1, Family<TYPES...> const >
{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...TYPES>
auto sgm::Make_Family(TYPES...types)-> Family<TYPES...>
{
	return Family<TYPES...>(types...);
}


template<class...TYPES>
auto sgm::Forward_as_Family(TYPES&&...types)-> Family<TYPES&&...>
{
	return Family<TYPES&&...>( std::forward<TYPES>(types)... );
}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class...TYPES>
struct std::tuple_size< sgm::Family<TYPES...> >{  enum{value = sizeof...(TYPES)};  };

template<class...TYPES>
struct std::tuple_size< sgm::Family<TYPES...> const> : tuple_size< sgm::Family<TYPES...> >{};


template<size_t N, class...TYPES>
struct std::tuple_element< N, sgm::Family<TYPES...> >
:	sgm::Family_member< N, sgm::Family<TYPES...> >
{
	static_assert( N < sizeof...(TYPES), "out of index" );
};

template<size_t N, class...TYPES>
struct std::tuple_element< N, sgm::Family<TYPES...> const >
:	sgm::Family_member< N, sgm::Family<TYPES...> const >
{
	static_assert( N < sizeof...(TYPES), "out of index" );
};



#endif