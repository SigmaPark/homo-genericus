#pragma once 
#ifndef _SGM_END_OF_LIFE_
#define _SGM_END_OF_LIFE_


#include "../Family/Family.hpp"


namespace sgm
{

	template<class FN, class...ARGS>
	class End_of_Life;

	template<class FN, class...ARGS>
	using EOL = End_of_Life<FN, ARGS...>;


	template<class FN, class...ARGS>
	static auto Finally(FN&& fn, ARGS&&...args)
	->	EOL< Alephless_t<FN&&>, Alephless_t<ARGS&&>... >;

}


template<class FN, class...ARGS>
class sgm::End_of_Life
{
public:
	template<class FN_, class...ARGS_>
	End_of_Life(FN_&& fn, ARGS_&&...args) 
	:	_fn( Forward<FN_>(fn) ), _fam( Forward<ARGS_>(args)... ){}

	~End_of_Life(){  _calc(*this);  }


private:
	FN _fn;
	Family<ARGS...> _fam;

	static size_t constexpr _nof_param_v = sizeof...(ARGS);


	template<class ME, class...ARGS_, size_t _IDX = sizeof...(ARGS_)>
	static auto _calc(ME& me, ARGS_&...args) noexcept
	->	Enable_if_t< _IDX == Decay_t<ME>::_nof_param_v >{  me._fn(args...);  }

	template<class ME, class...ARGS_, size_t _IDX = sizeof...(ARGS_)>
	static auto _calc(ME& me, ARGS_&...args) noexcept
	->	Enable_if_t< _IDX != Decay_t<ME>::_nof_param_v >
	{
		_calc(me, args..., me._fam.template get<_IDX>()); 
	}
};


template<class FN, class...ARGS>
auto sgm::Finally(FN&& fn, ARGS&&...args)-> EOL< Alephless_t<FN&&>, Alephless_t<ARGS&&>... >
{
	return {Forward<FN>(fn), Forward<ARGS>(args)...};
}


#endif // end of #ifndef _SGM_END_OF_LIFE_