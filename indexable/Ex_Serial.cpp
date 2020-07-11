#include "Serial.hpp"
#include <vector>

using namespace sgm;


struct Tutorial : No_Making
{
private:
	template<class CON1, class CON2>
	static bool same_iterables(CON1&& con1, CON2&& con2)
	{
		auto itr1 = con1.cbegin();
		auto itr2 = con2.cbegin();

		bool res = con1.size() == con2.size();

		while (itr1 != con1.cend() && res)
			res = *itr1++ == *itr2++;

		return res;
	}


public:
	template<unsigned> static void Case();


	template<> static void Case<0>()
	{
		Serial<float> const sr{5, 2, 1};

		assert( same_iterables(sr, Serial<float, 3>{5, 2, 1}) );
	}


	template<> static void Case<1>()
	{
		Serial<double> sr1, sr2 = sr1, sr1_{4, 3}, sr2_(sr1_.begin(), sr1_.end());

		assert
		(	sr1.empty() && sr2.empty() && sr1_.size() == 2
		&&	same_iterables(sr1_, sr2_)
		);


		Serial<float> sr5(4), sr6(4, 7.f);

		assert
		(	sr5.capacity() == 4 && sr5.size() == 0 
		&&	same_iterables(sr6, Serial<float>{7.f, 7.f, 7.f, 7.f})
		);


		Serial<double, 5> sr3{1, 3, 5, 7, 9}, sr4 = sr3;

		assert
		(	same_iterables(sr3, sr4)
		&&	sr3.size() == 5
		);
	}


	template<> static void Case<2>()
	{
		Serial<float> sr1{1, 2, 3}, sr2 = sr1, sr3 = sr1;

		sr1.~Serial(),
		assert
		(	sr1.size() == 0 && sr2.size() != 0
		&&	same_iterables(sr2, sr3)
		);
	}
	//========//========//========//========//=======#//========//========//========//========//===

	struct No_Copy_No_Assign
	{
		No_Copy_No_Assign() = default;

		No_Copy_No_Assign(No_Copy_No_Assign const&) = delete;
		No_Copy_No_Assign(No_Copy_No_Assign&&){}
		
		auto operator=(No_Copy_No_Assign const&)->No_Copy_No_Assign& = delete;
	};


	template<> static void Case<3>()
	{
		using type = No_Copy_No_Assign;

		Serial<type> sr(4);

		sr >> type() >> type() >> type() >> type(),  assert(sr.size() == 4);

		sr.pop_back().pop_back(),  assert(sr.size() == 4 - 2);

		sr = Serial<type>(3),  assert(sr.size() == 0 && sr.capacity() == 3);

		for
		(	sr = Serial<type>(3)
		;	sr.size() != sr.capacity()
		;	sr >> type()
		);

		assert(sr.size() == 3);

		sr.clear(),  assert(sr.size() == 0 && sr.capacity() == 3 && sr.empty());
	}
	//========//========//========//========//=======#//========//========//========//========//===
	
	
	template<> static void Case<4>()
	{
		Serial<float> sr1{4, 2}, sr2{9, 7, 5, 3, 1}, sr3{3, 0, -3};

		sr1 = sr2,  assert( same_iterables(sr1, sr2) );
		sr2 = sr3,  assert( same_iterables(sr2, sr3) && sr2.capacity() == 5 );

		sr3 = {7, 5},  
		assert( same_iterables(sr3, Serial<float>{7, 5}) && sr3.capacity() == 2 );

		Serial<int> sr4{12, 3, 45}, sr5{6789};

		sr4.swap(sr5),
		assert
		(	same_iterables(sr4, Serial<int>{6789})
		&&	same_iterables(sr5, Serial<int>{12, 3, 45})
		);

		std::swap(sr4, sr5),
		assert
		(	same_iterables(sr5, Serial<int>{6789})
		&&	same_iterables(sr4, Serial<int>{12, 3, 45})
		);

		Serial<int, 2> sr6{2, 4}, sr7{6, 8};

		sr6.swap(sr7),
		assert
		(	same_iterables(sr6, Serial<int>{6, 8}) 
		&&	same_iterables(sr7, Serial<int>{2, 4})
		);
	}
	//========//========//========//========//=======#//========//========//========//========//===


};
//========//========//========//========//=======#//========//========//========//========//=======#



int main()
{
	Tutorial::Case<0>();
	Tutorial::Case<1>();
	Tutorial::Case<2>();
	Tutorial::Case<3>();
	Tutorial::Case<4>();

	return 0;
}
