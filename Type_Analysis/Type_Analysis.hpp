#pragma once

#ifndef _SGM_TYPE_ANALYSIS_
#define _SGM_TYPE_ANALYSIS_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version of language support is required.
#endif


namespace sgm
{
	

	struct No_Making{  No_Making() = delete;  };


	template<class...> using Void_t = void;


	enum class None_t{NONE};
	static auto const None = None_t::NONE;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<bool B>
	struct Boolean_type : No_Making
	{
		enum : bool{value = B};  
		using type = Boolean_type;
	};

	using False_t = Boolean_type<false>;
	using True_t = Boolean_type<true>;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T> struct _Referenceless : No_Making{  using type = T;  };
	template<class T> struct _Referenceless<T&> : _Referenceless<T>{};
	template<class T> struct _Referenceless<T&&> : _Referenceless<T>{};

	template<class T> using Referenceless_t = typename _Referenceless<T>::type;


	template<class T> struct _Pointerless : No_Making{  using type = T;  };
	template<class T> struct _Pointerless<T*> : _Pointerless<T>{};
	template<class T> struct _Pointerless<T* const> : _Pointerless<T>{};

	template<class T> using Pointerless_t = typename _Pointerless<T>::type;


	template<class T> struct _Constless : No_Making{  using type = T;  };
	template<class T> struct _Constless<T const> : _Constless<T>{};

	template<class T> using Constless_t = typename _Constless<T>::type;


	template<class T> struct _Volatileless : No_Making{  using type = T;  };
	template<class T> struct _Volatileless<T volatile> : _Volatileless<T>{};

	template<class T> using Volatileless_t = typename _Volatileless<T>::type;


	template<class T> using Decay_t = Volatileless_t<  Constless_t< Referenceless_t<T> >  >;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class, class> struct isSame : False_t{};
	template<class T> struct isSame<T, T> : True_t{};


	template<class T>
	struct isVoid : isSame<T, void>{};


	template<class T>
	struct isNone : isSame<T, None_t>{};


	template<class T>
	struct isConst : Boolean_type<  !isSame< T, Constless_t<T> >::value  >{};


	template<class T> struct isLvalueReference : False_t{};
	template<class T> struct isLvalueReference<T&> : True_t{};

	template<class T> struct isRvalueReference : False_t{};
	template<class T> struct isRvalueReference<T&&> : True_t{};

	template<class T> 
	struct isReference 
	:	Boolean_type< isLvalueReference<T>::value || isRvalueReference<T>::value >
	{};


	template<class T>
	struct isVolatile : Boolean_type<  !isSame< T, Volatileless_t<T> >::value  >{};


	template<class T>
	struct isPointer : Boolean_type<  !isSame< T, Pointerless_t<T> >::value  >{};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	static auto Declval()-> T { return *(Referenceless_t<T>*)nullptr; }
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<bool CONDITION, class, class> struct _Selective;
	
	template<class T1, class T2> 
	struct _Selective<true, T1, T2> : No_Making{  using type = T1;  };
	
	template<class T1, class T2> 
	struct _Selective<false, T1, T2> : No_Making{  using type = T2;  };

	template<bool CONDITION, class T1, class T2>
	using Selective_t = typename _Selective<CONDITION, T1, T2>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<bool B, class T = void> struct Enable_if : No_Making{};
	template<class T> struct Enable_if<true, T> : No_Making{  using type = T;  };

	template<bool B, class T = void>
	using Enable_if_t = typename Enable_if<B, T>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class FROM, class TO>
	struct isConvertible
	{
	private:
		template<class F, class T>
		static auto _calc(int)
		->	typename isPointer< decltype( static_cast<T>(Declval<F>()) )* >::type;

		template<class, class>
		static auto _calc(...)-> False_t;

	public:
		enum : bool
		{	value 
			=	(	decltype( _calc<FROM, TO>(0) )::value
				||	(isVoid<FROM>::value && isVoid<TO>::value)
				)
		};
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	struct isClass
	{
	private:
		template<class Q> static auto _calc(int Q::*)-> True_t;
		template<class> static auto _calc(...)-> False_t;

	public:
		enum : bool{value = decltype( _calc< Decay_t<T> >(0) )::value};
	};


	template<class DERV, class BASE>
	struct is_inherited_from
	{
	private:
		template<class B> static auto _calc(B const volatile*)-> True_t;
		template<class> static auto _calc(void const volatile*)-> False_t;


		template<class D, class B>
		static auto _test(int)-> decltype(  _calc<B>( (D*)nullptr )  );

		template<class, class>
		static auto _test(...)-> False_t;

	public:
		enum : bool
		{	value
			=	(	isClass<DERV>::value && isClass<BASE>::value
				&&	decltype( _test<DERV, BASE>(0) )::value
				)
		};
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class CON>
	using Deref_t = decltype(*Declval<CON>().begin());
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<int, class...TYPES> struct Nth_Type;

	template<int N> struct Nth_Type<N> : No_Making{  using type = void;  };

	template<int N, class T, class...TYPES>
	struct Nth_Type<N, T, TYPES...> : No_Making
	{
		using type = Selective_t< N == 0, T, typename Nth_Type<N - 1, TYPES...>::type >;
	};


	template<int N, class...TYPES>
	using Nth_t = typename Nth_Type<N, TYPES...>::type;

	template<class...TYPES> 
	using First_t = Nth_t<0, TYPES...>;
	
	template<class...TYPES> 
	using Last_t = Nth_t<sizeof...(TYPES) - 1, TYPES...>;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<bool B, class T> 
	struct _Guaranteed : No_Making
	{
		static_assert(B, "type guarantee failed.");

		using type = T;
	};

	template<bool B, class T = void> using Guaranteed_t = typename _Guaranteed<B, T>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	struct invariable : No_Making{};		using invar = invariable;
	struct Variable : No_Making{};		using Var = Variable;

	template<class T>
	struct is_Mutability : Boolean_type< isSame<T, invar>::value || isSame<T, Var>::value >{};


	template< class T, bool = isPointer<T>::value || isReference<T>::value >
	struct is_immutable;


	template<class T>
	struct is_immutable<T, false> : isConst<T>{};


	template<class T>
	struct is_immutable<T, true> : is_immutable<  Pointerless_t< Referenceless_t<T> >  >{};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


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
		template<class Q> struct _Same_to_T : is_inherited_from<Q, T>{};

	public:
		template<class...TYPES>
		struct among : Check_All<_Same_to_T>::template for_any<TYPES...>{};
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T>
	static auto Forward(Referenceless_t<T>& t)-> T&&{  return static_cast<T&&>(t);  }

	template<class T>
	static auto Forward(Referenceless_t<T>&& t) throw()-> T&&{  return static_cast<T&&>(t);  }


	template< class T, class Res_t = Referenceless_t<T>&& >
	static auto Move(T&& t) throw()-> Res_t{  return static_cast<Res_t>(t);  }
	//========//========//========//========//=======#//========//========//========//========//===


}

#endif