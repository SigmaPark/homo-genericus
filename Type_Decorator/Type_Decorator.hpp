#pragma once

#ifndef _SGM_TYPE_DECORATOR_
#define _SGM_TYPE_DECORATOR_

#include "..\Type_Analysis\Type_Analysis.hpp"

namespace sgm
{


	struct Type_Decorator{  template<class...> using type = void;  };


	template<class T> 
	struct is_Type_Decorator : is_inherited_from< Decay_t<T>, Type_Decorator >{};


	template<class T>
	struct Decorated
	{
		template<class Q = void, class...QS>
		struct by
		:	Decorated
			<	Selective_t
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