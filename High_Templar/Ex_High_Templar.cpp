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
	static void Container(CON&& con, size_t row_size = 20)
	{
		auto d = row_size;

		for(auto const& t : con)
		{
			if(d == 0)
				std::cout << std::endl,
				d = row_size;
			

			std::cout << t << ' ',
			d--;
		}

		std::cout << "\n\n";
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#

int main()
{
	using namespace sgm;

	auto constexpr i_arr = indices_v<20, 1>;
	auto const c1 = Repack<std_list>(i_arr);
	auto const c2 = Filter(c1, [](auto const& x){  return x % 2 == 0;  });
	auto const c3 = Morph(c2, [](auto const& x) {  return double(3*x / 2);  });
	auto const c4 = Rankers(c3, 3, [](auto x, auto y){  return abs(x - 10) < abs(y - 10);  });
	auto const c5 = Sort(c4, std::less<>());
	auto const val = Fold(c5, std::multiplies<>(), -1.0);

	Show::Container(c1);
	Show::Container(c2);
	Show::Container(c3);
	Show::Container(c4);
	Show::Container(c5);
	std::cout << val << '\n';

	return 0;
}