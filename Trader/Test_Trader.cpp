#include "..\Specification\Specification.hpp"
#include "Trader.hpp"


using sgm::Trader;
using sgm::spec::is_True;


template<unsigned>  static void Test();

template<>
void Test<1>()
{
	int x = 1, y = 0;

	{
		auto &&t = sgm::make_Trader(x, [&y](int const &a){  y = 2*a + 3;  });

		is_True(t == 1);

		t = 3;
	}

	is_True(y == 2*3 + 3);
}


template<>
void Test<2>()
{
	int x = 5;

	[](int &t){  t = 10;  }
	(	std::ref( sgm::make_Trader(x, [&x](int const &a){  x = 3*a;  }) )
	);

	is_True(x == 3*10);
}


struct AA
{
	int x;

	auto value()-> int&{  return x;  }
};


template<>
void Test<3>()
{
	AA aa{1};
	int y = 0;

	sgm::make_Trader(aa, [&y](AA const &aa){  y = aa.x*2;  }).value() = 3;

	is_True(y == 2*3);
}


template<>
void Test<4>()
{
	class BB
	{
	public:
		BB(AA aa) : _a(aa){}

		auto value()
		{
			return 
			sgm::make_Trader
			(	AA{_a.value() * 10} , [&r = _a](AA const &t){  r.value() = t.x / 10;  }
			);
		}

		auto aval() const{  return _a.x;  }

	private:
		AA _a;
	};

	BB bb(AA{3});

	[](AA &a){  a.value() = a.value() + 20;  }
	(	std::ref(bb.value())
	);

	is_True(bb.aval() == 5);
}


#include <cmath>
#include <tuple>


template<>
void Test<5>()
{
	class Coordinate
	{
	public:
		Coordinate(float const x, float const y, float const z) : _x(x), _y(y), _z(z){}

		auto Cartesian() const-> std::tuple<float, float, float>{  return{_x, _y, _z};  }
		auto Cartesian()-> std::tuple<float&, float&, float&>{  return{_x, _y, _z};  }

		auto Spherical() const
		->	std::tuple<float, float, float>{  return _xyz_to_sphere(_x, _y, _z);  }

		auto Spherical()
		{
			return 
			sgm::make_Trader
			(	_xyz_to_sphere(_x, _y, _z)
			,	[&x = _x, &y = _y, &z = _z](std::tuple<float, float, float> const &sphere)
				{
					auto const[rho, theta, phi] = sphere;

					std::tie(x, y, z) = _sphere_to_xyz(rho, theta, phi);
				}
			);
		}


		static auto _xyz_to_sphere(float const x, float const y, float const z)
		->	std::tuple<float, float, float>
		{
			auto const rho = (float)std::sqrt( std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2) );
	
			return{ rho, (float)std::atan(y/x), std::acos(z/rho) };
		}
	
		static auto _sphere_to_xyz(float const rho, float const theta, float const phi)
		->	std::tuple<float, float, float>
		{
			float const 
				ct = std::cos(theta), st = std::sin(theta),
				cp = std::cos(phi), sp = std::sin(phi);
	
			return{ rho*sp*ct, rho*sp*st, rho*cp };
		}


	private:
		float _x, _y, _z;
	};


	Coordinate coordi(1, 1, 1);

	auto same_floats_f
	=	[]
		(	std::tuple<float, float, float> const coordi1
		,	std::tuple<float, float, float> const coordi2
		)
		{
			auto const[x1, x2, x3] = coordi1;
			auto const[y1, y2, y3] = coordi2;
			float constexpr eps = .00001f;

			return 
			(	std::abs(x1 - y1) < eps 
			&&	std::abs(x2 - y2) < eps 
			&&	std::abs(x3 - y3) < eps
			);
		};
	
	is_True
	(	same_floats_f( coordi.Cartesian(), std::make_tuple(1.f, 1.f, 1.f) )  
	&&	same_floats_f
		(	coordi.Spherical()
		,	Coordinate::_xyz_to_sphere(1, 1, 1)
		)
	);

	auto const[rho, theta, phi] = sgm::eval(coordi.Spherical());
	float const Pi = 2.f*acos(0.f);

	coordi.Spherical() = std::make_tuple(rho, theta + Pi/2, phi);

	is_True
	(	same_floats_f( coordi.Cartesian(), std::make_tuple(-1.f, 1.f, 1.f) )
	&&	same_floats_f( coordi.Spherical(), Coordinate::_xyz_to_sphere(-1, 1, 1) )
	);
}


#include "Test_Trader.hpp"
#include <iostream>


void Test_sgm_Trader::test()
{
	try
	{
		::Test<1>();
		::Test<2>();
		::Test<3>();
		::Test<4>();
		::Test<5>();

		std::wcout << L"Trader Test Complete.\n";
	}
	catch(...)
	{
		std::wcout << L"Trader Test Failed.\n";

		throw;
	}
}