#include "Type_Analysis.hpp"
#include <cassert>

namespace
{
	class UnitTest
	{
	public:
		UnitTest() = delete;


		static void Case01()
		{
			static_assert(std::is_same<sgm::Pointerless_t<int**const**>, int>::value, "");
		}


		static void Case02()
		{
			static_assert
			(	sgm::Has_Type<int>::among<double, int, int const>::value
			,	""
			);
		}


		static void Case03()
		{
			static_assert
			(	sgm::Check_All<std::is_integral>::for_all<size_t, int, unsigned>::value
			,	""
			);

			static_assert
			(	sgm::Check_All<std::is_integral>::for_any<size_t*, int*, unsigned>::value
			,	""
			);
		}
	};
}

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

int main()
{
	return 0;
}