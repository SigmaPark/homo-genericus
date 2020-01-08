#include "Carrier.hpp"
#include <cassert>
#include <vector>


class Test
{
public:
	Test() = delete;


	template<typename C1, typename C2>
	static bool have_same_contents(C1&& c1, C2&& c2)
	{
		bool ans = c1.size() == c2.size();

		for
		(	size_t idx = 0
		;	idx < c1.size() && ans
		;	ans = c1[idx] == c2[idx], idx++
		);

		return ans;
	}


	static void Method1()
	{
		int const fa[3] = {4, 2, 9};

		auto const
			sa1 = sgm::Static_CArr<int, 3>(fa),
			sa2 = sa1,
			sa3 = sgm::make_SttCArr<int>(4, 2, 9),
			sa4 = sgm::SttCArr_from<int, 3>(std::vector<int>{4, 2, 9});

		sgm::Static_CArr<int, 3> sa5;

		sa5.at(0) = 4, sa5.at(1) = 2, sa5.at(2) = 9;

		assert
		(	have_same_contents(sa1, sa2)
		&&	have_same_contents(sa2, sa3)
		&&	have_same_contents(sa3, sa4)
		&&	have_same_contents(sa4, sa5)
		);
	}


	static void Method2()
	{
		auto const
			da1 = sgm::make_DnmCArr<int>(3, 6, 9),
			da2 = da1,
			da3 = sgm::DnmCArr_from<int>(std::vector<int>{3, 6, 9});

		auto da4 = sgm::make_DnmCArr<int>(1, 2, 3);

		da4.at(0) = 3, da4.at(1) = 6, da4.at(2) = 9;

		assert
		(	have_same_contents(da1, da2)
		&&	have_same_contents(da2, da3)
		&&	have_same_contents(da3, da4)
		);
	}
};


int main()
{
	Test::Method1();
	Test::Method2();

	return 0;
}