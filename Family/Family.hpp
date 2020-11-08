#pragma once

#if defined (_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version of language support is required.
#endif

#ifndef _SGM_FAMILY_
#define _SGM_FAMILY_

#include "..\Type_Analysis\Type_Analysis.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	
	template<class...TYPES>
	class Family;


	template<size_t N, class...FAM>
	struct Family_member;


	template<class T>
	struct is_Family;


	template<class...TYPES>
	static auto Make_Family(TYPES...types)-> Family<TYPES...>;

	template<class...TYPES>
	static auto Forward_as_Family(TYPES&&...types)-> Family<TYPES&&...>;


	template<class FAM1, class FAM2>
	struct _Merge_Fam_Helper;

	template<class FAM1, class FAM2>
	static auto Merge_Families(FAM1&& fam1, FAM2&& fam2)
	->	typename _Merge_Fam_Helper< std::decay_t<FAM1>, std::decay_t<FAM2> >::res_t;


	template<class T>
	using remove_aleph_t
	=	std::conditional_t< std::is_rvalue_reference<T>::value, std::remove_reference_t<T>, T >;
	

	template<class...TYPES>
	struct _Harden_Helper;

	template<class...TYPES>
	static auto Harden(Family<TYPES...>& fam)-> typename _Harden_Helper<TYPES...>::res_t;

	template<class...TYPES>
	static auto Harden(Family<TYPES...>&& fam)-> typename _Harden_Helper<TYPES...>::res_t;

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


template<> struct sgm::Family_member<0, sgm::Family<>>
{
	using type = std::nullptr_t;
	using family_type = Family<>;	
};

template<class T, class...TYPES>
struct sgm::Family_member< 0, sgm::Family<T, TYPES...> >
{
	using type = T;
	using family_type = Family<T, TYPES...>;
};

template<size_t N, class T, class...TYPES>
struct sgm::Family_member< N, sgm::Family<T, TYPES...> > 
:	Family_member< N - 1, Family<TYPES...> >
{};


template<> struct sgm::Family_member<0, sgm::Family<> const>
{
	using type = std::nullptr_t;
	using family_type = Family<> const;
};

template<class T, class...TYPES>
struct sgm::Family_member< 0, sgm::Family<T, TYPES...> const >
{
	using type = T const;
	using family_type = Family<T, TYPES...> const;
};

template<size_t N, class T, class...TYPES>
struct sgm::Family_member< N, sgm::Family<T, TYPES...> const > 
:	Family_member< N - 1, Family<TYPES...> const >
{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
struct sgm::is_Family : std::is_base_of< Family<>, std::decay_t<T> >, No_Making{};
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
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...TYPES1, class...TYPES2>
struct sgm::_Merge_Fam_Helper< sgm::Family<TYPES1...>, sgm::Family<TYPES2...> > : No_Making
{
#pragma warning(push)
#pragma warning(disable : 4348)
	template
	<	unsigned IDX = 0 
	,	unsigned
		=	IDX == sizeof...(TYPES1) + sizeof...(TYPES2)
			?	0
			:	
			( IDX < sizeof...(TYPES1) )
			?	1
			:	2
	>
	struct Nth;
#pragma warning(pop)

	using res_t = Family<TYPES1..., TYPES2...>;

	
	template<unsigned IDX>
	struct Nth<IDX, 0> : No_Making
	{
		template<class FAM1, class FAM2, class...ARGS>
		static auto calc(FAM1&&, FAM2&&, ARGS&&...args)-> res_t
		{
			return Family<TYPES1..., TYPES2...>( std::forward<ARGS>(args)... );
		}
	};

	template<unsigned IDX>
	struct Nth<IDX, 1> : No_Making
	{
		template<class FAM1, class FAM2, class...ARGS>
		static auto calc(FAM1&& fam1, FAM2&& fam2, ARGS&&...args)-> res_t
		{
			return
			Nth<IDX + 1>::calc
			(	std::forward<FAM1>(fam1), std::forward<FAM2>(fam2), std::forward<ARGS>(args)...
			,	std::forward< Nth_t<IDX, TYPES1...> >( std::get<IDX>(fam1) )
			);
		}
	};

	template<unsigned IDX>
	struct Nth<IDX, 2> : No_Making
	{
		template<class FAM1, class FAM2, class...ARGS>
		static auto calc(FAM1&& fam1, FAM2&& fam2, ARGS&&...args)-> res_t
		{
			return
			Nth<IDX + 1>::calc
			(	std::forward<FAM1>(fam1), std::forward<FAM2>(fam2), std::forward<ARGS>(args)...
			,	std::forward< Nth_t<IDX - sizeof...(TYPES1), TYPES2...> >
				(	std::get<IDX - sizeof...(TYPES1)>(fam2) 
				)
			);
		}
	};
};


template<class FAM1, class FAM2>
auto sgm::Merge_Families(FAM1&& fam1, FAM2&& fam2)
->	typename _Merge_Fam_Helper< std::decay_t<FAM1>, std::decay_t<FAM2> >::res_t
{
	return
	_Merge_Fam_Helper< std::decay_t<FAM1>, std::decay_t<FAM2> >::template Nth<>::calc
	(	std::forward<FAM1>(fam1), std::forward<FAM2>(fam2)
	);
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
//========//========//========//========//=======#//========//========//========//========//=======#


template<class...TYPES>
struct sgm::_Harden_Helper : No_Making
{
	using res_t = Family< remove_aleph_t<TYPES>... >;


	template<bool>
	struct _impl;

	template<>
	struct _impl<true> : No_Making
	{
		template<class FAM, class...ARGS>
		static auto calc(FAM&&, ARGS&&...args)-> res_t
		{
			return { std::forward<ARGS>(args)... };
		}
	};

	template<>
	struct _impl<false> : No_Making
	{
		template<class FAM, class...ARGS>
		static auto calc(FAM&& fam, ARGS&&...args)-> res_t
		{
			enum {IDX = sizeof...(ARGS)};
			using elem_t = Nth_t<IDX, TYPES...>;

			return
			_impl<IDX + 1 == sizeof...(TYPES)>::calc
			(	std::forward<FAM>(fam), std::forward<ARGS>(args)...
			,	static_cast< remove_aleph_t<elem_t>&& >( std::get<IDX>(fam) )
			);
		}
	};
};


template<class...TYPES>
auto sgm::Harden(sgm::Family<TYPES...>& fam)-> typename _Harden_Helper<TYPES...>::res_t
{
	return _Harden_Helper<TYPES...>::template _impl<sizeof...(TYPES) == 0>::calc(fam);
}

template<class...TYPES>
auto sgm::Harden(sgm::Family<TYPES...>&& fam)-> typename _Harden_Helper<TYPES...>::res_t
{
	return 
	_Harden_Helper<TYPES...>::template _impl<sizeof...(TYPES) == 0>::calc( std::move(fam) );
}





#endif