/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_INTERFACE_TRAITS_
#define _SGM_INTERFACE_TRAITS_


#include "SGM/Type_Analysis/Type_Analysis.hpp"


#ifndef SGM_INTERFACE_CHECK
	#define SGM_INTERFACE_CHECK(TEMPLATE_ARGS, TITLE, ...)	\
		template<class CLASS, TEMPLATE_ARGS>	\
		struct TITLE : sgm::Unconstructible	\
		{	\
		private:	\
			template<class _CLASS>	 /* Declaration Only */		\
			static auto _calc(int)-> sgm::SFINAE_t< decltype(__VA_ARGS__) >;\
			\
			template<class> /* Declaration Only */ static auto _calc(...)-> sgm::False_t;	\
		\
		public:	\
			static bool constexpr value = decltype( _calc< sgm::Decay_t<CLASS> >(0) )::value;	\
			using type = sgm::Boolean<value>;	\
		}

#else
	#error SGM_INTERFACE_CHECK was already defined somewhere else.
#endif
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	SGM_INTERFACE_CHECK
	(	class...ARGS
	,	Has_Operator_New
	,	new _CLASS( sgm::Mock<ARGS>()... )
	);

	SGM_INTERFACE_CHECK
	(	class...
	,	Has_Operator_Delete
	,	delete sgm::Mock<_CLASS*>()
	);


	SGM_INTERFACE_CHECK
	(	class RHS = CLASS
	,	Has_Operator_Copy_Assignment
	,	sgm::Mock<_CLASS>() = sgm::Mock<RHS const&>()
	);
	
	SGM_INTERFACE_CHECK
	(	class RHS = CLASS
	,	Has_Operator_Move_Assignment
	,	sgm::Mock<_CLASS>() = sgm::Move(sgm::Mock<RHS>())
	);


	SGM_INTERFACE_CHECK
	(	class...ARGS
	,	Has_Operator_invocation
	,	sgm::Mock<_CLASS>()( sgm::Mock<ARGS>()... )
	);


#ifndef _SGM_BINARY_OPERATOR_INTERFACE
	#define _SGM_BINARY_OPERATOR_INTERFACE(TITLE, OP) \
		SGM_INTERFACE_CHECK	\
		(	class RHS = CLASS	\
		,	 Has_Operator_##TITLE	\
		,	sgm::Mock<_CLASS>() OP sgm::Mock<RHS>()	\
		)	


	_SGM_BINARY_OPERATOR_INTERFACE(Same, ==);
	_SGM_BINARY_OPERATOR_INTERFACE(NotSame, !=);
	_SGM_BINARY_OPERATOR_INTERFACE(Less, <);
	_SGM_BINARY_OPERATOR_INTERFACE(LessOrSame, <=);
	_SGM_BINARY_OPERATOR_INTERFACE(Greater, >);
	_SGM_BINARY_OPERATOR_INTERFACE(GreaterOrSame, >=);

	_SGM_BINARY_OPERATOR_INTERFACE(Plus, +);
	_SGM_BINARY_OPERATOR_INTERFACE(Minus, -);
	_SGM_BINARY_OPERATOR_INTERFACE(Multiply, *);
	_SGM_BINARY_OPERATOR_INTERFACE(Divide, /);
	_SGM_BINARY_OPERATOR_INTERFACE(Mod, %);

	_SGM_BINARY_OPERATOR_INTERFACE(And, &&);
	_SGM_BINARY_OPERATOR_INTERFACE(Or, ||);

	_SGM_BINARY_OPERATOR_INTERFACE(BitAnd, &);
	_SGM_BINARY_OPERATOR_INTERFACE(BitOr, |);
	_SGM_BINARY_OPERATOR_INTERFACE(Xor, ^);
	_SGM_BINARY_OPERATOR_INTERFACE(LShift, <<);
	_SGM_BINARY_OPERATOR_INTERFACE(RShift, >>);

	_SGM_BINARY_OPERATOR_INTERFACE(PlusAssign, +=);
	_SGM_BINARY_OPERATOR_INTERFACE(MinusAssign, -=);
	_SGM_BINARY_OPERATOR_INTERFACE(MultiplyAssign, *=);
	_SGM_BINARY_OPERATOR_INTERFACE(DivideAssign, /=);
	_SGM_BINARY_OPERATOR_INTERFACE(ModAssign, %=);
	_SGM_BINARY_OPERATOR_INTERFACE(BitAndAssign, &=);
	_SGM_BINARY_OPERATOR_INTERFACE(BitOrAssign, |=);
	_SGM_BINARY_OPERATOR_INTERFACE(XorAssign, ^=);
	_SGM_BINARY_OPERATOR_INTERFACE(LShiftAssign, <<=);
	_SGM_BINARY_OPERATOR_INTERFACE(RShiftAssign, >>=);

	_SGM_BINARY_OPERATOR_INTERFACE(Ptr2Mem, ->*);

	
	#undef _SGM_BINARY_OPERATOR_INTERFACE
#else
	#error _SGM_BINARY_OPERATOR_INTERFACE was already defined somewhere else.
#endif


	SGM_INTERFACE_CHECK
	(	class CLASS2 = CLASS
	,	Has_Operator_Comma
	,	sgm::Mock<_CLASS>().operator,(sgm::Mock<CLASS2>())
	);


#ifndef _SGM_UNARY_OPERATOR_INTERFACE
	#define _SGM_UNARY_OPERATOR_INTERFACE(PRE, TITLE, POST)	\
		SGM_INTERFACE_CHECK(class..., Has_Operator_##TITLE, PRE sgm::Mock<_CLASS>() POST)


	_SGM_UNARY_OPERATOR_INTERFACE(+, Posit,/**/);
	_SGM_UNARY_OPERATOR_INTERFACE(-, Negate, /**/);
	
	_SGM_UNARY_OPERATOR_INTERFACE(++, Pre_increase, /**/);
	_SGM_UNARY_OPERATOR_INTERFACE(--, Pre_Decrease, /**/);
	_SGM_UNARY_OPERATOR_INTERFACE(/**/, Post_increase, ++);
	_SGM_UNARY_OPERATOR_INTERFACE(/**/, Post_Decrease, --);

	_SGM_UNARY_OPERATOR_INTERFACE(*, Deref, /**/);
	_SGM_UNARY_OPERATOR_INTERFACE(&, Ref, /**/);

	_SGM_UNARY_OPERATOR_INTERFACE(~, BitNot, /**/);
	_SGM_UNARY_OPERATOR_INTERFACE(!, Not, /**/);


	#undef _SGM_UNARY_OPERATOR_INTERFACE
#else
	#error _SGM_UNARY_OPERATOR_INTERFACE was already defined somewhere else.
#endif


	SGM_INTERFACE_CHECK
	(	class IDX_TYPE = int
	,	Has_Operator_index, sgm::Mock<_CLASS>()[sgm::Mock<IDX_TYPE>()]
	);	
	
}
//========//========//========//========//=======#//========//========//========//========//=======#


#ifndef SGM_HAS_MEMFUNC
	#define SGM_HAS_MEMFUNC(MEMFUNC)	\
		SGM_INTERFACE_CHECK	\
		(	class..._ARGUMENTS	\
		,	Has_MemFunc_##MEMFUNC		\
		,	sgm::Mock<_CLASS>().MEMFUNC(sgm::Mock<_ARGUMENTS>()...) \
		);	\
		\
		template<class R, class C, class...TYPES>	\
		struct Has_MemFunc_##MEMFUNC##_Returning : sgm::Unconstructible	\
		{	\
		private:	\
			template<class _Q, class..._TYPES>	\
			static auto _calc(int)	\
			->	sgm::SFINAE_t	\
				<	decltype( sgm::Mock<_Q>().MEMFUNC(sgm::Mock<_TYPES>()...) )	\
				,	decltype( sgm::Mock<_Q>().MEMFUNC(sgm::Mock<_TYPES>()...) )	\
				>;	\
			\
			template<class...>	\
			static auto _calc(...)-> sgm::None;	\
			\
		public:	\
			static bool constexpr value	\
			=	sgm::is_Same< decltype( _calc<C, TYPES...>(0) ), R >::value;	\
			\
			using type = sgm::Boolean<value>;	\
		}

#else
	#error SGM_HAS_MEMFUNC was already defined somewhere else.
#endif


#ifndef SGM_HAS_MEMBER
	/**	Not only member data but also 
	*	1)	member data
	*	2)	static member data
	*	3)	unscoped enumerator
	*	can be substituted to MEMBER.
	*/
	#define SGM_HAS_MEMBER(MEMBER)	\
		SGM_INTERFACE_CHECK	\
		(	class...	\
		,	Has_Member_##MEMBER	\
		,	sgm::Mock<_CLASS>().MEMBER	\
		)
	
#else
	#error SGM_HAS_MEMBER was already defined somewhere else.
#endif


#ifndef SGM_HAS_NESTED_TYPE
	#define SGM_HAS_NESTED_TYPE(TYPE)	\
		SGM_INTERFACE_CHECK	\
		(	class...	\
		,	Has_NestedType_##TYPE	\
		,	sgm::Mock<typename _CLASS::TYPE>()		\
		)

#else
	#error SGM_HAS_NESTED_TYPE was already defined somewhere else.
#endif
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	template<class F>
	class CRTP_of;

}


template< template<class> class TC, class T >
class sgm::CRTP_of< TC<T> >
{
private:
	class _CRTP : public T{  friend class TC<T>;  };


protected:
	auto crtp() const noexcept-> _CRTP const&{  return *static_cast<_CRTP const*>(this);  }
	auto crtp() noexcept-> _CRTP&{  return *static_cast<_CRTP*>(this);  }
};


#define SGM_CRTP_TEMPLATE_INTERFACE(TITLE, ...)	\
	auto constexpr crtp_override_##TITLE() const noexcept-> void	\
	{	\
		static_assert	\
		(	(__VA_ARGS__)	\
		,	"crtp method \"" #TITLE "\" doesn't satisfy type requirements ."	\
		);	\
	}	\
	\
	template<bool B = false>	\
	auto constexpr TITLE(...) const noexcept-> void		\
	{	\
		static_assert(B, "crtp method \"" #TITLE "\" is not overriden .");  \
	}	\
	\
	template<class RES, class...ARGS, class HOST>	\
	auto constexpr check_return_type_of_##TITLE(HOST&&) const noexcept	\
	->	sgm::is_Same	\
		<	decltype	\
			(	sgm::Mock<HOST>().TITLE(sgm::Mock<ARGS>()...) \
			)	\
		,	RES	\
		>


#define SGM_CRTP_OVERRIDE(TITLE, ...) this->template crtp_override_##TITLE __VA_ARGS__ ()


#endif // end of #ifndef _SGM_INTERFACE_TRAITS_