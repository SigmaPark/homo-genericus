/*  SPDX-FileCopyrightText: (c) 2025 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/CV_Complete/CV_Complete.hpp"
#include "Test_CV_Complete.hpp"
#include <memory>
#include <vector>


static void intro()
{
	sgm::h2u::mdo
	<<	sgm::h2u::Title(L"CV_Complete")
	<<	L"CV_Complete is a template wrapper that preserves const-volatile (CV) qualifiers "
	<<	L"when dereferencing any dereferenceable type. It ensures that the CV-qualifiers "
	<<	L"of the wrapper itself are properly propagated to the dereferenced result."
	<<	sgm::h2u::newl
	<<	sgm::h2u::newl
	<<	L"Key features:" << sgm::h2u::newl
	<<	L"- Works with any dereferenceable type (pointers, smart pointers, iterators)"
	<<	sgm::h2u::newl
	<<	L"- Preserves const, volatile, and const volatile qualifications" << sgm::h2u::newl
	<<	L"- Provides both operator* and operator-> with proper CV-qualification"
	<<	sgm::h2u::newl
	<<	L"- Template-based design for maximum flexibility" << sgm::h2u::newl
	<<	sgm::h2u::newl
	<<	L"This is particularly useful when you need to ensure CV-correctness "
	<<	L"across different pointer-like types in generic code."
	<<	sgm::h2u::empty_line;
}


static void raw_pointer_tests()
{
	sgm::h2u::mdo << sgm::h2u::Title(L"Raw Pointer Tests", 2);

BEGIN_CODE_BLOCK(raw_pointer_tests)
	int value = 42;
	int* ptr = &value;

	// Test regular (non-CV) pointer
	{
		sgm::CV_Complete<int*> wrapper{ptr};

		static_assert(sgm::is_Same<decltype(*wrapper), int&>::value);
		SGM_H2U_ASSERT(*wrapper == 42);
	}

	// Test const wrapper
	{
		sgm::CV_Complete<int*> const const_wrapper{ptr};
	
		static_assert(sgm::is_Same<decltype(*const_wrapper), int const&>::value);
		SGM_H2U_ASSERT(*const_wrapper == 42);
	}

	// Test volatile wrapper
	{
		sgm::CV_Complete<int*> volatile volatile_wrapper{ptr};

		static_assert
		(	sgm::is_Same<decltype(*volatile_wrapper), int volatile&>::value
		);
		SGM_H2U_ASSERT(*volatile_wrapper == 42);
	}

	// Test const volatile wrapper
	{
		sgm::CV_Complete<int*> const volatile cv_wrapper{ptr};

		static_assert
		(	sgm::is_Same<decltype(*cv_wrapper), int const volatile&>::value
		);
		SGM_H2U_ASSERT(*cv_wrapper == 42);
	}
END_CODE_BLOCK_AND_LOAD(raw_pointer_tests)
}


static void unique_ptr_tests()
{
	sgm::h2u::mdo << sgm::h2u::Title(L"unique_ptr Tests", 2);

BEGIN_CODE_BLOCK(unique_ptr_tests)
	auto unique_ptr = std::make_unique<int>(100);

	// Test regular unique_ptr wrapper
	{
		sgm::CV_Complete< std::unique_ptr<int> > wrapper{std::move(unique_ptr)};

		static_assert(sgm::is_Same<decltype(*wrapper), int&>::value);
		SGM_H2U_ASSERT(*wrapper == 100);
	}

	// Create new unique_ptr for const test
	auto unique_ptr2 = std::make_unique<int>(200);

	// Test const unique_ptr wrapper
	{
		sgm::CV_Complete< std::unique_ptr<int> > const const_wrapper
		{	std::move(unique_ptr2)
		};
		
		static_assert(sgm::is_Same<decltype(*const_wrapper), int const&>::value);
		SGM_H2U_ASSERT(*const_wrapper == 200);
	}
END_CODE_BLOCK_AND_LOAD(unique_ptr_tests)

	sgm::h2u::mdo
	<<	L"**Note:** volatile and const volatile tests are not valid for unique_ptr "
	<<	L"because std::unique_ptr does not support volatile-qualified access. "
	<<	L"This is a design limitation - volatile wrappers only work with raw pointers."
	<<	sgm::h2u::empty_line;
}


static void vector_iterator_tests()
{
	sgm::h2u::mdo << sgm::h2u::Title(L"Vector Iterator Tests", 2);

BEGIN_CODE_BLOCK(vector_iterator_tests)
	std::vector<int> vec = {1, 2, 3, 4, 5};
	auto iter = vec.begin();

	// Test regular iterator wrapper
	{
		sgm::CV_Complete<typename std::vector<int>::iterator > wrapper{iter};

		static_assert(sgm::is_Same<decltype(*wrapper), int&>::value);
		SGM_H2U_ASSERT(*wrapper == 1);
	}

	// Test const iterator wrapper
	{
		sgm::CV_Complete<typename std::vector<int>::iterator > const const_wrapper
		{	iter
		};

		static_assert(sgm::is_Same<decltype(*const_wrapper), int const&>::value);
		SGM_H2U_ASSERT(*const_wrapper == 1);
	}

END_CODE_BLOCK_AND_LOAD(vector_iterator_tests)

	sgm::h2u::mdo
	<<	L"**Note:** volatile and const volatile tests are not valid for iterators "
	<<	L"because std::vector<int>::iterator does not support volatile-qualified access. "
	<<	L"This is a design limitation - volatile wrappers only work with raw pointers."
	<<	sgm::h2u::empty_line;
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_HOW2USE_TESTS(sgm::h2u::Test_, CV_Complete, /**/)
{	::intro
,	::raw_pointer_tests
,	::unique_ptr_tests
,	::vector_iterator_tests
};