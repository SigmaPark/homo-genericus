
#pragma once

#ifndef _SGM_ARMORY_
#define _SGM_ARMORY_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif


#include <optional>
#include <vector>
#include <numeric>


////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	template<unsigned PoD, typename T1, typename T2 = T1>
	static bool is_close(T1 t1, T2 t2)
	{
		static_assert
		(	(	std::is_convertible_v< std::decay_t<T2>, std::decay_t<T1> >
			&&	std::is_arithmetic_v< std::decay_t<T1> >
			)
		,	"types to be compared are not arithmetric or dismatched each other."
		);

		return abs(t1 - t2) < (T1)pow(10.0, PoD) * std::numeric_limits<T1>::epsilon();
	}


	template<unsigned PoD, typename T>
	static bool is_almost_0(T t)
	{
		return is_close<PoD>( t, T(0) );
	}


	template<unsigned PoD, typename T>
	static bool is_big_enough(T t)
	{
		static_assert(std::is_arithmetic_v< std::decay_t<T> >, "T is not arithmetric.");

		return ( t - std::numeric_limits<T>::max() / (T)pow(10.0, PoD) ) >= T(0);
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
	template<typename den_t = double, unsigned PoD = 1, typename F = nullptr_t, typename CON>
	static decltype(auto) Mean(CON&& con, [[maybe_unused]] F&& wgt_func = nullptr)
	{
		using std::cbegin, std::cend, std::decay_t;
		using elem_t = decay_t<decltype( *cbegin(con) )>;
		using opt_elem_t = std::optional<elem_t>;

		if( cbegin(con) == cend(con) )	// con is empty
			return opt_elem_t{};

		if constexpr(std::is_same_v< decay_t<F>, nullptr_t >)	// uniformly weighted average
			return 
			opt_elem_t
			(	std::reduce( cbegin(con), cend(con) )
			/	static_cast<den_t>(  std::distance( cbegin(con), cend(con) )  )
			);
		else if constexpr(std::is_invocable_r_v< den_t, decay_t<F>, elem_t >)
		{
			auto wgts = reserved_vector<den_t>(  std::distance( cbegin(con), cend(con) )  );

			for(auto const& elem : con)
				wgts.emplace_back( wgt_func(elem) );

			if
			(	auto const den = std::reduce( cbegin(wgts), cend(wgts) )
			;	sgm::is_almost_0<PoD>(den)
			)
				return opt_elem_t{};
			else
				return
				opt_elem_t
				(	std::inner_product
					(	std::next( cbegin(wgts) ), cend(wgts), std::next( cbegin(con) )
					,	*cbegin(wgts) * *cbegin(con)
					)
				/	den
				);
		}
		else static_assert(false, "no suitable method was found.");
	}
}

#endif