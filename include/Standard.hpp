/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_STANDARD_
#define _SGM_STANDARD_


#ifndef SGM_CXX_STANDARD
	#ifdef __cplusplus
		#if __cplusplus >= 201700L
		#define SGM_CXX_STANDARD 2017
		#elif __cplusplus >= 201400L
		#define SGM_CXX_STANDARD 2014
		#elif __cplusplus >= 201100L
		#define SGM_CXX_STANDARD 2011
		#else
		#define SGM_CXX_STANDARD 0
		#endif

	#endif
#endif


#endif // end of #ifndef _SGM_STANDARD_