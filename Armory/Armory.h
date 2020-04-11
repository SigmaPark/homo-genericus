
#pragma once

#ifndef _SGM_ARMORY_
#define _SGM_ARMORY_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif

#ifdef max
	#undef max
#endif
#ifdef min
	#undef min
#endif

#include <cassert>
#include <optional>
#include <vector>
#include <numeric>
#include <cmath>
#include <limits>
#include <execution>


////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{

	class Exception
	{
	public:
		virtual wchar_t const* tag() const { return nullptr; }

		bool operator==(Exception const& e) const{  return tag() == e.tag();  }
		bool operator!=(Exception const& e) const{  return tag() != e.tag();  }
		bool operator==(wchar_t const* t) const{  return tag() == t;  }
		bool operator!=(wchar_t const* t) const{  return tag() != t;  }

	protected:
		Exception(){};
	};


	#ifndef SGM_EXCEPTION
	#define SGM_EXCEPTION(exception_tag) \
		[]	\
		{	\
			class exception_tag : public Exception	\
			{	\
			public:	\
				wchar_t const* tag() const override{  return L#exception_tag;  }	\
			};	\
			throw exception_tag();	\
		}()
	#endif

	#ifndef SGM_EXCEPTION2
	#define SGM_EXCEPTION2(exception_name, ...) \
		[]	\
		{	\
			class exception_name : public Exception	\
			{	\
			public:	\
				wchar_t const* tag() const override{  return L#__VA_ARGS__;  }	\
			};	\
			throw exception_name();	\
		}()
	#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<unsigned DP, typename T1, typename T2 = T1>
	static bool is_close(T1 t1, T2 t2)
	{
		static_assert
		(	std::is_convertible_v<T2, T1> && std::is_arithmetic_v<T1>
		,	"types to be compared are not arithmetric or dismatched each other."
		);
		
		if constexpr(std::is_floating_point_v<T1>)
			return abs(t1 - t2) < (T1)pow(10, DP) * std::numeric_limits<T1>::epsilon();
		else if constexpr(std::is_integral_v<T1>)
			return t1 == t2;
		else static_assert(false, "no suitable method was found.");
	}


	template<unsigned DP, typename T>
	static bool is_almost_0(T t)
	{
		return is_close<DP>( t, T(0) );
	}


	template<unsigned DP, typename T>
	static bool is_big_enough(T t)
	{
		static_assert(std::is_arithmetic_v<T>, "T is not arithmetric.");

		return t >= std::numeric_limits<T>::max() / (T)pow(10, DP);
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<  typename T, typename LESS = std::less< std::decay_t<T> >  >
	class Bound
	{
	public:
		Bound(T t, LESS lc = std::less< std::decay_t<T> >()) 
		:	val( std::forward<T>(t) ), less_comp( std::forward<LESS>(lc) )
		{}


		Bound(Bound const&) = delete;

		
		template<typename L>
		decltype(auto) over(L&& lower) const &&
		{
			return
			less_comp(val, lower) 
			?	static_cast< std::decay_t<T> >(lower)
			:	std::forward<T>(val);
		}


		template<typename U>
		decltype(auto) under(U&& upper) const &&
		{
			return
			less_comp(upper, val) 
			?	static_cast< std::decay_t<T> >(upper)
			:	std::forward<T>(val);
		}


		template<typename L, typename U>
		decltype(auto) between(L&& lower, U&& upper) const &&
		{
			if( !less_comp(lower, upper) )
				SGM_EXCEPTION2
				(	Wrong_Number_inputs
				,	lower value is greater than upper value
				);

			return
			less_comp(val, lower) 
			?	static_cast< std::decay_t<T> >(lower)
			:
			less_comp(upper, val)
			?	static_cast< std::decay_t<T> >(upper)
			:	std::forward<T>(val);
		}


	private:
		T val;
		LESS less_comp;
	};


	template<typename T>
	Bound(T&&)-> Bound<T&&>;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<typename T>
	static auto reserved_vector(size_t s)-> std::vector<T>
	{
		std::vector<T> res{};

		res.reserve(s);

		return res;
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	
	template<typename CON>
	using element_t
	=	std::decay_t
		<	decltype
			(	*std::cbegin
				(	std::declval< std::decay_t<CON> >() 
				)
			)
		>;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	/**	Average
	*	
	*	It returns mean value of elements in container con as std::optional format.
	*
	*	You can get a weighted mean by putting its weight function 
	*	or a container which has weight values as 2nd parameter.
	*
	*	It returns std::nullopt when con has no element or the sum of weights are zero.
	*
	*	You can choose execution policy by putting std::execution policy object as 3rd parameter.
	*		
	*	Requirements
	*	1) the container con has iterator method fit to STL
	*	2) operator+ between elements in con and CON::operator/(den_t) are required.
	*/
	template<	typename den_t = double, typename CON, typename W, typename POL>
	static decltype(auto) Mean(CON&& con, [[maybe_unused]] W&& wgt, POL pol)
	{
		using std::cbegin, std::cend, std::decay_t;
		using elem_t = element_t<CON>;//decay_t<decltype( *cbegin(con) )>;

		if constexpr	//	uniformly weighted average
		(	auto const [eager_reduce, con_size]
			=	std::make_pair
				(	[pol](auto const& c)
					{
						assert( cbegin(c) != cend(c) && "c is empty." );

						return
						std::reduce
						(	pol, std::next( cbegin(c) ), cend(c), *cbegin(c)
						,	std::plus< element_t<decltype(c)> >()
						);
					}
				,	std::distance( cbegin(con), cend(con) )
				)
		;	std::is_same_v< decay_t<W>, nullptr_t >
		)	
			return
			con_size == 0
			?	std::optional<elem_t>{}
			:	std::optional<elem_t>( eager_reduce(con) / static_cast<den_t>(con_size) );
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------
		else if constexpr
		(	auto eager_tran_reduce
			=	[pol, con_size, &con, &eager_reduce](auto&& wc)
				{
					using producted_t 
					=	decltype
						(	std::declval< element_t<decltype(wc)> >()
						*	std::declval<elem_t>()
						);

					using wgted_t		
					=	std::conditional_t
						<	std::is_convertible_v<producted_t, elem_t>, elem_t, producted_t
						>;	// type casting to prevent side-effect from template expression

					if (con_size == 0)
						return std::optional<wgted_t>{};

					if( auto const den = eager_reduce(wc); sgm::is_almost_0<1>(den) )
						return std::optional<wgted_t>{};
					else
						return
						std::optional<wgted_t>
						(	std::transform_reduce
							(	pol
							,	std::next( cbegin(wc) ), cend(wc)
							,	std::next( cbegin(con) )
							,	*cbegin(wc) * *cbegin(con) 
							,	std::plus<wgted_t>()
							,	[](auto const& w, elem_t const& t)-> wgted_t{  return w*t;  }
							)
						/	den
						);
				}
		;	std::is_invocable_v< decay_t<W>, elem_t >
		)
		{
			std::vector<  std::invoke_result_t< decay_t<W>, elem_t >  > wcon(con_size);

			std::transform( pol, cbegin(con), cend(con), wcon.begin(), std::forward<W>(wgt) );

			return eager_tran_reduce(wcon);
		}
		else	//	wgt is regarded as an iterable container having weighting coefficients.
			return eager_tran_reduce( std::forward<W>(wgt) );
	}


	template<typename den_t = double, typename CON, typename _2ND>
	static decltype(auto) Mean(CON&& con, _2ND&& second)
	{
		using std::forward;

		if constexpr(std::is_execution_policy_v< std::decay_t<_2ND> >)
			return Mean<den_t>( forward<CON>(con), nullptr_t(), forward<_2ND>(second) );
		else //	second stands for weighting coefficients.
			return Mean( forward<CON>(con), forward<_2ND>(second), std::execution::seq );
	}


	template<typename den_t = double, typename CON>
	static decltype(auto) Mean(CON&& con)
	{
		return Mean<den_t>( std::forward<CON>(con), nullptr_t(), std::execution::seq );
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


}

#endif