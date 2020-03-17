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


	template<class> class is_Flag;


	template<class...FLAGS> 
	class Flag_t
	{
	public:


		template<class _FLAG>
		auto operator&(_FLAG) const-> Flag_t<FLAGS..., _FLAG>
		{
			static_assert(is_Flag<_FLAG>::value, "expected type is sgm::Flag_t");

			return Flag_t<FLAGS..., _FLAG>();
		}


		template<class F>
		class has : No_Making
		{
		public: enum : bool{value = Has_Type<F>:: template among<FLAGS...>::value};
		};


		template<class> class is_superset_of;
		template<class> class is_subset_of;
		template<class> class has_same_flags_to : std::false_type{};

		template<class..._FLAGS>
		class has_same_flags_to< Flag_t<_FLAGS...> >
		{
		public:
			enum : bool
			{	value 
				=	(	is_superset_of< Flag_t<_FLAGS...> >::value
					&&	is_subset_of< Flag_t<_FLAGS...> >::value
					)
			};
		};


	private:
		template<> class is_superset_of<Flag_t<>> : public std::true_type{};
		
		template<class _F, class..._FLAGS>
		class is_superset_of< Flag_t<_F, _FLAGS...> >
		{
		public: 
			enum : bool
			{	value 
				=	Has_Type<_F>::template among<FLAGS...>::value
					?	is_superset_of< Flag_t<_FLAGS...> >::value
					:	false 
			};
		};


		template<class..._FLAGS>
		class is_subset_of< Flag_t<_FLAGS...> >
		{
		public:
			enum : bool
			{	value = Flag_t<_FLAGS...>::template is_superset_of< Flag_t<FLAGS...> >::value
			};
		};
	};


	template<class FLAG>
	class is_Flag : No_Making
	{
	public: enum : bool{value = std::is_convertible< FLAG, Flag_t<FLAG> >::value};
	};





}


#endif