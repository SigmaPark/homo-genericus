#pragma once


#ifndef _SGM_VERSION_
#define _SGM_VERSION_


#define _SGM_VNUM(MAJOR, MINOR)	1000*MAJOR + 100*MINOR


#if	( !defined(_MSVC_LANG) || _MSVC_LANG >= 201102L ) &&	\
	( !defined(_MSC_VER) || _MSC_VER >= 1900 ) &&		\
	( !defined(__clang__) || _SGM_VNUM(__clang_major__, __clang_minor__) >= 3300 ) &&	\
	( !defined(__GNUC__) || _SGM_VNUM(__GNUC__, __GNUC_MINOR__) >= 4700 )
	
	#define SGM_SYNTEX_VERSION_CPP11
#endif
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#if	( !defined(_MSVC_LANG) || _MSVC_LANG >= 201402L ) &&	\
	( !defined(_MSC_VER) || _MSC_VER >= 1910 ) &&		\
	( !defined(__clang__) || _SGM_VNUM(__clang_major__, __clang_minor__) >= 3400) &&	\
	( !defined(__GNUC__) || _SGM_VNUM(__GNUC__, __GNUC_MINOR__) >= 5000 )
	
	#define SGM_SYNTEX_VERSION_CPP14
#endif
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#if	( !defined(_MSC_VER) || _MSC_VER >= 1914 ) &&		\
	( !defined(__clang__) || _SGM_VNUM(__clang_major__, __clang_minor__) >= 6000) &&	\
	( !defined(__GNUC__) || _SGM_VNUM(__GNUC__, __GNUC_MINOR__) >= 7000 )
	
	#define SGM_SYNTEX_VERSION_CPP17
#endif
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#undef _SGM_VNUM

#endif // end of #ifndef _SGM_VERSION_
