#include <iterator>
#include <vector>
#include <cassert>
#include "High_Templar11.hpp"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

class Tutorial : public sgm::No_Making
{
public:
	template<unsigned> static void Case();

	template<>
	static void Case<1>()
	{
		std::vector<int> v1{2, 4, 8};

		std::vector<int> const a1 = sgm::Morph(v1, [](int i)-> int{  return 2*i;  });

		auto itr = v1.cbegin();
		
		for(auto const& x : a1)
			assert(x == 2 * *itr++);
	}


	template<>
	static void Case<2>()
	{
		auto domain = sgm::indices(5);
		
		auto itr = domain.cbegin();

		for(size_t n = 0; n < domain.size(); n++)
			assert(*itr++ == n);
	}

};
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

int main()
{
	Tutorial::Case<1>();
	Tutorial::Case<2>();

	return 0;
}