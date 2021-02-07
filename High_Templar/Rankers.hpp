#pragma once

#ifndef _SGM_RANKERS_
#define _SGM_RANKERS_

#include "..\Serial\Serial.hpp"


namespace sgm
{
	namespace ht
	{

		template<class CON>
		struct _increasing_order;


		template
		<	class CON, class FUNC = _increasing_order<CON>
		,	class = Enable_if_t< is_iterable<CON>::value >  
		>
		static auto Rankers
		(	CON&& con, size_t const nof_ranker, FUNC&& comp = _increasing_order<CON>()
		)->	Serial< Decay_t<decltype(*con.begin())> >;

	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class CON>
struct sgm::ht::_increasing_order
{
private:
	using type = Decay_t<  decltype( *Declval<CON>().begin() )  >;

public:
	bool operator()(type const& t1, type const& t2) const{  return t1 < t2;  }
};


#include <list>
#include <algorithm>


template<class CON, class FUNC, class>
static auto sgm::ht::Rankers(CON&& con, size_t const nof_ranker, FUNC&& comp)
->	Serial< Decay_t<decltype(*con.begin())> >
{
	using elem_t = Decay_t<decltype( *con.begin() )>;

	size_t const k = std::min(nof_ranker, con.size());
	std::list<elem_t> kset;
	auto itr = con.begin();

	for(size_t d = k;  d-->0;  ++itr)
		kset.emplace_back(*itr);

	kset.sort(comp);

	if(k == nof_ranker)
		for(;  itr != con.end();  ++itr)
			if( comp(*itr, kset.back()) )
				kset.emplace( std::lower_bound(kset.begin(), kset.end(), *itr, comp), *itr ),
				kset.pop_back();

	return Serial<elem_t>(kset);
}



#endif