/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_FP_PARAMETERS_
#define _SGM_FP_PARAMETERS_


#include "../Family/Family.hpp"


namespace sgm::fp
{

	template<class...ARGS>
	static auto Parameters(ARGS&&...args);

	
	template<class...TYPES>
	class Param_Pack;


	SGM_USER_DEFINED_TYPE_CHECK
	(	class...TYPES
	,	Param_Pack, <TYPES...>
	);


	template<class PPK1, class PPK2>
	static auto Merge_Params(PPK1&& ppk1, PPK2&& ppk2);


	template<class PPK>
	static auto Harden_Params(PPK&& ppk);


	template<class FUNC, class PPK, class...ARGS>
	static decltype(auto) Apply_Params(FUNC&& func, PPK&& ppk, ARGS&&...args);


	enum class _Construction_by_Family_Tag{};


	template<class T, class...ARGS>
	static auto _Param_Pack_Flattening(T&& t, ARGS&&...args);

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class...TYPES>
class sgm::fp::Param_Pack : public Family<TYPES...>
{
public:
	using fam_t = Family<TYPES...>;

	Param_Pack(TYPES...types) : fam_t( static_cast<TYPES&&>(types)... ){}

	Param_Pack(_Construction_by_Family_Tag, fam_t& fam) : fam_t(fam){}
	Param_Pack(_Construction_by_Family_Tag, fam_t const& fam) : fam_t(fam){}
	Param_Pack(_Construction_by_Family_Tag, fam_t&& fam) : fam_t( Move(fam) ){}
	Param_Pack(_Construction_by_Family_Tag, fam_t const&& fam) : fam_t( Move(fam) ){}


	auto as_Family() & noexcept-> fam_t&{  return *this;  }
	auto as_Family() const& noexcept-> fam_t const&{  return *this;  }
	auto as_Family() && noexcept-> fam_t&&{  return Move(*this);  }
	auto as_Family() const&& noexcept-> fam_t const&&{  return Move(*this);  }


	template<size_t IDX>
	decltype(auto) constexpr get() & noexcept{  return as_Family().template get<IDX>();  }
	
	template<size_t IDX>
	decltype(auto) constexpr get() const& noexcept{  return as_Family().template get<IDX>();  }

	template<size_t IDX>
	decltype(auto) constexpr get() && noexcept{  return as_Family().template get<IDX>();  }

	template<size_t IDX>
	decltype(auto) constexpr get() const&& noexcept{  return as_Family().template get<IDX>();  }


	template<class Q>
	decltype(auto) constexpr get() & noexcept{  return as_Family().template get<Q>();  }
	
	template<class Q>
	decltype(auto) constexpr get() const& noexcept{  return as_Family().template get<Q>();  }

	template<class Q>
	decltype(auto) constexpr get() && noexcept{  return as_Family().template get<Q>();  }

	template<class Q>
	decltype(auto) constexpr get() const&& noexcept{  return as_Family().template get<Q>();  }


	template<size_t IDX>
	decltype(auto) constexpr forward() & 
	noexcept{  return as_Family().template forward<IDX>();  }
	
	template<size_t IDX>
	decltype(auto) constexpr forward() const& 
	noexcept{  return as_Family().template forward<IDX>();  }

	template<size_t IDX>
	decltype(auto) constexpr forward() && 
	noexcept{  return as_Family().template forward<IDX>();  }

	template<size_t IDX>
	decltype(auto) constexpr forward() const&& 
	noexcept{  return as_Family().template forward<IDX>();  }
};


namespace sgm::fp
{

	template<class...TYPES>
	Param_Pack(_Construction_by_Family_Tag, Family<TYPES...>&)-> Param_Pack<TYPES...>;
	
	template<class...TYPES>
	Param_Pack(_Construction_by_Family_Tag, Family<TYPES...> const&)-> Param_Pack<TYPES...>;
	
	template<class...TYPES>
	Param_Pack(_Construction_by_Family_Tag, Family<TYPES...>&&)-> Param_Pack<TYPES...>;
	
	template<class...TYPES>
	Param_Pack(_Construction_by_Family_Tag, Family<TYPES...> const&&)-> Param_Pack<TYPES...>;

}


namespace std
{

	template<class...TYPES>
	struct tuple_size< sgm::fp::Param_Pack<TYPES...> >
	:	sgm::As_value_itself<size_t, sizeof...(TYPES)>{};
	
	template<class...TYPES>
	struct tuple_size< sgm::fp::Param_Pack<TYPES...> const > 
	:	tuple_size< sgm::fp::Param_Pack<TYPES...> >{};	


	template<size_t N, class...TYPES>
	struct tuple_element< N, sgm::fp::Param_Pack<TYPES...> >
	:	sgm::As_type_itself< sgm::Nth_t<N, TYPES...> >
	{
		static_assert( N < sizeof...(TYPES), "out of index" );
	};
	
	template<size_t N, class...TYPES>
	struct tuple_element< N, sgm::fp::Param_Pack<TYPES...> const >
	:	sgm::As_type_itself< sgm::Nth_t<N, TYPES...> const >
	{
		static_assert( N < sizeof...(TYPES), "out of index" );
	};


	template<size_t N, class...TYPES>
	static auto constexpr get(sgm::fp::Param_Pack<TYPES...>& ppk) noexcept
	->	SGM_DECLTYPE_AUTO(  ppk.template get<N>()  )

	template<size_t N, class...TYPES>
	static auto constexpr get(sgm::fp::Param_Pack<TYPES...> const& ppk) noexcept
	->	SGM_DECLTYPE_AUTO(  ppk.template get<N>()  )

	template<size_t N, class...TYPES>
	static auto constexpr get(sgm::fp::Param_Pack<TYPES...>&& ppk) noexcept
	->	SGM_DECLTYPE_AUTO(  Move(ppk).template get<N>()  )

	template<size_t N, class...TYPES>
	static auto constexpr get(sgm::fp::Param_Pack<TYPES...> const&& ppk) noexcept
	->	SGM_DECLTYPE_AUTO(  Move(ppk).template get<N>()  )


	template<class Q, class...TYPES>
	static auto constexpr get(sgm::fp::Param_Pack<TYPES...>& ppk) noexcept
	->	SGM_DECLTYPE_AUTO(  ppk.template get<Q>()  )

	template<class Q, class...TYPES>
	static auto constexpr get(sgm::fp::Param_Pack<TYPES...> const& ppk) noexcept
	->	SGM_DECLTYPE_AUTO(  ppk.template get<Q>()  )

	template<class Q, class...TYPES>
	static auto constexpr get(sgm::fp::Param_Pack<TYPES...>&& ppk) noexcept
	->	SGM_DECLTYPE_AUTO(  Move(ppk).template get<Q>()  )

	template<class Q, class...TYPES>
	static auto constexpr get(sgm::fp::Param_Pack<TYPES...> const&& ppk) noexcept
	->	SGM_DECLTYPE_AUTO(  Move(ppk).template get<Q>()  )

}


namespace sgm::fp
{

	template<class PPK1, class PPK2>
	auto Merge_Params(PPK1&& ppk1, PPK2&& ppk2)
	{
		return 
		Param_Pack
		(	_Construction_by_Family_Tag{}
		,	Merge_Families( Forward<PPK1>(ppk1).as_Family(), Forward<PPK2>(ppk2).as_Family() )
		);
	}
	
	
	template<class PPK>
	auto Harden_Params(PPK&& ppk)
	{
		return 
		Param_Pack(  _Construction_by_Family_Tag{}, Harden( Forward<PPK>(ppk).as_Family() )  );
	}
	
	
	template<class T, class...ARGS>
	auto _Param_Pack_Flattening(T&& t, [[maybe_unused]] ARGS&&...args)
	{
		if constexpr( sizeof...(ARGS) != 0 )
			return
			Merge_Params
			(	_Param_Pack_Flattening( Forward<T>(t) )
			,	_Param_Pack_Flattening( Forward<ARGS>(args)... )
			);
		else if constexpr(is_Param_Pack<T>::value)
			return Forward<T>(t);
		else
			return Param_Pack(  _Construction_by_Family_Tag{}, Family<T&&>( Forward<T>(t) )  );
	}
	
	
	template<class...ARGS>
	auto Parameters(ARGS&&...args)
	{
		return _Param_Pack_Flattening( Forward<ARGS>(args)... );
	}
	
	
	template<class FUNC, class PPK, class...ARGS>
	decltype(auto) Apply_Params(FUNC&& func, [[maybe_unused]] PPK&& ppk, ARGS&&...args)
	{
		using fam_t = typename Decay_t<PPK>::fam_t;
		size_t constexpr IDX = sizeof...(ARGS);
	
		if constexpr(IDX == std::tuple_size<fam_t>::value)
			return func( Forward<ARGS>(args)... );
		else
			return
			Apply_Params
			(	Forward<FUNC>(func), Forward<PPK>(ppk)
			,	Forward<ARGS>(args)...
			,	Move_if
				<	is_Rvalue_Reference< typename Family_member<IDX, fam_t>::type&& >::value  
				>
				( std::get<IDX>(ppk.as_Family()) )
			);
	}

}


#endif // end of #ifndef _SGM_FP_PARAMETERS_