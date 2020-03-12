#include "Flags.hpp"


class FFF : public sgm::Flag_t{};



int main()
{
	using namespace sgm;

	static_assert(Flags<FFF, Flag_t>::has<FFF>::value, "");

	return 0;
}