#pragma once

#ifndef _SGM_MULTIPLE_
#define _SGM_MULTIPLE_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif

#include "..\Type_Analysis\Type_Analysis.hpp"
#include <tuple>
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm::fp0
{
	
	template<class...TYPES>
	class Multiple;


	template<class...ARGS>
	static auto Make_Multiple(ARGS&&...args);

	template<class...ARGS>
	static auto Forward_as_Multiple(ARGS&&...args);


	template<class...TYPES>
	static auto to_Multiple(std::tuple<TYPES...>& tu);

	template<class...TYPES>
	static auto to_Multiple(std::tuple<TYPES...> const& tu);

	template<class...TYPES>
	static auto to_Multiple(std::tuple<TYPES...>&& tu);

	template<class...TYPES>
	struct _MTP_Cast_Helper;


	template<class>
	struct _is_Multiple;

	template<class T>
	static bool constexpr is_Multiple_v = _is_Multiple< std::decay_t<T> >::value;

	template<class T, class...ARGS>
	static decltype(auto) Make_Flat_MTP(T&& t, ARGS&&...args);

	template<class T, class...ARGS>
	static decltype(auto) Forward_as_Flat_MTP(T&& t, ARGS&&...args);


	template<unsigned Di>
	struct _2FMTP_Helper;

	template<unsigned D1, unsigned D2, class...ARGS>
	static auto Forward_as_2FMTP(ARGS&&...args);


	template<class T>
	using remove_aleph_t
	=	std::conditional_t< std::is_rvalue_reference_v<T>, std::remove_reference_t<T>, T >;


	template< template<class...> class TC >
	struct Type_Check_by;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template< template<class...> class TC >
struct sgm::fp0::Type_Check_by : No_Making
{
	template<class...TYPES>
	static bool constexpr for_any_v = Check_All<TC>::template for_any<TYPES...>::value;

	template<class...TYPES>
	static bool constexpr for_all_v = Check_All<TC>::template for_all<TYPES...>::value;

	template<class...TYPES>
	static bool constexpr for_any_not_v = !for_all_v<TYPES...>;

	template<class...TYPES>
	static bool constexpr for_none_v = !for_any_v<TYPES...>;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...TYPES>
class sgm::fp0::Multiple
{
	template<class...> struct _Merge_Helper;

public:
	using tuple_t = std::tuple<TYPES...>;
	static auto constexpr DIMENSION = sizeof...(TYPES);


	Multiple(TYPES...types) : _tu(  tuple_t( std::forward<TYPES>(types)... )  ){}


	auto operator*() const&-> tuple_t const&{  return _tu;  }
	auto operator*() &-> tuple_t&{  return _tu;  }
	auto operator*() &&-> tuple_t&&{  return std::move(_tu);  }


	template<unsigned IDX> decltype(auto) get(){  return std::get<IDX>(_tu);  }
	template<unsigned IDX> decltype(auto) get() const{  return std::get<IDX>(_tu);  }


	template< unsigned IDX = Size_info<unsigned>::MAXIMUM >
	decltype(auto) forward()
	{	
		if constexpr(IDX == Size_info<unsigned>::MAXIMUM)
			return _forward();
		else
			return static_cast< Nth_t<IDX, TYPES...> >(get<IDX>());
	}

	template< unsigned IDX = Size_info<unsigned>::MAXIMUM >
	decltype(auto) forward() const
	{
		if constexpr(IDX == Size_info<unsigned>::MAXIMUM)
			return _forward();
		else
		{
			using elem_t = Nth_t<IDX, TYPES...>;

			static_assert(!std::is_lvalue_reference_v<elem_t>);
			
			return static_cast<elem_t>(get<IDX>());
		}
	}


	auto hardened(){  return _hardened();  }

	auto hardened() const
	{
		static_assert(Type_Check_by<std::is_rvalue_reference>::for_none_v<TYPES...>);

		return *this;
	}


	template<class...TYPES2>
	auto operator+(Multiple<TYPES2...>& mtp)
	{
		return _Merge_Helper<TYPES2...>::calc(*this, mtp);
	}

	template<class...TYPES2>
	auto operator+(Multiple<TYPES2...> const& mtp)
	{
		return _Merge_Helper<TYPES2...>::calc(*this, mtp);
	}

	template<class...TYPES2>
	auto operator+(Multiple<TYPES2...>&& mtp)
	{
		return _Merge_Helper<TYPES2...>::calc( *this, std::move(mtp) );
	}


private:
	std::tuple<TYPES...> _tu;


	template<class...ARGS>
	auto _hardened(ARGS&&...args)
	{
		if constexpr
		(	auto constexpr tu_size = sizeof...(TYPES), nof_args = sizeof...(ARGS)
		;	tu_size == nof_args
		)
			return Multiple< remove_aleph_t<TYPES>... >( std::forward<ARGS>(args)... );
		else
		{
			using elem_t = Nth_t<nof_args, TYPES...>;

			using maybe_aleph_t
			=	std::conditional_t
				<	std::is_rvalue_reference_v<elem_t>
				,	elem_t
				,	decltype(get<nof_args>())
				>;

			return
			_hardened
			(	std::forward<ARGS>(args)..., static_cast<maybe_aleph_t>(get<nof_args>()) 
			);
		}
	}


	template<class...ARGS>
	auto _forward(ARGS&&...args)
	{
		if constexpr
		(	auto constexpr tu_size = sizeof...(TYPES), nof_args = sizeof...(ARGS)
		;	tu_size == nof_args
		)
			return Multiple<TYPES&&...>( std::forward<ARGS>(args)... );
		else
		{	
			using elem_t = Nth_t<nof_args, TYPES...>;

			return 
			_forward( std::forward<ARGS>(args)..., static_cast<elem_t&&>(get<nof_args>()) );
		}
	}


	template<class...TYPES2>
	struct _Merge_Helper : No_Making
	{
		template<class MTP1, class MTP2, class...ARGS>
		static auto calc(MTP1&& mtp1, MTP2&& mtp2, ARGS&&...args)
		{
			using std::forward;

			if constexpr
			(	auto constexpr nof_args = sizeof...(ARGS)
			;	nof_args == mtp1.DIMENSION + mtp2.DIMENSION
			)
				return Multiple<TYPES..., TYPES2...>( forward<ARGS>(args)... );
			else if constexpr(nof_args < mtp1.DIMENSION)
				return
				calc
				(	forward<MTP1>(mtp1), forward<MTP2>(mtp2), forward<ARGS>(args)...
				,	static_cast< decltype(mtp1.forward<nof_args>())&& >(mtp1.get<nof_args>())
				);
			else
			{
				auto constexpr idx = nof_args - mtp1.DIMENSION;

				return
				calc
				(	forward<MTP1>(mtp1), forward<MTP2>(mtp2), forward<ARGS>(args)...	
				,	static_cast< decltype(mtp2.forward<idx>())&& >(mtp2.get<idx>())
				);
			}
		}
	};

}; // end of class sgm::fp0::Multiple<TYPES...>
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...ARGS>
auto sgm::fp0::Make_Multiple(ARGS&&...args)
{
	return
	Multiple< std::remove_reference_t<ARGS>... >
	(	static_cast< std::remove_reference_t<ARGS> >(args)...
	);
}

template<class...ARGS>
auto sgm::fp0::Forward_as_Multiple(ARGS&&...args)
{
	return Multiple<decltype(args)...>( static_cast<decltype(args)>(args)... );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...TYPES>
struct sgm::fp0::_MTP_Cast_Helper : No_Making
{
	template<class TU, class...ARGS>
	static auto cast([[maybe_unused]]TU&& tu, ARGS&&...args)
	{
		using std::forward;
		
		if constexpr
		(	auto constexpr tu_size = sizeof...(TYPES), nof_args = sizeof...(ARGS)
		;	tu_size == nof_args
		)
			return Multiple<TYPES...>( forward<ARGS>(args)... );
		else
			return
			cast
			(	forward<TU>(tu), forward<ARGS>(args)...
			,	static_cast< Nth_t<nof_args, TYPES...> >( std::get<nof_args>(tu) )
			);
	}
};


template<class...TYPES>
auto sgm::fp0::to_Multiple(std::tuple<TYPES...>& tu)
{
	return _MTP_Cast_Helper<TYPES...>::cast(tu);
}

template<class...TYPES>
auto sgm::fp0::to_Multiple(std::tuple<TYPES...> const& tu)
{
	return _MTP_Cast_Helper<TYPES...>::cast(tu);
}

template<class...TYPES>
auto sgm::fp0::to_Multiple(std::tuple<TYPES...>&& tu)
{
	return _MTP_Cast_Helper<TYPES...>::cast( std::move(tu) );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class>
struct sgm::fp0::_is_Multiple : std::false_type, No_Making{};

template<class...TYPES>
struct sgm::fp0::_is_Multiple< sgm::fp0::Multiple<TYPES...> > : std::true_type, No_Making{};


template<class T, class...ARGS>
decltype(auto) sgm::fp0::Make_Flat_MTP(T&&t, [[maybe_unused]]ARGS&&...args)
{
	using std::forward;

	if constexpr( sizeof...(ARGS) > 0 )
		return Make_Flat_MTP( forward<T>(t) ) + Make_Flat_MTP( forward<ARGS>(args)... );
	else if constexpr(is_Multiple_v<T>)
		return forward<T>(t);
	else
		return Make_Multiple( forward<T>(t) );
}


template<class T, class...ARGS>
decltype(auto) sgm::fp0::Forward_as_Flat_MTP(T&& t, [[maybe_unused]]ARGS&&...args)
{
	if constexpr( sizeof...(ARGS) > 0 )
		return
		(	Forward_as_Flat_MTP( std::forward<T>(t) )
		+	Forward_as_Flat_MTP( std::forward<ARGS>(args)... )
		);
	else if constexpr(is_Multiple_v<T>)
		return t.forward();
	else
		return Forward_as_Multiple( std::forward<T>(t) );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned D1>
struct sgm::fp0::_2FMTP_Helper : No_Making
{
	template<unsigned N = 0, class MTP1, class MTP2, class...ARGS>
	static decltype(auto) calc(MTP1&& mtp1, MTP2&& mtp2, Multiple<ARGS...>& amtp)
	{
		if constexpr(N == amtp.DIMENSION)
			return Forward_as_Multiple( std::move(mtp1), std::move(mtp2) ).hardened();
		else if constexpr(N < D1)
			return
			calc<N + 1>( mtp1 + Forward_as_Multiple(amtp.forward<N>()), std::move(mtp2), amtp );
		else
			return
			calc<N + 1>( std::move(mtp1), mtp2 + Forward_as_Multiple(amtp.forward<N>()), amtp );
	}
};


template<unsigned D1, unsigned D2, class...ARGS>
static auto sgm::fp0::Forward_as_2FMTP(ARGS&&...args)
{
	auto amtp = Forward_as_Flat_MTP( std::forward<ARGS>(args)... );

	static_assert(D1 + D2 == amtp.DIMENSION);

	return _2FMTP_Helper<D1>::calc(Multiple(), Multiple(), amtp);
}


#endif // end of #ifndef _SGM_MULTIPLE_