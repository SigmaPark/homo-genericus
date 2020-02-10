#pragma once

#ifndef _SGM_TYPE_ANALYSIS_
#define _SGM_TYPE_ANALYSIS_

namespace sgm
{


	template<class T>
	class _Original_Helper
	{
	private:
		template<class Q> struct _Original			{  using type = Q;  };
		template<class Q> struct _Original<Q const>	{  using type = Q;  };
		template<class Q> struct _Original<Q&>		{  using type = Q;  };
		template<class Q> struct _Original<Q const&>	{  using type = Q;  };
		template<class Q> struct _Original<Q&&>		{  using type = Q;  };

	public:
		_Original_Helper() = delete;

		using type = typename _Original<T>::type;
	};

	//	Similar to std::decay_t
	template<class T>
	using _Original_t = typename _Original_Helper<T>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
	
	
	template<bool B, class true_t, class false_t> class _Selective;

	template<class true_t, class false_t>
	class _Selective<true, true_t, false_t>
	{
	public:
		_Selective() = delete;

		using type = true_t;
	};

	template<class true_t, class false_t>
	class _Selective<false, true_t, false_t>
	{
	public:
		_Selective() = delete;

		using type = false_t;
	};

	//	Similar to std::conditional_t
	template<bool B, class true_t, class false_t>
	using _Selective_t = typename _Selective<B, true_t, false_t>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	class _Ref_Traits_Helper
	{
	private:
		template<class Q> 
		struct _CVL			{  enum : bool{C = false, V = true, L = true};	};
		template<class Q> 
		struct _CVL<Q const>	{  enum : bool{C = true, V = true, L = true};	};
		template<class Q> 
		struct _CVL<Q&>		{  enum : bool{C = false, V = false, L = true};	};
		template<class Q> 
		struct _CVL<Q const&>	{  enum : bool{C = true, V = false, L = true};	};
		template<class Q> 
		struct _CVL<Q&&>		{  enum : bool{C = false, V = false, L = false};	};

	public:
		_Ref_Traits_Helper() = delete;

		enum : bool{is_Const = _CVL<T>::C, is_Val = _CVL<T>::V, is_LRef = _CVL<T>::L};
	};

	template<class T>
	using _RTH = _Ref_Traits_Helper<T>;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T, class Q>
	static auto _Forward(Q&& q)
	->	_Selective_t	
		<	_RTH<decltype(q)>::is_Val	
		,	_Selective_t< _RTH<decltype(q)>::is_Const, T const, T >	
		,	_Selective_t	
			<	_RTH<decltype(q)>::is_LRef	
			,	_Selective_t< _RTH<decltype(q)>::is_Const, T const&, T& >	
			,	T&&	
			>	
		>
	{
		return
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
		(q);
	}


	template<class T>
	static auto _Move(T&& t)-> _Original_t<decltype(t)>&&
	{
		return ( _Original_t<decltype(t)>&& ) t;
	}
	//========//========//========//========//=======#//========//========//========//========//===
	

}

#endif