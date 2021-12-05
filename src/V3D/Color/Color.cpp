#include "Color.hpp"
#include <cstdlib>
#include <utility>


auto v3d::Colorable::_mod(float const x, int const den)-> float
{
	float const q = x / float(den);

	return den*(  q - float( static_cast<int>(q) )  );
}


auto v3d::Colorable::hsv_to_rgb(v3d::Color_hsv const& hsv)-> Color_rgb
{
	float const H = hsv.h,  C = hsv.v * hsv.s;

	auto revolver_f
	=	[H, C, X = C*(  1.f - std::abs( _mod(H/60.f, 2) - 1.f )  ), m = hsv.v - C](int8_t const load)
		{	
			float const
				deg = H - 60.f*load,
				sdeg = deg > 180.f ? deg - 360.f : deg,
				t
				=	std::abs(sdeg) < 60.f || sdeg == (-60.f) ? C
				:	std::abs(sdeg) > 120.f || sdeg == 120.f ? 0.f
				:	/* otherwise */ X;

			return static_cast<uint8_t>( 255*(t*m) );
		};

	return {revolver_f(0), revolver_f(2), revolver_f(-2)};
}


auto v3d::Colorable::rgb_to_hsv(Color_rgb const& rgb)-> Color_hsv
{
	float const r = float(rgb.r)/255,  g = float(rgb.g)/255,  b = float(rgb.b)/255;

	auto const [pmin, pmax]
	=	[](float const* p1, float const* p2, float const* p3)
		{
			auto pswap_f = [](float const*& q1, float const*& q2){  if(*q1 > *q2)  std::swap(q1, q2);  };

			pswap_f(p1, p2),  pswap_f(p2, p3),  pswap_f(p1, p2);

			struct res_t{  float const *_1,  *_2;  };

			return res_t{p1, p3};
		}(&r, &g, &b);

	auto is_almost0_f = [](float const t){  return std::abs(t) < 1e-6f;  };

	float const
		&V = *pmax,  delta = V - *pmin,
		S = is_almost0_f(V) ? 0.f : delta/V,
		H 
		=	60.f
			*	(	is_almost0_f(delta) ? 0.f
				:	pmax == &r ? _mod( (g-b)/delta, 6 )
				:	pmax == &g ? (b-r)/delta + 2.f
				:	pmax == &b ? (r-g)/delta + 4.f
				:	/* otherwise */ 0.f
				);

	return {H, S, V};
}