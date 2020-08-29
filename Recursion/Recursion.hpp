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

	template< size_t IDX = std::tuple_size<TU>::value - 1 >
	auto get() const-> decltype( std::get<IDX>(_tu) ) const&{  return std::get<IDX>(_tu);  }

	auto first() const SGM_DECLTYPE_AUTO(  get<0>()  )
	auto last() const SGM_DECLTYPE_AUTO(  get()  )
};


template<class ITR, class>
class sgm::Chain
{
	using deref_t = decltype(*Declval<ITR>());


public:
	Chain(ITR h, ITR t) : _head(h), _tail(t){}

	template<  class CON, class = std::enable_if_t< is_iterable<CON>::value >  >
	Chain(CON&& con) : Chain(con.begin(), con.end()){}


	template<  class CON, class = std::enable_if_t< is_iterable<CON>::value >  >
	auto operator=(CON&& con)-> Chain&
	{
		return *this = Chain(con.begin(), con.end());
	}

	auto operator()(ITR h, ITR t)-> Chain&{  return *this = Chain(h, t);  }


	auto head() const-> deref_t const&{  return *_head;  }
	auto tail() const-> deref_t const&{  return *_tail;  }
	auto body() const-> Chain{  return Chain( Next(_head), _tail );  }

	bool is_empty() const{  return _head == _tail;  }
	operator bool() const{  return !is_empty();  }

private:
	ITR _head, _tail;
};


struct sgm::_Recursion_Helper : No_Making
{
	template<class T>
	static auto carry(T& t)-> std::conditional_t< is_immutable<T>::value, std::decay_t<T>, T& >
	{
		return t;
	}

	template<class T>
	static auto carry(T&& t) SGM_DECLTYPE_AUTO(  std::move(t)  )
};


namespace sgm
{

	template<class...ARGS>
	auto Recursion(ARGS&&...args) SGM_DECLTYPE_AUTO
	(
		Recursor<  std::tuple< decltype( _Recursion_Helper::carry<ARGS>(args) )... >  >
		(	std::forward_as_tuple
			(	_Recursion_Helper::carry<ARGS>(args)...
			)
		)
	)


	template<  class ITR, class = std::enable_if_t< is_iterator<ITR>::value >  >
	auto make_Chain(ITR head, ITR tail) SGM_DECLTYPE_AUTO(  Chain<ITR>(head, tail)  )


	template<  class CON, class = std::enable_if_t< is_iterable<CON>::value >  >
	auto make_Chain(CON&& con) SGM_DECLTYPE_AUTO
	(	
		Chain< std::decay_t<decltype(con.begin())> >( std::forward<CON>(con) )
	)

}



#endif // end of #ifndef _SGM_RECURSION11_