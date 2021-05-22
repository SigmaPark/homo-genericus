#pragma once

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version of language support is required.
#endif

#ifndef _SGM_PLY_
#define _SGM_PLY_

#include <list>
#include <string>
#include <fstream>
#include <variant>
#include "..\Carrier\Carrier.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{
	namespace ply
	{
		
		class Header
		{
		public:
			Header() 
			:	_vertex_prop(), _normal_prop(), _color_prop(), _face_prop(), _edge_prop()
			,	_nofV(0), _nofF(0), _nofE(0)
			{}


			bool has_vertex() const;
			bool has_normal() const;
			bool has_color() const;
			bool has_alpha() const;
			bool has_face() const;
			bool has_edge() const;

			auto nof_vertex() const-> size_t;
			auto nof_face() const-> size_t;
			auto nof_edge() const-> size_t;


		private:
			struct Property
			{
				enum class Prec_t{none, i8, ui8, i16, ui16, i32, ui32, f32, f64} precision;
				
				enum dimen_num{ UNFIXED = -1 };
				
				signed dimension;

				Property() : precision(Prec_t::none), dimension(UNFIXED){}
				Property(Prec_t prec, signed dim) : precision(prec), dimension(dim){}
			}	_vertex_prop, _normal_prop, _color_prop, _face_prop, _edge_prop;
			
			size_t _nofV, _nofF, _nofE;


			static auto _make_prop(std::string const& str_prec, signed dim)-> Property
			{
			}
		};
	}
}

#endif // end of #ifndef _SGM_PLY_