/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Span.hpp"
#include "Test_Span.hpp"
#include "../Array/Array.hpp"
#include "../List/List.hpp"


using sgm::spec::is_True;
using sgm::spec::Are_Equivalent_Ranges;
using sgm::spec::Specimen;
using sgm::Array;
using sgm::List;


static void intro()
{
    using namespace sgm::spec;

    mdo
    <<  Title("introduction")
    <<  "Span is an iterable type that refers to a certain length of range "
    <<  "between two iteraters or pointers."
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
    using namespace sgm::spec;

    mdo <<  Title("Span from Primitive Array") << newl;
    
BEGIN_CODE_BLOCK(primitive_array_ex)
    {
        Specimen arr[] = { Specimen(2), Specimen(4), Specimen(6) };

        auto spn = sgm::Span(arr);

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges
        (   arr
        ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
        );
    }
END_CODE_BLOCK_AND_LOAD(primitive_array_ex)

    {
        Specimen arr[] = { Specimen(2), Specimen(4), Specimen(6) };

        auto spn = sgm::Span<2>(arr);

        Are_Equivalent_Ranges( spn, Array<Specimen, 2>{Specimen(2), Specimen(4)} );

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges
        (   arr
        ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
        );
    }
    {
        Specimen arr[] = { Specimen(2), Specimen(4), Specimen(6) };

        auto cspn = sgm::Span<3>( sgm::immut(arr) );

        Are_Equivalent_Ranges(cspn, arr);

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
    using namespace sgm::spec;

    mdo
    <<  Title("Static-sized Span")
    <<  "sgm::Span can be static-sized iterable by setting constexpr size_t as a template "
    <<  "parameter explicitly." << empty_line;

    mdo << Title("by a Starting pointer", 2) << newl;

BEGIN_CODE_BLOCK(static_size_array_by_starting_pointer_ex)
    {
        Array<Specimen, 3> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span<3>(arr.data());

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges
        (   arr
        ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
        );
    }
	{
        Array<Specimen, 3> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span<3>(arr.cdata()); 

        Are_Equivalent_Ranges(cspn, arr); 

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


    mdo << Title("by an iterable", 2) << newl;

BEGIN_CODE_BLOCK(static_size_array_by_iterable_ex)
    {
        Array<Specimen, 3> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span<3>(arr); 

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges
        (   arr
        ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
        );
    }
	{
        Array<Specimen, 3> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span<3>( sgm::immut(arr) );

        Are_Equivalent_Ranges(cspn, arr); 

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
    using namespace sgm::spec;

    mdo 
    <<  Title("Span by iterables with Size in Runtime")
    <<  "sgm::Span without explicit size infomation as a template parameter also can refer to "
    <<  "a range."
    <<  empty_line;

    mdo << Title("by an iterable", 2) << newl;

BEGIN_CODE_BLOCK(dynamic_size_array_by_iterable_ex)
    {
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span(arr);

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges
        (   arr
        ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
        );
    }
	{
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span( sgm::immut(arr) ); 

        Are_Equivalent_Ranges(cspn, arr); 

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


    mdo << Title("by 2 iterators", 2) << newl;

BEGIN_CODE_BLOCK(dynamic_size_array_by_2_iterators_ex)
    {
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span(arr.begin(), arr.end());

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges
        (   arr
        ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
        );
    }
	{  
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span(arr.cbegin(), arr.cend()); 

        Are_Equivalent_Ranges(cspn, arr); 

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


    mdo << Title("by an iterator and size", 2) << newl;

BEGIN_CODE_BLOCK(dynamic_size_array_by_iterator_and_size)
    {
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span(arr.begin(), 3); 

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges
        (   arr
        ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
        );
    }
	{
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span(arr.cbegin(), 3); 

        Are_Equivalent_Ranges(cspn, arr); 

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
    {
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto spn = sgm::Span(arr.data(), arr.size());

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges
        (   arr
        ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
        );
    }
	{
        Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

        auto cspn = sgm::Span(arr.cdata(), arr.size()); 

        Are_Equivalent_Ranges(cspn, arr); 

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

        Are_Equivalent_Ranges(cspn, arr); 

        static_assert
        (   (   sgm::is_immutable<decltype(cspn[0])>::value
            &&  sgm::is_immutable<decltype(cspn[1])>::value
            &&  sgm::is_immutable<decltype(cspn[2])>::value
            )
        ,   ""
        );
    }
}


static void Linked_List()
{
    List<Specimen> Li{Specimen(2), Specimen(4), Specimen(6)};

    {
        auto spn = sgm::Span(Li);

        Are_Equivalent_Ranges(spn, Li);

        {
            auto itr = spn.begin();

            *itr++ = -1;
            *itr = -3;
        }

        Are_Equivalent_Ranges
        (   Li
        ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
        );
    }
    {
        auto spn = sgm::Span(Li.begin(), Li.end());

        Are_Equivalent_Ranges(spn, Li);

        {
            auto itr = spn.begin();

            *itr++ = -1;
            *itr = -3;
        }

        Are_Equivalent_Ranges
        (   Li
        ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
        );
    }
    {
        auto spn = sgm::Span(Li.begin(), 3);

        Are_Equivalent_Ranges(spn, Li);

        {
            auto itr = spn.begin();

            *itr++ = -1;
            *itr = -3;
        }

        Are_Equivalent_Ranges
        (   Li
        ,   Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} 
        );
    }
	{
        auto cspn = sgm::Span( sgm::immut(Li) );

        Are_Equivalent_Ranges(cspn, Li);

        static_assert
        (   (   sgm::is_immutable<decltype(*cspn.begin())>::value
            &&  sgm::is_immutable<decltype(*cspn.end())>::value
            )
        ,   ""
        );
    }
	{
        auto cspn = sgm::Span(Li.cbegin(), Li.cend());

        Are_Equivalent_Ranges(cspn, Li);

        static_assert
        (   (   sgm::is_immutable<decltype(*cspn.begin())>::value
            &&  sgm::is_immutable<decltype(*cspn.end())>::value
            )
        ,   ""
        );
    }
	{
        auto cspn = sgm::Span(Li.cbegin(), 3);

        Are_Equivalent_Ranges(cspn, Li);

        static_assert
        (   (   sgm::is_immutable<decltype(*cspn.begin())>::value
            &&  sgm::is_immutable<decltype(*cspn.end())>::value
            )
        ,   ""
        );
    }
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Span, /**/)
{   ::intro
,   ::Span_is_iterable
,   ::Primitive_Array
,   ::Static_Size_Array
,   ::Dynamic_Size_Array
,   ::Pointer_and_Size
,   ::Linked_List
};
