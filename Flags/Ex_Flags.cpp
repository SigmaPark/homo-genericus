#include "Flags.hpp"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#


class FFF : public sgm::Flag_t<FFF>{};
class FF1 : public sgm::Flag_t<FF1>{};
class FF2 : public sgm::Flag_t<FF2>{};




int main()
{
	using namespace sgm;

	static_assert(is_Flag<FFF>::value, "");

	static_assert
	(	decltype(FFF() & FF2() & FF1())::has_same_flags_to< Flag_t<FF2, FF1, FFF> >::value
	,	""
	);

	return 0;
}


#if 0 



#endif 