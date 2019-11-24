#include <array>
#include <list>
#include "sgmHOF.h"
#include <iostream>
#include <cmath>


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
				std::cout << '\n',
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
	auto const c1 = Repack_c<1, std_list>(i_arr);
	auto const c2 = Filter_c<2>(c1, [](auto const& x){  return x % 2 == 0;  });
	auto const c3 = Morph_c<2>(c2, [](auto const& x){  return double(3*x / 2);  });
	
	auto const c4
	=	Rankers_c<3>(c3, 3, [](auto x, auto y){  return abs(x - 10) < abs(y - 10);  });

	auto const c5 = Sort_c<2>(c4, std::less<>());
	auto const val = Fold_c<1>(c5);

	Show::Container(c1);
	Show::Container(c2);
	Show::Container(c3);
	Show::Container(c4);
	Show::Container(c5);

	std::cout << val << '\n';

	Functor ftr
	=	(	Fold_c<1> 
		|	Sort_c<1>
		|	Rankers_c<3>( __, 3, [](auto x, auto y) {  return abs(x - 10) < abs(y - 10);  } )
		|	Morph_c<2>( __, [](auto const& x) {  return double(3 * x / 2);  } )
		|	Filter_c<2>( __, [](auto const& x) {  return x % 2 == 0;  } )
		|	Repack_c<1, std_list>
		);

	std::cout << ftr(i_arr) << '\n';

	return 0;
}