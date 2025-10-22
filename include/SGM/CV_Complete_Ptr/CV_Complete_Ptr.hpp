/*  SPDX-FileCopyrightText: (c) 2025 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_CV_COMPLETE_PTR_
#define _SGM_CV_COMPLETE_PTR_


#include "SGM/Type_Analysis/Type_Analysis.hpp"


namespace sgm
{

	template<class T>
	class CV_Complete_Ptr;

}


template<class T>
class sgm::CV_Complete_Ptr
{
public:
	static_assert(!is_Reference<T>::value, "T should not be reference type.");

	constexpr CV_Complete_Ptr() noexcept : _p{nullptr}{}

	auto get() noexcept-> T*{  return _p;  }
	auto get() const noexcept-> T const*{  return _p;  }
	auto get() volatile noexcept-> T volatile*{  return _p;  }
	auto get() const volatile noexcept-> T const volatile*{  return _p;  }

	auto operator==(T const* p) const volatile noexcept-> bool{  return get() == p;  }
	auto operator==(T const volatile* p) const volatile noexcept-> bool{  return get() == p;  }

	auto operator==(CV_Complete_Ptr const rhs) const volatile noexcept
	->	bool{  return *this == rhs.get();  }

	auto operator==(CV_Complete_Ptr const volatile rhs) const volatile noexcept
	->	bool{  return *this == rhs.get();  }

	template<class Q>
	auto operator!=(Q& q) const volatile noexcept-> bool{  return !(*this == q);  }

	operator bool() const volatile noexcept{  return get() != nullptr;  }

	auto operator*() noexcept-> T&{  return *get();  }
	auto operator*() const noexcept-> T const&{  return *get();  }
	auto operator*() volatile noexcept-> T volatile&{  return *get();  }
	auto operator*() const volatile noexcept-> T const volatile&{  return *get();  }

	auto operator->() noexcept-> T*{  return get();  }
	auto operator->() const noexcept-> T const*{  return get();  }
	auto operator->() volatile noexcept-> T volatile*{  return get();  }
	auto operator->() const volatile noexcept-> T const volatile*{  return get();  }

	auto operator++() noexcept-> CV_Complete_Ptr&{  return ++_p,  *this;  }
	auto operator++() volatile noexcept-> CV_Complete_Ptr volatile&{  return ++_p,  *this;  }
	auto operator--() noexcept-> CV_Complete_Ptr&{  return --_p,  *this;  }
	auto operator--() volatile noexcept-> CV_Complete_Ptr volatile&{  return --_p,  *this;  }

	auto operator++(int) volatile noexcept-> CV_Complete_Ptr
	{
		auto res = *this;

		++*this;

		return res;
	}

	auto operator--(int) volatile noexcept-> CV_Complete_Ptr
	{
		auto res = *this;

		--*this;

		return res;
	}



private:
	T* _p;
};


#endif
