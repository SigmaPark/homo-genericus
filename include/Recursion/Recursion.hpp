/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_RECURSION_
#define _SGM_RECURSION_


#include "../Family/Family.hpp"


#define SGM_RECURSION(...) \
	(auto...args) mutable	\
	{	\
		auto _tied = sgm::Tie(args...);	\
		auto &[__VA_ARGS__] = _tied;	\
		\
		while(true)


#define return_RECURSION(...) _tied = sgm::Forward_as_Family(__VA_ARGS__)	\


#define FROM_INITIAL_FUNCTION	\
	}	\
	%	sgm::_Recursion_helper_detail::_Binding_tag{}	\
	%	[&, recursion = sgm::_Recursion_helper_detail::_recursion]



namespace sgm::_Recursion_helper_detail
{
	
	enum class _Binding_tag{};

	template<class L>
	struct _Binded_Lambda;

	template<class F, class...TYPES, class...ARGS>
	static decltype(auto) _Apply
	(	F&& f, [[maybe_unused]] Family<TYPES...>&& fam, ARGS&&...args
	)	noexcept;

	static auto constexpr _recursion
	=	[](auto&&...args){  return Make_Family( Forward<decltype(args)>(args)... );  };

}


template<class L>
struct sgm::_Recursion_helper_detail::_Binded_Lambda
{
	L _func;
};


namespace sgm::_Recursion_helper_detail
{

	template<class F, class...TYPES, class...ARGS>
	decltype(auto) _Apply
	(	F&& f, [[maybe_unused]] Family<TYPES...>&& fam, ARGS&&...args
	)	noexcept
	{
		auto constexpr FAM_SIZE = sizeof...(TYPES), IDX = sizeof...(ARGS);
	
		if constexpr(FAM_SIZE == IDX)
			return f( Forward<ARGS>(args)... );
		else
			return
			_Apply
			(	Move(f), Move(fam), Forward<ARGS>(args)...
			,	static_cast< Nth_t<IDX, TYPES&&...> >( std::get<IDX>(fam) )
			);
	}

}


template<class L>
static auto operator%(L&& lambda_f, sgm::_Recursion_helper_detail::_Binding_tag) noexcept
->	sgm::_Recursion_helper_detail::_Binded_Lambda<L>
{
	return { sgm::Move(lambda_f) };
}


template<class L, class T>
static decltype(auto) operator%
(	sgm::_Recursion_helper_detail::_Binded_Lambda<L>&& binded_lambda, T&& init
)	noexcept
{
	using namespace sgm;

	if constexpr(is_Family<T>::value)
		return _Recursion_helper_detail::_Apply( Move(binded_lambda._func), Move(init) );
	else
		return
		[b = Move(binded_lambda), f = Move(init)](auto&&...args) mutable
		{
			return Move(b) % f( Forward<decltype(args)>(args)... );
		};
}

#endif // end of #ifndef _SGM_RECURSION_