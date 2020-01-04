#include "Armory.h"
#include <iostream>


int main()
{
	using namespace sgm;

	std::wcout << std::boolalpha
	<<	is_close<2>(1.0, .99999) << ", " << is_close<5>(1.f, .99999f) << '\n'
	<<	is_big_enough<1>(std::numeric_limits<int>::max()/1000) << ", "
	<<	is_big_enough<5>(std::numeric_limits<double>::max()/10.0) << '\n'
	<<	Mean<float>(std::vector<float>{1,2,3,4,5,6}).value() << '\n'
	<<	Mean(std::vector<float>{1,2,3,4,5,6}, [](auto){  return 1.f;  }).value() << '\n'
	<<	Mean
		(	std::vector<float>{1,2,3,4,5,6}
		,	std::vector<float>{1,1,1,1,1,1}, std::execution::par
		).value() << '\n'
	<<	Bound(0).between(-1, 1) << '\n'
	;

	try
	{
		auto const g = Bound(0).between(1, -1);
	}
	catch(sgm::Exception const& e)
	{
		std::wcerr << L"Exception tag : " << e.tag() << '\n';
	}


	return 0;
}