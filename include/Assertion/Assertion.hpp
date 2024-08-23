/*  SPDX-FileCopyrightText: (c) 2023 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_ASSERTION_
#define _SGM_ASSERTION_


#if defined(_DEBUG) || defined(SGM_ACTIVATE_ASSERTION)

#include <iostream>
#include <string>
#include <cstdlib>
#include "../Type_Analysis/Type_Analysis.hpp"


#define SGM_ASSERT(...)		\
	[](bool const statement)-> void	\
	{	\
		if(statement)	\
			return;	\
		\
		sgm::_assertion_detail::Log_message	\
		(	_SGM_DOUBLE_UNDERBAR_MACRO_HELPER(FILE)	\
		,	std::to_string( _SGM_DOUBLE_UNDERBAR_MACRO_HELPER(LINE) )	\
		,	#__VA_ARGS__	\
		);	\
		\
		sgm::_assertion_detail::User_guide();		\
	}( static_cast<bool>(__VA_ARGS__) )


namespace sgm
{
	namespace _assertion_detail
	{
		
		inline static auto Log_message
		(	std::string const file_path, std::string const err_line
		,	std::string const statement
		)-> void
		{
			std::cerr
			<<	std::endl
			<<	"[Assertion Fail]" << std::endl
			<<	"  File : " << file_path << std::endl
			<<	"  Line : " << err_line << std::endl
			<<	"  " << statement << std::endl 
			<<	std::endl;
		}


		inline static auto User_guide()-> void
		{
			static std::string const guide_msg 
			=	"Enter \'c\' to continue, or enter any other character to exit.\n";

			char user_input = ' ';	

			std::cerr << guide_msg;
			std::cin >> user_input;
			
			if(user_input == 'c' || user_input == 'C')
				std::cin.get(),  
				std::cerr << "program continues.\n";
			else	
				std::cerr << "program exits.\n",  
				std::abort();
		}

	}
}
#else
#define SGM_ASSERT(...)		(void)0
#endif


#endif // end of #ifndef _SGM_ASSERTION_