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
		,	class = Guaranteed_t< is_iterable<CON>::value >  
		>
		static auto Rankers(CON&& con, size_t const nof_ranker, FUNC&& comp = {})
		->	Serial< Decay_t<decltype(*con.begin())> >;

	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class CON>
struct sgm::ht::_increasing_order
{
private:
	using type = Decay_t<  decltype( *Declval<CON>().begin() )  >;

public:
	bool operator()(type const &t1, type const &t2) const{  return t1 < t2;  }
};


#include <queue>


template<class CON, class FUNC, class>
static auto sgm::ht::Rankers(CON &&con, size_t const nof_ranker, FUNC &&comp)
->	Serial< Decay_t<decltype(*con.begin())> >
{
	using elem_t = Decay_t<decltype( *con.begin() )>;

	size_t const nof_elem = con.size();

	if(nof_elem == 0)
		return {};
	else 
	{
		auto pcomp_f
		=	[&comp](elem_t const *p1, elem_t const *p2)-> bool{  return comp(*p1, *p2);  };

		std::priority_queue< elem_t const*, std::vector<elem_t const*>, decltype(pcomp_f) > 
			prq(pcomp_f);

		size_t const k = std::min(nof_ranker, nof_elem);
		auto itr = con.begin();

		for(size_t d = k;  d-->0;  itr++)
			prq.emplace(&*itr);

		if(k == nof_ranker)
			for(;  itr != con.end();  itr++)
				if( pcomp_f(&*itr, prq.top()) )
					prq.pop(),  prq.emplace(&*itr);

		Serial<elem_t const*> ptr_buff(k);

		while(!prq.empty())
			ptr_buff >> prq.top(),  prq.pop();

		Serial<elem_t> res(k);

		for(auto ritr = ptr_buff.rbegin();  ritr != ptr_buff.rend();)
			res >> **ritr++;

		return res;
	}
}


#endif