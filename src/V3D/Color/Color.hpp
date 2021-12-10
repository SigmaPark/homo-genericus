#pragma once
#ifndef _V3D_COLOR_
#define _V3D_COLOR_

#include "Sigma\Trader\Trader.hpp"
#include <cstdint>


#ifndef SGM_SYNTEX_VERSION_CPP17
	#error C++17 or higher version of language support is required.
#endif


namespace v3d
{
	
	struct Color_rgb;
	struct Color_rgb_a;
	struct Color_hsv;

	class Colorable;

}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


namespace v3d::trait
{

	template<class T>  static bool constexpr is_Color_v = std::is_constructible_v<Colorable, T>;
	template<class T>  using is_Color = sgm::Boolean_type< is_Color_v<T> >;

}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


struct v3d::Color_rgb
{
	uint8_t r, g, b;


	static auto constexpr Red()-> Color_rgb{  return {255, 0, 0};  }
	static auto constexpr Green()-> Color_rgb{  return {0, 255, 0};  }
	static auto constexpr Blue()-> Color_rgb{  return {0, 0, 255};  }
	static auto constexpr White()-> Color_rgb{  return {255, 255, 255};  }
	static auto constexpr Black()-> Color_rgb{  return {0, 0, 0};  }
	static auto constexpr Gray()-> Color_rgb{  return {127, 127, 127};  }
};

struct v3d::Color_rgb_a : Color_rgb{  uint8_t a = 255;  };
struct v3d::Color_hsv{  float h, s, v;  };


class v3d::Colorable
{
public:
	Colorable(Color_rgb const& rgb) : _rgb(rgb){}
	Colorable(Color_rgb&& rgb = {}) : _rgb( std::move(rgb) ){}
	Colorable(Color_hsv const& hsv) : _rgb( hsv_to_rgb(hsv) ){}

	template
	<	class Q
	,	class
		=	std::enable_if_t
			<	std::is_constructible_v<Colorable, Q&&> && !std::is_same_v< std::decay_t<Q>, Colorable >
			>
	>
	auto operator=(Q&& q) noexcept(std::is_rvalue_reference_v<Q&&>)
	->	Colorable&{  return *this = Colorable( std::forward<Q>(q) );  }


	auto crgb() const-> Color_rgb const&{  return _rgb;  }
	decltype(auto) rgb() const{  crgb();  }
	auto rgb()-> Color_rgb&{  return _rgb;  }

	auto chsv() const-> Color_hsv{  return rgb_to_hsv(_rgb);  }
	decltype(auto) hsv() const{  return chsv();  }

	decltype(auto) hsv()
	{
		return
		sgm::make_Trader
		(	rgb_to_hsv(_rgb)
		,	[&y = _rgb](Color_hsv const& t){  y = hsv_to_rgb(t);  }
		);
	}


	static auto hsv_to_rgb(Color_hsv const& hsv)-> Color_rgb;
	static auto rgb_to_hsv(Color_rgb const& rgb)-> Color_hsv;

private:
	Color_rgb _rgb;


	static auto _mod(float const x, int const den)-> float;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#endif // end of #ifndef _V3D_COLOR_