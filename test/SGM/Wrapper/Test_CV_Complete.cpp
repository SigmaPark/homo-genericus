/*  SPDX-FileCopyrightText: (c) 2025 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Wrapper/CV_Complete.hpp"
#include "Test_CV_Complete.hpp"
#include <memory>
#include <vector>


static void intro()
{
	h2u::mdo
	<<	h2u::Title(L"CV_Complete")
	<<	L"CV_Complete is a template wrapper that preserves const-volatile (CV) qualifiers "
	<<	L"when dereferencing any dereferenceable type. It ensures that the CV-qualifiers "
	<<	L"of the wrapper itself are properly propagated to the dereferenced result."
	<<	h2u::newl
	<<	h2u::newl
	<<	L"Key features:" << h2u::newl
	<<	L"- Works with any dereferenceable type (pointers, smart pointers, iterators)"
	<<	h2u::newl
	<<	L"- Preserves const, volatile, and const volatile qualifications" << h2u::newl
	<<	L"- Provides both operator* and operator-> with proper CV-qualification"
	<<	h2u::newl
	<<	L"- Template-based design for maximum flexibility" << h2u::newl
	<<	h2u::newl
	<<	L"This is particularly useful when you need to ensure CV-correctness "
	<<	L"across different pointer-like types in generic code."
	<<	h2u::empty_line;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void Raw_pointer_tests()
{
	h2u::mdo << h2u::Title(L"Raw Pointer Tests", 2);

BEGIN_CODE_BLOCK(Raw_pointer_tests)
	int value = 42;
	int* ptr = &value;

	// Test regular (non-CV) pointer
	{
		sgm::CV_Complete<int*> wrapper{ptr};

		static_assert(sgm::is_Same<decltype(*wrapper), int&>::value);
		H2U_ASSERT(*wrapper == 42);
	}

	// Test const wrapper
	{
		sgm::CV_Complete<int*> const const_wrapper{ptr};
	
		static_assert(sgm::is_Same<decltype(*const_wrapper), int const&>::value);
		H2U_ASSERT(*const_wrapper == 42);
	}

	// Test volatile wrapper
	{
		sgm::CV_Complete<int*> volatile volatile_wrapper{ptr};

		static_assert
		(	sgm::is_Same<decltype(*volatile_wrapper), int volatile&>::value
		);
		H2U_ASSERT(*volatile_wrapper == 42);
	}

	// Test const volatile wrapper
	{
		sgm::CV_Complete<int*> const volatile cv_wrapper{ptr};

		static_assert
		(	sgm::is_Same<decltype(*cv_wrapper), int const volatile&>::value
		);
		H2U_ASSERT(*cv_wrapper == 42);
	}
END_CODE_BLOCK_AND_LOAD(Raw_pointer_tests)
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void unique_ptr_tests()
{
	h2u::mdo << h2u::Title(L"unique_ptr Tests", 2);

BEGIN_CODE_BLOCK(unique_ptr_tests)
	auto unique_ptr = std::make_unique<int>(100);

	// Test regular unique_ptr wrapper
	{
		sgm::CV_Complete< std::unique_ptr<int> > wrapper{std::move(unique_ptr)};

		static_assert(sgm::is_Same<decltype(*wrapper), int&>::value);
		H2U_ASSERT(*wrapper == 100);
	}

	// Create new unique_ptr for const test
	auto unique_ptr2 = std::make_unique<int>(200);

	// Test const unique_ptr wrapper
	{
		sgm::CV_Complete< std::unique_ptr<int> > const const_wrapper
		{	std::move(unique_ptr2)
		};
		
		static_assert(sgm::is_Same<decltype(*const_wrapper), int const&>::value);
		H2U_ASSERT(*const_wrapper == 200);
	}
END_CODE_BLOCK_AND_LOAD(unique_ptr_tests)

	h2u::mdo
	<<	L"**Note:** volatile and const volatile tests are not valid for unique_ptr "
	<<	L"because std::unique_ptr does not support volatile-qualified access. "
	<<	L"This is a design limitation - volatile wrappers only work with raw pointers."
	<<	h2u::empty_line;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static void vector_iterator_tests()
{
	h2u::mdo << h2u::Title(L"Vector Iterator Tests", 2);

BEGIN_CODE_BLOCK(vector_iterator_tests)
	std::vector<int> vec = {1, 2, 3, 4, 5};
	auto iter = vec.begin();

	// Test regular iterator wrapper
	{
		sgm::CV_Complete<typename std::vector<int>::iterator > wrapper{iter};

		static_assert(sgm::is_Same<decltype(*wrapper), int&>::value);
		H2U_ASSERT(*wrapper == 1);
	}

	// Test const iterator wrapper
	{
		sgm::CV_Complete<typename std::vector<int>::iterator > const const_wrapper
		{	iter
		};

		static_assert(sgm::is_Same<decltype(*const_wrapper), int const&>::value);
		H2U_ASSERT(*const_wrapper == 1);
	}

END_CODE_BLOCK_AND_LOAD(vector_iterator_tests)

	h2u::mdo
	<<	L"**Note:** volatile and const volatile tests are not valid for iterators "
	<<	L"because std::vector<int>::iterator does not support volatile-qualified access. "
	<<	L"This is a design limitation - volatile wrappers only work with raw pointers."
	<<	h2u::empty_line;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace _detail_false_constness
{

BEGIN_CODE_BLOCK(pimpl_with_false_constness)
	// Traditional Pimpl implementation
	class Traditional_Pimpl_Foo
	{
		class impl;
		impl* _pimpl;

	public:
		Traditional_Pimpl_Foo();
		~Traditional_Pimpl_Foo();

		// This allows mutation even when called on const object!
		// (False const problem)
		auto value() const-> int&;
	};


	class Traditional_Pimpl_Foo::impl
	{
		int _value;

	public:
		impl() : _value{42}{}
		auto value()-> int&{  return _value;  }
	};


	Traditional_Pimpl_Foo::Traditional_Pimpl_Foo() : _pimpl{new impl{}}
	{}

	Traditional_Pimpl_Foo::~Traditional_Pimpl_Foo()
	{
		delete _pimpl;
	}

	auto Traditional_Pimpl_Foo::value() const-> int&{  return _pimpl->value();  }
END_CODE_BLOCK(pimpl_with_false_constness)	

BEGIN_CODE_BLOCK(pimpl_with_cv_complete)
	// CV_Complete-based Pimpl implementation
	class CV_Complete_Pimpl_Foo
	{
		class impl;
		sgm::CV_Complete<impl*> _pimpl;

	public:
		CV_Complete_Pimpl_Foo();
		~CV_Complete_Pimpl_Foo();

		// This properly returns int const& when called on const object
		auto value()-> int&;
		auto value() const-> int const&;
	};


	class CV_Complete_Pimpl_Foo::impl
	{
		int _value;

	public:
		impl() : _value{100}{}
		auto value()-> int&{  return _value;  }
		auto value() const-> int const&{  return _value;  }
	};


	CV_Complete_Pimpl_Foo::CV_Complete_Pimpl_Foo() : _pimpl{new impl{}}
	{}

	CV_Complete_Pimpl_Foo::~CV_Complete_Pimpl_Foo()
	{
		delete _pimpl.base;
	}

	auto CV_Complete_Pimpl_Foo::value()-> int&{  return _pimpl->value();  }
	auto CV_Complete_Pimpl_Foo::value() const-> int const&{  return _pimpl->value();  }	
END_CODE_BLOCK(pimpl_with_cv_complete)

}


static void Pimpl_false_const_comparison()
{
	h2u::mdo << h2u::Title(L"Pimpl False Const Problem Comparison", 2);

	h2u::mdo
	<<	L"The traditional Pimpl idiom using raw pointers has a **false const problem**. "
	<<	L"When the outer class is const-qualified, the pointer itself becomes const, "
	<<	L"but the pointed-to object remains mutable. This breaks const-correctness." 
	<<	h2u::newl
	<<	h2u::Load_code_block(L"pimpl_with_false_constness")
	<<	h2u::empty_line;

	h2u::mdo
	<<	L"CV_Complete solves this by properly propagating CV-qualifiers through "
	<<	L"the indirection, maintaining true const-correctness." << h2u::newl
	<<	h2u::Load_code_block(L"pimpl_with_cv_complete")
	<<	h2u::empty_line;

	using namespace _detail_false_constness;

BEGIN_CODE_BLOCK(comparison_pimpl_tests)
	// ========== Comparison Test ==========

	// Traditional Pimpl: False const problem
	{
		Traditional_Pimpl_Foo const const_obj;

		// Despite const_obj being const, we can still modify the value!
		// This is the false const problem.
		const_obj.value() = 999;  // Compiles! (BAD)

		H2U_ASSERT(const_obj.value() == 999);
	}

	// CV_Complete Pimpl: True const correctness
	{
		CV_Complete_Pimpl_Foo const const_obj;

		static_assert
		(	sgm::is_Same<decltype(const_obj.value()), int const&>::value
		, 	"Constness from CV_Complete_Pimpl_Foo::impl sould be preserved." 
		);

		// const_obj.value() = 999;  // Would NOT compile! (GOOD)

		// Non-const object allows modification
		CV_Complete_Pimpl_Foo mutable_obj;

		mutable_obj.value() = 999;
		H2U_ASSERT(mutable_obj.value() == 999);
	}
END_CODE_BLOCK_AND_LOAD(comparison_pimpl_tests)

	h2u::mdo
	<<	L"**Key takeaway:** CV_Complete enables proper const-correctness in Pimpl, "
	<<	L"preventing accidental mutations through const interfaces. "
	<<	L"Traditional raw pointer Pimpl allows mutation of the pointed-to object "
	<<	L"even when accessed through a const method."
	<<	h2u::empty_line;
}
//========//========//========//========//=======#//========//========//========//========//=======#


H2U_HOW2USE_TESTS(sgm::test::Test_, CV_Complete, /**/)
{	::intro
,	::Raw_pointer_tests
,	::unique_ptr_tests
,	::vector_iterator_tests
,	::Pimpl_false_const_comparison
};