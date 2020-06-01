#include "indexable_impl.hpp"


struct Tutorial : sgm::No_Making
{
	template<unsigned> static void Case();

	template<> static void Case<0>()
	{

	}

};

int main()
{

	Tutorial::Case<0>();

	return 0;
}