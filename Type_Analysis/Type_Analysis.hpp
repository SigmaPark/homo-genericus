#pragma once

#ifndef _SGM_TYPE_ANALYSIS_
#define _SGM_TYPE_ANALYSIS_

namespace sgm
{


	class No_Making{  public: No_Making() = delete;  };

	class True_Value	: public No_Making{  public:  enum : bool{ value = true };  };
	class False_Value	: public No_Making{  public:  enum : bool{ value = false }; };
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	class _Original_Helper : public No_Making
	{
		template<class Q> class _Original			{  public: using type = Q;  };
		template<class Q> class _Original<Q const>	{  public: using type = Q;  };
		template<class Q> class _Original<Q&>		{  public: using type = Q;  };
		template<class Q> class _Original<Q const&>	{  public: using type = Q;  };
		template<class Q> class _Original<Q&&>		{  public: using type = Q;  };

	public: using type = typename _Original<T>::type;
	};

	//	Similar to std::decay_t
	template<class T> using _Original_t = typename _Original_Helper<T>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T1, class T2> class is_Same : public False_Value{};
	template<class T> class is_Same<T, T> : public True_Value{};


	template<class T1, class T2>
	class have_Same_Origin : public is_Same< _Original_t<T1>, _Original_t<T2> >{};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
	

	template<class T>
	class Remove_pointer : public No_Making
	{
		template<class Q> class rmv{  public: using type = Q;  };
		template<class Q> class rmv<Q*>{  public: using type = typename rmv<Q>::type;  };
		template<class Q> class rmv<Q* const>{  public: using type = typename rmv<Q>::type;  };

	public: using type = typename rmv<T>::type;
	};

	template<class T> using Remove_pointer_t = typename Remove_pointer<T>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T> static auto Declval()-> T{  return *(T*)nullptr;  }
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class F, class...ARGS>
	class _Result_of : public No_Making
	{
	public: using type = decltype( Declval<F>()( Declval<ARGS>()... )  );
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
	

	template<bool B, class true_t, class false_t> class _Selective;

	template<class true_t, class false_t>
	class _Selective<true, true_t, false_t> : public No_Making
	{ 
	public: using type = true_t;  
	};

	template<class true_t, class false_t>
	class _Selective<false, true_t, false_t> : public No_Making
	{
	public: using type = false_t;
	};

	//	Similar to std::conditional_t
	template<bool B, class true_t, class false_t>
	using _Selective_t = typename _Selective<B, true_t, false_t>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	class _Ref_Traits_Helper : public No_Making
	{
	private:
		template<class Q> 
		class _CVL			{  public: enum : bool{C = false, V = true, L = true};	};
		template<class Q> 		
		class _CVL<Q const>	{  public: enum : bool{C = true, V = true, L = true};		};
		template<class Q> 		 
		class _CVL<Q&>		{  public: enum : bool{C = false, V = false, L = true};	};
		template<class Q> 		  
		class _CVL<Q const&>	{  public: enum : bool{C = true, V = false, L = true};	};
		template<class Q> 		
		class _CVL<Q&&>		{  public: enum : bool{C = false, V = false, L = false};	};

	public: enum : bool{is_Const = _CVL<T>::C, is_Val = _CVL<T>::V, is_LRef = _CVL<T>::L};
	};

	template<class T> using _RTH = _Ref_Traits_Helper<T>;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	#ifndef SGM_DECLTYPE_AUTO
		#define SGM_DECLTYPE_AUTO(...)	-> decltype(__VA_ARGS__){  return __VA_ARGS__;  }
	#endif


	template<class T, class Q>
	static auto _Forward(Q&& q) SGM_DECLTYPE_AUTO
	(
		static_cast
		<	_Selective_t	
			<	_RTH<decltype(q)>::is_Val	
			,	_Selective_t< _RTH<decltype(q)>::is_Const, T const, T >	
			,	_Selective_t	
				<	_RTH<decltype(q)>::is_LRef	
				,	_Selective_t< _RTH<decltype(q)>::is_Const, T const&, T& >	
				,	T&&	
				>	
			>
		>
		(q)
	)
	
	template<class T>
	static auto _Move(T&& t) SGM_DECLTYPE_AUTO
	(
		(_Original_t<decltype(t)>&&) t
	)
	//========//========//========//========//=======#//========//========//========//========//===
	

}

#endif