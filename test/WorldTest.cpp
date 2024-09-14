/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "WorldTest.hpp"
#include <iostream>


static void test() noexcept(false);


template<class A, class...ARGS>
static void log_message(A&& a, ARGS&&...args) noexcept
{
    std::wcout << static_cast<A&&>(a);
    
    if constexpr( sizeof...(ARGS) != 0 )
        log_message( static_cast<ARGS&&>(args)... );
}


static void system_pause() noexcept
{
    std::wcout << L"Press Enter key to continue. . .";
    std::cin.get();   
}


bool wt::Tests(wchar_t const* const module_title) noexcept
{
    ::log_message
    (   L"//========//========//========//========//=======#\n"
    ,   module_title, L" test Start\n"
    );

    try
    {
        ::test();

        ::log_message
        (   module_title, L" test Complete\n"
        ,   L"//========//========//========//========//=======#\n"
        );

        ::system_pause();
        
        return true;
    }
    catch(...)
    {
        std::wcout << L"Unexpected Error!\n";
    }

    ::log_message
    (   module_title, L" test Failed"
    ,   L"//========//========//========//========//=======#\n"
    );

    ::system_pause();

    return false;
}
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Specification/Test_Specification.hpp"

#include "SGM/Array/Test_Array.hpp"
#include "SGM/Abbreviable/Test_Avatar.hpp"
#include "SGM/Abbreviable/Test_Convoy.hpp"
#include "SGM/Abbreviable/Test_Nullable.hpp"
#include "SGM/Abbreviable/Test_Pinweight.hpp"
#include "SGM/Boomerang/Test_Boomerang.hpp"
#include "SGM/Concurrency/Test_Concurrency.hpp"
#include "SGM/Family/Test_Family.hpp"
#include "SGM/Flag_Set/Test_Flag_Set.hpp"
#include "SGM/Functor/Test_Functor.hpp"
#include "SGM/Functor/Test_Parameters.hpp"
#include "SGM/High_Templar/Test_High_Templar.hpp"
#include "SGM/interface_Traits/Test_interface_Traits.hpp"
#include "SGM/iterable/Test_iterable.hpp"
#include "SGM/List/Test_List.hpp"
#include "SGM/Mathexpr/Test_Mathexpr.hpp"
#include "SGM/Operators_of/Test_Operators_of.hpp"
#include "SGM/Queue/Test_Queue.hpp"
#include "SGM/Recursion/Test_Recursion.hpp"
#include "SGM/Span/Test_Span.hpp"
#include "SGM/Type_Analysis/Test_Type_Analysis.hpp"
#include "SGM/Mathexpr/Test_Mathexpr.hpp"


void test() noexcept(false)
{
    sgm::spec::Test_Specification::test();

    sgm::spec::Test_Array::test();
    sgm::spec::Test_Avatar::test();
    sgm::spec::Test_Convoy::test();
    sgm::spec::Test_Nullable::test();
    sgm::spec::Test_Pinweight::test();
    sgm::spec::Test_Boomerang::test();
    sgm::spec::Test_Concurrency::test();
    sgm::spec::Test_Family::test();
    sgm::spec::Test_Flag_Set::test();
    sgm::spec::Test_High_Templar::test();
    sgm::spec::Test_interface_Traits::test();
    sgm::spec::Test_iterable::test();    
    sgm::spec::Test_List::test();
    sgm::spec::Test_Mathexpr::test();
    sgm::spec::Test_Operators_of::test();
    sgm::spec::Test_Queue::test();
    sgm::spec::Test_Recursion::test();
	sgm::spec::Test_Span::test();
    sgm::spec::Test_Type_Analysis::test();
    sgm::fp::spec::Test_Functor::test();
    sgm::fp::spec::Test_Parameters::test();
}


int main(int const, char const* const[])
{
    wchar_t const* const os   
#if defined(_WINDOWS_SOLUTION_)
    =   L"Windows";
#elif defined(_POSIX_SOLUTION_)
    =   L"POSIX";
#else
    =   L"Others";
#endif

    return wt::Tests(os),  (int)0;
}