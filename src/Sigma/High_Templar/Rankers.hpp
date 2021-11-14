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
		<	class RG, class _deref_t = decltype( *Begin(Declval<RG>()) )
		,	bool = is_LvalueReference<_deref_t>::value
		>
		struct _Rankers_Helper;


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


template<class RG, class _deref_t>
struct sgm::ht::_Rankers_Helper<RG, _deref_t, true> : No_Making
{
private:
	using _elem_t = Decay_t<_deref_t>;

	
	template<class comp_t>
	static auto _reversed_cptrs(RG rg, size_t const k, bool const is_k_smaller, comp_t &&comp)
	->	Serial<_elem_t const*>
	{
		auto pcomp_f
		=	[&comp](_elem_t const *p1, _elem_t const *p2)-> bool{  return comp(*p1, *p2);  };

		std::priority_queue< _elem_t const*, std::deque<_elem_t const*>, decltype(pcomp_f) >
			prq(pcomp_f);

		auto itr = Begin(rg);

		for(size_t d = k;  d-->0;  itr++)
			prq.emplace(&*itr);

		if(is_k_smaller)
			for(;  itr != End(rg);  itr++)
				if( pcomp_f(&*itr, prq.top()) )
					prq.pop(),  prq.emplace(&*itr);

		Serial<_elem_t const*> ptr_buff(k);

		while(!prq.empty())
			ptr_buff >> prq.top(),  prq.pop();

		return ptr_buff;
	}


public:
	template<class comp_t>
	static auto calc(RG rg, size_t const nof_ranker, comp_t &&comp)-> Serial<_elem_t>
	{
		size_t const nof_elem = Size(rg);

		if(nof_elem == 0)
			return {};
		else
		{
			bool const is_smaller_than_range_size = nof_ranker < nof_elem;
			size_t const k = is_smaller_than_range_size ? nof_ranker : nof_elem;

			auto const ptr_buff
			=	_reversed_cptrs( rg, k, is_smaller_than_range_size, Forward<comp_t>(comp) );

			Serial<_elem_t> res(k);

			for(auto ritr = ptr_buff.rbegin();  ritr != ptr_buff.rend();)
				res >> **ritr++;

			return res;
		}
	}
};


template<class RG, class _deref_t>
struct sgm::ht::_Rankers_Helper<RG, _deref_t, false> : No_Making
{
private:
	using _elem_t = Decay_t<_deref_t>;

public:
	template<class comp_t>
	static auto calc(RG rg, size_t const nof_rankers, comp_t &&comp)-> Serial<_elem_t>
	{
		return
		_Rankers_Helper< Serial<_elem_t> >::calc
		(	Serial<_elem_t>( Begin(rg), End(rg) ), nof_rankers, Forward<comp_t>(comp)
		);
	}
};


template<class RG, class FUNC, class>
static auto sgm::ht::Rankers(RG &&rg, size_t const nof_ranker, FUNC &&comp)
->	Serial< Decay_t<decltype( *Begin(rg) )> >
{
	return _Rankers_Helper<RG&&>::calc( Forward<RG>(rg), nof_ranker, Forward<FUNC>(comp) );
}


#endif