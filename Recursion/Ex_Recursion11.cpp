#include "Recursion11.hpp"


int main()
{
	static_assert
	(	std::is_same
		<	sgm::Last_t< int, double, double const* >
		,	double const*
		>::value
	,	""
	);

	return 0;
}

#if 0

using namespace sgm;

int const a
=	Recursion(10, 0, 1)
	>>	[](int r, int before, int current)-> Recursion_Result
		{
			return 
			r < 1
			?	Escape()
			:	Feed_Back(r - 1, current, before + current); 

		}
	>>	[](int n)-> bool
		{
			return n % 2 == 1 ? true : false;
		}
	;


#endif