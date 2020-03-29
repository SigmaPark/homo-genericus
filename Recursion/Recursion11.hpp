#pragma once

#ifndef _SGM_RECURSION11_
#define _SGM_RECURSION11_

#include "..\Type_Analysis\Type_Analysis.hpp"
#include "..\Compound\Compound.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	template<class...TYPES>
	using Last_t = typename Compound_element< sizeof...(TYPES) - 1, Compound<TYPES...> >::type;


	template<class T>
	struct Fix
	{
		Fix(T t) : t(t){}
		auto get()-> T{  return value;  }

	private:
		T value;
	};

	template<class...> class _Recursion;


	template<class...ARGS>
	static auto Feed_Back(ARGS&&...args)-> _Recursion<ARGS...>
	{	
		
	}

	
	//class Recursive_Result
	//{
	//	Recursive_Result(void* pAny) : _pAny(pAny), _done(false){}

	//	template<  class T, std::enable_if_t< !std::is_same<T, void*>::value >  >
	//	Recursive_Result(T&& t) : _pAny( std::move(t) ), _done(true){}

	//private:
	//	void* _pAny;
	//	bool _done;

	//	template<class...> friend class _Recursion;
	//};
	



	template<class...ARGS>
	class _Recursion 
	{
	public:
		static_assert( sizeof...(ARGS) >= 2,	"lack of arguments." );

		_Recursion(ARGS...args) : _cmp(args...), _done(false){}

		_Recursion(Last_t<ARGS...> last) : _cmp(), _done(true)
		{
			Get<sizeof...(ARGS) - 1>(_cmp) = last;  
		}
		
		template<class FUNC>
		auto operator>>(FUNC&& func)-> Last_t<ARGS...>
		{
			while(!_done)
				
		}
	
	private:
		Compound<ARGS...> _cmp;
		bool _done;
	};

}

#endif