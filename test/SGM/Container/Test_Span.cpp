/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Container/Span.hpp"
#include "Test_Span.hpp"
#include "SGM/Container/Array.hpp"
#include "SGM/Container/List.hpp"


using h2u::Are_Equivalent_Ranges;
using h2u::Specimen;
using sgm::Array;
using sgm::List;


static void intro()
{
    using namespace h2u;

    mdo
    <<  Title(L"introduction")
    <<  L"Span is an iterable type that refers to a certain length of range "
    <<  L"between two iteraters or pointers."
    <<  empty_line;
}


static void Span_is_iterable()
{   
    {
        Array<Specimen, 3> sarr{};
        Array<Specimen> darr{};
        List<Specimen> Li{};


        static_assert
        (   sgm::Boolean_And
            <   sgm::is_iterable< decltype( sgm::Span(sarr) ) >
            ,   sgm::is_iterable< decltype( sgm::Span(darr) ) >
            ,   sgm::is_iterable< decltype( sgm::Span(Li) ) >
            >:: value
        ,   ""
        );
    }
    {
        Array<Specimen, 3> const sarr{};
        Array<Specimen> const darr{};
        List<Specimen> const Li{};

        auto ss = sgm::Span(sarr);
        auto ds = sgm::Span(darr);
        auto Ls = sgm::Span(Li);

        static_assert
        (   sgm::Boolean_And
            <   sgm::is_iterable< decltype(ss) >
            ,   sgm::is_iterable< decltype(ds) >
            ,   sgm::is_iterable< decltype(Ls) >
            ,   sgm::is_immutable< decltype(*ss.begin()) >
            ,   sgm::is_immutable< decltype(*ds.begin()) >
            ,   sgm::is_immutable< decltype(*Ls.begin()) >
            >:: value
        ,   ""
        );
    }
}


static void Primitive_Array()
{
    using namespace h2u;

    mdo <<  Title(L"Span from Primitive Array") << newl;
    
BEGIN_CODE_BLOCK(primitive_array_ex)
    {
        Specimen arr[] = { Specimen(2), Specimen(4), Specimen(6) };

        auto spn = sgm::Span(arr);

        H2U_ASSERT( Are_Equivalent_Ranges(spn, arr) );

        spn[0] = -1;
        spn[1] = -3;

        H2U_ASSERT
        (   Are_Equivalent_Ranges
            (   arr
            ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
            )
        );
    }
END_CODE_BLOCK_AND_LOAD(primitive_array_ex)

    {
        Specimen arr[] = { Specimen(2), Specimen(4), Specimen(6) };

        auto spn = sgm::Span<2>(arr);

        H2U_ASSERT
        (   Are_Equivalent_Ranges( spn, Array<Specimen, 2>{Specimen(2), Specimen(4)} )
        );

        spn[0] = -1;
        spn[1] = -3;

        H2U_ASSERT
        (   Are_Equivalent_Ranges
            (   arr
            ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
            )
        );
    }
    {
        Specimen arr[] = { Specimen(2), Specimen(4), Specimen(6) };

        auto cspn = sgm::Span<3>( sgm::immut(arr) );

        H2U_ASSERT( Are_Equivalent_Ranges(cspn, arr) );

        static_assert
        (   (   sgm::is_immutable<decltype(cspn[0])>::value
            &&  sgm::is_immutable<decltype(cspn[1])>::value
            &&  sgm::is_immutable<decltype(cspn[2])>::value
            )
        ,   ""
        );
    }

    mdo << empty_line;
}


static void Static_Size_Array()
{
    using namespace h2u;

    mdo
    <<  Title(L"Static-sized Span")
    <<  L"sgm::Span can be static-sized iterable by setting constexpr size_t as a template "
    <<  L"parameter explicitly." << empty_line;

    mdo << Title(L"by a Starting Pointer", 2) << newl;

BEGIN_CODE_BLOCK(static_size_array_by_starting_pointer_ex)
    {
        Array<Specimen, 3> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span<3>(arr.data());

        H2U_ASSERT( Are_Equivalent_Ranges(spn, arr) );

        spn[0] = -1;
        spn[1] = -3;

        H2U_ASSERT
        (   Are_Equivalent_Ranges
            (   arr
            ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
            )
        );
    }
	{
        Array<Specimen, 3> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span<3>(arr.cdata()); 

        H2U_ASSERT( Are_Equivalent_Ranges(cspn, arr) ); 

        static_assert
        (   (   sgm::is_immutable<decltype(cspn[0])>::value
            &&  sgm::is_immutable<decltype(cspn[1])>::value
            &&  sgm::is_immutable<decltype(cspn[2])>::value
            )
        ,   ""
        );
    }
END_CODE_BLOCK_AND_LOAD(static_size_array_by_starting_pointer_ex)

    mdo << empty_line;


    mdo << Title(L"by an iterable", 2) << newl;

BEGIN_CODE_BLOCK(static_size_array_by_iterable_ex)
    {
        Array<Specimen, 3> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span<3>(arr); 

        H2U_ASSERT( Are_Equivalent_Ranges(spn, arr) );

        spn[0] = -1;
        spn[1] = -3;

        H2U_ASSERT
        (   Are_Equivalent_Ranges
            (   arr
            ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
            )
        );
    }
	{
        Array<Specimen, 3> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span<3>( sgm::immut(arr) );

        H2U_ASSERT( Are_Equivalent_Ranges(cspn, arr) ); 

        static_assert
        (   (   sgm::is_immutable<decltype(cspn[0])>::value
            &&  sgm::is_immutable<decltype(cspn[1])>::value
            &&  sgm::is_immutable<decltype(cspn[2])>::value
            )
        ,   ""
        );
    }
END_CODE_BLOCK_AND_LOAD(static_size_array_by_iterable_ex)

    mdo << empty_line;
}


static void Dynamic_Size_Array()
{
    using namespace h2u;

    mdo 
    <<  Title(L"Span by iterables with Size in Runtime")
    <<  L"sgm::Span without explicit size infomation as a template parameter also can refer to "
    <<  L"a range."
    <<  empty_line;

    mdo << Title(L"by an iterable", 2) << newl;

BEGIN_CODE_BLOCK(dynamic_size_array_by_iterable_ex)
    {
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span(arr);

        H2U_ASSERT( Are_Equivalent_Ranges(spn, arr) );

        spn[0] = -1;
        spn[1] = -3;

        H2U_ASSERT
        (   Are_Equivalent_Ranges
            (   arr
            ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
            )
        );
    }
	{
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span( sgm::immut(arr) ); 

        H2U_ASSERT( Are_Equivalent_Ranges(cspn, arr) ); 

        static_assert
        (   (   sgm::is_immutable<decltype(cspn[0])>::value
            &&  sgm::is_immutable<decltype(cspn[1])>::value
            &&  sgm::is_immutable<decltype(cspn[2])>::value
            )
        ,   ""
        );
    }
END_CODE_BLOCK_AND_LOAD(dynamic_size_array_by_iterable_ex)

    mdo << empty_line;


    mdo << Title(L"by 2 iterators", 2) << newl;

BEGIN_CODE_BLOCK(dynamic_size_array_by_2_iterators_ex)
    {
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span(arr.begin(), arr.end());

        H2U_ASSERT( Are_Equivalent_Ranges(spn, arr) );

        spn[0] = -1;
        spn[1] = -3;

        H2U_ASSERT
        (   Are_Equivalent_Ranges
            (   arr
            ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
            )
        );
    }
	{  
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span(arr.cbegin(), arr.cend()); 

        H2U_ASSERT( Are_Equivalent_Ranges(cspn, arr) ); 

        static_assert
        (   (   sgm::is_immutable<decltype(cspn[0])>::value
            &&  sgm::is_immutable<decltype(cspn[1])>::value
            &&  sgm::is_immutable<decltype(cspn[2])>::value
            )
        ,   ""
        );
    }
END_CODE_BLOCK_AND_LOAD(dynamic_size_array_by_2_iterators_ex)

    mdo << empty_line;


    mdo << Title(L"by an iterator and Size", 2) << newl;

BEGIN_CODE_BLOCK(dynamic_size_array_by_iterator_and_size)
    {
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span(arr.begin(), 3); 

        H2U_ASSERT( Are_Equivalent_Ranges(spn, arr) );

        spn[0] = -1;
        spn[1] = -3;

        H2U_ASSERT
        (   Are_Equivalent_Ranges
            (   arr
            ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
            )
        );
    }
	{
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span(arr.cbegin(), 3); 

        H2U_ASSERT( Are_Equivalent_Ranges(cspn, arr) ); 

        static_assert
        (   (   sgm::is_immutable<decltype(cspn[0])>::value
            &&  sgm::is_immutable<decltype(cspn[1])>::value
            &&  sgm::is_immutable<decltype(cspn[2])>::value
            )
        ,   ""
        );
    }
END_CODE_BLOCK_AND_LOAD(dynamic_size_array_by_iterator_and_size)

    mdo << empty_line;
}


static void Pointer_and_Size()
{
    using namespace h2u;

    mdo
    <<  Title(L"Span refering to Contiguous Memories")
    <<  L"A contiguous memory range also can be refered by sgm::Span." 
    <<  empty_line;

    mdo << Title(L"by a Pointer and Size", 2) << newl;
    
BEGIN_CODE_BLOCK(contiguous_memory_by_pointer_and_size_ex)
    {
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span(arr.data(), arr.size());

        H2U_ASSERT( Are_Equivalent_Ranges(spn, arr) );

        spn[0] = -1;
        spn[1] = -3;

        H2U_ASSERT
        (   Are_Equivalent_Ranges
            (   arr
            ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
            )
        );
    }
	{
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span(arr.cdata(), arr.size()); 

        H2U_ASSERT( Are_Equivalent_Ranges(cspn, arr) ); 

        static_assert
        (   (   sgm::is_immutable<decltype(cspn[0])>::value
            &&  sgm::is_immutable<decltype(cspn[1])>::value
            &&  sgm::is_immutable<decltype(cspn[2])>::value
            )
        ,   ""
        );
    }
	{
        Array<Specimen> const arr{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span(arr.data(), arr.size()); 

        H2U_ASSERT( Are_Equivalent_Ranges(cspn, arr) ); 

        static_assert
        (   (   sgm::is_immutable<decltype(cspn[0])>::value
            &&  sgm::is_immutable<decltype(cspn[1])>::value
            &&  sgm::is_immutable<decltype(cspn[2])>::value
            )
        ,   ""
        );
    }
END_CODE_BLOCK_AND_LOAD(contiguous_memory_by_pointer_and_size_ex)

    mdo << empty_line;


    mdo << Title(L"by 2 Pointers", 2) << newl;

BEGIN_CODE_BLOCK(contiguous_memory_by_2_pointer_ex)
    {
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto begin_ptr = arr.data(), end_ptr = arr.data() + arr.size();
        auto spn = sgm::Span(begin_ptr, end_ptr);

        H2U_ASSERT( Are_Equivalent_Ranges(spn, arr) );

        spn[0] = -1;
        spn[1] = -3;

        H2U_ASSERT
        (   Are_Equivalent_Ranges
            (   arr
            ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
            )
        );
    }
	{
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto begin_cptr = arr.cdata(), end_cptr = arr.cdata() + arr.size();
        auto cspn = sgm::Span(begin_cptr, end_cptr);

        H2U_ASSERT( Are_Equivalent_Ranges(cspn, arr) ); 

        static_assert
        (   (   sgm::is_immutable<decltype(cspn[0])>::value
            &&  sgm::is_immutable<decltype(cspn[1])>::value
            &&  sgm::is_immutable<decltype(cspn[2])>::value
            )
        ,   ""
        );
    }
	{
        Array<Specimen> const carr{Specimen(2), Specimen(4), Specimen(6)};

        auto begin_ptr = carr.data(), end_ptr = carr.data() + carr.size();
        auto cspn = sgm::Span(begin_ptr, end_ptr);

        H2U_ASSERT( Are_Equivalent_Ranges(cspn, carr) ); 

        static_assert
        (   (   sgm::is_immutable<decltype(cspn[0])>::value
            &&  sgm::is_immutable<decltype(cspn[1])>::value
            &&  sgm::is_immutable<decltype(cspn[2])>::value
            )
        ,   ""
        );
    }
END_CODE_BLOCK_AND_LOAD(contiguous_memory_by_2_pointer_ex)

    mdo << empty_line;
}


static void Linked_List()
{
    using namespace h2u;

    mdo
    <<  Title(L"Range on Non-contiguous Memories")
    <<  L"sgm::Span doesn't care whether a refered range is on conteguous memories or not. "
    <<  L"Here's are exmples with non-contiguous memory ranges by linked list containers." 
    <<  empty_line;


    mdo << Title(L"by an iterable", 2) << newl;

BEGIN_CODE_BLOCK(linked_list_by_iterable_ex)
    {
        List<Specimen> Li{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span(Li);

        H2U_ASSERT( Are_Equivalent_Ranges(spn, Li) );

        {
            auto itr = spn.begin();

            *itr++ = -1;
            *itr = -3;
        }

        H2U_ASSERT
        (   Are_Equivalent_Ranges
            (   Li
            ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
            )
        );
    }
	{
        List<Specimen> Li{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span( sgm::immut(Li) );

        H2U_ASSERT( Are_Equivalent_Ranges(cspn, Li) );

        static_assert
        (   (   sgm::is_immutable<decltype(*cspn.begin())>::value
            &&  sgm::is_immutable<decltype(*cspn.end())>::value
            )
        ,   ""
        );
    }
END_CODE_BLOCK_AND_LOAD(linked_list_by_iterable_ex)

    mdo << empty_line;


    mdo << Title(L"by 2 iterators", 2) << newl;

BEGIN_CODE_BLOCK(linked_list_by_2_iterators_ex)
    {
        List<Specimen> Li{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span(Li.begin(), Li.end());

        H2U_ASSERT( Are_Equivalent_Ranges(spn, Li) );

        {
            auto itr = spn.begin();

            *itr++ = -1;
            *itr = -3;
        }

        H2U_ASSERT
        (   Are_Equivalent_Ranges
            (   Li
            ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
            )
        );
    }
	{
        List<Specimen> Li{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span(Li.cbegin(), Li.cend());

        H2U_ASSERT( Are_Equivalent_Ranges(cspn, Li) );

        static_assert
        (   (   sgm::is_immutable<decltype(*cspn.begin())>::value
            &&  sgm::is_immutable<decltype(*cspn.end())>::value
            )
        ,   ""
        );
    }
END_CODE_BLOCK_AND_LOAD(linked_list_by_2_iterators_ex)

    mdo << empty_line;


    mdo << Title(L"by an iterator and Size", 2) << newl;

BEGIN_CODE_BLOCK(linked_list_by_iterator_and_size_ex)
    {
        List<Specimen> Li{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span(Li.begin(), 3);

        H2U_ASSERT( Are_Equivalent_Ranges(spn, Li) );

        {
            auto itr = spn.begin();

            *itr++ = -1;
            *itr = -3;
        }

        H2U_ASSERT
        (   Are_Equivalent_Ranges
            (   Li
            ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
            )
        );
    }
	{
        List<Specimen> Li{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span(Li.cbegin(), 3);

        H2U_ASSERT( Are_Equivalent_Ranges(cspn, Li) );

        static_assert
        (   (   sgm::is_immutable<decltype(*cspn.begin())>::value
            &&  sgm::is_immutable<decltype(*cspn.end())>::value
            )
        ,   ""
        );
    }
END_CODE_BLOCK_AND_LOAD(linked_list_by_iterator_and_size_ex)

    mdo << empty_line;
}
//========//========//========//========//=======#//========//========//========//========//=======#


H2U_HOW2USE_TESTS(sgm::test::Test_, Span, /**/)
{   ::intro
,   ::Span_is_iterable
,   ::Primitive_Array
,   ::Static_Size_Array
,   ::Dynamic_Size_Array
,   ::Pointer_and_Size
,   ::Linked_List
};
