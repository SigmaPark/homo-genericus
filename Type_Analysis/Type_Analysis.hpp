#pragma once

#ifndef _SGM_TYPE_ANALYSIS_
#define _SGM_TYPE_ANALYSIS_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version of language support is required.
#endif


#include <type_traits>

namespace sgm
{
	

	struct No_Making{  No_Making() = delete;  };
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	//	same to std::declval in <utility>
	template<class T> 
	static auto Declval()-> T{  return *(std::remove_reference_t<T>*)nullptr;  }


	template<class CON>
	using Deref_t = decltype(*Declval<CON>().begin());
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<int, class...TYPES> struct Nth_Type;

	template<int N> struct Nth_Type<N> : No_Making{  using type = void;  };

	template<int N, class T, class...TYPES>
	struct Nth_Type<N, T, TYPES...> : No_Making
	{
		using type = std::conditional_t< N == 0, T, typename Nth_Type<N - 1, TYPES...>::type >;
	};


	template<int N, class...TYPES>
	using Nth_t = typename Nth_Type<N, TYPES...>::type;

	template<class...TYPES> 
	using First_t = Nth_t<0, TYPES...>;
	
	template<class...TYPES> 
	using Last_t = Nth_t<sizeof...(TYPES) - 1, TYPES...>;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	struct _Pointerless : No_Making
	{
	private:
		template<class Q> class rmv{  public: using type = Q;  };
		template<class Q> class rmv<Q*>{  public: using type = typename rmv<Q>::type;  };
		template<class Q> class rmv<Q* const>{  public: using type = typename rmv<Q>::type;  };

	public: using type = typename rmv<T>::type;
	};

	template<class T> using Pointerless_t = typename _Pointerless<T>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<bool B, class T> 
	struct _Guaranteed : No_Making
	{
		static_assert(B, "type guarantee failed.");

		using type = T;
	};

	template<bool B, class T = void> using Guaranteed_t = typename _Guaranteed<B, T>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	struct Mutability : No_Making{};
	struct invariable : Mutability{};		using invar = invariable;
	struct Variable :	Mutability{};			using Var = Variable;

	template<class T>
	struct is_Mutability : std::is_base_of<Mutability, T>, No_Making{};


	template< class T, bool = std::is_pointer<T>::value || std::is_reference<T>::value >
	struct is_immutable;


	template<class T>
	struct is_immutable<T, false> : std::is_const<T>, No_Making{};


	template<class T>
	struct is_immutable<T, true>
	:	is_immutable<  std::remove_pointer_t< std::remove_reference_t<T> >  >
	{};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	//	same to std::bool_constant in C++17
	template<bool B>
	struct Boolean_type : std::integral_constant<bool, B>, No_Making{};

	using False_t = Boolean_type<false>;
	using True_t = Boolean_type<true>;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T1, class T2>
	struct are_mutually_convertible 
	:	Boolean_type< std::is_convertible<T1, T2>::value && std::is_convertible<T2, T1>::value >
	{};


	template< template<class...> class FUNCTOR >
	struct Check_All : No_Making
	{
		template<class...> struct for_all;

		template<class Q, class...TYPES>
		struct for_all<Q, TYPES...>
		:	Boolean_type< FUNCTOR<Q>::value && for_all<TYPES...>::value >
		{};

		template<> struct for_all<> : True_t{};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template<class...> struct for_any;

		template<class Q, class...TYPES>
		struct for_any<Q, TYPES...>
		:	Boolean_type< FUNCTOR<Q>::value || for_any<TYPES...>::value >
		{};

		template<> struct for_any<> : False_t{};
	};


	template<class T>
	struct Has_Type : No_Making
	{
	private:
		template<class Q>
		struct _same_or_child 
		:	Boolean_type< std::is_base_of<T, Q>::value || std::is_same<T, Q>::value >
		{};

	public:
		template<class...TYPES>
		struct among : Check_All<_same_or_child>::template for_any<TYPES...>{};
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<bool TEMP_HOST> struct Move_if;

	template<>
	struct Move_if<false> : No_Making
	{
		template<class T>
		static auto cast(T&& t)-> decltype( std::forward<T>(t) ){  return std::forward<T>(t);  }
	};

	template<>
	struct Move_if<true> : No_Making
	{
		template<class T>
		static auto cast(T&& t)-> decltype( std::move(t) ){  return std::move(t);  }
	};
	//========//========//========//========//=======#//========//========//========//========//===


}

#endif