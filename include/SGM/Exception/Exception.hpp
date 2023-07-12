/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_EXCEPTION_
#define _SGM_EXCEPTION_


namespace sgm
{

    class Exception;

}
//========//========//========//========//=======#//========//========//========//========//=======#


class sgm::Exception
{
public:
    constexpr Exception(wchar_t const* const message = L"") noexcept : _message(message){}

    virtual auto what() const noexcept-> wchar_t const*{  return _message;  }

private:
    wchar_t const* _message;
};


#endif // end of #ifndef _SGM_EXCEPTION_