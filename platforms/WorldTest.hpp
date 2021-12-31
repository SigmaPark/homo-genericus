/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


namespace wt
{

    bool Tests(char const*const module_title) noexcept;

}


#define SGM_WORLD_TEST(TITLE)  int main(){  return wt::Tests(#TITLE),  (int)0;  }