/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_TYPE_ANALYSIS_
#define _SGM_TYPE_ANALYSIS_


#include <cstddef>
#include "SGM/Standard.hpp"


#if SGM_CXX_STANDARD < 2011
    #error C++11 or higher version language support is required.
#endif


#ifndef SGM_MACROPACK
    #define SGM_MACROPACK(...)  __VA_ARGS__
#endif

#define _SGM_DOUBLE_UNDERBAR_MACRO_HELPER(MACRO)  __##MACRO##__


#ifndef SGM_DECLTYPE_AUTO
    #define SGM_DECLTYPE_AUTO(...)  decltype(__VA_ARGS__){  return __VA_ARGS__;  }
#endif


namespace sgm
{

    using std::size_t;
    using std::ptrdiff_t;

}


namespace sgm
{

    struct Unconstructible{  Unconstructible() = delete;  };


    template<class T>
    struct As_type_itself : Unconstructible{  using type = T;  };


    template<class T, T VAL>
    struct As_value_itself : Unconstructible{  static T constexpr value = VAL;  };


    template<bool B>
    struct Boolean : As_type_itself< Boolean<B> >, As_value_itself<bool, B>{};

    using False_t = Boolean<false>;
    using True_t = Boolean<true>;


    template<bool CONDITION, class, class>
    struct Selective;

    template<class T1, class T2>
    struct Selective<true, T1, T2> : As_type_itself<T1>{};

    template<class T1, class T2>
    struct Selective<false, T1, T2> : As_type_itself<T2>{};

    template<bool CONDITION, class T1, class T2>
    using Selective_t = typename Selective<CONDITION, T1, T2>::type;


    template<class...TYPES>
    struct Boolean_And;

    template<class T, class...TYPES>
    struct Boolean_And<T, TYPES...> : Selective_t< T::value, Boolean_And<TYPES...>, False_t >{};

    template<>
    struct Boolean_And<> : True_t{};


    template<class...TYPES>
    struct Boolean_Or;

    template<class T, class...TYPES>
    struct Boolean_Or<T, TYPES...> : Selective_t< T::value, True_t, Boolean_Or<TYPES...> >{};

    template<>
    struct Boolean_Or<> : False_t{};


    template<class T>
    struct Boolean_Not : Boolean<!T::value>{};

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

    template<class T>  /* Declaration Only */
    static auto Mock() noexcept-> T;

    template<class TEST, class T = True_t>
    using SFINAE_t = decltype( (void)Mock<TEST>(),  Mock<T>() );


    template<class...>
    using Void_t = void;

    struct None{  constexpr None() = default;  };


    template<bool B, class T = void>
    struct Enable_if : Unconstructible{};

    template<class T>
    struct Enable_if<true, T> : As_type_itself<T>{};

    template<bool B, class T = void>
    using Enable_if_t = typename Enable_if<B, T>::type;


    template<bool B, class T>
    struct Guaranteed : As_type_itself<T>{  static_assert(B, "type guarantee failed.");  };

    template<bool B, class T = void>
    using Guaranteed_t = typename Guaranteed<B, T>::type;


    struct Omni_Convertible
    {
    	template<class T>
    	operator T() noexcept{  return *static_cast<T*>( reinterpret_cast<void*>(this) );  }
    };

    template<bool B = false>
    static auto Compile_Fails() noexcept-> Omni_Convertible
    {
        static_assert(B, "Compile Fails.");

        return {};
    }

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

    template<class T>
    struct Referenceless : As_type_itself<T>{};

    template<class T>
    struct Referenceless<T&> : Referenceless<T>{};

    template<class T>
    struct Referenceless<T&&> : Referenceless<T>{};

    template<class T>
    using Referenceless_t = typename Referenceless<T>::type;


    template<class T>
    struct Pointerless : As_type_itself<T>{};

    template<class T>
    struct Pointerless<T*> : Pointerless<T>{};

    template<class T>
    struct Pointerless<T* const> : Pointerless<T>{};

    template<class T>
    using Pointerless_t = typename Pointerless<T>::type;


    template<class T>
    struct Constless : As_type_itself<T>{};

    template<class T>
    struct Constless<T const> : Constless<T>{};

    template<class T>
    using Constless_t = typename Constless<T>::type;


    template<class T>
    struct Volatileless : As_type_itself<T>{};

    template<class T>
    struct Volatileless<T volatile> : Volatileless<T>{};

    template<class T>
    using Volatileless_t = typename Volatileless<T>::type;


    template<class T>
    using Decay_t = Volatileless_t<  Constless_t< Referenceless_t<T> >  >;

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

    template<class, class>
    struct is_Same : False_t{};

    template<class T>
    struct is_Same<T, T> : True_t{};


    template<class T>
    struct is_Void : is_Same<T, void>{};


    template<class FROM, class TO, class = void>
    struct is_Convertible : Boolean_And< is_Void<FROM>, is_Void<TO> >{};

    template<class FROM, class TO>
    struct is_Convertible
    <   FROM, TO
    ,   Void_t
        <   decltype( static_cast<TO(*)()>(nullptr) )
        ,   decltype( Mock<void(*)(TO)>()(Mock<FROM>()) ) 
        > 
    > : True_t{};


    template<class T1, class T2>
    using Has_Same_Origin = is_Same< Decay_t<T1>, Decay_t<T2> >;


    template<class FROM, class TO>
    using is_Convertible_but_Different_Origin
    =   Boolean< is_Convertible<FROM, TO>::value && !Has_Same_Origin<FROM, TO>::value >;


    template<class T>
    struct is_None : is_Same< Decay_t<T>, None >{};


    template<class T>
    struct is_Const : Boolean_Not<  is_Same< T, Constless_t<T> >  >{};


    template<class T>
    struct is_Volatile : Boolean_Not<  is_Same< T, Volatileless_t<T> >  >{};


    template<class T>
    struct is_Pointer : Boolean_Not<  is_Same< T, Pointerless_t<T> >  >{};


    template<class T>
    struct is_Lvalue_Reference : False_t{};

    template<class T>
    struct is_Lvalue_Reference<T&> : True_t{};

    template<class T>
    struct is_Rvalue_Reference : False_t{};

    template<class T>
    struct is_Rvalue_Reference<T&&> : True_t{};

    template<class T>
    struct is_Reference : Boolean_Or< is_Lvalue_Reference<T>, is_Rvalue_Reference<T> >{};


    template<class T>
    struct is_Bounded_Array : False_t{};

    template<class T, size_t N>
    struct is_Bounded_Array<T[N]> : True_t{};

    template<class T>
    struct is_Unbounded_Array : False_t{};

    template<class T>
    struct is_Unbounded_Array<T[]> : True_t{};

    template<class T>
    struct is_Primitive_Array : Boolean_Or< is_Bounded_Array<T>, is_Unbounded_Array<T> >{};


    template<class T, class = void>
    struct is_Class_or_Union : False_t{};

    template<class T>
    struct is_Class_or_Union
    <   T, Void_t<  decltype( static_cast< int Decay_t<T>::* >(0) )  >
    > : True_t{};


    template<class BASE, class DERV>
    struct is_inherited : Unconstructible
    {
    private:
        template<class B> /* Declaration Only */
        static auto _calc(B const volatile*)-> True_t;

        template<class...> /* Declaration Only */
        static auto _calc(void const volatile*)-> False_t;

        template<class B, class D> /* Declaration Only */
        static auto _eval(int)-> decltype(  _calc<B>( static_cast<D*>(nullptr) )  );

        template<class, class> /* Declaration Only */
        static auto _eval(...)-> False_t;

    public:
        static bool constexpr value 
        =   Boolean_And
            <   is_Class_or_Union<BASE>, is_Class_or_Union<DERV>
            ,   decltype( _eval< Referenceless_t<BASE>, Referenceless_t<DERV> >(0) )
            >:: value; 

        using type = Boolean<value>;
    };

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
    
    template< class T, class RES = Referenceless_t<T>&& >
    static auto Move(T&& t) noexcept-> RES{  return static_cast<RES>(t);  }


    template<class T>
    static auto Forward(Referenceless_t<T>& t) noexcept-> T&&{  return static_cast<T&&>(t);  }

    template<class T>
    static auto Forward(Referenceless_t<T>&& t) noexcept-> T&&{  return Move(t);  }


    template<bool CONDITION>
    struct _Move_if_Helper;

    template<>
    struct _Move_if_Helper<false> : Unconstructible
    {
        template<class T>
        static auto _calc(T&& t) noexcept-> SGM_DECLTYPE_AUTO(  Forward<T>(t)  )
    };

    template<>
    struct _Move_if_Helper<true> : Unconstructible
    {
        template<class T>
        static auto _calc(T&& t) noexcept-> SGM_DECLTYPE_AUTO(  Move(t)  )
    };

    template<bool CONDITION, class T>
    static auto Move_if(T&& t) noexcept-> SGM_DECLTYPE_AUTO
    (
        _Move_if_Helper<CONDITION>::_calc( static_cast<T&&>(t) )  
    )


    template<class T>
    static auto Swap(T& a, T& b) noexcept-> void
    {
        T temp = Move(a);

        a = Move(b);
        b = Move(temp);
    }


    template<class T>
    static auto immut(T const& t) noexcept-> T const&{  return t;  }

    template<class T>
    static auto immut(T const* p) noexcept-> T const*{  return p;  }

    template<class T>
    static auto immut(T const&&)-> T const&& = delete;


    template<class T>
    static auto Decay(T&& t) noexcept-> Decay_t<T>{  return t;  }


	template<class T>
	static auto Address_of(T& t) noexcept
	->	Enable_if_t< Boolean_Or< is_Class_or_Union<T>, is_Primitive_Array<T> >::value, T* >
	{
		return 
		reinterpret_cast<T*>
		(	&const_cast<wchar_t&>( reinterpret_cast<wchar_t const volatile&>(t) )
		);
	}

	template<class T>
	static auto Address_of(T& t) noexcept
	->	Enable_if_t< !Boolean_Or< is_Class_or_Union<T>, is_Primitive_Array<T> >::value, T* >
	{
		return &t;
	}


    template<class SRC, class TGT>
    struct Qualify_Like
    {
        using _refless_src = Referenceless_t<SRC>;
        using _t1 = Selective_t< is_Const<_refless_src>::value, TGT const, TGT >;
        
        using type
        =   Selective_t
            <   is_Same<SRC, _refless_src>::value, _t1
            ,   Selective_t< is_Rvalue_Reference<SRC>::value, _t1&&, _t1& >
            >;
    };

    template<class SRC, class TGT>
    using Qualify_Like_t = typename Qualify_Like<SRC, TGT>::type;


    template<class T>
    struct Alephless
    :   As_type_itself<  Selective_t< is_Rvalue_Reference<T>::value, Referenceless_t<T>, T >  >
    {};

    template<class T>
    using Alephless_t = typename Alephless<T>::type;

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
    
    template<unsigned, class...TYPES>
    struct Nth_Type;

    template<unsigned N>
    struct Nth_Type<N> : As_type_itself<void>{};

    template<unsigned N, class T, class...TYPES>
    struct Nth_Type<N, T, TYPES...> 
    :   Selective_t< N==0, As_type_itself<T>, Nth_Type<N-1, TYPES...> >{};

    template<unsigned N, class...TYPES>
    using Nth_t = typename Nth_Type<N, TYPES...>::type;

    template<class...TYPES>
    using First_t = Nth_t<0, TYPES...>;

    template<class...TYPES>
    using Last_t = Nth_t<sizeof...(TYPES)-1, TYPES...>;


    template<unsigned N>
    struct _Nth_Param_Helper : Unconstructible
    {
        template<class T, class...ARGS>
        static auto calc(T&&, ARGS&&...args) noexcept-> Nth_t<N-1, ARGS&&...>
        {
            return _Nth_Param_Helper<N-1>::calc( Forward<ARGS>(args)... );
        }
    };

    template<>
    struct _Nth_Param_Helper<0> : Unconstructible
    {
        template<class T, class...ARGS>
        static auto calc(T&& t, ARGS&&...) noexcept-> T&&{  return Forward<T>(t);  }
    };

    template<unsigned N, class...ARGS>
    static auto Nth_Param(ARGS&&...args) noexcept-> SGM_DECLTYPE_AUTO
    (
        _Nth_Param_Helper<N>::calc( Forward<ARGS>(args)... )  
    )


    template<class FUNC, class...ARGS>
    using invocation_Result_t = decltype( Mock<FUNC>()(Mock<ARGS>()...) );

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
    
    template< template<class...> class FUNCTOR, class...TYPES >
    struct Check_if_All;

    template< template<class...> class FUNCTOR, class T, class...TYPES >
    struct Check_if_All<FUNCTOR, T, TYPES...>
    :   Boolean_And< FUNCTOR<T>, Check_if_All<FUNCTOR, TYPES...> >{};

    template< template<class...> class FUNCTOR >
    struct Check_if_All<FUNCTOR> : True_t{};


    template< template<class...> class FUNCTOR, class...TYPES >
    struct Check_if_Any;

    template< template<class...> class FUNCTOR, class T, class...TYPES >
    struct Check_if_Any<FUNCTOR, T, TYPES...>
    :   Boolean_Or< FUNCTOR<T>, Check_if_Any<FUNCTOR, TYPES...> >{};

    template< template<class...> class FUNCTOR >
    struct Check_if_Any<FUNCTOR> : False_t{};


    template<class...TYPES>
    struct Aleph_Check : Check_if_Any<is_Rvalue_Reference, TYPES...>{};


    template<class T>
    struct Has_Type : Unconstructible
    {
    private:
        template<class Q>
        struct _is_same_to_T : Boolean_Or< is_Same<Q, T>, is_inherited<T, Q> >{};

    public:
        template<class...TYPES>
        struct among : Check_if_Any<_is_same_to_T, TYPES...>{};
    };


    template< template<class...> class FUNCTOR, class...TYPES >
    struct Satisfying_Among;

    template< template<class...> class FUNCTOR, class T, class...TYPES >
    struct Satisfying_Among<FUNCTOR, T, TYPES...>
    :   Selective_t< FUNCTOR<T>::value, As_type_itself<T>, Satisfying_Among<FUNCTOR, TYPES...> >
    {};

    template< template<class...> class FUNCTOR >
    struct Satisfying_Among<FUNCTOR> : As_type_itself<None>{};

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
    
	template<bool IS_MUTABLE>
	struct Mutability_tag;

	using Variable_t = Mutability_tag<true>;
	using invariable_t = Mutability_tag<false>;


    template<  class T, bool = Boolean_Or< is_Pointer<T>, is_Reference<T> >::value  >
    struct is_immutable;

    template<class T>
    struct is_immutable<T, false> : is_Const<T>{};

    template<class T>
    struct is_immutable<T, true> : is_immutable<  Pointerless_t< Referenceless_t<T> >  >{};

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

    template<class MEMFN_PTR, class HOST_PTR>
    class Member_Function
    {
    private:
        template<class...ARGS>
        struct _Helper
        :   As_type_itself
            <   decltype( (Mock<HOST_PTR>()->*Mock<MEMFN_PTR>())(Mock<ARGS>()...) )
            >
        ,   As_value_itself
            <   bool
            ,   noexcept( (Mock<HOST_PTR>()->*Mock<MEMFN_PTR>())(Mock<ARGS>()...) )
            >
        {};
        

    public:
        Member_Function(HOST_PTR const host_ptr, MEMFN_PTR const memfn_ptr) noexcept
        :   _host_ptr(host_ptr), _memfn_ptr(memfn_ptr){}


        template<class...ARGS>
        auto operator()(ARGS&&...args) const noexcept(_Helper<ARGS...>::value)
        ->  typename _Helper<ARGS...>::type
        {   
            return (_host_ptr->*_memfn_ptr)( Forward<ARGS>(args)... );
        }


    private:
        HOST_PTR _host_ptr;
        MEMFN_PTR _memfn_ptr;
    };


    template<class MEMFN_PTR, class HOST>
    static auto Memfunc(HOST& host, MEMFN_PTR const memfn_ptr) noexcept-> SGM_DECLTYPE_AUTO
    (
        Member_Function< MEMFN_PTR, Referenceless_t<HOST>* >( Address_of(host), memfn_ptr )
    )

}
//========//========//========//========//=======#//========//========//========//========//=======#


#ifndef SGM_USER_DEFINED_TYPE_CHECK
    #define SGM_USER_DEFINED_TYPE_CHECK(TEM_DECL, TITLE, ...)   \
        template<class Q_CLASS> \
        struct is_##TITLE : sgm::Unconstructible    \
        {   \
        private:    \
            template<TEM_DECL>  /* Declaration Only */  \
            static auto _calc(TITLE __VA_ARGS__ const volatile*)-> sgm::True_t; \
            \
            template<class...>  \
            static auto _calc(void const volatile*)-> sgm::False_t; \
            \
        public: \
            using type \
            =   decltype(  _calc( static_cast< sgm::Referenceless_t<Q_CLASS>* >(nullptr) )  );\
            \
            static bool constexpr value = type::value;  \
        }


#else
    #error SGM_USER_DEFINED_TYPE_CHECK was already defined somewhere else.
#endif


#endif // end of #ifndef _SGM_TYPE_ANALYSIS_