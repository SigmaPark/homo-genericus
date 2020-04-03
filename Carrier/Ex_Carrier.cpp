#include <initializer_list>
#include <iterator>
#include <vector>
#include "Carrier.hpp"
#include <cassert>
#include <utility>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

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

		//sgm::is_iterable<initList<int>, int>::value;

		sgm::Carrier<int> const crr1 = std::move(v1);
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
		
		assert( same_iterables(crr1, initList<int>{15, 2, 44, 3}) );

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
		{
			int n1 = *Pa.first, n2 = *Pa.second;

			assert(n1 == n2);
		}
	}


	static void Method4()
	{
		sgm::Carrier<double> const dv{1.0, 3.0, 7.0, 12.0, 20.0};

		auto itr = dv.rbegin();

		//assert(itr[2] == 3.0);

		std::vector<int> const v1{4, 5, 6}, v2{0, 2, 4};
	}


	static void Method5()
	{
		std::vector<int> v1 = sgm::Carrier<int>{3, 1, 4};

		assert( v1.at(0) == 3 );
		assert( v1.at(1) == 1 );
		assert( v1.at(2) == 4 );
	}


	static void Method6()
	{
		sgm::Carrier<int> const ccrr1{2, 5};
		sgm::Carrier<int> crr2{20, 500};

		auto itr1 = ccrr1.begin();
		auto itr2 = crr2.begin();

		//*itr1 = 60;		// compile error
		*itr2 = 11;

		//tr2 = itr1;	// compile error
		itr1 = itr2;


	}

	
	static void Method7()
	{
		std::vector<double> v{1,3,5,7};

		sgm::Carrier<double> crr(v.begin(), v.begin() + 2);
	}


	static void Method8()
	{
		{
			sgm::Carrier<int> crr{3, 5, 7, 9, 11};

			crr.pop_back(2);
		
			size_t n = 0;
		
			for(auto itr = crr.begin(); itr != crr.end(); ++itr)
				++n,
				assert( *itr == int(2*n + 1) );

			assert(n == 3);
		}
		{
			sgm::Carrier<double> crr{2, 4, 6, 8, 10, 12};

			crr.pop_back_from(crr.begin() + 3);

			size_t n = 0;

			for(auto itr = crr.begin(); itr != crr.end(); ++itr)
				++n,
				assert( *itr == int(2*n) );

			assert(n == 3);
		}
	}


};



////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

int main()
{
	UnitTest::Method1();
	UnitTest::Method2();
	UnitTest::Method3();
	UnitTest::Method4();
	UnitTest::Method5();
	UnitTest::Method6();
	UnitTest::Method7();
	UnitTest::Method8();

	return 0;
}

