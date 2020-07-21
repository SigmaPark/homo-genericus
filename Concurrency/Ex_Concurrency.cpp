#include "Concurrency.hpp"
#include <vector>

struct Tutorial 
{
	static void Case1()
	{
		std::vector<int> target(100);

		auto func
		=	[&target](std::vector<int>::const_iterator bi, std::vector<int>::const_iterator ei)
			{
				
			};
	}
};


int main()
{
	

	return 0;
}