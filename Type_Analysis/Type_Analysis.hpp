#pragma once

#ifndef _SGM_TYPE_ANALYSIS_
#define _SGM_TYPE_ANALYSIS_

#include <type_traits>

namespace sgm
{


	#ifndef SGM_DECLTYPE_AUTO
		#define SGM_DECLTYPE_AUTO(...)	-> decltype(__VA_ARGS__){  return __VA_ARGS__;  }
	#endif

	class No_Making{  public: No_Making() = delete;  };
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	class _Pointerless : public No_Making
	{
		template<class Q> class rmv{  public: using type = Q;  };
		template<class Q> class rmv<Q*>{  public: using type = typename rmv<Q>::type;  };
		template<class Q> class rmv<Q* const>{  public: using type = typename rmv<Q>::type;  };

	public: using type = typename rmv<T>::type;
	};

	template<class T> using Pointerless_t = typename _Pointerless<T>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<bool B, class T> 
	class _Provided : public No_Making
	{
		static_assert(B, "type condition failed.");

	public: using type = T;
	};

	template<bool B, class T> using Provided_t = typename _Provided<B, T>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	class is_immutable : public No_Making
	{
	public: 
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
	class are_mutually_convertible : public No_Making
	{
	public:
		enum : bool
		{	value = std::is_convertible<T1, T2>::value && std::is_convertible<T2, T1>::value
		};
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	//	same to std::declval in <utility>
	template<class T> static auto Declval()
	->	std::decay_t<T>{  return *(std::decay_t<T>*)nullptr;  }
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	class Has_Type : public No_Making
	{
	public:
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
	class Check_All : public No_Making
	{
	public:
		template<class...> class for_all;

		template<class Q, class...TYPES>
		class for_all<Q, TYPES...>
		{
		public: enum : bool{value = FUNCTOR<Q>::value ? for_all<TYPES...>::value : false};
		};

		template<> class for_all<>{  public: enum : bool{value = true};  };
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<class...> class for_any;

		template<class Q, class...TYPES>
		class for_any<Q, TYPES...>
		{
		public: enum : bool{value = FUNCTOR<Q>::value ? true : for_any<TYPES...>::value};
		};

		template<> class for_any<>{  public: enum : bool{value = false}; };
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


}

#endif