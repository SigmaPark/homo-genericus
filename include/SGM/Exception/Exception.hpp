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
    constexpr Exception(char const* const message = "") noexcept : _message(message){}

    virtual auto what() const noexcept-> char const*{  return _message;  }

private:
    char const* _message;
};


#endif // end of #ifndef _SGM_EXCEPTION_