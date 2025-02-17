/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_LETTER_
#define _SGM_LETTER_


#include <cstdio>
#include <cstddef>
#include <string>


namespace sgm
{

	struct Letter_Conversion;

}


struct sgm::Letter_Conversion
{
	Letter_Conversion() = delete;

#pragma warning(push)
#pragma warning(disable : 4996)
	static auto Mbs_to_Wcs(std::string const& mbs)-> std::wstring
	{
		std::wstring res(mbs.size(), L'\0');

		mbstowcs(res.data(), mbs.c_str(), res.size());
		
		return res;
	}
	
	
	static auto Wcs_to_Mbs(std::wstring const& wcs)-> std::string
	{
		std::string res(wcs.size(), '\0');

		wcstombs(res.data(), wcs.c_str(), res.size());

		return res;
	}
#pragma warning(pop)
};


#endif // end of #ifndef _SGM_LETTER_