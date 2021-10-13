#pragma once

#include "..\Test_v3d.hpp"
#include "Hamilton.hpp"


namespace v3d::test
{
	
	enum class _Equiv_Hamilton_Tag;
	struct HamiltonTutorial;

}


struct v3d::test::HamiltonTutorial
{
	HamiltonTutorial() = delete;
	
	static void test();
};


template<>
struct v3d::test::_Equivalent<v3d::test::_Equiv_Hamilton_Tag>
{
	template<class L, class R, class...TYPES>
	static bool calc(L Lhs, R rhs, TYPES...args)
	{
		if constexpr( sizeof...(TYPES) > 0 )
			return calc(Lhs, rhs) && calc(Lhs, args...);
		else if constexpr(trait::has_Matrix_interface_v<L>)
		{
			bool res = Lhs.rows() == rhs.rows() && Lhs.cols() == rhs.cols();
				
			for(size_t i = Lhs.rows();  i-->0;)
				for(size_t j = Lhs.cols();  res && j-->0;)
					res = calc( Lhs(i, j), rhs(i, j) );

			return res;
		}
		else if constexpr(trait::is_complex_v<L> || trait::is_complex_v<R>)
		{
			using real_t = std::decay_t<decltype( std::real(Lhs) )>;
			using complex_t = std::complex<real_t>;

			auto const c1 = static_cast<complex_t>(Lhs), c2 = static_cast<complex_t>(rhs);

			return calc(c1.real(), c2.real()) && calc(c1.imag(), c2.imag());
		}
		else
			return _Equivalent<_Equiv_Number_Tag>::calc(Lhs, rhs);
	}
};