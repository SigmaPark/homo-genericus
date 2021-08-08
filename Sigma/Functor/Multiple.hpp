#pragma once

#ifndef _SGM_MULTIPLE_
#define _SGM_MULTIPLE_


#include "..\Type_Analysis\Type_Analysis.hpp"
#include "..\Family\Family.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm::fp
{
	
	template<class...TYPES>
	class Multiple;


	template<class...ARGS>
	static auto Make_Multiple(ARGS&&...args);

	template<class...ARGS>
	static auto Forward_as_Multiple(ARGS&&...args);


	SGM_USER_DEFINED_TYPE_CHECK14(_, Multiple, class...TYPES, TYPES...);


	template<class T, class...ARGS>
	static decltype(auto) Make_Flat_MTP(T&& t, ARGS&&...args);

	template<class T, class...ARGS>
	static decltype(auto) Forward_as_Flat_MTP(T&& t, ARGS&&...args);


	template<unsigned Di>
	struct _2FMTP_Helper;

	template<unsigned D1, unsigned D2, class...ARGS>
	static auto Forward_as_2FMTP(ARGS&&...args);


	template<class F, class...TYPES>
	static decltype(auto) Apply(F&& f, Multiple<TYPES...>& mtp);

	template<class F, class...TYPES>
	static decltype(auto) Apply(F&& f, Multiple<TYPES...>&& mtp);

	template<class F, class...TYPES>
	static decltype(auto) Apply(F&& f, Multiple<TYPES...> const& mtp);

	template<class...TYPES>
	struct _Apply_Helper;


	template< template<class...> class TC >
	struct Type_Check_by;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template< template<class...> class TC >
struct sgm::fp::Type_Check_by : No_Making
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
class sgm::fp::Multiple
{
	template<class...> struct _Merge_Helper;

	static unsigned constexpr _ULMAX = 0xffffffffUL;

public:
	using fam_t = Family<TYPES...>;
	static auto constexpr DIMENSION = sizeof...(TYPES);


	Multiple(TYPES...types) : _fam(  fam_t( Forward<TYPES>(types)... )  ){}


	auto operator*() const&-> fam_t const&{  return _fam;  }
	auto operator*() &-> fam_t&{  return _fam;  }
	auto operator*() &&-> fam_t&&{  return Move(_fam);  }


	template<unsigned IDX> decltype(auto) get(){  return std::get<IDX>(_fam);  }
	template<unsigned IDX> decltype(auto) get() const{  return std::get<IDX>(_fam);  }


	template<unsigned IDX = _ULMAX>
	decltype(auto) forward()
	{	
		if constexpr(IDX == _ULMAX)
			return _forward();
		else
			return static_cast< Nth_t<IDX, TYPES...> >(get<IDX>());
	}

	template<unsigned IDX = _ULMAX>
	decltype(auto) forward() const
	{
		if constexpr(IDX == _ULMAX)
			return _forward();
		else
		{
			using elem_t = Nth_t<IDX, TYPES...>;

			static_assert(!is_LvalueReference<elem_t>::value);
			
			return static_cast<elem_t>(get<IDX>());
		}
	}


	auto hardened(){  return _hardened();  }

	auto hardened() const
	{
		static_assert(Type_Check_by<is_RvalueReference>::for_none_v<TYPES...>);

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
		return _Merge_Helper<TYPES2...>::calc( *this, Move(mtp) );
	}


private:
	fam_t _fam;


	template<class...ARGS>
	auto _hardened(ARGS&&...args)
	{
		if constexpr
		(	auto constexpr tu_size = sizeof...(TYPES), nof_args = sizeof...(ARGS)
		;	tu_size == nof_args
		)
			return Multiple< Alephless_t<TYPES>... >( Forward<ARGS>(args)... );
		else
			return
			_hardened
			(	Forward<ARGS>(args)...
			,	static_cast<  Alephless_t< Nth_t<nof_args, TYPES...> >&&  >(get<nof_args>())
			);
	}


	template<class...ARGS>
	auto _forward(ARGS&&...args)
	{
		if constexpr
		(	auto constexpr tu_size = sizeof...(TYPES), nof_args = sizeof...(ARGS)
		;	tu_size == nof_args
		)
			return Multiple<TYPES&&...>( Forward<ARGS>(args)... );
		else
		{	
			using elem_t = Nth_t<nof_args, TYPES...>;

			return 
			_forward( Forward<ARGS>(args)..., static_cast<elem_t&&>(get<nof_args>()) );
		}
	}


	template<class...TYPES2>
	struct _Merge_Helper : No_Making
	{
		template<class MTP1, class MTP2, class...ARGS>
		static auto calc(MTP1&& mtp1, MTP2&& mtp2, ARGS&&...args)
		{
			if constexpr
			(	auto constexpr nof_args = sizeof...(ARGS)
			;	nof_args == mtp1.DIMENSION + mtp2.DIMENSION
			)
				return Multiple<TYPES..., TYPES2...>( Forward<ARGS>(args)... );
			else if constexpr(nof_args < mtp1.DIMENSION)
				return
				calc
				(	Forward<MTP1>(mtp1), Forward<MTP2>(mtp2), Forward<ARGS>(args)...
				,	static_cast< decltype(mtp1.template forward<nof_args>())&& >
					(	mtp1.template get<nof_args>()
					)
				);
			else
			{
				auto constexpr idx = nof_args - mtp1.DIMENSION;

				return
				calc
				(	Forward<MTP1>(mtp1), Forward<MTP2>(mtp2), Forward<ARGS>(args)...	
				,	static_cast< decltype(mtp2.template forward<idx>())&& >
					(	mtp2.template get<idx>()
					)
				);
			}
		}
	};

}; // end of class sgm::fp0::Multiple<TYPES...>
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...ARGS>
auto sgm::fp::Make_Multiple(ARGS&&...args)
{
	return
	Multiple< Referenceless_t<ARGS>... >( static_cast< Referenceless_t<ARGS> >(args)... );
}

template<class...ARGS>
auto sgm::fp::Forward_as_Multiple(ARGS&&...args)
{
	return Multiple<decltype(args)...>( static_cast<decltype(args)>(args)... );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, class...ARGS>
decltype(auto) sgm::fp::Make_Flat_MTP(T&&t, [[maybe_unused]]ARGS&&...args)
{
	if constexpr( sizeof...(ARGS) > 0 )
		return Make_Flat_MTP( Forward<T>(t) ) + Make_Flat_MTP( Forward<ARGS>(args)... );
	else if constexpr(is_Multiple_v<T>)
		return Forward<T>(t);
	else
		return Make_Multiple( Forward<T>(t) );
}


template<class T, class...ARGS>
decltype(auto) sgm::fp::Forward_as_Flat_MTP(T&& t, [[maybe_unused]]ARGS&&...args)
{
	if constexpr( sizeof...(ARGS) > 0 )
		return
		(	Forward_as_Flat_MTP( Forward<T>(t) )
		+	Forward_as_Flat_MTP( Forward<ARGS>(args)... )
		);
	else if constexpr(is_Multiple_v<T>)
		return t.forward();
	else
		return Forward_as_Multiple( Forward<T>(t) );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<unsigned D1>
struct sgm::fp::_2FMTP_Helper : No_Making
{
	template<unsigned N = 0, class MTP1, class MTP2, class...ARGS>
	static decltype(auto) calc(MTP1&& mtp1, MTP2&& mtp2, Multiple<ARGS...>& amtp)
	{
		if constexpr(N == amtp.DIMENSION)
			return Forward_as_Multiple( Move(mtp1), Move(mtp2) ).hardened();
		else if constexpr(N < D1)
			return
			calc<N + 1>
			(	mtp1 + Forward_as_Multiple(amtp.template forward<N>()), Move(mtp2), amtp 
			);
		else
			return
			calc<N + 1>
			(	Move(mtp1), mtp2 + Forward_as_Multiple(amtp.template forward<N>()), amtp 
			);
	}
};


template<unsigned D1, unsigned D2, class...ARGS>
static auto sgm::fp::Forward_as_2FMTP(ARGS&&...args)
{
	auto amtp = Forward_as_Flat_MTP( Forward<ARGS>(args)... );

	static_assert(D1 + D2 == amtp.DIMENSION);

	return _2FMTP_Helper<D1>::calc(Multiple(), Multiple(), amtp);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class F, class...TYPES>
decltype(auto) sgm::fp::Apply(F&& f, Multiple<TYPES...>& mtp)
{
	return _Apply_Helper<TYPES...>::calc( Forward<F>(f), mtp );
}

template<class F, class...TYPES>
decltype(auto) sgm::fp::Apply(F&& f, Multiple<TYPES...>&& mtp)
{
	return _Apply_Helper<TYPES...>::calc( Forward<F>(f), Move(mtp) );
}

template<class F, class...TYPES>
decltype(auto) sgm::fp::Apply(F&& f, Multiple<TYPES...> const& mtp)
{
	return _Apply_Helper<TYPES...>::calc( Forward<F>(f), mtp );
}


template<class...TYPES>
struct sgm::fp::_Apply_Helper
{
	template<class F, class MTP, class...ARGS>
	static decltype(auto) calc(F&& f, [[maybe_unused]]MTP&& mtp, ARGS&&...args)
	{
		if constexpr( auto constexpr IDX = sizeof...(ARGS);  sizeof...(TYPES) == IDX )
			return f( Forward<ARGS>(args)... );
		else
			return
			calc
			(	Forward<F>(f), Forward<MTP>(mtp)
			,	Forward<ARGS>(args)..., mtp.template get<IDX>()
			);
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#endif // end of #ifndef _SGM_MULTIPLE_