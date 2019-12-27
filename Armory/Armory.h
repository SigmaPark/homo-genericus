
#pragma once

#ifndef _SGM_ARMORY_
#define _SGM_ARMORY_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif

#include <cassert>
#include <optional>
#include <vector>
#include <numeric>
#include <cmath>
#include <limits>


////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	template<unsigned DP, typename T1, typename T2 = T1>
	static bool is_close(T1 t1, T2 t2)
	{
		static_assert
		(	(	std::is_convertible_v< std::decay_t<T2>, std::decay_t<T1> >
			&&	std::is_arithmetic_v< std::decay_t<T1> >
			)
		,	"types to be compared are not arithmetric or dismatched each other."
		);

		return abs(t1 - t2) < (T1)pow(10.0, DP) * std::numeric_limits<T1>::epsilon();
	}


	template<unsigned DP, typename T>
	static bool is_almost_0(T t)
	{
		return is_close<DP>( t, T(0) );
	}


	template<unsigned DP, typename T>
	static bool is_big_enough(T t)
	{
		static_assert(std::is_arithmetic_v< std::decay_t<T> >, "T is not arithmetric.");

		return ( t - std::numeric_limits<T>::max() / (T)pow(10.0, DP) ) >= T(0);
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<typename T>
	static auto reserved_vector(size_t s)-> std::vector<T>
	{
		std::vector<T> res{};

		res.reserve(s);

		return res;
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	/**
	*	@brief	average 
	*	@details	
	*		It returns mean value of elements in container con as std::optional format.
	*		You can get a weighted mean by putting its weight function as 2nd parameter.
	*		It returns std::nullopt when con has no element or the sum of weights are zero.
	*		
	*		Requirements
	*		1) the container con has iterator method fit to STL
	*		2) operator+ between elements in con and CON::operator/(den_t) are required.
	*/
	template<typename den_t = double, unsigned DP = 1, typename F = nullptr_t, typename CON>
	static decltype(auto) Mean(CON&& con, [[maybe_unused]] F&& wgt_func = nullptr)
	{
		using std::cbegin, std::cend, std::decay_t;
		using elem_t = decay_t<decltype( *cbegin(con) )>;

		if constexpr		// uniformly weighted average
		(	auto const [eager_reduce, con_size]
			=	std::make_pair
				(	[](auto const& c)
					{
						assert( cbegin(c) != cend(c) );

						return 
						std::reduce
						(	std::next( cbegin(c) ), cend(c), *cbegin(c)
						,	std::plus< decay_t<decltype( *cbegin(c) )> >()
						);
					}
				,	std::distance( cbegin(con), cend(con) )
				)
		;	std::is_same_v< decay_t<F>, nullptr_t >
		)	
			return
			con_size == 0
			?	std::optional<elem_t>{}
			:	std::optional<elem_t>( eager_reduce(con) / static_cast<den_t>(con_size) );
		else if constexpr(std::is_invocable_r_v< den_t, decay_t<F>, elem_t >)
		{
			using producted_t = decltype(std::declval<den_t>() * std::declval<elem_t>());

			using wgted_t		
			=	std::conditional_t
				<	std::is_convertible_v<producted_t, elem_t>, elem_t, producted_t
				>;

			auto wgts = reserved_vector<den_t>(con_size);

			if(con_size == 0)
				return std::optional<wgted_t>{};
				
			for(auto const& elem : con)
				wgts.emplace_back( wgt_func(elem) );

			if( auto const den = eager_reduce(wgts); sgm::is_almost_0<DP>(den) )
				return std::optional<wgted_t>{};
			else
				return
				std::optional<wgted_t>
				(	std::inner_product
					(	std::next( cbegin(wgts) ), cend(wgts), std::next( cbegin(con) )
					,	*cbegin(wgts) * *cbegin(con) 
					,	std::plus<wgted_t>()
					,	[](den_t const& w, elem_t const& elem)-> wgted_t{  return w*elem;  }
					)
				/	den
				);
		}
		else static_assert(false, "no suitable method was found.");
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

}

#endif