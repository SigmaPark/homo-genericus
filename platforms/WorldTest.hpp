/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#include "../include/SGM/Letter/Letter.hpp"


namespace wt
{

    bool Tests(wchar_t const*const module_title) noexcept;

}


#define SGM_WORLD_TEST(TITLE) \
    int main(int const, char const* const[])    \
    {   \
        return wt::Tests( sgm::Letter_Conversion::Mbs_to_Wcs(#TITLE).c_str() ),  (int)0;\
    }
