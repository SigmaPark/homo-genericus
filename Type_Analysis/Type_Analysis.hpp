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
	using Elem_t = decltype(*Declval<CON>().begin());
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<  class T, class = std::enable_if_t< std::is_integral<T>::value >  >
	struct Size_info : No_Making
	{
	private:
	#pragma warning(push)
	#pragma warning(disable : 4348)
		template< class Q, unsigned = sizeof(Q), bool = std::is_unsigned<Q>::value > 
		struct _info;
	#pragma warning(pop)

		template<class Q> struct _info<Q, 8, true>
		{
			enum : T{MAXIMUM = 0xffffffffffffffffui64, MINIMUM = 0};
		};

		template<class Q> struct _info<Q, 4, true>
		{
			enum : T{MAXIMUM = 0xffffffffUL, MINIMUM = 0};
		};

		template<class Q> struct _info<Q, 2, true>
		{
			enum : T{MAXIMUM = 0xffffui16, MINIMUM = 0}; 
		};

		template<class Q> struct _info<Q, 1, true>
		{
			enum : T{MAXIMUM = 0xffui8, MINIMUM = 0}; 
		};

		template<class Q> struct _info<Q, 8, false>
		{  
			enum : T{MAXIMUM = 0x7fffffffffffffffi64, MINIMUM = -0x8000000000000000i64}; 
		};

		template<class Q> struct _info<Q, 4, false>
		{
			enum : T{MAXIMUM = 0x7fffffffi32, MINIMUM = -0x80000000i32};
		};

		template<class Q> struct _info<Q, 2, false>
		{
			enum : T{MAXIMUM = 0x7fffi16, MINIMUM = -0x8000i16}; 
		};

		template<class Q> struct _info<Q, 1, false>
		{
			enum : T{MAXIMUM = 0x7fi8, MINIMUM = -0x80i8}; 
		};


	public:
		enum : T{MAXIMUM = _info<T>::MAXIMUM, MINIMUM = _info<T>::MINIMUM};
	};
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


#ifndef SGM_COMPILE_FAILED
	#define SGM_COMPILE_FAILED(...) \
		static_assert([]() constexpr{  return false;  }(), #__VA_ARGS__)
#else
	#error SGM_COMPILE_FAILED was already defined somewhere else.
#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template< class T, bool = std::is_pointer<T>::value || std::is_reference<T>::value >
	struct is_immutable;


	template<class T>
	struct is_immutable<T, false> : std::is_const<T>{  is_immutable() = delete;  };


	template<class T>
	struct is_immutable<T, true>
	:	is_immutable<  std::remove_pointer_t< std::remove_reference_t<T> >  >
	{};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T1, class T2>
	struct are_mutually_convertible : No_Making
	{
		enum : bool
		{	value = std::is_convertible<T1, T2>::value && std::is_convertible<T2, T1>::value
		};
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	struct Has_Type : No_Making
	{
		template<class...> class among;

		template<class Q, class...TYPES>
		class among<Q, TYPES...>
		{
		public: 
			enum : bool
			{	value 
				=	std::is_base_of<T, Q>::value || std::is_same<T, Q>::value 
					?	true 
					:	among<TYPES...>::value
			};
		};

		template<> class among<> {  public: enum : bool{value = false}; };
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template< template<class...> class FUNCTOR >
	struct Check_All : No_Making
	{
		template<class...> struct for_all;

		template<class Q, class...TYPES>
		struct for_all<Q, TYPES...>
		{
			enum : bool{value = FUNCTOR<Q>::value ? for_all<TYPES...>::value : false};
		};

		template<> struct for_all<>{  enum : bool{value = true};  };
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<class...> struct for_any;

		template<class Q, class...TYPES>
		struct for_any<Q, TYPES...>
		{
			enum : bool{value = FUNCTOR<Q>::value ? true : for_any<TYPES...>::value};
		};

		template<> struct for_any<>{  enum : bool{value = false};  };
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