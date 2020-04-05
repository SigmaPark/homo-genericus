#pragma once

#ifndef _SGM_MATRIX_INTERFACE_
#define _SGM_MATRIX_INTERFACE_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif

#include "..\interface_Traits\interface_Traits.hpp"
#include <cassert>
#include <vector>
#include <array>
#include <iterator>
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm::mxi
{


	struct MatSize : No_Making{  static int constexpr DYNAMIC = -1;  };
	struct Storage : No_Making{  static int constexpr ColMajor = 0, RowMajor = 1;  };

	template
	<	class T, int R = MatSize::DYNAMIC, int C = MatSize::DYNAMIC
	,	int STRG = Storage::ColMajor
	> 
	struct Matrix_impl;




	template
	<	class T, int R = MatSize::DYNAMIC, int C = MatSize::DYNAMIC
	,	int STRG = Storage::ColMajor
	,	class MatOperator = Matrix_impl<T, R, C, STRG>
	>
	class Matrix
	{
	private:
		SGM_HAS_MEMFUNC(resize);

		static bool constexpr _is_dynamic_v = R == MatSize::DYNAMIC || C == MatSize::DYNAMIC;

	public:
		using elem_t = T;
		static int constexpr RSIZE = R, CSIZE = C, MAJOR = STRG;

		Matrix() : _data(), _rsize(R), _csize(C){}

		template<  class Q, class = std::enable_if_t< std::is_scalar_v<Q> >  >
		Matrix(std::initializer_list<Q>&& iL) 
		:	_data
			(	[&iL]()-> decltype(_data)
				{
					static_assert
					(	!_is_dynamic_v
					,	"initializer_listing is not available when dynamic."
					);

					std::array<elem_t, R*C> res{};

					for
					(	auto[itr, idx] = std::pair(iL.begin(), 0)
					;	itr != iL.end()
					;	++idx, ++itr
					)
						res[STRG == Storage::RowMajor ? idx : idx%C*R + idx/C]
						=	static_cast<elem_t>(*itr);
						
					return res;
				}()
			)
		,	_rsize(R), _csize(C)
		{}


		template<  class CON, class = std::enable_if_t< is_iterable<CON, elem_t>::value >  >
		Matrix(CON&& con, int r = R, int c = C)
		:	_data
			(	[&con, &r, &c]()-> decltype(_data)
				{
					assert( _is_dynamic_v || (r == R && c == C) );

					std::decay_t<decltype(_data)> res{};

					if constexpr(Has_MemFunc_resize<decltype(res), size_t>::value)
						assert(r > 0 || c > 0),
						res.resize(con.size()),
						(r < 0 ? r : c) = (int)res.size() / (r < 0 ? c : r);

					for
					(	auto[itr, idx] = std::pair(con.begin(), 0)
					;	itr != con.end()
					;	++idx, ++itr
					)
						res[STRG == Storage::RowMajor ? idx : idx%c*r + idx/c]
						=	static_cast<elem_t>(*itr);
						
					return res;
				}()
			)
		,	_rsize(r), _csize(c)
		{}



		auto operator()(int i, int j)-> elem_t{  return MatOperator::data(i, j);  }

		auto rows() const-> size_t{  return _rsize;  }
		auto cols() const-> size_t{  return _csize;  }

		auto data() const-> elem_t const*{  return _data.data();  }


	private:
		std::conditional_t	
		<	R != MatSize::DYNAMIC && C != MatSize::DYNAMIC
		,	std::array<T, R*C>, std::vector<T>
		>
		_data;

		int _rsize, _csize;
		
	};


	template<	class T, int STRG, class MatOperator>
	class Matrix_ : public Matrix<T, MatSize::DYNAMIC, MatSize::DYNAMIC, STRG, MatOperator>{};


} // end of namespace sgm::mxi

#endif // end of #ifndef _SGM_MATRIX_INTERFACE_

#if 0

	operator(i, j)

	col(j), row(i)

	cols()

	norm / squrednorm

	dot / cross

	


#endif