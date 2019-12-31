#pragma once

#ifndef _SGM_CALLBACK_
#define _SGM_CALLBACK_


#include <functional>
#include <any>
#include <optional>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{
	
	template<unsigned CNL = 1>
	class Callback
	{
	public:
		enum ID_t { ALL = 0 };


		
		Callback() : _cb(), _id(){}

		template<typename R = void, typename...ARGS>
		Callback(ARGS...args) : _cb(std::function<R(ARGS...)>), _id() {}


		template<typename...ARGS>
		void call(ARGS...args) const
		{
			std::any_cast< std::function<void(ARGS...)> >(_cb)(args...);
		}


		template<typename...ARGS>
		bool callable_with() const
		{
			return std::any_cast< std::function<void(ARGS...)> >(&_cb) != nullptr;
		}


		template<typename CB>
		void ready(CB&& cb){  _cb = std::forward<CB>(cb);  }


		void reset(){  _cb.reset();  }


	private:
		std::any mutable _cb;
		std::optional<int> mutable _id;
	};

}

#endif


#if 0

	Callback cb;

	cb = f1(A, B)-> C;

	cb(Root_t<int> a,)



#endif