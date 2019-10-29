#pragma once

#ifndef _SGM_FUNCTOR_
#define _SGM_FUNCTOR_

#ifdef _MSC_VER
	static_assert(_MSC_VER >= 1914, "C++17 or higher version language support is required.");
#endif

#include <type_traits>
#include <memory>
#include <tuple>

#ifndef LAMBDIZE
#define LAMBDIZE(TemFunc, ...)	\
	[](auto&&...args){  return TemFunc __VA_ARGS__ (std::forward<decltype(args)>(args)...);  }
#else
	static_assert(false, "macro LAMBDIZE is already defined elsewhere.");
#endif

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{

	template<size_t D, typename F>
	class Functor;


	//	함수의 매개변수 개수를 Functor객체에 입력하여 생성시킨다.
	template<size_t D>
	class Dimension {};

	template<size_t D>
	static auto constexpr Dim = Dimension<D>();
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	class Functor_t
	{
	protected:
		Functor_t() = default;
	};


	template<typename FTR>
	static bool constexpr is_Functor_v 
	=	std::is_base_of_v< Functor_t, std::decay_t<FTR> >;
	//========//========//========//========//=======#//========//========//========//========//===


	template<size_t D, typename G> 
	decltype(auto) constexpr operator/(G&& g, Dimension<D>)
	{
		return Functor<D, G>( std::forward<G>(g) );
	}
	//========//========//========//========//=======#//========//========//========//========//===


	//	튜플타입을 구별해내기 위한 템플릿 장치
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


	//	하나 이상의 리턴값을 표현하기 위한 객체.
	class Flat_Pack_t
	{
	protected:
		Flat_Pack_t() = default;
	};


	template<typename FPK>
	static bool constexpr is_Flat_Pack_v = std::is_base_of_v< Flat_Pack_t, std::decay_t<FPK> >;


	template<typename>
	class Flat_Pack;


	template<typename T>
	static auto make_fpack(T&& t)
	{
		if constexpr(is_Flat_Pack_v<T>)
			return Flat_Pack(t());
		else if constexpr(is_tuple_v<T>)
			return Flat_Pack( std::move(t) );
		else
			return Flat_Pack( std::make_tuple(t) );
	}


	template<typename TU>
	class Flat_Pack : public Flat_Pack_t
	{
	public:
		using tuple_t = TU;

		Flat_Pack(TU const& tu) : _tu(tu){}
		Flat_Pack(TU&& tu) : _tu( std::move(tu) ){}

		decltype(auto) operator()() const{  return _tu;  }

		template<typename FP>
		decltype(auto) operator+(FP&& fpack)
		{
			static_assert(is_Flat_Pack_v<FP>);
			
			return make_fpack( std::tuple_cat(_tu, fpack()) );
		}

	private:
		TU const _tu;
	};
	//========//========//========//========//=======#//========//========//========//========//===


	class Tupling_Helper_t
	{
	public:
		Tupling_Helper_t() = delete;

		enum class Curry{ NONE, FRONT, REAR };
	};


	template<size_t N, Tupling_Helper_t::Curry C>
	class Tupling_Helper
	{
	public:
		Tupling_Helper() = delete;

		template<typename SPF, typename TU, typename...ARGS>
		static decltype(auto) constexpr calc(SPF&& spf, TU&& tu, ARGS&&...args)
		{
			return
			Tupling_Helper<N - 1, C>::calc
			(	std::forward<SPF>(spf), std::forward<TU>(tu)
			,	std::get<N - 1>( std::forward<TU>(tu) ), std::forward<ARGS>(args)...
			);
		}
	};


	template<Tupling_Helper_t::Curry C>
	class Tupling_Helper<0, C>
	{
	public:
		Tupling_Helper() = delete;

		template<typename SPF, typename TU, typename...ARGS>
		static decltype(auto) constexpr calc(SPF&& spf, TU&&, ARGS&&...args)
		{
			if constexpr(C == Tupling_Helper_t::Curry::NONE)
				return (*spf)( std::forward<ARGS>(args)... );
			else if constexpr(C == Tupling_Helper_t::Curry::FRONT)
				return 
				[spf, args...](auto&&...params)
				{
					return (*spf)( args..., std::forward<decltype(params)>(params)... );
				};
			else if constexpr(C == Tupling_Helper_t::Curry::REAR)
				return
				[spf, args...](auto&&...params)
				{
					return (*spf)( std::forward<decltype(params)>(params)..., args... );
				};
			else
				static_assert(false, "no suitable action is found.");
		}
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<size_t NOF_PART, size_t S, size_t...SIZES>
	class Partition_Helper
	{
	public:
		Partition_Helper() = delete;

		template<typename TTU, typename TU, typename A, typename...ARGS>
		static decltype(auto) constexpr calc(TTU&& ttu, TU&& tu, A&& a, ARGS&&...args)
		{
			if constexpr(S == 0)
				return 
				Partition_Helper<NOF_PART - 1, SIZES...>::calc
				(	std::tuple_cat(  std::move(ttu), std::make_tuple( std::move(tu) )  )
				,	std::tuple()
				,	std::forward<A>(a), std::forward<ARGS>(args)...
				);
			else
				return
				Partition_Helper<NOF_PART, S - 1, SIZES...>::calc
				(	std::move(ttu)
				,	std::tuple_cat(  std::move(tu), std::make_tuple( std::forward<A>(a) )  )
				,	std::forward<ARGS>(args)...
				);
		}
	};


	template<>
	class Partition_Helper<1, 0>
	{
	public:
		Partition_Helper() = delete;

		template<typename TTU, typename TU>
		static decltype(auto) constexpr calc(TTU&& ttu, TU&& tu)
		{
			return std::tuple_cat(  std::move(ttu), std::make_tuple( std::move(tu) )  );
		}
	};


	template<size_t...SIZES>
	class Partition
	{
	public:
		Partition() = delete;

		template<typename...ARGS>
		static decltype(auto) constexpr calc(ARGS&&...args)
		{
			return
			Partition_Helper<sizeof...(SIZES), SIZES...>::calc
			(	std::tuple(), std::tuple(), std::forward<ARGS>(args)...
			);
		}
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<size_t D, typename F>
	class Functor : Functor_t
	{
	public:
		static_assert(D != 0, "dimensionless.");
		

		enum { DIMENSION = D };


		template<typename...ARGS>
		decltype(auto) operator()(ARGS&&...args) const
		{
			static_assert( D == sizeof...(ARGS), "dimension mismatched." );

			return (*_spf)( std::forward<ARGS>(args)... );
		}


		template<typename...ARGS>
		decltype(auto) front(ARGS&&...args) const
		{
			static_assert( D > sizeof...(ARGS), "function over estimation." );

			return
			[cpy_spf = _spf, args...](auto&&...params)
			{
				return (*cpy_spf)( args..., std::forward<decltype(params)>(params)... );
			} / Dim<D - sizeof...(ARGS)>;
		}


		template<typename...ARGS>
		decltype(auto) rear(ARGS&&...args) const
		{
			static_assert( D > sizeof...(ARGS), "function over estimation." );

			return
			[cpy_spf = _spf, args...](auto&&...params)
			{
				return (*cpy_spf)( std::forward<decltype(params)>(params)..., args... );
			} / Dim<D - sizeof...(ARGS)>;
		}


		template<typename PACK>
		decltype(auto) operator[](PACK&& pack) const
		{
			if constexpr (is_Flat_Pack_v<PACK>)
				return
				Tupling_Helper
				<	std::tuple_size< std::decay_t<typename PACK::tuple_t> >::value
				,	Tupling_Helper_t::Curry::NONE
				>
				::	calc( _spf, std::forward<PACK>(pack()) );			
			else if constexpr (is_tuple_v<PACK>)
				return
				Tupling_Helper
				<	std::tuple_size< std::decay_t<PACK> >::value, Tupling_Helper_t::Curry::NONE
				>
				::	calc( _spf, std::forward<PACK>(pack) );
			else
				static_assert(false, "PACK is not sgm::Flat_Pack or tuple type.");
		}


		template<typename TU>
		decltype(auto) front_tuple(TU&& tu) const
		{
			static_assert(is_tuple_v<TU>, "TU is not tuple type.");

			size_t constexpr tu_size = std::tuple_size< std::decay_t<TU> >::value;

			return
			Tupling_Helper<tu_size, Tupling_Helper_t::Curry::FRONT>
			::	calc( _spf, std::forward<TU>(tu) ) / Dim<D - tu_size>;
		}


		template<typename TU>
		decltype(auto) rear_tuple(TU&& tu) const
		{
			static_assert(is_tuple_v<TU>, "TU is not tuple type.");

			size_t constexpr tu_size = std::tuple_size< std::decay_t<TU> >::value;

			return
			Tupling_Helper<tu_size, Tupling_Helper_t::Curry::REAR>
			::	calc( _spf, std::forward<TU>(tu) ) / Dim<D - tu_size>;
		}


		template<typename FTR>
		decltype(auto) operator<=(FTR&& ftr) const
		{
			static_assert
			(	is_Functor_v<FTR>
			,	"sgm::Functor type is needed after sgm::Functor::operator<="
			);

			return
			[clone = *this, ftr](auto&&...args)
			{
				if constexpr (D == 1)
					return clone(  ftr( std::forward<decltype(args)>(args)... )  );
				else
					return clone[ftr( std::forward<decltype(args)>(args)... )];
			} / Dim< std::decay_t<FTR>::DIMENSION >;
		}
	

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
				auto&& [tu1, tu2]
				=	Partition< D, std::decay_t<FTR>::DIMENSION >
					::	calc( std::forward<decltype(args)>(args)... );

				return
				make_fpack( clone[std::move(tu1)] ) + make_fpack( ftr[std::move(tu2)] );
			} / Dim< D + std::decay_t<FTR>::DIMENSION >;
		}


	private:
		template<size_t _D, typename G>
		friend decltype(auto) constexpr operator/(G&&, Dimension<_D>);
		
		Functor(F&& f) : _spf( std::make_shared<F const>(f) )
		{
			static_assert
			(	!is_Functor_v<F>
			,	"copy/move constructor of sgm::Functor didn't work properly."
			);
		}

		std::shared_ptr<F const> const _spf;
	};
	//========//========//========//========//=======#//========//========//========//========//===



	template<typename T, typename F, size_t D>
	decltype(auto) constexpr operator^(T&& t, sgm::Functor<D, F>& ftr)
	{
		static_assert(D >= 2, "the dimension of sgm::Functor is insufficient.");

		return _infixer_Helper::make_infixer(  ftr.front( std::forward<T>(t) )  );
	}


	template<size_t D, typename F>
	class _infixedFunctor : public Functor<D, F>
	{
	public:
		template<typename T>
		decltype(auto) operator^(T&& t) const &&
		{
			static_assert(D >= 1, "the dimension of sgm::Functor is insufficient.");

			if constexpr(D == 1)
				return (*this)( std::forward<T>(t) );
			else
				return this->front( std::forward<T>(t) );
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
		template<typename T, typename F, size_t D>
		friend decltype(auto) constexpr operator^(T&&, sgm::Functor<D, F>&);

		template<size_t D, typename F>
		static decltype(auto) make_infixer(Functor<D, F>&& ftr)
		{
			return _infixedFunctor<D, F>( std::move(ftr) );
		}
	};
	//========//========//========//========//=======#//========//========//========//========//===


}

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

#endif // End of #ifndef _SGM_FUNCTOR_