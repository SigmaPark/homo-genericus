#pragma once

#ifndef _SGM_CHAIN_
#define _SGM_CHAIN_

#include "..\interface_Traits\interface_Traits.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	//template<class TU>
	//class Recursor;


	template<  class ITR, class = Enable_if_t< is_iterator<ITR>::value >  >
	class constChain;

	template<  class ITR, class = Enable_if_t< is_iterator<ITR>::value >  >
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

	template<  class CON, class = Enable_if_t< is_iterable<CON>::value >  >
	constChain(CON& con) : constChain(con.begin(), con.end()){}


	template<  class CON, class = Enable_if_t< is_iterable<CON>::value >  >
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

	auto operator*()-> typename cc_t::deref_t&{  return *cc_t::_head;  }
};


namespace sgm
{

	template<class ITR>
	static auto Chaining(ITR head, ITR tail)	
	->	Enable_if_t
		<	is_iterator<ITR>::value
		,	Selective_t
			<	is_immutable<decltype(*head)>::value, constChain<ITR>, Chain<ITR> 
			>
		>
	{
		return {head, tail};
	}


	template<class CON>
	static auto Chaining(CON& con)
	->	Enable_if_t
		<	is_iterable<CON>::value
		,	Selective_t
			<	is_immutable<decltype(*con.begin())>::value
			,	constChain< Decay_t<decltype(con.begin())> >
			,	Chain< Decay_t<decltype(con.begin())> >
			>
		>
	{
		return con;
	}

}



#endif // end of #ifndef _SGM_CHAIN_