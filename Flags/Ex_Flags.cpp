#include "Flags.hpp"
#include <cassert>
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#


//struct FFF : sgm::Flag<FFF>{};
//struct FF1 : sgm::Flag<FF1>{};
//struct FF2 : sgm::Flag<FF2>{};
//
//
//template<class Q>
//using FM
//=	sgm::FlagMatching
//	<	Q
//	,	sgm::Flag<FFF, FF1>, sgm::Flag<FF2, FFF>, sgm::Flag<FF2, FF1>
//	>;
//
//
//struct Foo : sgm::Branch<FM, Foo>
//{
//	SGM_FLAG_SWITCH;
//	
//	SGM_FLAG_CASE(FFF, FF2)
//	{
//		template<class T> static auto calc(T n)-> double{  return 2*(double)(n);  }
//	};
//	
//	SGM_FLAG_CASE(FF1, FFF)
//	{
//		template<class T> static auto calc(T n)-> int{  return 3*(int)(n);  }
//	};
//	
//	SGM_FLAG_CASE(FF1, FF2)
//	{
//		template<class T> static auto calc(T)-> int*{  return nullptr;  }
//	}; 
//};



int main()
{
	//using namespace sgm;

	//static_assert(is_Flag<FFF>::value, "");

	//static_assert
	//(	decltype(FFF() & FF2() & FF1())::has_same_flags_to< Flag<FF2, FF1, FFF> >::value
	//,	""
	//);

	//auto x = Foo::calc<int>(FFF() & FF1(), 3);

	//assert(x == 9);

	return 0;
}

