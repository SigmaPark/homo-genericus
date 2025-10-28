/*  SPDX-FileCopyrightText: (c) 2025 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_CV_COMPLETE_
#define _SGM_CV_COMPLETE_


#include "SGM/TMP/interface_Traits.hpp"


namespace sgm
{

	template<class T>
	struct CV_Complete;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
struct sgm::CV_Complete
{
	static_assert(!is_Reference<T>::value, "T should not be reference type.");
	static_assert(Has_Operator_Deref<T>::value, "T should be dereferenceable.");

	using value_type = Referenceless_t< decltype(*Mock<T>()) >;

private:
	static bool constexpr _is_Nxct_dereferenceable = noexcept(*Mock<T>());

public:
	T base;

	auto operator*() noexcept(_is_Nxct_dereferenceable)
	-> 	value_type&{  return *base;  }

	auto operator*() const noexcept(_is_Nxct_dereferenceable)
	-> 	value_type const&{  return *base;  }

	auto operator*() volatile noexcept(_is_Nxct_dereferenceable)
	-> 	value_type volatile&{  return *base;  }

	auto operator*() const volatile noexcept(_is_Nxct_dereferenceable)
	-> 	value_type const volatile&{  return *base;  }

	auto operator->() noexcept(_is_Nxct_dereferenceable)
	-> 	value_type*{  return &**this;  }

	auto operator->() const noexcept(_is_Nxct_dereferenceable)
	-> 	value_type const*{  return &**this;  }

	auto operator->() volatile noexcept(_is_Nxct_dereferenceable)
	-> 	value_type volatile*{  return &**this;  }

	auto operator->() const volatile noexcept(_is_Nxct_dereferenceable)
	-> 	value_type const volatile*{  return &**this;  }
};


#endif // end of #ifndef _SGM_CV_COMPLETE_