#include "Rail.hpp"


struct Tutorial : sgm::No_Making
{
	template<class T, int N>
	class AA
	{
	public:
		static void method1(){}

		template< class = std::enable_if_t<(N > 2)> >
		static void method2(){}
	};

	template<unsigned N> static void Case();

	template<> static void Case<1>()
	{
		AA<int, 4>::method2();

		//AA<int, 1>::method2();
	}

};

int main()
{
	Tutorial::Case<1>();

	return 0;
}