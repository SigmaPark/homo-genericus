#pragma once

#ifndef _SGM_RECURSION11_
#define _SGM_RECURSION11_

#include "..\interface_Traits\interface_Traits.hpp"
#include <tuple>
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	struct _Recursion_Helper;

	template<class TU>
	class Recursor;


	template<  class ITR, class = std::enable_if_t< is_iterator<ITR>::value >  >
	class Chain;
}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class TU>
class sgm::Recursor
{
private:
	TU _tu;


public:
	Recursor(TU&& tu) : _tu( std::forward<TU>(tu) ){}


	template<class...ARGS>
	auto operator()(ARGS&&...args)-> Recursor&
	{
		_tu = std::forward_as_tuple(args...);

		return *this;
	}


	template
	<	signed IDX
	,	int N 
		=	IDX == 0
			?	0
			:	(IDX < 0) ? std::tuple_size<TU>::value + IDX : IDX - 1
	>
	auto _() const-> decltype( std::get<N>(_tu) ) const&{  return std::get<N>(_tu);  }
};


template<class ITR, class>
class sgm::Chain
{
	using deref_t = decltype(*Declval<ITR>());


public:
	Chain(ITR h, ITR t) : _head(h), _tail(t){}

	template<  class CON, class = std::enable_if_t< is_iterable<CON>::value >  >
	Chain(CON& con) : Chain(con.begin(), con.end()){}


	template<  class CON, class = std::enable_if_t< is_iterable<CON>::value >  >
	auto operator=(CON&& con)-> Chain&{  return *this = Chain(con.begin(), con.end());  }

	auto operator()(ITR h, ITR t)-> Chain&{  return *this = Chain(h, t);  }

	auto head() const-> deref_t const&{  return *_head;  }
	auto body() const-> Chain{  return Chain( Next(_head), _tail );  }

	bool is_empty() const{  return _head == _tail;  }
	operator bool() const{  return !is_empty();  }


private:
	ITR _head, _tail;
};


namespace sgm
{

	template<class...ARGS>
	auto Recursion(ARGS&&...args) SGM_DECLTYPE_AUTO
	(
		Recursor<  std::tuple< std::decay_t<ARGS>... >  >
		(	std::forward_as_tuple( std::forward<ARGS>(args)... )
		)
	)


	template<  class ITR, class = std::enable_if_t< is_iterator<ITR>::value >  >
	auto Chaining(ITR head, ITR tail) SGM_DECLTYPE_AUTO(  Chain<ITR>(head, tail)  )


	template<  class CON, class = std::enable_if_t< is_iterable<CON>::value >  >
	auto Chaining(CON& con) SGM_DECLTYPE_AUTO
	(	
		Chain< std::decay_t<decltype(con.begin())> >(con)
	)

}



#endif // end of #ifndef _SGM_RECURSION11_