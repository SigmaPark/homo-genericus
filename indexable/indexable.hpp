#pragma once

#ifdef USE_STL_VECTOR_AND_ARRAY
	//	using standard vector and array
	#include "indexable_impl_with_STL.hpp"
#else
	//	using implementation with C style array
	#include "indexable_impl.hpp"
#endif