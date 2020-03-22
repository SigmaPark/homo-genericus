#pragma once

#ifndef _SGM_TYPE_ANALYSIS_
#define _SGM_TYPE_ANALYSIS_

#include <type_traits>

namespace sgm
{


	#ifndef SGM_DECLTYPE_AUTO
		#define SGM_DECLTYPE_AUTO(...)	-> decltype(__VA_ARGS__){  return __VA_ARGS__;  }
	#endif

	struct No_Making{  No_Making() = delete;  };
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
	struct _Provided : No_Making
	{
		static_assert(B, "type condition failed.");

		using type = T;
	};

	template<bool B, class T> using Provided_t = typename _Provided<B, T>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


#ifdef _ITERATOR_
	template<class> using _Void_t = void;

	template <class T, class = void> struct is_iterator : public std::false_type{};

	template <class T>
	struct is_iterator<  T, _Void_t< typename std::iterator_traits<T>::iterator_category >  > 
	:	public std::true_type 
	{};
#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	struct is_immutable : No_Making
	{
		enum : bool
		{	value 
			=	std::is_const
				<	std::conditional_t
					<	std::is_pointer<T>::value
					,	std::remove_pointer_t<T>
					,	std::remove_reference_t<T> 
					>
				>::value
		};
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T1, class T2>
	struct are_mutually_convertible : No_Making
	{
		enum : bool
		{	value = std::is_convertible<T1, T2>::value && std::is_convertible<T2, T1>::value
		};
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	//	same to std::declval in <utility>
	template<class T> static auto Declval()
	->	std::decay_t<T>{  return *(std::decay_t<T>*)nullptr;  }


	template<class CON>
	using Elem_t = decltype(*Declval<CON>().begin());

	template<class CON>
	using RawElem_t = std::decay_t< Elem_t<CON> >;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	struct Has_Type : No_Making
	{
		template<class...> class among;

		template<class Q, class...TYPES>
		class among<Q, TYPES...>
		{
		public: enum : bool{value = std::is_same<T, Q>::value ? true : among<TYPES...>::value};
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
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	struct Mutability : No_Making{};
	struct invariable : Mutability{};		using invar = invariable;
	struct Variable :	Mutability{};			using Var = Variable;

	template<class T>
	struct is_Mutability : No_Making
	{
		enum : bool{value = std::is_base_of<Mutability, T>::value};
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


}

#endif