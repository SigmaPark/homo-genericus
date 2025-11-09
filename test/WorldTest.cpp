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


#include "SGM/Container/Test_Array.hpp"
#include "SGM/Wrapper/Test_Avatar.hpp"
#include "SGM/Wrapper/Test_Convoy.hpp"
#include "SGM/Wrapper/Test_Nullable.hpp"
#include "SGM/Wrapper/Test_Pinweight.hpp"
#include "SGM/Wrapper/Test_Boomerang.hpp"
#include "SGM/Concurrency/Test_Fork_and_Join.hpp"
#include "SGM/Concurrency/Test_Concurrent_Pipeline.hpp"
#include "SGM/Wrapper/Test_CV_Complete.hpp"
#include "SGM/Utility/Test_Family.hpp"
#include "SGM/Utility/Test_Flag_Set.hpp"
#include "SGM/FP/Test_Functor.hpp"
#include "SGM/FP/Test_Parameters.hpp"
#include "SGM/FP/Test_High_Templar.hpp"
#include "SGM/TMP/Test_interface_Traits.hpp"
#include "SGM/Container/Test_iterable.hpp"
#include "SGM/Container/Test_List.hpp"
#include "SGM/Utility/Test_Mathexpr.hpp"
#include "SGM/Wrapper/Test_Operators_of.hpp"
#include "SGM/Container/Test_Queue.hpp"
#include "SGM/FP/Test_Recursion.hpp"
#include "SGM/Container/Test_Span.hpp"
#include "SGM/TMP/Test_Type_Analysis.hpp"


void test() noexcept(false)
{
    sgm::test::Test_Array::test();
    sgm::test::Test_Avatar::test();
    sgm::test::Test_Convoy::test();
    sgm::test::Test_Nullable::test();
    sgm::test::Test_Pinweight::test();
    sgm::test::Test_Boomerang::test();
    sgm::test::Test_Fork_and_Join::test();
    sgm::test::Test_Concurrent_Pipeline::test();
    sgm::test::Test_CV_Complete::test();
    sgm::test::Test_Family::test();
    sgm::test::Test_Flag_Set::test();
    sgm::test::Test_High_Templar::test();
    sgm::test::Test_interface_Traits::test();
    sgm::test::Test_iterable::test();    
    sgm::test::Test_List::test();
    sgm::test::Test_Mathexpr::test();
    sgm::test::Test_Operators_of::test();
    sgm::test::Test_Queue::test();
    sgm::test::Test_Recursion::test();
	sgm::test::Test_Span::test();
    sgm::test::Test_Type_Analysis::test();
    sgm::test::Test_Functor::test();
    sgm::test::Test_Parameters::test();
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