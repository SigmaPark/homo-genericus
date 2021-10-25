#include "..\Specification\Specification.hpp"
#include <type_traits>

using namespace sgm;


static void Test_No_Making() noexcept
{
	class AA : No_Making{};

	static_assert
	(	!std::is_constructible<AA>::value
	,	"Not constructable if it inherites sgm::No_Making ."
	);

	//AA aa;		// Compile Error
}


static void Test_Boolean_type() noexcept
{
	static_assert
	(	(	std::is_same< Boolean_type<true>, True_t >::value
		&&	std::is_same< Boolean_type<false>, False_t >::value
		&&	True_t::value	
		&&	!False_t::value
		)
	,	""
	);
}


static void Test_Specifiers() noexcept
{
	class T;

	static_assert
	(	(	std::is_same< Referenceless_t<T>, T >::value
		&&	std::is_same< Referenceless_t<T&>, T >::value
		&&	std::is_same< Referenceless_t<T&&>, T >::value
		&&	std::is_same< Referenceless_t<T const&>, T const >::value
		)
	,	""
	);

	static_assert
	(	(	std::is_same< Constless_t<T>, T >::value
		&&	std::is_same< Constless_t<T&>, T& >::value
		&&	std::is_same< Constless_t<T&&>, T&& >::value
		&&	std::is_same< Constless_t<T const>, T >::value
		&&	std::is_same< Constless_t<T const&>, T const& >::value
		&&	std::is_same< Constless_t<T const*>, T const* >::value
		&&	std::is_same< Constless_t<T* const>, T* >::value
		&&	std::is_same< Constless_t<T const* const>, T const* >::value
		)
	,	""
	);

	static_assert
	(	(	std::is_same< Decay_t<T>, T >::value
		&&	std::is_same< Decay_t<T&>, T >::value
		&&	std::is_same< Decay_t<T&&>, T >::value
		&&	std::is_same< Decay_t<T const&>, T >::value
		&&	std::is_same< Decay_t<T const*>, T const* >::value
		&&	std::is_same< Decay_t<T* const>, T* >::value
		&&	std::is_same< Decay_t<T const* const>, T const* >::value
		)
	,	""
	);
}


static void Test_Declval() noexcept
{
	class AA : No_Making{};
	class BB;

	static_assert
	(	(	is_Same<AA, decltype(Declval<AA>())>::value
		&&	is_Same<BB, decltype(Declval<BB>())>::value
		)
	,	""
	);
}


#include "Test_Type_Analysis.hpp"
#include <iostream>


SGM_SPEC_TESTS(sgm_Type_Analysis)
{	::Test_No_Making
,	::Test_Boolean_type
,	::Test_Specifiers
,	::Test_Declval
};