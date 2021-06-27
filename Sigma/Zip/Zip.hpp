#pragma once

#ifndef _SGM_ZIP_
#define _SGM_ZIP_


#include "..\Family\Family.hpp"
#include "..\Functional_iterator\Functional_iterator.hpp"


namespace sgm
{
	
	template<class...CS>  class Zipper;
	//template<class FAM>  class _Zip_Functor;
	//template<class FAM>

}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


//template<class FAM>
//class sgm::_Zip_Functor
//{
//public:
//	_Zip_Functor(FAM *pfam) : _pfam(pfam){}
//
//	decltype(auto) operator()(size_t const idx);
//
//private:
//	FAM *_pfam;
//
//
//	template<class...ARGS>
//	static auto _get([[maybe_unused]] size_t const idx, [[maybe_unused]] FAM &fam, ARGS&...args)
//	{
//		size_t constexpr IDX = sizeof...(ARGS);
//
//		if constexpr(IDX == std::tuple_size< std::decay_t<FAM> >::value)
//			return Tie(args...);
//		else
//			return _get(  idx, fam, args..., *Next( std::get<IDX>(fam).begin(), idx )  );
//	
//	}
//};


template<class...CS>  
class sgm::Zipper
{
	static_assert( (is_iterable<CS>::value &&...) );

public:
	template<class..._CS>
	Zipper(_CS&&...args) : _fam( std::forward<_CS>(args)... ){}



private:
	Family<CS...> _fam;



	//template<size_t IDX = 0>
	//static auto _read_size(Family<CS...> const &fam)-> size_t
	//{
	//	if constexpr( IDX == sizeof...(CS) )
	//		return std::get<0>(fam).size();
	//	else if constexpr( is_random_access_iterator< decltype(Declval< Nth_t<IDX, CS...> >().begin()) >::value )
	//		return std::get<IDX>(fam).size();
	//	else
	//		return _read_size<IDX+1>(fam);
	//}
};


namespace sgm
{
	template<class..._CS>  Zipper(_CS&&...)-> Zipper< remove_aleph_t<_CS&&>... >;
}



#endif // end of #ifndef _SGM_ZIP_