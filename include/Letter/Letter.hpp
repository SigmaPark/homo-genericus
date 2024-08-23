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

	static std::size_t constexpr string_buffer_size_v = 0x1000;

#pragma warning(push)
#pragma warning(disable : 4996)
	static auto Mbs_to_Wcs(std::string const& mbs)-> std::wstring
	{
		static wchar_t buf[string_buffer_size_v];

		mbstowcs(buf, mbs.c_str(), string_buffer_size_v);
		
		return buf;
	}
	
	
	static auto Wcs_to_Mbs(std::wstring const& wcs)-> std::string
	{
		static char buf[string_buffer_size_v];

		wcstombs(buf, wcs.c_str(), string_buffer_size_v);

		return buf;
	}
#pragma warning(pop)
};


#endif // end of #ifndef _SGM_LETTER_