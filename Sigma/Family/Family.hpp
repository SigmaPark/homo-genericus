#pragma once


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
	static auto Forward_as_Family(TYPES&&...types)
	noexcept(Check_All<is_RvalueReference>::template for_any<TYPES&&...>::value)
	-> Family<TYPES&&...>;

	template<class...TYPES>
	static auto Tie(TYPES&...types)-> Family<TYPES&...>;


	template<class FAM1, class FAM2>
	struct _Merge_Fam_Helper;

	template<class FAM1, class FAM2>
	static auto Merge_Families(FAM1&& fam1, FAM2&& fam2)
	->	typename _Merge_Fam_Helper< Decay_t<FAM1>, Decay_t<FAM2> >::res_t;


	template<class T>
	struct remove_aleph;

	template<class T>
	using remove_aleph_t = typename remove_aleph<T>::type;
	

	template<bool, template<class...> class TL,  class...TYPES>
	struct _Harden_Helper;

	template< template<class...> class TL, class...TYPES >
	static auto Harden(TL<TYPES...>& tuple_like)
	->	typename _Harden_Helper<true, TL, TYPES...>::res_t;

	template< template<class...> class TL, class...TYPES >
	static auto Harden(TL<TYPES...>&& tuple_like)
	->	typename _Harden_Helper<true, TL, TYPES...>::res_t;


	template<size_t N, class Q, class...TYPES>  struct _IDX_Helper;
	template<size_t N>  struct _getFam_Helper;
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
	static auto get(sgm::Family<TYPES...>&& fam) noexcept
	->	typename sgm::Family_member< N, sgm::Family<TYPES...> >::type&&
	{
		return
		static_cast
		<	typename sgm::Family_member< N, sgm::Family<TYPES...> >::family_type&&  
		>
		(fam)._val;
	}


	template<class Q, class...TYPES>
	static auto get(sgm::Family<TYPES...>& fam)-> SGM_DECLTYPE_AUTO
	(	sgm::_getFam_Helper< sgm::_IDX_Helper<sizeof...(TYPES), Q, TYPES...>::value >::calc(fam)
	)

	template<class Q, class...TYPES>
	static auto get(sgm::Family<TYPES...> const& fam)-> SGM_DECLTYPE_AUTO
	(	sgm::_getFam_Helper< sgm::_IDX_Helper<sizeof...(TYPES), Q, TYPES...>::value >::calc(fam)
	)

	template<class Q, class...TYPES>
	static auto get(sgm::Family<TYPES...>&& fam) noexcept-> SGM_DECLTYPE_AUTO
	(	sgm::_getFam_Helper< sgm::_IDX_Helper<sizeof...(TYPES), Q, TYPES...>::value >::calc
		(	Move(fam) 
		)
	)


#ifndef _UTILITY_
	template<class>
	struct tuple_size;

	template<size_t, class>
	struct tuple_element;
#endif


	template<class...TYPES>
	struct tuple_size< sgm::Family<TYPES...> >{  enum{value = sizeof...(TYPES)};  };
	
	template<class...TYPES>
	struct tuple_size< sgm::Family<TYPES...> const > : tuple_size< sgm::Family<TYPES...> >{};
	
	
	template<size_t N, class...TYPES>
	struct tuple_element< N, sgm::Family<TYPES...> >
	:	sgm::Family_member< N, sgm::Family<TYPES...> >
	{
		static_assert( N < sizeof...(TYPES), "out of index" );
	};
	
	template<size_t N, class...TYPES>
	struct tuple_element< N, sgm::Family<TYPES...> const >
	:	sgm::Family_member< N, sgm::Family<TYPES...> const >
	{
		static_assert( N < sizeof...(TYPES), "out of index" );
	};

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class Q, class...TYPES>
struct sgm::_IDX_Helper<0, Q, TYPES...>{  enum : size_t{value = 0xffffffffffffffffui64};  };


template<size_t N, class Q, class...TYPES>
struct sgm::_IDX_Helper
{
private:
	enum : size_t{_IDX = sizeof...(TYPES) - N};

public:
	enum : size_t
	{	value
		=	is_Same< Q, Nth_t<_IDX, TYPES...> >::value
			?	_IDX
			:	_IDX_Helper<N-1, Q, TYPES...>::value
	};
};


template<size_t N>
struct sgm::_getFam_Helper
{
	template<class FAM>
	static auto calc(FAM&& fam) noexcept(is_RvalueReference<FAM&&>::value)
	->	SGM_DECLTYPE_AUTO(  std::get<N>( Forward<FAM>(fam) )  )
};

template<>
struct sgm::_getFam_Helper<0xffffffffffffffffui64>
{
	template<class FAM> static auto calc(FAM) noexcept-> None{  return{};  }
};
//========//========//========//========//=======#//========//========//========//========//=======#


template<>
class sgm::Family<>
{
public:
	bool operator==(Family) const{  return true;  }
	bool operator!=(Family) const{  return false;  }
};


#pragma warning(push)
//	default construct may not be generated depending on types of members
#pragma warning(disable : 4510)

template<class T, class...TYPES>
class sgm::Family<T, TYPES...> : public Family<TYPES...>
{
public:
	using _upper_t = Family<TYPES...>;
	enum{SIZE = sizeof...(TYPES) + 1};

	T _val;


	Family() = default;

	Family(T t, TYPES...types)
	:	_upper_t( static_cast<TYPES>(types)... ), _val( static_cast<T>(t) ){}


	Family(Family const& fam)
	:	_upper_t( static_cast<_upper_t const&>(fam) ), _val( static_cast<T>(fam._val) ){}

	Family(Family&& fam) noexcept
	:	_upper_t( static_cast<_upper_t&&>(fam) ), _val( Forward<T>(fam._val) ){}


	template
	<	class FAM, class FAM_UPPER = typename Decay_t<FAM>::_upper_t
	,	class 
		=	Enable_if_t
			<	is_Family<FAM>::value && !is_Same< Decay_t<FAM>, Family >::value 
			>
	>
	Family(FAM&& fam) noexcept(is_RvalueReference<FAM&&>::value)
	:	_upper_t( CopiedRef_t<FAM&&, FAM_UPPER>(fam) )
	,	_val( Move_if< is_RvalueReference<FAM&&>::value >::cast(fam._val) )
	{
		static_assert
		(	is_Convertible<decltype(fam._val), T>::value
		,	"no method to convert family member."
		);
	}

	
	auto operator=(Family const& fam)-> Family&
	{
		_val = static_cast<T>(fam._val),
		static_cast<_upper_t&>(*this) = static_cast<_upper_t const&>(fam);

		return *this;
	}

	auto operator=(Family&& fam) noexcept-> Family&
	{
		_val = Forward<T>(fam._val),
		static_cast<_upper_t&>(*this) = static_cast<_upper_t&&>(fam);

		return *this;
	}


	template
	<	class FAM, class FAM_UPPER = typename Decay_t<FAM>::_upper_t
	,	class 
		=	Enable_if_t
			<	is_Family<FAM>::value && !is_Same< Decay_t<FAM>, Family >::value 
			>
	>
	auto operator=(FAM&& fam) noexcept(is_RvalueReference<FAM&&>::value)-> Family&
	{
		_val = Move_if< is_RvalueReference<FAM&&>::value >::cast(fam._val);
		static_cast<_upper_t&>(*this) = CopiedRef_t<FAM&&, FAM_UPPER>(fam);

		return *this;
	}


	bool operator==(Family const& fam) const noexcept
	{
		return 
		(	_val == fam._val 
		&&	static_cast<_upper_t const&>(*this) == static_cast<_upper_t const&>(fam)
		);
	}

	bool operator!=(Family const& fam) const noexcept{  return !(*this == fam);  }
};
#pragma warning(pop)
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<> struct sgm::Family_member<0, sgm::Family<>>
{
	using type = None;
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
:	Family_member< N - 1, Family<TYPES...> >{};


template<> struct sgm::Family_member<0, sgm::Family<> const>
{
	using type = None;
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
:	Family_member< N - 1, Family<TYPES...> const >{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
struct sgm::is_Family : is_inherited_from< Decay_t<T>, Family<> >, No_Making{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...TYPES>
auto sgm::Make_Family(TYPES...types)-> Family<TYPES...>{  return {types...};  }


template<class...TYPES>
auto sgm::Forward_as_Family(TYPES&&...types) 
noexcept(Check_All<is_RvalueReference>::template for_any<TYPES&&...>::value)
-> Family<TYPES&&...>{  return { Forward<TYPES>(types)... };  }


template<class...TYPES>
auto sgm::Tie(TYPES&...types)-> Family<TYPES&...>{  return {types...};  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...TYPES1, class...TYPES2>
struct sgm::_Merge_Fam_Helper< sgm::Family<TYPES1...>, sgm::Family<TYPES2...> > : No_Making
{
#pragma warning(push)
#pragma warning(disable : 4348)
	template
	<	unsigned IDX = 0 
	,	unsigned
		=	IDX == sizeof...(TYPES1) + sizeof...(TYPES2) ? 0
		:	( IDX < sizeof...(TYPES1) ) ? 1
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
			return Family<TYPES1..., TYPES2...>( Forward<ARGS>(args)... );
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
			(	Forward<FAM1>(fam1), Forward<FAM2>(fam2), Forward<ARGS>(args)...
			,	Forward< Nth_t<IDX, TYPES1...> >( std::get<IDX>(fam1) )
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
			(	Forward<FAM1>(fam1), Forward<FAM2>(fam2), Forward<ARGS>(args)...
			,	Forward< Nth_t<IDX - sizeof...(TYPES1), TYPES2...> >
				(	std::get<IDX - sizeof...(TYPES1)>(fam2) 
				)
			);
		}
	};
};


template<class FAM1, class FAM2>
auto sgm::Merge_Families(FAM1&& fam1, FAM2&& fam2)
->	typename _Merge_Fam_Helper< Decay_t<FAM1>, Decay_t<FAM2> >::res_t
{
	return
	_Merge_Fam_Helper< Decay_t<FAM1>, Decay_t<FAM2> >::template Nth<>::calc
	(	Forward<FAM1>(fam1), Forward<FAM2>(fam2)
	);
}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
struct sgm::remove_aleph
{
	using type = Selective_t< is_RvalueReference<T>::value, Referenceless_t<T>, T >;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template< template<class...> class TL, class...TYPES >
auto sgm::Harden(TL<TYPES...>& tL)
->	typename sgm::_Harden_Helper<true, TL, TYPES...>::res_t
{
	return _Harden_Helper<sizeof...(TYPES) == 0, TL, TYPES...>::calc(tL);
}

template< template<class...> class TL, class...TYPES >
auto sgm::Harden(TL<TYPES...>&& tL)
->	typename sgm::_Harden_Helper<true, TL, TYPES...>::res_t
{
	return _Harden_Helper<sizeof...(TYPES) == 0, TL, TYPES...>::calc( Move(tL) );
}


template< template<class...> class TL, class...TYPES >
struct sgm::_Harden_Helper<true, TL, TYPES...> : No_Making
{
	using res_t = TL< remove_aleph_t<TYPES>... >;
	
	template<class Q, class...ARGS>
	static auto calc(Q&&, ARGS&&...args)-> res_t{  return {Forward<ARGS>(args)...};  }
};

template< template<class...> class TL, class...TYPES >
struct sgm::_Harden_Helper<false, TL, TYPES...> : No_Making
{
	template<class Q, class...ARGS>
	static auto calc(Q&& q, ARGS&&...args)
	->	typename sgm::_Harden_Helper<true, TL, TYPES...>::res_t
	{
		enum {IDX = sizeof...(ARGS)};
		using elem_t = Nth_t<IDX, TYPES...>;

		return
		sgm::_Harden_Helper<IDX + 1 == sizeof...(TYPES), TL, TYPES...>::calc
		(	Forward<Q>(q), Forward<ARGS>(args)...
		,	static_cast< remove_aleph_t<elem_t>&& >( std::get<IDX>(q) )
		);
	}
};
//========//========//========//========//=======#//========//========//========//========//=======#



#endif