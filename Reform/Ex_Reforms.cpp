#include "Reform.h"
#include "ReformEigen.h"
#include "ReformGL.h"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

DEFINE_SGM_REAL_TYPE(2, double);
DEFINE_SGM_INDEX_TYPE(2, unsigned);

class Test
{
public:
	Test() = delete;

	static void Case1()
	{
		static_assert
		(	std::is_same_v<sgm::Real_t<>, float>
		&&	std::is_same_v<sgm::idx_t<>, size_t>
		&&	std::is_same_v<sgm::Real_t<2>, double>
		&&	std::is_same_v<sgm::idx_t<2>, unsigned>
		);
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<  typename T, typename = std::enable_if_t<std::is_floating_point_v <T> >  >
	static bool is_equal(T t1, T t2, T allowance = 30)
	{
		return abs(t1 - t2) < allowance * std::numeric_limits<T>::epsilon();
	}


	template
	<	typename T, unsigned D, typename = std::enable_if_t<std::is_floating_point_v <T> >
	>
	static bool is_equal(sgm::Vector<T, D> t1, sgm::Vector<T, D> t2, T allowance = 30)
	{
		return (t1 - t2).norm() < D * allowance * std::numeric_limits<T>::epsilon();
	}


	static void Case2()
	{
		using namespace sgm;
		using Real1 = Real_t<1>;

		Vector<Real1, 3> const ZeroVec(0, 0, 0);
		UnitVector<Real1, 3> const Xaxis(1, 0, 0), Yaxis(0, 1, 0), Zaxis(0, 0, 1);
		auto const pi = static_cast<Real1>( 2*acos(0) );

		if 
		(	is_equal<Real1>( Xaxis.dot(Yaxis()), 0 )
		&&	is_equal<Real1>( Xaxis.dot(Xaxis()), 1 )
		&&	is_equal<Real1>( Xaxis.angle(-Xaxis), pi )
		&&	is_equal<Real1>( Xaxis.angle(Zaxis), pi/2 )
		&&	Xaxis.is_parallel(-Xaxis)
		&&	Xaxis.is_orthogonal(Zaxis)
		&&	is_equal<Real1>
			(	Projected<Real1>(	Vector<Real1, 3>(1, 1, 2), Plane<Real1, 3>(ZeroVec, Zaxis) )
			,	2
			)
		)	
			return;
		else 
			throw false;
	}
};
//========//========//========//========//=======#//========//========//========//========//=======#


int main()
{
	Test::Case1();
	Test::Case2();

	return 0;
}