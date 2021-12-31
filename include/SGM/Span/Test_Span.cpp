/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Span.hpp"
#include "Test_Span.hpp"
#include "../Array/Array.hpp"


using sgm::spec::is_True;
using sgm::spec::Are_Equivalent_Ranges;
using sgm::spec::Specimen;
using sgm::Array;


static void Test01()
{
    Array<Specimen, 3> arr{Specimen(2), Specimen(4), Specimen(6)};

    {
        auto spn = sgm::Span<Specimen, 3>(arr.data());

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges( arr, Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} );
    }
    {
        auto spn = sgm::Span<Specimen, 3>::by(arr);

        Are_Equivalent_Ranges(spn, arr);

        spn[0] = -1;
        spn[1] = -3;

        Are_Equivalent_Ranges( arr, Array<Specimen, 3>{Specimen(-1), Specimen(-3), Specimen(6)} );
    }
	{
        auto cspn = sgm::Span<Specimen const, 3>(arr.data());

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
        auto cspn = sgm::Span<Specimen, 3>::by( immut(arr) );

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
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Span, /**/)
{   ::Test01
};
