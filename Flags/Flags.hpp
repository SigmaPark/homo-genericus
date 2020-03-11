#pragma once

#ifndef _SGM_FLAGS_
#define _SGM_FLAGS_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

#include "..\Type_Analysis\Type_Analysis.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{
	
	class Flag_t : public No_Making{};


	template<class T>
	class is_Flag : public No_Making
	{
	public: enum : bool{value = std::is_base_of<Flag_t, T>::value};
	};

	
	template<class...FLAGS> 
	class _FlagSet
	{
		template<class...> class _TC;

		static_assert
		(	Check_All<is_Flag>::for_all< _TC<FLAGS...> >::value
		,	"expected type is sgm::Flag_t or its derived type."
		);

	public:
		
	};

	template<class...FLAGS> using Flags = _FlagSet<FLAGS...>;










}


#endif