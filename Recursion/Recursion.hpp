#pragma once

#ifndef _SGM_RECURSION11_
#define _SGM_RECURSION11_

#include "..\interface_Traits\interface_Traits.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	//template<class TU>
	//class Recursor;


	template<  class ITR, class = std::enable_if_t< is_iterator<ITR>::value >  >
	class constChain;

	template<  class ITR, class = std::enable_if_t< is_iterator<ITR>::value >  >
	class Chain;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class ITR, class>
class sgm::constChain 
{
protected:
	using deref_t = decltype(*Declval<ITR>());

	ITR _head, _tail;


public:
	constChain(ITR h, ITR t) : _head(h), _tail(t){}

	template<  class CON, class = std::enable_if_t< is_iterable<CON>::value >  >
	constChain(CON& con) : constChain(con.begin(), con.end()){}


	template<  class CON, class = std::enable_if_t< is_iterable<CON>::value >  >
	auto operator=(CON&& con)-> constChain&
	{
		return *this = constChain(con.begin(), con.end());  
	}


	auto operator()(ITR h, ITR t)-> constChain&{  return *this = constChain(h, t);  }


	auto operator*() const-> deref_t const&{  return *_head;  }


	auto operator++()-> constChain&
	{
		_head++;

		return *this;  
	}
	
	auto operator++(int)-> constChain
	{
		auto const res = *this;

		++(*this);

		return res;
	}


	bool is_empty() const{  return _head == _tail;  }
	operator bool() const{  return !is_empty();  }
};


template<class ITR, class>
class sgm::Chain : public constChain<ITR>
{
	using cc_t = constChain<ITR>;


public:
	Chain(ITR h, ITR t) : cc_t(h, t){}

	template<class CON>
	Chain(CON& con) : cc_t(con){}

	auto operator*()-> typename cc_t::deref_t&{  return *_head;  }
};


namespace sgm
{

	template<class ITR>
	static auto Chaining(ITR head, ITR tail)	
	->	std::enable_if_t
		<	is_iterator<ITR>::value
		,	std::conditional_t
			<	is_immutable<decltype(*head)>::value, constChain<ITR>, Chain<ITR> 
			>
		>
	{
		return {head, tail};
	}


	template<class CON>
	static auto Chaining(CON& con)
	->	std::enable_if_t
		<	is_iterable<CON>::value
		,	std::conditional_t
			<	is_immutable<decltype(*con.begin())>::value
			,	constChain< std::decay_t<decltype(con.begin())> >
			,	Chain< std::decay_t<decltype(con.begin())> >
			>
		>
	{
		return con;
	}

}



#endif // end of #ifndef _SGM_RECURSION11_