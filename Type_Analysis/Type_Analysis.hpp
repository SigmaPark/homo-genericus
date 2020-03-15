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


#ifdef _ITERATOR_
	template<class> using _Void_t = void;

	template <class T, class = void> class is_iterator : public std::false_type{};

	template <class T>
	class is_iterator<	 T, _Void_t< typename std::iterator_traits<T>::iterator_category >  > 
	:	public std::true_type 
	{};
#endif
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

	
	template< template<class __Query_t, class __CONST_IF_FALSE, class...> class PROXY >
	class DirectProxy : No_Making
	{
	private:
		template<class T>
		class Remove_CR_and_Proxy : No_Making
		{
			template<class Q> 
			struct _Raw
			{
				using type 
				=	std::conditional_t
					<	std::is_same< Q, std::decay_t<Q> >::value
					,	Q
					,	typename _Raw< std::decay_t<Q> >::type
					>;  
			};

			template<class Q, class...ARGS> 
			struct _Raw< PROXY<Q, ARGS...> >
			{
				using type = typename _Raw< std::decay_t<Q> >::type;  
			};

		
		public: using type = typename _Raw<T>::type;
		};		

		
		template<class T>
		class is_Proxy : No_Making
		{
		public: 
			enum : bool
			{	value 
				=	!std::is_same
					<	std::decay_t<T>, typename Remove_CR_and_Proxy<T>::type 
					>::value
			};
		};


	protected:
		template
		<	class T
		,	class CONST_IF_FALSE // but not necessarily std::true_type when mutable
			=	std::conditional_t< is_immutable<T>::value, std::false_type, std::true_type >
		,	class = std::conditional_t< is_Proxy<T>::value, std::true_type, std::false_type >
		,	class RawT = std::decay_t<T>
		>
		class Proxy;


		template<class proxy_t, class CONST_IF_FALSE, class RawT>
		class Proxy<proxy_t, CONST_IF_FALSE,	std::true_type, RawT> : public RawT
		{
		public: template<class Q> Proxy(Q&& q) : RawT( std::forward<Q>(q) ){}
		};


		template<class T, class CONST_IF_FALSE, class RawT>
		class Proxy<T, CONST_IF_FALSE, std::false_type, RawT>
		{
		public:
			using Self_t = Proxy;
			using type = RawT;

			enum : bool{MAYBE_MUTABLE = CONST_IF_FALSE::value};

			Proxy(type const&){}
			Proxy(type&&){}
		};
		

	};







	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

}

#endif