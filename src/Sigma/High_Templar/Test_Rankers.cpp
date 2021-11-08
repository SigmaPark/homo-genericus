#include "Test_Rankers.hpp"
#include "Rankers.hpp"

#include "..\Specification\Specification.hpp"
#include <functional>


using namespace sgm;


static void Test01()
{
	Serial<int> const sr{0, 8, 4, 2, 7, 9, 1, 3, 5, 6, 3, 3, 2, 9};
	
	spec::is_True
	(	spec::Judge::have_the_same
		(	ht::Rankers(sr, 5)
		,	Serial<int, 5>{0, 1, 2, 2, 3}
		)
	&&	spec::Judge::have_the_same
		(	ht::Rankers(sr, 4, std::greater<int>())
		,	Serial<int, 4>{9, 9, 8, 7}
		)
	);	
}


SGM_SPEC_TESTS(sgm_Rankers)
{	::Test01
};
