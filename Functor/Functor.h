/**	FP style 1st-class citizen callable object, sgm::Functor
*/
#pragma once

#ifndef _SGM_FUNCTOR_
#define _SGM_FUNCTOR_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif

#include <type_traits>
#include <memory>
#include <tuple>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

#ifndef SGM_FUNCTOR
	/**	Macro function which converts template function into sgm::Functor

	*	You can use template-specializing signature < type1, type2, ... >
		as variadic parameters "..." 
	*/
	#define SGM_FUNCTOR(TemFunc, Dimension, ...)	\
	(	[](auto&&...args)		\
		{	\
			return TemFunc __VA_ARGS__ ( std::forward<decltype(args)>(args)... );	\
		}	\
	/	sgm::Dim<(Dimension)>	\
	)
#else
	#error macro SGM_FUNCTOR is already defined elsewhere.
#endif


////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{

	/**	An object as parameter placeholder argument
	*/
	class Blanker
	{
	private:
		template<typename...ARGS>
		static bool constexpr is_well_used()
		{
			if constexpr(unsigned constexpr nofBlank = _nofB<0, ARGS...>(); nofBlank == 0)
				return true;
			else if constexpr(nofBlank > 1)
				return false;
			else if constexpr(has_front_blank_v<ARGS...> || has_rear_blank_v<ARGS...>)
				return true;
			else
				return false;
		}


		template<unsigned N, typename T, typename...TYPES>
		static unsigned constexpr _nofB()
		{
			if constexpr(std::is_same_v< Blanker, std::decay_t<T> >)
				return _nofB<N + 1, TYPES...>();
			else
				return _nofB<N, TYPES...>();
		}

		template<unsigned N>
		static unsigned constexpr _nofB()
		{
			return N;
		}


	public:
		template<typename...ARGS>
		static bool constexpr has_front_blank_v
		=	std::is_same_v	
			<	Blanker, std::decay_t<  std::tuple_element_t< 0, std::tuple<ARGS...> >  >
			>;


		template<typename...ARGS>
		static bool constexpr has_rear_blank_v
		=	std::is_same_v
			<	Blanker
			,	std::decay_t
				<	std::tuple_element_t< sizeof...(ARGS) - 1, std::tuple<ARGS...> >  
				>
			>;


		template<typename...ARGS>
		static bool constexpr is_well_used_v = is_well_used<ARGS...>();
	};


	//	pre-defined constexpr for placeholder
	inline static Blanker constexpr __;
	//========//========//========//========//=======#//========//========//========//========//===


	class Functor_t
	{
	protected:
		Functor_t() = default;
	};


	/**	Boolean const expression to varify if one is sgm::Functor type
	*/
	template<typename FTR>
	static bool constexpr is_Functor_v = std::is_base_of_v< Functor_t, std::decay_t<FTR> >;
	//========//========//========//========//=======#//========//========//========//========//===

	 
	class _Tuple_Detect_Helper
	{
	public:
		_Tuple_Detect_Helper() = delete;

		template<typename...TYPES>
		static bool constexpr calc(std::tuple<TYPES...> const*){  return true;  }

		template<typename T>
		static bool constexpr calc(T const*){  return false;  }
	};
	

	template<typename T>
	static bool constexpr is_tuple_v 
	=	_Tuple_Detect_Helper::calc( static_cast< std::decay_t<T> const* >(nullptr) );
	//========//========//========//========//=======#//========//========//========//========//===


	class Multiple_t
	{
	protected:
		Multiple_t() = default;
	};


	template<typename FPK>
	static bool constexpr is_Multiple_v = std::is_base_of_v< Multiple_t, std::decay_t<FPK> >;


	/**	Data structure to express multiple output from sgm::Functor

	*	Member operator() gives itself as tuple type.

	*	Member operator+ does concatenation like std::tuple_cat,
		i.e.,
			{ t1, t2 } + { t3, t4, t5 } 
		becomes
			{ t1, t2, t3, t4, t5 }
		.
	*/
	template<typename TU>
	class Multiple : Multiple_t
	{
	public:
		using tuple_t = TU;

		enum { DIMENSION = std::tuple_size_v< std::decay_t<TU> > };

		Multiple(TU const& tu) : _tu(tu){}
		Multiple(TU&& tu) : _tu( std::move(tu) ){}

		decltype(auto) operator()() const{  return _tu;  }

		template<typename MTP>
		decltype(auto) operator+(MTP&& mtp)
		{
			static_assert(is_Multiple_v<MTP>, "sgm::Multiple::operator+ failed.");
			
			return
			Multiple<decltype( std::tuple_cat(_tu, mtp()) )>( std::tuple_cat(_tu, mtp()) );
		}

	private:
		TU const _tu;
	};


	//	Builder for sgm::Multiple
	template<typename T, typename...TYPES>
	static decltype(auto) Params(T&& t, TYPES&&...types)
	{
		if constexpr( sizeof...(TYPES) > 0 )
			return Params( std::forward<T>(t) ) + Params( std::forward<TYPES>(types)... );
		else if constexpr(is_Multiple_v<T>)
			return Multiple(t());
		else if constexpr(is_tuple_v<T>)
			return Multiple( std::forward<T>(t) );
		else
			return Multiple(  std::make_tuple( std::forward<T>(t) )  );
	}
	//========//========//========//========//=======#//========//========//========//========//===


	/**	Template class which notifies dimension of Functor, the number of parameters

	*	Its template alias sgm::Dim for short.
	*/
	template<unsigned D, typename...ARGS>
	class Dimension 
	{
	public:
		Dimension(ARGS&&...args) : _tu(  std::make_tuple( std::forward<ARGS>(args)... )  ){}

		template<typename..._ARGS>
		decltype(auto) operator()(_ARGS&&..._args) const
		{
			return Dimension<D, _ARGS...>( std::forward<_ARGS>(_args)... );
		}

	private:
		template<unsigned _D, typename G, typename..._ARGS>
		friend decltype(auto) constexpr operator/(G&&, Dimension<_D, _ARGS...>);

		std::tuple<ARGS...> const _tu;
	};


	template<unsigned D>
	static auto const Dim = Dimension<D>();
	//========//========//========//========//=======#//========//========//========//========//===


	/**	Template class for sgm::Functor types

	*	If you want not to use "auto" declaration for readability, use this as type declaration. 

	*	Note that automatic type deduction for template class constructor is available
		from C++17 so that you might not have to write template signature explicitly,
		i.e., just
			sgm::Functor foo = .... ;
		is fine instead of
			sgm::Functor<DIGIT, F_t> foo = .... ;
		.
	*/
	template<unsigned D, typename F>
	class Functor;


	/**	Operator type builder for sgm::Functor

	*	This derives sgm::Functor from lambda or other callable objects with sgm::Dim<DIGIT>.
	*/
	template<unsigned D, typename G> 
	decltype(auto) constexpr operator/(G&& g, Dimension<D>)
	{
		return Functor<D, G>( std::forward<G>(g) );
	}


	/**	Building and Evaluation
	*/
	template<unsigned _D, typename G, typename..._ARGS>
	decltype(auto) constexpr operator/(G&& g, Dimension<_D, _ARGS...> d)
	{
		return (std::forward<G>(g) / Dim<_D>)( Params(d._tu) );
	}
	//========//========//========//========//=======#//========//========//========//========//===


	template<unsigned N>
	class _Multipling_Helper
	{
	public:
		_Multipling_Helper() = delete;

		template<typename F, typename MTP, typename...ARGS>
		static decltype(auto) constexpr calc(F&& f,[[maybe_unused]] MTP&& mtp, ARGS&&...args)
		{
			if constexpr(N == 0)
				return f( std::forward<ARGS>(args)... );
			else
				return
				_Multipling_Helper<N - 1>::calc
				(	std::forward<F>(f), std::forward<MTP>(mtp)
				,	std::get<N - 1>(mtp()), std::forward<ARGS>(args)...
				);
		}
	};


	template<typename F, typename MTP>
	static decltype(auto) constexpr Multipling(F&& f, MTP&& mtp)
	{
		return
		_Multipling_Helper< std::decay_t<MTP>::DIMENSION >::calc
		(	std::forward<F>(f), std::forward<MTP>(mtp)
		);
	}


	template<typename T, typename...TYPES>
	static bool constexpr _has_Multiple()
	{
		if constexpr(is_Multiple_v<T>)
			return true;
		else if constexpr( sizeof...(TYPES) == 0 )
			return false;
		else
			return _has_Multiple<TYPES...>();
	}


	template<typename...TYPES>
	static bool constexpr _has_Multiple_v = _has_Multiple<TYPES...>();
	//========//========//========//========//=======#//========//========//========//========//===


	template<unsigned NOF_PART, unsigned S, unsigned...SIZES>
	class _Partition_Helper
	{
	public:
		_Partition_Helper() = delete;

		template<typename TTU, typename TU, typename A, typename...ARGS>
		static decltype(auto) constexpr calc(TTU&& ttu, TU&& tu, A&& a, ARGS&&...args)
		{
			if constexpr(S == 0)
				return 
				_Partition_Helper<NOF_PART - 1, SIZES...>::calc
				(	std::tuple_cat(  std::move(ttu), std::make_tuple( std::move(tu) )  )
				,	std::tuple()
				,	std::forward<A>(a), std::forward<ARGS>(args)...
				);
			else
				return
				_Partition_Helper<NOF_PART, S - 1, SIZES...>::calc
				(	std::move(ttu)
				,	std::tuple_cat(  std::move(tu), std::make_tuple( std::forward<A>(a) )  )
				,	std::forward<ARGS>(args)...
				);
		}
	};


	template<>
	class _Partition_Helper<1, 0>
	{
	public:
		_Partition_Helper() = delete;

		template<typename TTU, typename TU>
		static decltype(auto) constexpr calc(TTU&& ttu, TU&& tu)
		{
			return std::tuple_cat(  std::move(ttu), std::make_tuple( std::move(tu) )  );
		}
	};


	template<unsigned...SIZES>
	class _Partition
	{
	public:
		_Partition() = delete;

		template<typename...ARGS>
		static decltype(auto) constexpr calc(ARGS&&...args)
		{
			return
			_Partition_Helper<sizeof...(SIZES), SIZES...>::calc
			(	std::tuple(), std::tuple(), std::forward<ARGS>(args)...
			);
		}
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<signed N>
	class _reverse_Params_Helper
	{
	public:
		_reverse_Params_Helper() = delete;

		template<typename TU, typename...ARGS>
		static decltype(auto) calc([[maybe_unused]] TU&& tu, ARGS&&...args)
		{
			if constexpr(N == 0)
				return Params( std::forward<ARGS>(args)... );
			else
				return
				_reverse_Params_Helper<N - 1>::calc
				(	std::move(tu)
				,	std::get<  std::tuple_size_v< std::decay_t<TU> > - N  >( std::move(tu) )
				,	std::forward<ARGS>(args)...
				);
		}
	};


	/**	Special Functor that passes N parameters as given

	*	The order of parameters becomes reverse if N is negative.
	*/
	template<signed N>
	static auto const Pass
	=	[](auto&&...args)
		{
			if constexpr(N >= 0)
				return Params( std::forward<decltype(args)>(args)... );
			else
				return
				_reverse_Params_Helper<-N>::calc
				(	std::make_tuple( std::forward<decltype(args)>(args)... )
				);
		} / Dim<(N >= 0 ? N : -N)>;
	//========//========//========//========//=======#//========//========//========//========//===


	/**	FP style 1st-class citizen callable object
	*/
	template<unsigned D, typename F>
	class Functor : Functor_t
	{
	public:
		static_assert(D != 0, "dimensionless.");

		//	The number of parameters it needs
		enum { DIMENSION = D };


		//	Evaluation
		template<typename...ARGS>
		decltype(auto) operator()(ARGS&&...args) const
		{
			if constexpr(_has_Multiple_v<ARGS...>)
				return
				Multipling
				(	[this](auto&&..._args)
					{
						return _eval( std::forward<decltype(_args)>(_args)... );
					}
				,	Params( std::forward<ARGS>(args)... )
				);
			else
				return _eval( std::forward<ARGS>(args)... );
		}


		//	Functional composition
		template<typename FTR>
		decltype(auto) operator|(FTR&& ftr) const
		{
			static_assert
			(	is_Functor_v<FTR>
			,	"sgm::Functor type is needed after sgm::Functor::operator|"
			);

			return
			[clone = *this, ftr](auto&&...args)
			{	
				return clone(  ftr( std::forward<decltype(args)>(args)... )  );
			} / Dim< std::decay_t<FTR>::DIMENSION >;
		}

	
		//	Merging two sgm::Functor objects into one which returns multiple output
		template<typename FTR>
		decltype(auto) operator+(FTR&& ftr) const
		{
			static_assert
			(	is_Functor_v<FTR>
			,	"sgm::Functor type is needed after sgm::Functor::operator+"
			);

			return
			[clone = *this, ftr](auto&&...args)
			{
				auto const [tu1, tu2]
				=	_Partition< D, std::decay_t<FTR>::DIMENSION >::calc
					(	std::forward<decltype(args)>(args)... 
					);

				return Params(  clone( Params(tu1) )  ) + Params(  ftr( Params(tu2) )  );
			} / Dim< D + std::decay_t<FTR>::DIMENSION >;
		}


	private:
		template<typename...ARGS>
		decltype(auto) _eval(ARGS&&...args) const
		{
			static_assert(Blanker::is_well_used_v<ARGS...>, "Blanker is misused.");

			if constexpr(Blanker::has_front_blank_v<ARGS...>)
				return _cut_front( std::forward<ARGS>(args)... );
			else if constexpr(Blanker::has_rear_blank_v<ARGS...>)
				return _cut_rear( std::forward<ARGS>(args)... );
			else if constexpr( sizeof...(ARGS) == D )
				return (*_spf)( std::forward<ARGS>(args)... );
			else
				static_assert(false, "no suitable evaluation method was found.");
		}


		//	Partial evaluation from front side
		template<typename...ARGS>
		decltype(auto) _push_front(ARGS&&...args) const
		{
			static_assert( D > sizeof...(ARGS), "function over estimation." );

			return
			[cpy_spf = _spf, args...](auto&&...params)
			{
				return (*cpy_spf)( args..., std::forward<decltype(params)>(params)... );
			} / Dim<D - sizeof...(ARGS)>;
		}


		//	Partial evaluation from rear side
		template<typename...ARGS>
		decltype(auto) _push_rear(ARGS&&...args) const
		{
			static_assert( D > sizeof...(ARGS), "function over estimation." );

			return
			[cpy_spf = _spf, args...](auto&&...params)
			{
				return (*cpy_spf)( std::forward<decltype(params)>(params)..., args... );
			} / Dim<D - sizeof...(ARGS)>;
		}


		template<typename T, typename...TYPES>
		decltype(auto) _cut_front(T&&, TYPES&&...types) const
		{
			return _push_rear( std::forward<TYPES>(types)... );
		}


		template<unsigned N, typename...TYPES, typename...ARGS>
		decltype(auto) _cut_rear_helper
		(	[[maybe_unused]] std::tuple<TYPES...>&& tu, ARGS&&...args
		)	const
		{
			if constexpr(N == 0)
				return _push_front( std::forward<ARGS>(args)... );
			else
				return
				_cut_rear_helper<N - 1>
				(	std::move(tu), std::get<N - 1>(tu), std::forward<ARGS>(args)...
				);
		}

		template<typename...TYPES>
		decltype(auto) _cut_rear(TYPES&&...types) const
		{
			return
			_cut_rear_helper<sizeof...(TYPES) - 1>
			(	std::make_tuple( std::forward<TYPES>(types)... )
			);
		}


		template<unsigned _D, typename G>
		friend decltype(auto) constexpr operator/(G&&, Dimension<_D>);
		
		/**	Private constructor
		
		*	operator/ can access here exclusively.

		*	Note that default copy/move constructors work separately as public
		*/
		Functor(F&& f) : _spf( std::make_shared<F const>(f) )
		{
			static_assert
			(	!is_Functor_v<F>
			,	"copy/move constructor of sgm::Functor didn't work properly."
			);
		}

		//	Callable object as shared_ptr
		std::shared_ptr<F const> const _spf;
	};
	//========//========//========//========//=======#//========//========//========//========//===

	class _infixer_Helper;


	/**	Infix operation maker

	*	Putting an sgm::Functor between two operator^ , 
		you can use the Functor as if it is an infixed binary operator.
		For example, with an sgm::Functor ftr,
			x ^ftr^ y 
		results 
			ftr(x, y) .
	*/
	template<typename T, typename F, unsigned D>
	decltype(auto) constexpr operator^(T&& t, sgm::Functor<D, F> const& ftr)
	{
		static_assert(D == 2, "not a binary operation.");

		return _infixer_Helper::make_infixer(  ftr( std::forward<T>(t), __ )  );
	}


	template<unsigned D, typename F>
	class _infixedFunctor : public Functor<D, F>
	{
	public:
		template<typename T>
		decltype(auto) operator^(T&& t) const &&
		{
			static_assert(D == 1, "the dimension of sgm::Functor is not matched.");

			return (*this)( std::forward<T>(t) );
		}

	private:
		friend class _infixer_Helper;

		_infixedFunctor(Functor<D, F>&& ftr) : Functor<D, F>( std::move(ftr) ){}
	};


	class _infixer_Helper
	{
	public:
		_infixer_Helper() = delete;

	private:
		template<typename T, typename F, unsigned D>
		friend decltype(auto) constexpr operator^(T&&, sgm::Functor<D, F> const&);

		template<unsigned D, typename F>
		static decltype(auto) make_infixer(Functor<D, F>&& ftr)
		{
			return _infixedFunctor<D, F>( std::move(ftr) );
		}
	};
	//========//========//========//========//=======#//========//========//========//========//===


}	// End of namespace sgm

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

#endif // End of #ifndef _SGM_FUNCTOR_