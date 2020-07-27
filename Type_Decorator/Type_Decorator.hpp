#pragma once

#ifndef _SGM_TYPE_DECORATOR_
#define _SGM_TYPE_DECORATOR_

#include "..\Type_Analysis\Type_Analysis.hpp"

namespace sgm
{


	struct Type_Decorator{  template<class...> using type = void;  };


	template<class T> 
	struct is_Type_Decorator : std::is_base_of< Type_Decorator, std::decay_t<T> >{};


	template<class T>
	struct Decorated
	{
		template<class Q = void, class...QS>
		struct by
		:	Decorated
			<	std::conditional_t
				<	is_Type_Decorator<Q>::value, typename Q:: template type<T>, T
				>
			>::	template by<QS...>
		{};

		template<> struct by<>{  using type = T;  };
	};

	template<class T, class...TDS>
	using Decorated_t = typename Decorated<T>::template by<TDS...>::type;


}// end of namespace sgm

#endif// end of #ifndef _SGM_TYPE_DECORATOR_