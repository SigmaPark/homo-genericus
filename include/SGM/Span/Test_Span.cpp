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


static void Static_Size_Array()
{
    Array<Specimen, 3> arr{Specimen(2), Specimen(4), Specimen(6)};

    {
        auto spn = sgm::Span<3>(arr.data());

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges( arr, Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} );
    }
    {
        auto spn = sgm::Span<3>(arr); 

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges( arr, Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} );
    }
	{
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
	{
        auto cspn = sgm::Span<3>( immut(arr) );

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


static void Dynamic_Size_Array()
{
    Array<Specimen> arr{Specimen(2), Specimen(4), Specimen(6)};

    {
        auto spn = sgm::Span(arr);

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges( arr, Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} );
    }
    {
        auto spn = sgm::Span(arr.begin(), arr.end());

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges( arr, Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} );
    }
    {
        auto spn = sgm::Span(arr.begin(), 3); 

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges( arr, Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} );
    }
	{
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
	{
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
	{
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

        Are_Equivalent_Ranges( Li, Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} );
    }
    {
        auto spn = sgm::Span(Li.begin(), Li.end());

        Are_Equivalent_Ranges(spn, Li);

        {
            auto itr = spn.begin();

            *itr++ = -1;
            *itr = -3;
        }

        Are_Equivalent_Ranges( Li, Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} );
    }
    {
        auto spn = sgm::Span(Li.begin(), 3);

        Are_Equivalent_Ranges(spn, Li);

        {
            auto itr = spn.begin();

            *itr++ = -1;
            *itr = -3;
        }

        Are_Equivalent_Ranges( Li, Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} );
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
{   ::Static_Size_Array
,   ::Dynamic_Size_Array
,   ::Linked_List
};
