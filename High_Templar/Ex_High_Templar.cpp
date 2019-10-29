#include <iostream>
#include <vector>
#include <list>
#include <forward_list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include "High_Templar.h"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

class Show
{
public:
	Show() = delete;

	template<typename CON>
	static void Container(CON&& con, size_t row_size = 10)
	{
		auto d = row_size;

		for(auto const& t : con)
		{
			if(d == 0)
				std::cout << '\n',
				d = row_size;

			std::cout << t << ' ',
			d--;
		}

		std::cout << '\n';
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#

void tutorial01()
{
	auto const vector_indices = sgm::indices(10);
	auto const vector_indices2 = sgm::indices(7, 1);
	auto const array_indices = sgm::indices<10>();
	auto const array_indices2 = sgm::indices<10, 5>();

	Show::Container(vector_indices);
	Show::Container(vector_indices2);
	Show::Container(array_indices);
	Show::Container(array_indices2);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#

void tutorial02()
{


}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#

int main()
{
	tutorial01();



	return 0;
}