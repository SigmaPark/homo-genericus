#pragma once

#ifndef _SGM_RANKERS_
#define _SGM_RANKERS_

#include "..\Serial\Serial.hpp"


namespace sgm
{
	namespace ht
	{

		template<class RG>
		struct _increasing_order;


		template
		<	class RG, class FUNC = _increasing_order<RG>
		,	class = Guaranteed_t< is_iterable<RG>::value >  
		>
		static auto Rankers(RG &&rg, size_t const nof_ranker, FUNC &&comp = {})
		->	Serial< Decay_t<decltype( *Begin(rg) )> >;

	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class RG>
struct sgm::ht::_increasing_order
{
private:
	using type = Decay_t<  decltype( *Begin(Declval<RG>()) )  >;

public:
	bool operator()(type const &t1, type const &t2) const{  return t1 < t2;  }
};


#include <queue>


template<class RG, class FUNC, class>
static auto sgm::ht::Rankers(RG &&rg, size_t const nof_ranker, FUNC &&comp)
->	Serial< Decay_t<decltype( *Begin(rg) )> >
{
	using elem_t = Decay_t<decltype( *Begin(rg) )>;

	size_t const nof_elem = Size(rg);

	if(nof_elem == 0)
		return {};
	else 
	{
		auto pcomp_f
		=	[&comp](elem_t const *p1, elem_t const *p2)-> bool{  return comp(*p1, *p2);  };

		std::priority_queue< elem_t const*, std::vector<elem_t const*>, decltype(pcomp_f) > 
			prq(pcomp_f);

		size_t const k = std::min(nof_ranker, nof_elem);
		auto itr = Begin(rg);

		for(size_t d = k;  d-->0;  itr++)
			prq.emplace(&*itr);

		if(k == nof_ranker)
			for(;  itr != End(rg);  itr++)
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