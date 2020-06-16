#include "indexable.hpp"
#include <cassert>

struct Tutorial : sgm::No_Making
{
private:
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
	template<unsigned> static void Case();


	template<> static void Case<1>()
	{
		using namespace sgm;

		indexable<double> ix1, ix2 = ix1;
		indexable<double, 5> ix3{1, 3, 5, 7, 9}, ix4 = ix3;
		indexable<float> ix5(4), ix6(4, 7.f);
		indexable<double> ix7 = ix3, ix8(ix3.begin(), ix3.end());

		ix1 = indexable<double>{10, 9, 8, 7};

		assert
		(	same_iterables(ix3, ix4)
		&&	ix5.capacity() == 4 && ix5.size() == 0 
		&&	same_iterables(ix6, indexable<float>{7.f, 7.f, 7.f, 7.f})
		&&	same_iterables(ix3, ix7) && same_iterables(ix3, ix8)
		&&	same_iterables(ix1, indexable<double, 4>{10, 9, 8, 7})
		);
	}


	template<> static void Case<2>()
	{
		float sarr[] = {12, 34, 56};

		static_assert
		(	(	std::is_same<decltype(sarr), float[3]>::value
			&&	!std::is_same<decltype(sarr), float*>::value
			)
		,	""
		);
	}


	template<> static void Case<3>()
	{
		using namespace sgm;

		std::vector<double> vec1 = indexable<double>{1, 3};
		
		assert( same_iterables(vec1, indexable<double>{1, 3}) );

		indexable<float> ix1{1, 2, 3}, ix2 = ix1, ix3 = ix1;

		ix1.~indexable();

		assert
		(	ix1.size() == 0 && ix2.size() != 0
		&&	same_iterables(ix2, ix3)
		);

	}


};





int main()
{
	Tutorial::Case<1>();
	Tutorial::Case<2>();
	Tutorial::Case<3>();

	return 0;
}