#include "Flags.hpp"
#include "..\Specification\Specification.hpp"


template<unsigned>  static void Test();


#include <type_traits>


template<>  void Test<1>()
{
	auto fs = sgm::Flags(0.f, 0.0, 0);
	auto x1 = sgm::Satisfying_Flag<std::is_integral>(fs);
	auto x2 = sgm::Satisfying_Flag<std::is_pointer>(fs);

	static_assert
	(	(	std::is_same<decltype(x1), int>::value
		&&	std::is_same<decltype(x2), sgm::None>::value
		)
	,	"" 
	);
}



#include "Test_Flags.hpp"
#include <iostream>


SGM_SPEC_TESTS(sgm_Flags)
{	::Test<1>
};