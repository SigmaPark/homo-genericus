#include "Carrier.hpp"
#include <cassert>
#include <vector>
#include <iostream>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

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
		std::vector<int> const Lvec{2, 5, 8};

		sgm::Static_CArr<int, 3> const
			stt1 = sgm::Static_CArr<int, 3>::from_iterable(Lvec),
			stt2 = sgm::Static_CArr<int, 3>::from_iterable(std::vector<int>{2, 5, 8}),
			stt3 = sgm::Static_CArr<int, 3>::from_indexed(Lvec),
			stt4 = sgm::Static_CArr<int, 3>::from_indexed(std::vector<int>{2, 5, 8}),
			stt5 = sgm::Static_CArr<int, 3>::from_iterable(std::initializer_list<int>{2, 5, 8});


		using stt_deep_t = sgm::Static_CArr<int, 3, sgm::Carry_t::DEEP>;

		stt_deep_t const
			stt6 = stt_deep_t::from_iterable(Lvec),
			stt7 = stt_deep_t::from_indexed(std::vector<int>{0, 2, 5, 8}, 1);

		assert
		(	have_same_contents(stt1, stt2)
		&&	have_same_contents(stt2, stt3)
		&&	have_same_contents(stt3, stt4)
		&&	have_same_contents(stt4, stt5)
		&&	have_same_contents(stt5, stt6)
		&&	have_same_contents(stt6, stt7)
		);
	}


	static void Method2()
	{
		std::vector<int> const Lvec{ 2, 5, 8 };

		sgm::Dynamic_CArr<int> const
			dnm1 = sgm::Dynamic_CArr<int>::from_iterable(Lvec),
			dnm2 = sgm::Dynamic_CArr<int>::from_iterable(std::vector<int>{2, 5, 8}),
			dnm3 = sgm::Dynamic_CArr<int>::from_indexed(Lvec),
			dnm4 = sgm::Dynamic_CArr<int>::from_indexed(std::vector<int>{2, 5, 8}),
			dnm5 = sgm::Dynamic_CArr<int>::from_iterable(std::initializer_list<int>{2, 5, 8});

		using dnm_deep_t = sgm::Dynamic_CArr<int, sgm::Carry_t::DEEP>;

		dnm_deep_t const
			dnm6 = dnm_deep_t::from_iterable(Lvec),
			dnm7 = dnm_deep_t::from_indexed(std::vector<int>{0, 2, 5, 8}, 1);

		assert
		(	have_same_contents(dnm1, dnm2)
		&&	have_same_contents(dnm2, dnm3)
		&&	have_same_contents(dnm3, dnm4)
		&&	have_same_contents(dnm4, dnm5)
		&&	have_same_contents(dnm5, dnm6)
		&&	have_same_contents(dnm6, dnm7)
		);
	}


	static void Method3()
	{
		static_assert
		(	(	sgm::CArrier::Made< sgm::Static_CArr<int, 3> >::value
			&&	sgm::CArrier::Made< sgm::Dynamic_CArr<int, sgm::Carry_t::DEEP> >::value
			&&	sgm::CArrier::Made_Static
				<	sgm::Static_CArr<double, 1, sgm::Carry_t::DEEP> 
				>::	value
			&&	sgm::CArrier::Made_Dynamic< sgm::Dynamic_CArr<double> >::value
			)
		,	"Not CArrier type"
		);
	}


	static void Method4()
	{
		static_assert
		(	(	sgm::CArrier::Made_Shallow< sgm::Static_CArr<int, 3> >::value
			&&	sgm::CArrier::Made_Deep< sgm::Dynamic_CArr<int, sgm::Carry_t::DEEP> >::value
			&&	!sgm::CArrier::Made_Shallow
				<	sgm::Static_CArr<double, 1, sgm::Carry_t::DEEP> 
				>::	value
			&&	sgm::CArrier::Made_Shallow< sgm::Dynamic_CArr<double> >::value
			)
		,	"dismatched Carry_t"
		);
	}


	static void Method5()
	{
		std::vector<int> const Lvec{2, 5, 8};

		sgm::Static_CArr<int, 3> const stt1 
		=	sgm::Static_CArr<int, 3>::from_iterable(Lvec);

		for(auto const& t : stt1)
			std::cout << t << ' ';
		std::cout << std::endl;

		for( auto t : sgm::Static_CArr<int, 3>::from_iterable(std::vector<int>{2, 5, 8}) )
			std::cout << t << ' ';
		std::cout << std::endl;


		sgm::Dynamic_CArr<int> dnm1
		=	sgm::Dynamic_CArr<int>::from_indexed(Lvec);

		for(auto itr = dnm1.begin(); itr != dnm1.end(); itr++)
			std::cout << *itr << ' ';
		std::cout << std::endl;

		for( auto t : sgm::Dynamic_CArr<int>::from_indexed(std::vector<int>{2, 5, 8}) )
			std::cout << t << ' ';
		std::cout << std::endl;
	}
};


int main()
{
	Test::Method1();
	Test::Method2();
	Test::Method3();
	Test::Method4();
	Test::Method5();

	return 0;
}

