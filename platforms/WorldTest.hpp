/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


namespace wt
{

    bool Tests(wchar_t const*const module_title) noexcept;

}


#define SGM_WORLD_TEST(TITLE) \
    int wmain(int const, wchar_t const* const[]){  return wt::Tests(L#TITLE),  (int)0;  }