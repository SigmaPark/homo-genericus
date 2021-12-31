/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_OPERATORS_OF_
#define _SGM_OPERATORS_OF_


#include "../Type_Analysis/Type_Analysis.hpp"


namespace sgm
{
	
	template<class T>
	class Operators_of;

}
//========//========//========//========//=======#//========//========//========//========//=======#


#if	!defined(_SGM_OPERATOR_HELPER) && !defined(_SGM_GENERIC_OPERATOR) &&	\
	!defined(_SGM_UNARY_OPERATOR) && !defined(_SGM_MULTIPARAM_OPERATOR)

	#define _SGM_OPERATOR_HELPER(SYM, TITLE)	\
		template<class Q, class...ARGS>	/* Declaration Only */	\
		static auto _has##TITLE(int)	\
		->	SFINAE_t< decltype( Declval<Q>().operator SYM(Declval<ARGS>()...) ) >;	\
		\
		template<class...> /* Declaration Only */	\
		static auto _has##TITLE(...)-> False_t;	\
		\
		template<bool IS_MUT_METHOD, class Q, class...ARGS>	\
		struct _op##TITLE##Case		\
		:	As_value_itself	\
			<	int	\
			,	(	decltype( _has##TITLE<Q, ARGS...>(0) )::value && IS_MUT_METHOD ? 1	\
				:	decltype( _has##TITLE<Q const, ARGS...>(0) )::value ? 2	\
				:	/* otherwise */ 0	\
				)	\
			>	\
		{};	\
		\
		template<int> \
		struct op##TITLE##Helper	\
		{	\
			template<class...> \
			static void calc(...) = delete;	\
		};	\
		\
		template<>	\
		struct op##TITLE##Helper<1>	\
		{	\
			template<class Q, class...ARGS>	\
			static auto calc(Q *p, ARGS&&...args)	\
			->	SGM_DECLTYPE_AUTO(  p->operator SYM( Forward<ARGS>(args)... )  )	\
		};	\
		\
		template<>	\
		struct op##TITLE##Helper<2>	\
		{	\
			template<class Q, class...ARGS>	\
			static auto calc(Q const *p, ARGS&&...args)	\
			->	SGM_DECLTYPE_AUTO(  p->operator SYM( Forward<ARGS>(args)... )  )	\
		}


	#define _SGM_GENERIC_OPERATOR(TPARAM1, TPARAM2, SYM, TITLE, ARG1, ARG2)	\
		template	\
		<	TPARAM1  \
			class Q \
			=	_operator_helper_detail::op##TITLE##Helper	\
				<	_operator_helper_detail::_op##TITLE##Case<true, T  TPARAM2>::value \
				>	\
		>	\
		auto operator SYM(ARG1)-> SGM_DECLTYPE_AUTO(  Q::calc(_p  ARG2)  )	\
		\
		template	\
		<	TPARAM1  \
			class Q \
			=	_operator_helper_detail::op##TITLE##Helper	\
				<	_operator_helper_detail::_op##TITLE##Case<false, T  TPARAM2>::value \
				>	\
		>	\
		auto operator SYM(ARG1) const-> SGM_DECLTYPE_AUTO(  Q::calc(_p  ARG2)  )


	#define _SGM_MULTIPARAM_OPERATOR(SYM, TITLE)	\
		_SGM_GENERIC_OPERATOR	\
		(	SGM_MACROPACK(class...ARGS,), SGM_MACROPACK(, ARGS&&...)	\
		,	SYM, TITLE	\
		,	ARGS&&...args, SGM_MACROPACK(, Forward<ARGS>(args)...)	\
		)

	#define _SGM_UNARY_OPERATOR(SYM, TITLE)	\
		_SGM_GENERIC_OPERATOR(/**/, /**/, SYM, TITLE, /**/, /**/)


//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	namespace _operator_helper_detail
	{
		
		_SGM_OPERATOR_HELPER(++, Post_increase);
		_SGM_OPERATOR_HELPER(--, Post_Decrease);

		_SGM_OPERATOR_HELPER(!, Not);
		_SGM_OPERATOR_HELPER(~, BitNot);
		_SGM_OPERATOR_HELPER(->, Arrow);
		_SGM_OPERATOR_HELPER(++, Pre_increase);
		_SGM_OPERATOR_HELPER(--, Pre_Decrease);

		_SGM_OPERATOR_HELPER(+, Plus);
		_SGM_OPERATOR_HELPER(-, Minus);
		_SGM_OPERATOR_HELPER(*, Star);
		_SGM_OPERATOR_HELPER(&, Ampersand);
		_SGM_OPERATOR_HELPER([], index);
		_SGM_OPERATOR_HELPER((), invoke);
		_SGM_OPERATOR_HELPER(/, Divide);
		_SGM_OPERATOR_HELPER(%, Mod);
		_SGM_OPERATOR_HELPER(==, Equal);
		_SGM_OPERATOR_HELPER(!=, NotEqual);
		_SGM_OPERATOR_HELPER(<, Less);
		_SGM_OPERATOR_HELPER(>, Greater);
		_SGM_OPERATOR_HELPER(<=, LessOrEqual);
		_SGM_OPERATOR_HELPER(>=, GreaterOrEqual);
		_SGM_OPERATOR_HELPER(&&, And);
		_SGM_OPERATOR_HELPER(||, Or);
		_SGM_OPERATOR_HELPER(|, BitOr);
		_SGM_OPERATOR_HELPER(^, Xor);
		_SGM_OPERATOR_HELPER(<<, LShift);
		_SGM_OPERATOR_HELPER(>>, RShift);
		_SGM_OPERATOR_HELPER(->*, ArrowStar);

		_SGM_OPERATOR_HELPER(+=, PlusAssign);
		_SGM_OPERATOR_HELPER(-=, MinusAssign);
		_SGM_OPERATOR_HELPER(*=, MultiplyAssign);
		_SGM_OPERATOR_HELPER(/=, DivideAssign);
		_SGM_OPERATOR_HELPER(%=, ModAssign);
		_SGM_OPERATOR_HELPER(&=, BitAndAssign);
		_SGM_OPERATOR_HELPER(|=, BitOrAssign);
		_SGM_OPERATOR_HELPER(^=, XorAssign);
		_SGM_OPERATOR_HELPER(<<=, LShiftAssign);
		_SGM_OPERATOR_HELPER(>>=, RShiftAssign);


		template<class T>
		static auto Address_of(T& t)
		->	Enable_if_t< Boolean_Or< is_Class_or_Union<T>, is_Primitive_Array<T> >::value, T* >
		{
			return 
			reinterpret_cast<T*>
			(	&const_cast<char&>( reinterpret_cast<char const volatile&>(t) )
			);
		}

		template<class T>
		static auto Address_of(T& t)
		->	Enable_if_t< !Boolean_Or< is_Class_or_Union<T>, is_Primitive_Array<T> >::value, T* >
		{
			return &t;
		}

	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
class sgm::Operators_of
{
protected:
	T* _p;


public:
	using value_type = T;
	

	Operators_of() : _p(nullptr){}
	Operators_of(T& t) : _p( _operator_helper_detail::Address_of(t) ){}

	~Operators_of(){  _p = nullptr;  }

	auto operator=(T& t)-> T*{  return _p = _operator_helper_detail::Address_of(t);  }
	
	operator T const&() const{  return *_p;  }
	operator T&(){  return *_p;  }


	_SGM_GENERIC_OPERATOR
	(	/**/, SGM_MACROPACK(, int), ++, Post_increase, int, SGM_MACROPACK(, 0) 
	)

	_SGM_GENERIC_OPERATOR
	(	/**/, SGM_MACROPACK(, int), --, Post_Decrease, int, SGM_MACROPACK(, 0) 
	)

	_SGM_UNARY_OPERATOR(!, Not)
	_SGM_UNARY_OPERATOR(~, BitNot)
	_SGM_UNARY_OPERATOR(->, Arrow)
	_SGM_UNARY_OPERATOR(++, Pre_increase)
	_SGM_UNARY_OPERATOR(--, Pre_Decrease)

	_SGM_MULTIPARAM_OPERATOR(+, Plus)
	_SGM_MULTIPARAM_OPERATOR(-, Minus)
	_SGM_MULTIPARAM_OPERATOR(*, Star)
	_SGM_MULTIPARAM_OPERATOR(&, Ampersand)
	_SGM_MULTIPARAM_OPERATOR([], index)
	_SGM_MULTIPARAM_OPERATOR((), invoke)
	_SGM_MULTIPARAM_OPERATOR(/, Divide)
	_SGM_MULTIPARAM_OPERATOR(%, Mod)
	_SGM_MULTIPARAM_OPERATOR(==, Equal)
	_SGM_MULTIPARAM_OPERATOR(!=, NotEqual)
	_SGM_MULTIPARAM_OPERATOR(<, Less)
	_SGM_MULTIPARAM_OPERATOR(>, Greater)
	_SGM_MULTIPARAM_OPERATOR(<=, LessOrEqual)
	_SGM_MULTIPARAM_OPERATOR(>=, GreaterOrEqual)
	_SGM_MULTIPARAM_OPERATOR(&&, And)
	_SGM_MULTIPARAM_OPERATOR(||, Or)
	_SGM_MULTIPARAM_OPERATOR(|, BitOr)
	_SGM_MULTIPARAM_OPERATOR(^, Xor)
	_SGM_MULTIPARAM_OPERATOR(<<, LShift)
	_SGM_MULTIPARAM_OPERATOR(>>, RShift)
	_SGM_MULTIPARAM_OPERATOR(->*, ArrowStar)

	_SGM_MULTIPARAM_OPERATOR(+=, PlusAssign)
	_SGM_MULTIPARAM_OPERATOR(-=, MinusAssign)
	_SGM_MULTIPARAM_OPERATOR(*=, MultiplyAssign)
	_SGM_MULTIPARAM_OPERATOR(/=, DivideAssign)
	_SGM_MULTIPARAM_OPERATOR(%=, ModAssign)
	_SGM_MULTIPARAM_OPERATOR(&=, BitAndAssign)
	_SGM_MULTIPARAM_OPERATOR(|=, BitOrAssign)
	_SGM_MULTIPARAM_OPERATOR(^=, XorAssign)
	_SGM_MULTIPARAM_OPERATOR(<<=, LShiftAssign)
	_SGM_MULTIPARAM_OPERATOR(>>=, RShiftAssign)

};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


	#undef _SGM_UNARY_OPERATOR
	#undef _SGM_MULTIPARAM_OPERATOR
	#undef _SGM_GENERIC_OPERATOR
	#undef _SGM_OPERATOR_HELPER
#else
	#error The macro was already defined somewhere else
#endif


#endif // end of #ifndef _SGM_OPERATORS_OF_