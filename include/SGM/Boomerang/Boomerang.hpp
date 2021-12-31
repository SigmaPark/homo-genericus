/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once 
#ifndef _SGM_BOOMERANG_
#define _SGM_BOOMERANG_


#include "../Operators_of/Operators_of.hpp"


namespace sgm
{

	template<  class T, class FDB, class = Boolean< is_Class_or_Union<T>::value >  >
	class Boomerang;

	template<class _T, class _FDB>
	static auto throw_Boomerang(_T&& t, _FDB&& feedback)
	->	Boomerang< Referenceless_t<_T>, Referenceless_t<_FDB>  >;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, class FDB>
class sgm::Boomerang<T, FDB, sgm::True_t> : public T
{
public:
	using value_type = T;


	Boomerang(Boomerang const&) = delete;
	~Boomerang(){  _feedback( Move(*this).ref() );  }

	auto ref() &&-> T&{  return *this;  }
	auto eval() const-> T{  return *this;  }

	
	template
	<	class Q
	,	class
		=	Enable_if_t
			<	is_Convertible<Q&&, T>::value && !is_Same< Decay_t<Q>, Boomerang >::value
			>	
	>
	auto operator=(Q&& q) &&
	->	Boomerang&&{  return Move(*this).ref() = Forward<Q>(q),  Move(*this);  }

	auto operator=(Boomerang&& bmr) && 
	->	Boomerang&&{  return Move(*this).ref() = bmr.eval(),  Move(*this);  }

	template<class Q>
	auto operator=(Q&&) &-> Boomerang& = delete;

	auto operator=(Boomerang const&) &-> Boomerang& = delete;


private:
	FDB _feedback;


	template<class _T, class _FDB>
	friend auto throw_Boomerang(_T&& t, _FDB&& feedback)
	->	Boomerang< Referenceless_t<_T>, Referenceless_t<_FDB>  >;


	template
	<	class Q
	,	class
		=	Enable_if_t
			<	is_Convertible<Q&&, T>::value && !is_Same< Decay_t<Q>, Boomerang >::value
			>
	>
	Boomerang(Q&& q, FDB feedback) : T( Forward<Q>(q) ), _feedback(feedback){}
};


template<class T, class FDB>
class sgm::Boomerang<T, FDB, sgm::False_t> : public Operators_of<T>
{
public:
	using value_type = T;


	Boomerang(Boomerang const&) = delete;
	~Boomerang(){  _feedback( Move(*this).ref() );  }

	auto ref() &&-> T&{  return _t;  }
	auto eval() const-> T{  return _t;  }

	
	template
	<	class Q
	,	class
		=	Enable_if_t
			<	is_Convertible<Q&&, T>::value && !is_Same< Decay_t<Q>, Boomerang >::value
			>	
	>
	auto operator=(Q&& q) &&
	->	Boomerang&&{  return Move(*this).ref() = Forward<Q>(q),  Move(*this);  }

	auto operator=(Boomerang&& bmr) && 
	->	Boomerang&&{  return Move(*this).ref() = bmr.eval(),  Move(*this);  }

	template<class Q>
	auto operator=(Q&&) &-> Boomerang& = delete;

	auto operator=(Boomerang const&) &-> Boomerang& = delete;


private:
	T _t;
	FDB _feedback;

	template<class _T, class _FDB>
	friend auto throw_Boomerang(_T&& t, _FDB&& feedback)
	->	Boomerang< Referenceless_t<_T>, Referenceless_t<_FDB>  >;


	template
	<	class Q
	,	class
		=	Enable_if_t
			<	is_Convertible<Q&&, T>::value && !is_Same< Decay_t<Q>, Boomerang >::value
			>
	>
	Boomerang(Q&& q, FDB feedback) : _t( Forward<Q>(q) ), _feedback(feedback)
	{
		static_cast< Operators_of<T>& >(*this) = _t;
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class _T, class _FDB>
auto sgm::throw_Boomerang(_T&& t, _FDB&& feedback)
->	Boomerang< Referenceless_t<_T>, Referenceless_t<_FDB> >
{
	return {Forward<_T>(t), Forward<_FDB>(feedback)};
}


namespace std
{

	template<class T, class FDB, class B>
	auto ref(sgm::Boomerang<T, FDB, B>&& bmr)-> T&{  return sgm::Move(bmr).ref();  } 

}


#endif //#ifndef _SGM_BOOMERANG_
 
