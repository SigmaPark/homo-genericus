#include <initializer_list>
#include <iterator>
#include <vector>
#include "Carrier.hpp"
#include <cassert>
#include <utility>


class UnitTest
{
private:
	template<class T>
	using initList = std::initializer_list<T>;


	template<class CON1, class CON2>
	static bool same_iterables(CON1&& con1, CON2&& con2)
	{
		bool res = con1.size() == con2.size();

		for
		(	auto itrPa = std::make_pair(con1.begin(), con2.begin())
		;	itrPa.first != con1.end() && res
		;	res = *itrPa.first == *itrPa.second, itrPa.first++, itrPa.second++
		);

		return res;
	}


public:
	UnitTest() = delete;


	static void Method1()
	{
		initList<int> v1{5, 2, 4, 3};

		sgm::Carrier<int> const crr1 = v1;
		sgm::Carrier<int> crr2 = initList<int>{5, 2, 4, 3};

		auto citr1 = crr1.cbegin();
		auto itr2 = v1.begin();

		for(; itr2 != v1.end(); citr1++, itr2++)
			assert(*citr1 == *itr2);

		itr2 = v1.begin();

		for(auto t : crr2)
			assert( t == *(itr2++) );
	}


	static void Method2()
	{
		sgm::Carrier<int> crr1;

		crr1 = initList<double>{5, 2, 4, 3};

		assert( same_iterables(crr1, initList<int>{5, 2, 4, 3}) );

		*crr1.begin() = 15;
		crr1[2] = 44;
		
		assert(same_iterables(crr1, initList<int>{15, 2, 44, 3}));

		sgm::Carrier<int> const crr2 = crr1;
		sgm::Carrier<int> crr3 = crr2;

		assert
		(	same_iterables(crr1, crr2)
		&&	same_iterables(crr2, crr3)
		);
	}


	static void Method3()
	{
		sgm::Carrier<int> const
			crr1{3, 1, 4, 1, 5, 9, 2},
			crr2{2, 9, 5, 1, 4, 1, 3};

		for
		(	auto Pa = std::make_pair(crr1.crbegin(), crr2.cbegin())
		;	Pa != std::make_pair(crr1.crend(), crr2.cend())
		;	Pa.first++, Pa.second++
		)
			assert(*Pa.first == *Pa.second);
	}


	static void Method4()
	{
		//sgm::Carrier<double> const dv{1.0, 3.0, 7.0, 12.0, 20.0};

		//auto&& itr = dv.rbegin();

		//assert(itr[2] == 3.0);

		//std::vector<double>::reverse_iterator&& s = itr++;

	}


};

int main()
{


	UnitTest::Method1();
	UnitTest::Method2();
	UnitTest::Method3();
	UnitTest::Method4();

	return 0;
}

