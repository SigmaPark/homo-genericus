#pragma once

#ifndef _SGM_MATRIX_INTERFACE_
#define _SGM_MATRIX_INTERFACE_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif

#include "..\interface_Traits\interface_Traits.hpp"
#include <cassert>
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
	struct Matrix_impl;							// should be defined later.


	template<class, int, int> struct internalData;	// should be defined later.


	template
	<	class T, int R = MatSize::DYNAMIC, int C = MatSize::DYNAMIC
	,	int STRG = Storage::ColMajor
	,	class MatOperator = Matrix_impl<T, R, C, STRG>
	,	class internalData_t = typename internalData<T, R, C>::type
	>
	class Matrix;


	template
	<	class T, int STRG = Storage::ColMajor
	,	class MatOperator = Matrix_impl<T, MatSize::DYNAMIC, MatSize::DYNAMIC, STRG>
	,	class internalData_t = typename internalData<T, MatSize::DYNAMIC, MatSize::DYNAMIC>::type
	>
	using Matrix_
	=	Matrix<T, MatSize::DYNAMIC, MatSize::DYNAMIC, STRG, MatOperator, internalData_t>;


	template<class T> struct _OperatorGuard;
	//========//========//========//========//=======#//========//========//========//========//===


	template<int R, int C>
	static bool constexpr is_DynamicMatSize_v = R == MatSize::DYNAMIC || C == MatSize::DYNAMIC;	


	SGM_HAS_MEMFUNC(rows);
	SGM_HAS_MEMFUNC(cols);

	template<class MAT>
	static bool constexpr Has_Matrix_interface_v
	=	(	Has_MemFunc_rows< std::decay_t<MAT> >::value 
		&&	Has_MemFunc_cols< std::decay_t<MAT> >::value
		);

	SGM_HAS_NESTED_TYPE(elem_t);
	SGM_HAS_NESTED_TYPE(impl_t);
	SGM_HAS_NESTED_TYPE(container_t);


	template<class MAT>
	static bool constexpr is_mxiMatrix_v
	=	(	Has_Matrix_interface_v<MAT>
		&&	Has_NestedType_elem_t<MAT>::value	
		&&	Has_NestedType_impl_t<MAT>::value
		&&	Has_NestedType_container_t<MAT>::value
		);


	template<class MAT> static decltype(auto) _impl_cast(MAT&& data);

	template<class T> static auto _makeOpGuard(T&& t)-> _OperatorGuard< std::decay_t<T> >;

	template<class M> static auto _RHS_cast(M&& m);
	//========//========//========//========//=======#//========//========//========//========//===

#if 0
	template<class T, int SIZE = MatSize::DYNAMIC, class MatrixOperator = void>
	using ColVec = Matrix<T, SIZE, 1, Storage::ColMajor, MatrixOperator>;

	template<class T, class MatrixOperator = void>
	using ColVec_ = Matrix<T, MatSize::DYNAMIC, 1, Storage::ColMajor, MatrixOperator>;

	template<class T, int SIZE = MatSize::DYNAMIC, class MatrixOperator = void>
	using RowVec = Matrix<T, 1, SIZE, Storage::RowMajor, MatrixOperator>;

	template<class T, class MatrixOperator = void>
	using RowVec_ = Matrix<T, 1, MatSize::DYNAMIC, Storage::RowMajor, MatrixOperator>;
#endif
	//========//========//========//========//=======#//========//========//========//========//===



	template<class T, int R, int C, int STRG, class MatOperator, class internalData_t>
	class Matrix
	{
	private:
		static_assert(std::is_scalar_v<T>, "Matrix element should be scalar type");


		SGM_HAS_MEMFUNC(data);

		SGM_HAS_MEMFUNC(resize);


	public:
		using elem_t = std::decay_t<T>;
		using impl_t = MatOperator;

		using container_t 
		=	Provided_t< Has_MemFunc_data<internalData_t>::value, internalData_t >;

		static int constexpr RSIZE = R, CSIZE = C, MAJOR = STRG;
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		Matrix() : _data(), _rsize(R), _csize(C){}


		template<  class Q, class = std::enable_if_t< std::is_scalar_v<Q> >  >
		Matrix(std::initializer_list<Q>&& iL) 
		:	_data
			(	[&iL]()-> decltype(_data)
				{
					static_assert
					(	!is_DynamicMatSize_v<R, C>
					,	"initializer_listing is not available when dynamic."
					);

					return 
					_init_by_iterable<STRG, elem_t>
					(	std::move(iL), R, C, container_t()
					);
				}()
			)
		,	_rsize(R), _csize(C)
		{}


		template<  class CON, class = std::enable_if_t< is_iterable<CON, elem_t>::value >  >
		Matrix(CON const& con, int r = R, int c = C)
		:	_data
			(	[&con, &r, &c]()-> decltype(_data)
				{
					assert
					(	( is_DynamicMatSize_v<R, C> || (r == R && c == C) )
					&&	L"size dismatched."
					);

					container_t res{};

					if constexpr(Has_MemFunc_resize<container_t, size_t>::value)
						assert(r > 0 || c > 0),
						res.resize(con.size()),
						(r < 0 ? r : c) = (int)res.size() / (r < 0 ? c : r);

					return _init_by_iterable<STRG, elem_t>( con, r, c, std::move(res) );
				}()
			)
		,	_rsize(r), _csize(c)
		{}


		template
		<	class MAT
		,	class
			=	std::enable_if_t
				<	!std::is_same_v< Matrix, std::decay_t<MAT> > && Has_Matrix_interface_v<MAT>
				>
		>
		Matrix(MAT&& m)
		:	_data
			(	[&m]()-> container_t
				{
					int const mr = (int)m.rows(), mc = (int)m.cols(), ms = mr*mc;

					assert
					(	( is_DynamicMatSize_v<R, C> || (R == mr && C == mc) ) 
					&&	L"size dismatched."
					);

					assert(mr > 0 && mc > 0 && L"invalid Matrix size");

					container_t res{};

					if constexpr(Has_MemFunc_resize<container_t, size_t>::value)
						res.resize(ms);

					if constexpr(std::is_function_v<MAT(int, int)>)
						for(int i = 0; i < mr; ++i)
							for(int j = 0; j < mc; ++j)
								res[index_conv<STRG>(mr, mc, i, j)] = m(i, j);
					else SGM_COMPILE_FAILED(no suitable method was found.);

					return res;
				}()
			)
		,	_rsize( (int)m.rows() ), _csize( (int)m.cols() )
		{}


		template<class MAT> 
		Matrix(_OperatorGuard<MAT>&& opg) : Matrix( opg.value() ){}


		Matrix(Matrix const&) = default;
		Matrix(Matrix&&) noexcept = default;
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto operator=(Matrix const&)-> Matrix& = default;
		auto operator=(Matrix&&) noexcept-> Matrix& = default;
		

		template
		<	class MAT
		,	class
			=	std::enable_if_t
				<	!std::is_same_v< Matrix, std::decay_t<MAT> > && Has_Matrix_interface_v<MAT>
				>
		>
		auto operator=(MAT&& m)-> Matrix&
		{
			if constexpr(is_DynamicMatSize_v<R, C>)
				resize( (int)m.rows(), (int)m.cols() );
			else
				assert(rows() == (size_t)m.rows() && cols() == (size_t)m.cols());

			if constexpr(std::is_function_v<MAT(int, int)>)
				for(int i = 0; i < rows(); ++i)
					for(int j = 0; j < cols(); ++j)
						(*this)(i, j) = m(i, j);
			else SGM_COMPILE_FAILED(no suitable method was found.);

			return *this;
		}


		template<class MAT>
		auto operator=(_OperatorGuard<MAT>&& opg)-> Matrix&{  return *this = opg.value();  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto rows() const-> size_t{  return _rsize;  }
		auto cols() const-> size_t{  return _csize;  }
		auto size() const-> size_t{  return _rsize*_csize;  }

		auto data() const-> elem_t const*	{  return _data.data();  }
		auto data()-> elem_t*				{  return _data.data();  }


		auto resize(int r, int c)-> Matrix_<elem_t, STRG>&
		{
			static_assert
			(	is_DynamicMatSize_v<R, C> && Has_MemFunc_resize<container_t, size_t>::value
			,	"cannot resize static-sized Matrix"
			);

			auto is_in
			=	[](int _r, int _c, int i, int j)-> bool
				{
					return 0 <= i && i < _r && 0 <= j && j < _c;
				};

			container_t res{};
			
			res.resize( size_t(r*c) );

			for(int i = 0; i < r; ++i)
				for(int j = 0; j < c; ++j)
					res[index_conv<STRG>(r, c, i, j)]
					=	is_in(_rsize, _csize, i, j)
						?	_data[index_conv<STRG>(_rsize, _csize, i, j)]
						:	elem_t(0);

			if constexpr(std::is_move_assignable_v<container_t>)
				_data = std::move(res);
			else
				_data = res;

			_rsize = r, _csize = c;

			return *this;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto operator()(int i, int j) const-> elem_t
		{  
			return _impl_cast(*this).element(i, j);
		}

		auto operator()(int i, int j)-> elem_t&{  return _impl_cast(*this).element(i, j);  }


		decltype(auto) block(int i, int j, int r, int c) const
		{  
			return _impl_cast(*this).block(i, j, r, c);
		}

		decltype(auto) block(int i, int j, int r, int c)
		{
			return _impl_cast(*this).block(i, j, r, c);
		}


		decltype(auto) row(int i) const	{  return block(i, 0, _csize, 1);  }
		decltype(auto) row(int i)		{  return block(i, 0, _csize, 1);  }

		decltype(auto) col(int j) const {  return block(0, j, 1, _rsize);  }
		decltype(auto) col(int j)		{  return block(0, j, 1, _rsize);  }


		template< class = std::enable_if_t<R == C> >
		decltype(auto) inverse() const
		{
			assert(_rsize == _csize && L"undefined inverse matrix");

			return _impl_cast(*this).inverse();
		}
		

		decltype(auto) transpose() const{  return _impl_cast(*this).transpose();  }	
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		decltype(auto) operator-() const
		{
			return _makeOpGuard( -_impl_cast(*this).core() );
		}


		template<  class MAT, class = std::enable_if_t< std::is_convertible_v<MAT, Matrix> >  >
		decltype(auto) operator+(MAT&& mat) const
		{
			return 
			_makeOpGuard(  _impl_cast(*this).core() + _RHS_cast( std::forward<MAT>(mat) )  );
		}

		template<  class MAT, class = std::enable_if_t< std::is_convertible_v<MAT, Matrix> >  >
		decltype(auto) operator-(MAT&& mat) const
		{
			return 
			_makeOpGuard(  _impl_cast(*this).core() - _RHS_cast( std::forward<MAT>(mat) )  );
		}

		template<  class MAT, class = std::enable_if_t< std::is_convertible_v<MAT, Matrix> >  >
		decltype(auto) operator*(MAT&& mat) const
		{
			return 
			_makeOpGuard(  _impl_cast(*this).core() * _RHS_cast( std::forward<MAT>(mat) )  );
		}

		template<  class S, class = std::enable_if_t< std::is_scalar_v<S> >  >
		decltype(auto) operator*(S s) const
		{
			return _makeOpGuard( _impl_cast(*this).core() * s );
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


	private:
		container_t _data;
		int _rsize, _csize;
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template<int _STRG>
		static auto index_conv(int r, int c, int i, int j)-> int
		{
			return _STRG == Storage::RowMajor ? i*c + j : i + j*r;
		}

		template<int _STRG>
		static auto index_conv(int r, int c, int idx)-> std::pair<int, int>
		{
			return 
			_STRG == Storage::RowMajor
			?	std::pair(idx/c, idx%c)
			:	std::pair(idx%r, idx/r);
		}


		template<int _STRG, class T, class res_t, class CON>
		static auto _init_by_iterable(CON&& con, int r, int c, res_t&& res)
		{
			for
			(	auto[itr, idx] = std::pair(con.begin(), 0)
			;	itr != con.end()
			;	++idx, ++itr
			)
				res[_STRG == Storage::RowMajor ? idx : idx%c*r + idx/c]
				=	static_cast<T>(*itr);
				
			return std::move(res);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

	};
	//========//========//========//========//=======#//========//========//========//========//===


	template
	<	class T, int R, int C, class S = T, class = std::enable_if_t< std::is_scalar_v<S> >
	>
	static decltype(auto) operator*(S s, Matrix<T, R, C> const& m){  return m*s;  }

	template<  class T, class S = T, class = std::enable_if_t< std::is_scalar_v<S> >  >
	static decltype(auto) operator*(S s, _OperatorGuard<T>&& m){  return std::move(m)*s;  }



	template<class T>
	struct _OperatorGuard
	{
		auto value() const-> T const&{  return _t;  }


		template<  class M, class = std::enable_if_t< Has_Matrix_interface_v<M> >  >
		decltype(auto) operator+(M&& mat) const
		{  
			return _makeOpGuard(  _t + _RHS_cast( std::forward<M>(mat) )  );
		}


		template<  class M, class = std::enable_if_t< Has_Matrix_interface_v<M> >  >
		decltype(auto) operator-(M&& mat) const
		{
			return _makeOpGuard(  _t - _RHS_cast( std::forward<M>(mat) )  );
		}


		template<  class M, class = std::enable_if_t< Has_Matrix_interface_v<M> >  >
		decltype(auto) operator*(M&& mat) const
		{
			return _makeOpGuard(  _t * _RHS_cast( std::forward<M>(mat) )  );
		}

		template<  class S, class = std::enable_if_t< std::is_scalar_v<S> >  >
		decltype(auto) operator*(S s) const
		{
			return _makeOpGuard(_t * s);
		}


	private:
		template<class T> 
		friend auto _makeOpGuard(T&& t)-> _OperatorGuard< std::decay_t<T> >;

		_OperatorGuard(T const& t) : _t(t){}

		T const _t;
	};


	template<class T> 
	static auto _makeOpGuard(T&& t)-> _OperatorGuard< std::decay_t<T> >
	{
		return 
		_OperatorGuard< std::decay_t<T> >( std::forward<T>(t) );
	}
	//========//========//========//========//=======#//========//========//========//========//===


	template<class MAT> 
	static decltype(auto) _impl_cast(MAT&& mat)
	{
		static_assert(Has_NestedType_impl_t<MAT>::value, "implementation class was not found.");

		return std::remove_reference_t<MAT>::impl_t::calc(mat.data(), mat.rows(), mat.cols());
	}


	template<class M> static auto _RHS_cast(M&& m)
	{
		static_assert(Has_Matrix_interface_v<M>, "rhs does not have Matrix interface.");

		if constexpr(is_mxiMatrix_v<M>)
			return _impl_cast( std::forward<M>(m) ).core();
		else
			return std::forward<M>(m);
	}


} // end of namespace sgm::mxi

#endif // end of #ifndef _SGM_MATRIX_INTERFACE_

