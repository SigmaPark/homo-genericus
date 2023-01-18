/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_FAMILY_
#define _SGM_FAMILY_


#include <limits>
#include "../Type_Analysis/Type_Analysis.hpp"


namespace sgm
{
	
	template<class...TYPES>
	class Family;


	template<class T0, class T1>
	class Duo;

	template<class T0, class T1, class T2>
	class Trio;


	enum class _As_it_is_t{} constexpr as_it_is{};


	template<size_t N, class...FAM>
	struct Family_member;


	template<class T>
	struct is_Family;


	template<size_t N, class Q, class...TYPES>  
	struct _Family_index_Helper;
	
	template<size_t N>  
	struct _Family_get_Helper;

}


namespace sgm
{
	
	template<class...TYPES>
	static auto Make_Family(TYPES...types)-> Family<TYPES...>;

	template<class...TYPES>
	static auto Forward_as_Family(TYPES&&...types) noexcept(Aleph_Check<TYPES&&...>::value)
	-> Family<TYPES&&...>;

	template<class...TYPES>
	static auto Tie(TYPES&...types)-> Family<TYPES&...>;

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace std
{

#ifndef _UTILITY_
	template<class>
	struct tuple_size;

	template<size_t, class>
	struct tuple_element;
#endif

	template<class...TYPES>
	struct tuple_size< sgm::Family<TYPES...> > : sgm::As_value_itself<size_t, sizeof...(TYPES)>{};

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

	
	template<size_t N, class...TYPES>
	static auto constexpr get(sgm::Family<TYPES...>& fam) noexcept
	->	typename sgm::Family_member< N, sgm::Family<TYPES...> >::type&
	{
		return
		static_cast
		<	typename sgm::Family_member< N, sgm::Family<TYPES...> >::family_type&  
		>
		(fam)._fv;
	}

	template<size_t N, class...TYPES>
	static auto constexpr get(sgm::Family<TYPES...> const& fam) noexcept
	->	typename sgm::Family_member< N, sgm::Family<TYPES...> const >::type const&
	{
		return
		static_cast
		<	typename sgm::Family_member< N, sgm::Family<TYPES...> const >::family_type const&
		>
		(fam)._fv;
	}

	template<size_t N, class...TYPES>
	static auto constexpr get(sgm::Family<TYPES...>&& fam) noexcept-> SGM_DECLTYPE_AUTO
	(
		static_cast<  typename sgm::Family_member< N, sgm::Family<TYPES...> >::type&&  >
		(	static_cast
			<	typename sgm::Family_member< N, sgm::Family<TYPES...> >::family_type&&
			>
			(fam)._fv
		)
	)

	template<size_t N, class...TYPES>
	static auto constexpr get(sgm::Family<TYPES...> const&& fam) noexcept-> SGM_DECLTYPE_AUTO
	(
		static_cast<  typename sgm::Family_member< N, sgm::Family<TYPES...> const >::type&&  >
		(	static_cast
			<	typename sgm::Family_member< N, sgm::Family<TYPES...> const >::family_type&&
			>
			(fam)._fv
		)
	)	

	template<class Q, class...TYPES>
	static auto constexpr get(sgm::Family<TYPES...>& fam) noexcept-> SGM_DECLTYPE_AUTO
	(	
		sgm::_Family_get_Helper
		<	sgm::_Family_index_Helper<sizeof...(TYPES), Q, TYPES...>::value 
		>::	calc(fam)
	)

	template<class Q, class...TYPES>
	static auto constexpr get(sgm::Family<TYPES...> const& fam) noexcept-> SGM_DECLTYPE_AUTO
	(	
		sgm::_Family_get_Helper
		<	sgm::_Family_index_Helper<sizeof...(TYPES), Q, TYPES...>::value 
		>::	calc(fam)
	)

	template<class Q, class...TYPES>
	static auto constexpr get(sgm::Family<TYPES...>&& fam) noexcept-> SGM_DECLTYPE_AUTO
	(	
		sgm::_Family_get_Helper
		<	sgm::_Family_index_Helper<sizeof...(TYPES), Q, TYPES...>::value 
		>::	calc( sgm::Move(fam) )
	)

	template<class Q, class...TYPES>
	static auto constexpr get(sgm::Family<TYPES...> const&& fam) noexcept-> SGM_DECLTYPE_AUTO
	(	
		sgm::_Family_get_Helper
		<	sgm::_Family_index_Helper<sizeof...(TYPES), Q, TYPES...>::value 
		>::	calc( sgm::Move(fam) )
	)

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<std::size_t N, class Q, class...TYPES>
struct sgm::_Family_index_Helper
:	Selective_t
	<	is_Same< Q, Nth_t<sizeof...(TYPES) - N, TYPES...> >::value
	,	As_value_itself<std::size_t, sizeof...(TYPES) - N>
	,	_Family_index_Helper<N-1, Q, TYPES...>
	>
{};


template<class Q, class...TYPES>
struct sgm::_Family_index_Helper<0, Q, TYPES...> 
:	As_value_itself< std::size_t, std::numeric_limits<std::size_t>::max() >{};


template<std::size_t N>
struct sgm::_Family_get_Helper
{
	template<class FAM>
	static auto calc(FAM&& fam) noexcept(Aleph_Check<FAM&&>::value)
	->	SGM_DECLTYPE_AUTO(  std::get<N>( Forward<FAM>(fam) )  )
};

template<>
struct sgm::_Family_get_Helper< std::numeric_limits<std::size_t>::max() >
{
	template<class FAM> 
	static auto calc(FAM) noexcept-> None{  return{};  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<> 
struct sgm::Family_member<0, sgm::Family<>> 
:	As_type_itself<None>{  using family_type = Family<>;  };

template<class T, class...TYPES>
struct sgm::Family_member< 0, sgm::Family<T, TYPES...> > 
:	As_type_itself<T>{  using family_type = Family<T, TYPES...>;  };

template<std::size_t N, class T, class...TYPES>
struct sgm::Family_member< N, sgm::Family<T, TYPES...> > 
:	Family_member< N-1, Family<TYPES...> >{};


template<> 
struct sgm::Family_member<0, sgm::Family<> const> 
:	As_type_itself<None>{  using family_type = Family<> const;  };

template<class T, class...TYPES>
struct sgm::Family_member< 0, sgm::Family<T, TYPES...> const > 
:	As_type_itself<T const>{  using family_type = Family<T, TYPES...> const;  };

template<std::size_t N, class T, class...TYPES>
struct sgm::Family_member< N, sgm::Family<T, TYPES...> const > 
:	Family_member< N-1, Family<TYPES...> const >{};
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, class...TYPES>
class sgm::Family<T, TYPES...> : public Family<TYPES...>
{
public:
	using _upper_t = Family<TYPES...>;
	static size_t constexpr size_v = sizeof...(TYPES) + 1;

	T _fv;


	constexpr Family() = default;

	constexpr Family(T t, TYPES...types)
	:	_upper_t( static_cast<TYPES>(types)... ), _fv( static_cast<T&&>(t) ){}


	Family(Family const& fam) : _upper_t( static_cast<_upper_t const&>(fam) ), _fv(fam._fv){}

	Family(Family&& fam) noexcept
	:	_upper_t( static_cast<_upper_t&&>(fam) ), _fv( static_cast<T&&>(fam._fv) ){}


	template
	<	class FAM, class FAM_UPPER = typename Decay_t<FAM>::_upper_t
	,	class 
		=	Enable_if_t<  is_Family<FAM>::value && !is_Same< Decay_t<FAM>, Family >::value  >
	>
	Family(FAM&& fam) noexcept(Aleph_Check<FAM&&>::value)
	:	_upper_t( Qualify_Like_t<FAM&&, FAM_UPPER>(fam) )
	,	_fv( Move_if< is_Rvalue_Reference<FAM&&>::value >(fam._fv) )
	{
		static_assert
		(	is_Convertible<decltype(fam._fv), T>::value
		,	"no method to convert family member."
		);
	}

	
	auto operator=(Family const& fam)-> Family&
	{
		_fv = static_cast<T>(fam._fv),
		_upper_of_this() = static_cast<_upper_t const&>(fam);

		return *this;
	}

	auto operator=(Family&& fam) noexcept-> Family&
	{
		_fv = Forward<T>(fam._fv),
		_upper_of_this() = static_cast<_upper_t&&>(fam);

		return *this;
	}

	template
	<	class FAM, class FAM_UPPER = typename Decay_t<FAM>::_upper_t
	,	class 
		=	Enable_if_t
			<	is_Family<FAM>::value 
			&&	!is_Same< Decay_t<FAM>, Family >::value
			&&	!is_Same
				<	Decay_t< typename Family_member<0, FAM>::type >
				,	_As_it_is_t  
				>::	value
			>
	>
	auto operator=(FAM&& fam) noexcept(Aleph_Check<FAM&&>::value)-> Family&
	{
		_fv = Move_if< is_Rvalue_Reference<FAM&&>::value >(fam._fv);
		_upper_of_this() = Qualify_Like_t<FAM&&, FAM_UPPER>(fam);

		return *this;
	}

	template
	<	class A_i_i, class...ARGS
	,	class = Enable_if_t<  is_Same< Decay_t<A_i_i>, _As_it_is_t >::value  >
	>
	auto operator=(Family<A_i_i, ARGS...>&& fam)-> Family&
	{
		_upper_of_this() = static_cast< Family<ARGS...>&& >( Move(fam) );

		return *this;
	}


	template
	<	class Q, class...ARGS
	,	class 
		=	Enable_if_t
			<	( is_Convertible<Q, T>::value || is_Same< Decay_t<Q>, Decay_t<T> >::value ) 
			&&	sizeof...(ARGS) == sizeof...(TYPES)  
			>
	>
	auto operator==(Family<Q, ARGS...> const& fam) const noexcept-> bool
	{
		return
		(	_fv == fam._fv
		&&	static_cast<_upper_t const&>(*this) == static_cast< Family<ARGS...> const& >(fam)
		);
	}

	template<class FAM>
	auto operator!=(FAM const& fam) const noexcept-> bool{  return !(*this == fam);  }


	template<size_t IDX>  
	auto constexpr get() const& noexcept-> SGM_DECLTYPE_AUTO(  std::get<IDX>(*this)  )
	
	template<size_t IDX>  
	auto constexpr get() & noexcept-> SGM_DECLTYPE_AUTO(  std::get<IDX>(*this)  )

	template<size_t IDX>  
	auto constexpr get() && noexcept-> SGM_DECLTYPE_AUTO(  std::get<IDX>( Move(*this) )  )

	template<size_t IDX>  
	auto constexpr get() const&& noexcept-> SGM_DECLTYPE_AUTO(  std::get<IDX>( Move(*this) )  )


	template<class Q>  
	auto constexpr get() const& noexcept-> SGM_DECLTYPE_AUTO(  std::get<Q>(*this)  )
	
	template<class Q>  
	auto constexpr get() & noexcept-> SGM_DECLTYPE_AUTO(  std::get<Q>(*this)  )

	template<class Q>  
	auto constexpr get() && noexcept-> SGM_DECLTYPE_AUTO(  std::get<Q>( Move(*this) )  )

	template<class Q>  
	auto constexpr get() const&& noexcept-> SGM_DECLTYPE_AUTO(  std::get<Q>( Move(*this) )  )


	template<size_t IDX>
	auto constexpr forward() & noexcept-> typename Family_member<IDX, Family>::type&&
	{
		return static_cast< typename Family_member<IDX, Family>::type&& >(get<IDX>());  
	}

	template<size_t IDX>
	auto constexpr forward() const& noexcept-> typename Family_member<IDX, Family>::type const&&
	{
		return static_cast< typename Family_member<IDX, Family>::type const&& >(get<IDX>());
	}

	template<size_t IDX>
	auto constexpr forward() && noexcept-> typename Family_member<IDX, Family>::type&&
	{
		return static_cast< typename Family_member<IDX, Family>::type&& >(get<IDX>());
	}

	template<size_t IDX>
	auto constexpr forward() const&& noexcept-> typename Family_member<IDX, Family>::type const&&
	{
		return static_cast< typename Family_member<IDX, Family>::type const&& >(get<IDX>());  
	}	


private:
	auto _upper_of_this()-> _upper_t&{  return static_cast<_upper_t&>(*this);  }
};


template<>
class sgm::Family<>
{
public:
	bool constexpr operator==(Family) const{  return true;  }
	bool constexpr operator!=(Family) const{  return false;  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#ifndef _SGM_FAMILY_MEMBER_ACCESS_FUNC
	#define _SGM_FAMILY_MEMBER_ACCESS_FUNC(TITLE, N)		\
		auto TITLE() & noexcept-> T##N&{  return _fam_t::template get<N>();  }	\
		auto TITLE() const& noexcept-> T##N const&{  return _fam_t::template get<N>();  }	\
		auto TITLE() && noexcept-> T##N&&{  return Move(_fam_t::template get<N>());  }		\
		auto TITLE() const&& noexcept-> T##N const&&{  return Move(_fam_t::template get<N>());  }

#else
	#error _SGM_FAMILY_MEMBER_ACCESS_FUNC was already defined somewhere else.
#endif


template<class T0, class T1>
class sgm::Duo : public Family<T0, T1>
{
private:
	using _fam_t = Family<T0, T1>;


public:
	using _fam_t::_fam_t;

	_SGM_FAMILY_MEMBER_ACCESS_FUNC(first, 0)
	_SGM_FAMILY_MEMBER_ACCESS_FUNC(second, 1)
};


template<class T0, class T1, class T2>
class sgm::Trio : public Family<T0, T1, T2>
{
private:
	using _fam_t = Family<T0, T1, T2>;


public:
	using _fam_t::_fam_t;

	_SGM_FAMILY_MEMBER_ACCESS_FUNC(first, 0)
	_SGM_FAMILY_MEMBER_ACCESS_FUNC(second, 1)
	_SGM_FAMILY_MEMBER_ACCESS_FUNC(third, 2)
};


#undef _SGM_FAMILY_MEMBER_ACCESS_FUNC
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
struct sgm::is_Family : is_inherited< Family<>, Decay_t<T> >{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template
	<	class FAM1, class FAM2, class RES_FAM = Family<>
	,	int
		=	(std::tuple_size<RES_FAM>::value < std::tuple_size<FAM1>::value) ? 0
		:	(	std::tuple_size<RES_FAM>::value
			<	std::tuple_size<FAM1>::value + std::tuple_size<FAM2>::value
			) ?	1
		:	/* otherwise */ 2
	>
	struct _Merge_Fam_Helper;


	template<class FAM1, class FAM2>
	auto Merge_Families(FAM1&& fam1, FAM2&& fam2) noexcept(Aleph_Check<FAM1&&, FAM2&&>::value)
	->	typename _Merge_Fam_Helper< Decay_t<FAM1>, Decay_t<FAM2> >::res_t;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...TYPES1, class...TYPES2, class...RES_TYPES>
struct sgm::_Merge_Fam_Helper
<	sgm::Family<TYPES1...>, sgm::Family<TYPES2...>, sgm::Family<RES_TYPES...>
,	0
>
:	public
	_Merge_Fam_Helper
	<	Family<TYPES1...>, Family<TYPES2...>
	,	Family< RES_TYPES..., Nth_t<sizeof...(RES_TYPES), TYPES1...> >
	>
{
private:
	static size_t constexpr IDX = sizeof...(RES_TYPES);
	using _next_t = Nth_t<IDX, TYPES1...>;

	using _upper_t
	=	_Merge_Fam_Helper< Family<TYPES1...>, Family<TYPES2...>, Family<RES_TYPES..., _next_t> >;

public:
	using res_t = typename _upper_t::res_t;


	template<class _FAM1, class _FAM2, class...ARGS>
	static auto calc(_FAM1&& fam1, _FAM2&& fam2, ARGS&&...args)-> res_t
	{
		return
		_upper_t::calc
		(	Forward<_FAM1>(fam1), Forward<_FAM2>(fam2), Forward<ARGS>(args)...
		,	Forward<_next_t>( std::get<IDX>(fam1) )
		);
	}
};


template<class...TYPES1, class...TYPES2, class...RES_TYPES>
struct sgm::_Merge_Fam_Helper
<	sgm::Family<TYPES1...>, sgm::Family<TYPES2...>, sgm::Family<RES_TYPES...>
,	1
>
:	public
	_Merge_Fam_Helper
	<	Family<TYPES1...>, Family<TYPES2...>
	,	Family
		<	RES_TYPES...
		,	Nth_t<sizeof...(RES_TYPES) - sizeof...(TYPES1), TYPES2...> 
		>
	>
{
private:
	static size_t constexpr IDX = sizeof...(RES_TYPES) - sizeof...(TYPES1);
	using _next_t = Nth_t<IDX, TYPES2...>;

	using _upper_t
	=	_Merge_Fam_Helper< Family<TYPES1...>, Family<TYPES2...>, Family<RES_TYPES..., _next_t> >;

public:
	using res_t = typename _upper_t::res_t;


	template<class _FAM1, class _FAM2, class...ARGS>
	static auto calc(_FAM1&& fam1, _FAM2&& fam2, ARGS&&...args)-> res_t
	{
		return
		_upper_t::calc
		(	Forward<_FAM1>(fam1), Forward<_FAM2>(fam2), Forward<ARGS>(args)...
		,	Forward<_next_t>( std::get<IDX>(fam2) )
		);
	}
};


template<class...TYPES1, class...TYPES2, class...RES_TYPES>
struct sgm::_Merge_Fam_Helper
<	sgm::Family<TYPES1...>, sgm::Family<TYPES2...>, sgm::Family<RES_TYPES...>
,	2
>
{
	using res_t = Family<RES_TYPES...>;


	template<class _FAM1, class _FAM2, class...ARGS>
	static auto calc(_FAM1&&, _FAM2&&, ARGS&&...args)
	->	SGM_DECLTYPE_AUTO(  res_t( Forward<ARGS>(args)... )  )
};


namespace sgm
{

	template<class FAM1, class FAM2>
	auto Merge_Families(FAM1&& fam1, FAM2&& fam2) noexcept(Aleph_Check<FAM1&&, FAM2&&>::value)
	->	typename _Merge_Fam_Helper< Decay_t<FAM1>, Decay_t<FAM2> >::res_t
	{
		return
		_Merge_Fam_Helper< Decay_t<FAM1>, Decay_t<FAM2> >::calc
		(	Forward<FAM1>(fam1), Forward<FAM2>(fam2)
		);
	}

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{

	template< bool, template<class...> class TFAM, class...TYPES >
	struct _Harden_Fam_Helper;
	

	template< template<class...> class TFAM, class...TYPES >
	static auto Harden(TFAM<TYPES...>& fam_like)
	->	typename _Harden_Fam_Helper<true, TFAM, TYPES...>::res_t;

	template< template<class...> class TFAM, class...TYPES >
	static auto Harden(TFAM<TYPES...>&& fam_like)
	->	typename _Harden_Fam_Helper<true, TFAM, TYPES...>::res_t;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template< template<class...> class TFAM, class...TYPES >
struct sgm::_Harden_Fam_Helper<true, TFAM, TYPES...> : Unconstructible
{
	using res_t = TFAM< Alephless_t<TYPES>... >;
	

	template<class FAM, class...ARGS>
	static auto calc(FAM&&, ARGS&&...args)-> res_t{  return {Forward<ARGS>(args)...};  }
};

template< template<class...> class TFAM, class...TYPES >
struct sgm::_Harden_Fam_Helper<false, TFAM, TYPES...> : Unconstructible
{
	template<class FAM, class...ARGS>
	static auto calc(FAM&& fam, ARGS&&...args)
	->	typename sgm::_Harden_Fam_Helper<true, TFAM, TYPES...>::res_t
	{
		size_t constexpr idx_v = sizeof...(ARGS);
		bool constexpr extracted_all_v = idx_v + 1 == sizeof...(TYPES);

		return
		sgm::_Harden_Fam_Helper<extracted_all_v, TFAM, TYPES...>::calc
		(	Forward<FAM>(fam), Forward<ARGS>(args)..., fam.template forward<idx_v>()
		);
	}
};


namespace sgm
{

	template< template<class...> class TFAM, class...TYPES >
	auto Harden(TFAM<TYPES...>& fam_like)
	->	typename _Harden_Fam_Helper<true, TFAM, TYPES...>::res_t
	{
		return _Harden_Fam_Helper<sizeof...(TYPES) == 0, TFAM, TYPES...>::calc(fam_like);
	}
	
	template< template<class...> class TFAM, class...TYPES >
	auto Harden(TFAM<TYPES...>&& fam_like)
	->	typename _Harden_Fam_Helper<true, TFAM, TYPES...>::res_t
	{
		return _Harden_Fam_Helper<sizeof...(TYPES) == 0, TFAM, TYPES...>::calc( Move(fam_like) );
	}


	template<class...TYPES>
	auto Make_Family(TYPES...types)-> Family<TYPES...>{  return {types...};  }
	
	
	template<class...TYPES>
	auto Forward_as_Family(TYPES&&...types) noexcept(Aleph_Check<TYPES&&...>::value)
	-> Family<TYPES&&...>{  return {Forward<TYPES>(types)...};  }
	
	
	template<class...TYPES>
	auto Tie(TYPES&...types)-> Family<TYPES&...>{  return {types...};  }

}
//========//========//========//========//=======#//========//========//========//========//=======#


#endif // end of #ifndef _SGM_FAMILY_