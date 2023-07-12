/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


namespace wt
{

    bool Tests(wchar_t const*const module_title) noexcept;

}


#define __MAKE_WSTRING_LITERAL_TITLE_HELPER__(PRE, TITLE) PRE##TITLE
#define __MAKE_WSTRING_LITERAL_TITLE__(TITLE) __MAKE_WSTRING_LITERAL_TITLE_HELPER__(L, TITLE)


#define SGM_WORLD_TEST(TITLE) \
    int wmain(int const, wchar_t const* const[])    \
    {   \
        return wt::Tests( __MAKE_WSTRING_LITERAL_TITLE__(#TITLE) ),  (int)0;\
    }
