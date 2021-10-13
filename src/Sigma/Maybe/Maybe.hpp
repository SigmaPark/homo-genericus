#pragma once

#include "..\Type_Analysis\Type_Analysis.hpp"
#include <new>


namespace sgm
{
	template<class T>  class Maybe;

	SGM_USER_DEFINED_TYPE_CHECK(_, Maybe, class T, T);

	template<class T>  static auto make_Maybe(T &&t)noexcept(is_RvalueReference<T&&>::value)
	->	Maybe< Referenceless_t<T> >;
	
	template<class T, class...ARGS>  static auto make_Maybe(ARGS&&...args)
	noexcept(Check_All<is_RvalueReference>::template for_any<ARGS&&...>::value)-> Maybe<T>;

	enum class NullMyb_t : char{} constexpr NullMyb{};

	struct Bad_Access_to_Maybe;
}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T> 
class sgm::Maybe
{
private:
	using _T = Decay_t<T>;

public:
	static_assert(!is_Array<T>::value, "T should not be array type .");


	Maybe() : _is_null(true){}
	Maybe(NullMyb_t const) : Maybe(){}
	
	template
	<	class...ARGS
	,	class 
		=	Enable_if_t
			<	sizeof...(ARGS) >= 2 
			||	(	!is_Maybe< First_t<ARGS...> >::value
				&&	!is_Same<  Decay_t< First_t<ARGS...> >, Maybe  >::value
				&&	!is_Same<  Decay_t< First_t<ARGS...> >, NullMyb_t  >::value
				)
			>
	>
	Maybe(ARGS&&...args) 
	noexcept(Check_All<is_RvalueReference>::template for_any<ARGS&&...>::value)
	:	_is_null(false){  _alloc( Forward<ARGS>(args)... );  }


	Maybe(Maybe const &myb) : _is_null(myb.is_null())
	{
		if(!myb.is_null())
			_alloc(*myb);
	}

	Maybe(Maybe &&myb) noexcept : _is_null(myb.is_null())
	{
		if(!myb.is_null())
			_alloc( Move(*myb) );
	}

	template
	<	class Q
	,	class = Enable_if_t< !is_Same<Q, T>::value && is_Convertible<Q, T>::value >
	>
	Maybe(Maybe<Q> const &myb) : _is_null(myb.is_null())
	{
		if(!myb.is_null())
			_alloc(*myb);
	}


	~Maybe() noexcept{  (**this).~_T(),  _is_null = true;  }

	template
	<	class Q
	,	class 
		=	Enable_if_t
			<	is_Convertible<Q&&, T>::value
			&&	!is_Maybe<Q>::value
			&&	!is_Same<  Decay_t<Q>, Maybe  >::value
			&&	!is_Same<  Decay_t<Q>, NullMyb_t  >::value
			>
	>
	auto operator=(Q &&q)
	->	Maybe&{  return **this = Forward<Q>(q),  _is_null = false,  *this;  }

	auto operator=(NullMyb_t const)-> Maybe&{  return _is_null = true,  *this;  }

	template
	<	class Q
	,	class = Enable_if_t< !is_Same<Q, T>::value && is_Convertible<Q, T>::value >
	>
	auto operator=(Maybe<Q> const &myb)-> Maybe&
	{
		if(!myb.is_null())
			**this = static_cast<T>(*myb);

		return _is_null = myb.is_null(),  *this;
	}

	auto operator=(Maybe const &myb)-> Maybe&
	{
		if(!myb.is_null())
			**this = *myb;

		return _is_null = myb.is_null(),  *this;
	}

	auto operator=(Maybe &&myb) noexcept-> Maybe&
	{
		if(!myb.is_null())
			**this = Move(*myb);

		return _is_null = myb.is_null(),  *this;
	}


	explicit operator bool() const{  return !is_null();  }
	bool is_null() const{  return _is_null;  }

	auto operator*() const-> T const&{  return *reinterpret_cast<T*>(_buf);  }
	auto operator*()-> T&{  return *reinterpret_cast<T*>(_buf);  }

	auto get() const noexcept(false)-> T const&
	{
		if(is_null())
			throw Bad_Access_to_Maybe{};
		else
			return **this;  
	}

	auto get() noexcept(false)-> T&
	{
		if(is_null())
			throw Bad_Access_to_Maybe{};
		else
			return **this;  
	}

	
	auto get_or(T const &t) const-> T const&{  return is_null() ? t : **this;  }
	auto get_or(T &t)-> T&{  return is_null() ? t : **this;  }

	auto operator->() const-> T const*{  return &**this;  }
	auto operator->()-> T*{  return &**this;  }


private:
	bool mutable _is_null;
	char mutable _buf[sizeof(T)] = {0, };


	template<class...ARGS>
	void _alloc(ARGS&&...args) 
	noexcept(Check_All<is_RvalueReference>::template for_any<ARGS&&...>::value)
	{
		new(_buf) _T( Forward<ARGS>(args)... );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T> 
auto sgm::make_Maybe(T &&t) noexcept(is_RvalueReference<T&&>::value)
-> Maybe< Referenceless_t<T> >{  return Forward(t);  }
	

template<class T, class...ARGS>  static auto sgm::make_Maybe(ARGS&&...args)
noexcept(Check_All<is_RvalueReference>::template for_any<ARGS&&...>::value)
->	Maybe<T>{  return {Forward<ARGS>(args)...};  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#include <exception>


struct sgm::Bad_Access_to_Maybe : public std::exception
{
public:
	auto what() const-> char const* override{  return "you accessed invalid sgm::Maybe.\n";  }

private:
	template<class T>  friend class sgm::Maybe;

	Bad_Access_to_Maybe() = default;
};