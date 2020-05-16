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
	
	
	struct MxSize : No_Making 
	{ 
		using type = size_t;

		static auto constexpr DYNAMIC = static_cast<type>(-1);
		static auto constexpr TEMPORARY = DYNAMIC - 1;
		
		template<type N> static bool constexpr is_dynamic_v = N == DYNAMIC;
		template<type N> static bool constexpr is_static_v = !is_dynamic_v<N>;
		template<type N> static bool constexpr is_temporary_v = N == TEMPORARY;
	};

	using MxSize_t = typename MxSize::type;
	using index_t = MxSize_t;
	//========//========//========//========//=======#//========//========//========//========//===


	template<class elem_t, MxSize_t ROW_SIZE, MxSize_t COLUMN_SIZE>
	class Mx_implementation;	// should be defined later


	template
	<	class elem_t = float
	,	MxSize_t ROW_SIZE = MxSize::DYNAMIC, MxSize_t COLUMN_SIZE = MxSize::DYNAMIC
	>
	class Matrix;
	//========//========//========//========//=======#//========//========//========//========//===


	struct MxTraits : No_Making
	{
	private:
		template<class> struct is_Dynamic{  SGM_COMPILE_FAILED(is not a Matrix); };

		template<class T, MxSize_t R, MxSize_t C>
		struct is_Dynamic< Matrix<T, R, C> > : No_Making
		{
			static bool constexpr value = MxSize::is_dynamic_v<R> || MxSize::is_dynamic_v<C>;
		};


	public:
		template<class M>
		static bool constexpr is_DynamicMx_v = is_Dynamic< std::decay_t<M> >::value;

		template<class M> static bool constexpr is_StaticMx_v = is_DynamicMx_v<M>;
	};

	SGM_HAS_NESTED_TYPE(elem_t);
	SGM_HAS_MEMBER(ROW_SIZE);
	SGM_HAS_MEMBER(COL_SIZE);

	template<class M>
	static bool constexpr is_mxiMatrix_v
	=	(	Has_NestedType_elem_t<M>::value
		&&	Has_Member_ROW_SIZE<M>::value
		&&	Has_Member_COL_SIZE<M>::value
		);
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T>
	static auto _temporary(T&& t)
	{
		return Matrix<T, MxSize::TEMPORARY, MxSize::TEMPORARY>( std::move(t) );
	}
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T, MxSize_t R, MxSize_t C>
	class Matrix : Mx_implementation<T, R, C>
	{
		using impl_t = Mx_implementation<T, R, C>;


	public:
		using elem_t = typename impl_t::elem_t;
		static MxSize_t constexpr ROW_SIZE = R, COL_SIZE = C;


		Matrix() : impl_t(){}


		template
		<	class Q
		,	class 
			=	std::enable_if_t
				<	std::is_scalar_v<Q> && (MxSize::is_static_v<R> || MxSize::is_static_v<C>)
				>
		>
		Matrix(std::initializer_list<Q>&& iL) : impl_t( std::move(iL), R, C ){}


		template
		<	class CON
		,	class
			=	std::enable_if_t
				<	is_iterable<CON, elem_t>::value && MxTraits::is_DynamicMx_v<Matrix>
				>
		>
		Matrix(CON&& con, MxSize_t r = R, MxSize_t c = C) 
		:	impl_t( std::forward<CON>(con), r, c )
		{
			assert
			(	(MxSize::is_dynamic_v<R> || R == r) && (MxSize::is_dynamic_v<C> || C == c)
			&&	L"static size cannot be modified.\n"
			);
		}


		template<class Q>
		Matrix(Q&& q) : impl_t( std::forward<Q>(q) ){}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template<  class Q, class = std::enable_if_t< is_mxiMatrix_v<Q> >  >
		auto operator=(Q&& q)-> Matrix&
		{
			impl_t::core() = q.core();

			return *this;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		decltype(auto) operator()(index_t i, index_t j){  return impl_t::element(i, j);  }
		auto operator()(index_t i, index_t j) const-> elem_t{  return impl_t::element(i, j);  }

		auto rows() const-> MxSize_t{  return impl_t::rows();  }
		auto cols() const-> MxSize_t{  return impl_t::cols();  }
		auto size() const-> MxSize_t{  return impl_t::size();  }

		decltype(auto) core() const	{  return impl_t::core();  }


		auto resize(MxSize_t r, MxSize_t c)-> Matrix&
		{
			static_assert
			(	MxTraits::is_DynamicMx_v<Matrix>, "static size Matrix cannot be resized."
			);

			assert
			(	(MxSize::is_dynamic_v<R> || R == r) && (MxSize::is_dynamic_v<C> || C == c)
			&&	L"static size cannot be modified.\n"
			);

			impl_t::resize(r, c);

			return *this;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto block(index_t i, index_t j, MxSize_t r, MxSize_t c) const
		{
			return _temporary( impl_t::block(i, j, r, c) );
		}

		auto block(index_t i, index_t j, MxSize_t r, MxSize_t c)
		{
			return _temporary( impl_t::block(i, j, r, c) );
		}

		auto row(index_t idx) const	{  return _temporary( impl_t::row(idx) );  }
		auto row(index_t idx)		{  return _temporary( impl_t::row(idx) );  }
		auto col(index_t idx) const	{  return _temporary( impl_t::col(idx) );  }
		auto col(index_t idx)		{  return _temporary( impl_t::col(idx) );  }

		auto transposed() const{  return _temporary(impl_t::transposed());  }
		auto inversed() const{  return _temporary(impl_t::inversed());  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		auto operator-() const{  return _temporary(-impl_t::core());  }

		template<  class Q, class = std::enable_if_t< is_mxiMatrix_v<Q> >  >
		auto operator+(Q&& q) const{  return _temporary(impl_t::core() + q.core());  }

		template<  class Q, class = std::enable_if_t< is_mxiMatrix_v<Q> >  >
		auto operator-(Q&& q) const{  return _temporary(impl_t::core() - q.core());  }

		template<  class Q, class = std::enable_if_t< is_mxiMatrix_v<Q> >  >
		auto operator*(Q&& q) const{  return _temporary(impl_t::core() * q.core());  }

		template<  class S, class = std::enable_if_t< std::is_scalar_v<S> >  >
		auto operator*(S s) const{  return _temporary(impl_t::core() * s);  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


	};// end of Matrix class


	template
	<	class S, class T, MxSize_t R, MxSize_t C
	,	class = std::enable_if_t< std::is_scalar_v<S> >
	>
	auto operator*(S s, Matrix<T, R, C> const& m){  return m * s;  }
	//========//========//========//========//=======#//========//========//========//========//===



} // end of namespace sgm::mxi


#if 0
namespace sgm::mxi
{


	struct MatSize : No_Making{  static int constexpr DYNAMIC = -1;  };
	struct Storage : No_Making{  static int constexpr ColMajor = 0, RowMajor = 1;  };


	template
	<	class T, int R = MatSize::DYNAMIC, int C = MatSize::DYNAMIC
	,	int STRG = Storage::ColMajor
	> 
	struct Matrix_impl;							// should be defined later.


	template<class, int> struct Vector_impl;			// should be defined later.


	template<class, int, int> struct internalData;	// should be defined later.


	template
	<	class T, int R = MatSize::DYNAMIC, int C = MatSize::DYNAMIC
	,	int STRG = Storage::ColMajor
	,	class MatOperator = Matrix_impl<T, R, C, STRG>
	,	class internalData_t = typename internalData<T, R, C>::type
	>
	class Matrix;


	template
	<	class T, int SIZE = MatSize::DYNAMIC
	,	class VectorOperator = Vector_impl<T, SIZE>
	,	class internalData_t = typename internalData<T, SIZE, 1>::type
	>
	class Vector;


	template<class T> struct _OperatorGuard;


	template
	<	class T, int STRG = Storage::ColMajor
	,	class MatOperator = Matrix_impl<T, MatSize::DYNAMIC, MatSize::DYNAMIC, STRG>
	,	class internalData_t = typename internalData<T, MatSize::DYNAMIC, MatSize::DYNAMIC>::type
	>
	using Matrix_
	=	Matrix<T, MatSize::DYNAMIC, MatSize::DYNAMIC, STRG, MatOperator, internalData_t>;
	//========//========//========//========//=======#//========//========//========//========//===


	struct Matrix_Traits : No_Making
	{
	private:
		template<class> 
		struct is_Dynamic : No_Making{  SGM_COMPILE_FAILED(is not a Matrix.);  };

		template<class T, int R, int C, int STRG>
		struct is_Dynamic< Matrix<T, R, C, STRG> > : No_Making
		{
			static bool constexpr value = R == MatSize::DYNAMIC || C == MatSize::DYNAMIC;
		};


		template<class> struct Major : No_Making{  SGM_COMPILE_FAILED(is not a Matrix.);  };

		template<class T, int R, int C, int STRG>
		struct Major< Matrix<T, R, C, STRG> > : No_Making
		{  
			static int constexpr value = STRG;
		};


		template<class> struct is_Temporary_data : std::true_type{};

		template<class T, int R, int C>
		struct is_Temporary_data< internalData<T, R, C> > : std::false_type{};


		template<class> struct is_Temporary;

		template<class T, int R, int C, int STRG, class MOP, class DATA>
		struct is_Temporary< Matrix<T, R, C, STRG, MOP, DATA> >
		:	is_Temporary_data< std::decay_t<DATA> >
		{};

		template<class T, int SIZE, class VOP, class DATA>
		struct is_Temporary< Vector<T, SIZE, VOP, DATA> >
		:	is_Temporary_data< std::decay_t<DATA> >
		{};


	public:
		template<class M> static bool constexpr is_Dynamic_v 
		=	is_Dynamic< std::decay_t<M> >::value;
		
		template<class M> 
		static bool constexpr is_ColMajor_v 
		=	Major< std::decay_t<M> >::value == Storage::ColMajor;
		
		template<class M> static bool constexpr is_RowMajor_v 
		=	Major< std::decay_t<M> >::value == Storage::RowMajor;

		template<class DATA> static bool constexpr is_Temporary_data_v
		=	is_Temporary_data< std::decay_t<DATA> >::value;

		template<class M> static bool constexpr is_Temporary_v 
		=	is_Temporary< std::decay_t<M> >::value;
	};
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


	SGM_HAS_MEMFUNC(size);

	template<class VEC>
	static bool constexpr Has_Vector_interface_v
	=	(	Has_MemFunc_size<VEC>::value
		&&	std::is_function_v<VEC(int)>
		);

	template<class VEC>
	static bool constexpr is_mxiVector_v
	=	(	Has_Vector_interface_v<VEC>
		&&	Has_NestedType_elem_t<VEC>::value	
		&&	Has_NestedType_impl_t<VEC>::value
		&&	Has_NestedType_container_t<VEC>::value
		);


	SGM_HAS_MEMFUNC(data);
	SGM_HAS_MEMFUNC(resize);


	template<class> struct is_OpGuard : std::false_type{};
	template<class T> struct is_OpGuard< _OperatorGuard<T> > : std::true_type{};

	template<class T>
	static bool constexpr is_OpGuard_v = is_OpGuard< std::decay_t<T> >::value;


	template<class Q> static decltype(auto) _impl_cast(Q&&);

	template<class Q> static auto _RHS_cast(Q&&);

	template<class T> 
	static auto _OpGuard(T&& t)-> _OperatorGuard< std::remove_reference_t<T> >;
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T, int R, int C, int STRG, class MatOperator, class internalData_t>
	class Matrix
	{
		static_assert(std::is_scalar_v<T>, "Matrix element should be scalar type");

	public:
		using elem_t = std::decay_t<T>;
		using impl_t = MatOperator;

		using container_t 
		=	Provided_t< Has_MemFunc_data<internalData_t>::value, internalData_t >;

		//static int constexpr RSIZE = R, CSIZE = C;
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		Matrix() : _data(), _rsize(R), _csize(C){}


		template<  class Q, class = std::enable_if_t< std::is_scalar_v<Q> >  >
		Matrix(std::initializer_list<Q>&& iL) 
		:	_data
			(	[&iL]()-> container_t
				{
					static_assert
					(	!Matrix_Traits::is_Dynamic_v<Matrix>
					,	"initializer_listing is not available when dynamic."
					);

					return 
					_init_by_iterable<STRG, elem_t>( std::move(iL), R, C, container_t() );
				}()
			)
		,	_rsize(R), _csize(C)
		{}


		template<  class CON, class = std::enable_if_t< is_iterable<CON, elem_t>::value >  >
		Matrix(CON const& con, int r = R, int c = C)
		:	_data
			(	[&con, &r, &c]()-> container_t
				{
					assert
					(	( Matrix_Traits::is_Dynamic_v<Matrix> || (r == R && c == C) )
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
					(	( Matrix_Traits::is_Dynamic_v<Matrix> || (R == mr && C == mc) ) 
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
		Matrix(_OperatorGuard<MAT>&& opg) : Matrix(opg.value()){}


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
			if constexpr(Matrix_Traits::is_Dynamic_v<Matrix>)
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


		auto rows() const-> int{  return _rsize;  }
		auto cols() const-> int{  return _csize;  }
		auto size() const-> int{  return _rsize*_csize;  }

		auto data() const-> elem_t const*	{  return _data.data();  }
		auto data()-> elem_t*				{  return _data.data();  }


		auto resize(int r, int c)-> Matrix_<elem_t, STRG>&
		{
			static_assert
			(	Matrix_Traits::is_Dynamic_v<Matrix> && Has_MemFunc_resize<container_t, size_t>::value
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

		auto operator()(int) = delete;


		decltype(auto) block(int i, int j, int r, int c) const
		{  
			return _OpGuard( _impl_cast(*this).block(i, j, r, c) );
		}

		decltype(auto) block(int i, int j, int r, int c)
		{
			return _OpGuard( _impl_cast(*this).block(i, j, r, c) );
		}


		decltype(auto) row(int i) const	{  return block(i, 0, 1, _csize);  }
		decltype(auto) row(int i)		{  return block(i, 0, 1, _csize);  }

		decltype(auto) col(int j) const {  return block(0, j, _rsize, 1);  }
		decltype(auto) col(int j)		{  return block(0, j, _rsize, 1);  }


		template< class = std::enable_if_t<R == C> >
		decltype(auto) inverse() const
		{
			assert(_rsize == _csize && L"undefined inverse matrix");

			return _OpGuard( _impl_cast(*this).inverse() );
		}
		

		decltype(auto) transpose() const
		{  
			return _OpGuard( _impl_cast(*this).transpose() );  
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		decltype(auto) operator-() const{  return _OpGuard( -_impl_cast(*this)() );  }


		template<  class MAT, class = std::enable_if_t< std::is_convertible_v<MAT, Matrix> >  >
		decltype(auto) operator+(MAT&& mat) const
		{
			return _OpGuard(  _impl_cast(*this)() + _RHS_cast( std::forward<MAT>(mat) )  );
		}

		template<  class MAT, class = std::enable_if_t< std::is_convertible_v<MAT, Matrix> >  >
		decltype(auto) operator-(MAT&& mat) const
		{
			return _OpGuard(  _impl_cast(*this)() - _RHS_cast( std::forward<MAT>(mat) )  );
		}

		template<  class MAT, class = std::enable_if_t< std::is_convertible_v<MAT, Matrix> >  >
		decltype(auto) operator*(MAT&& mat) const
		{
			return _OpGuard(  _impl_cast(*this)() * _RHS_cast( std::forward<MAT>(mat) )  );
		}

		template<  class S, class = std::enable_if_t< std::is_scalar_v<S> >  >
		decltype(auto) operator*(S s) const
		{
			return _OpGuard( _impl_cast(*this)() * s );
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
		static decltype(auto) _init_by_iterable(CON&& con, int r, int c, res_t&& res)
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
	

	};//	end of class Matrix
	//========//========//========//========//=======#//========//========//========//========//===


	template
	<	class T, int R, int C, class S = T, class = std::enable_if_t< std::is_scalar_v<S> >
	>
	static decltype(auto) operator*(S s, Matrix<T, R, C> const& m){  return m*s;  }

	template
	<	class T, class S, class = std::enable_if_t< std::is_scalar_v<S> >  
	>
	static decltype(auto) operator*(S s, _OperatorGuard<T>&& t){  return std::move(t)*s;  }



	template<class T>
	struct _OperatorGuard
	{
		auto value()-> T&{  return _t;  }


		template
		<	class Q
		,	class = std::enable_if_t< Has_Matrix_interface_v<Q> || Has_Vector_interface_v<Q> >
		>
		decltype(auto) operator=(Q&& q)
		{
			static_assert(!is_immutable<T>::value, "cannot write over immutable value.");

			_t = _RHS_cast( std::forward<Q>(q) );

			return *this;
		}


		template
		<	class Q
		,	class = std::enable_if_t< Has_Matrix_interface_v<Q> || Has_Vector_interface_v<Q> >
		>
		decltype(auto) operator+(Q&& q) const
		{  
			return _OpGuard(  _t + _RHS_cast( std::forward<Q>(q) )  );
		}


		template
		<	class Q
		,	class = std::enable_if_t< Has_Matrix_interface_v<Q> || Has_Vector_interface_v<Q> >
		>
		decltype(auto) operator-(Q&& q) const
		{  
			return _OpGuard(  _t - _RHS_cast( std::forward<Q>(q) )  );
		}


		template
		<	class Q
		,	class = std::enable_if_t< Has_Matrix_interface_v<Q> || Has_Vector_interface_v<Q> >
		>
		decltype(auto) operator*(Q&& q) const
		{  
			return _OpGuard(  _t * _RHS_cast( std::forward<Q>(q) )  );
		}

		template<  class S, class = std::enable_if_t< std::is_scalar_v<S> >  >
		decltype(auto) operator*(S s) const
		{
			return _OpGuard(_t * s);
		}


	private:
		template<class T> 
		friend auto _OpGuard(T&& t)-> _OperatorGuard< std::remove_reference_t<T> >;

		_OperatorGuard(T& t) : _t(t){}
		_OperatorGuard(T&& t) : _t( std::move(t) ){}

		T _t;	// should be fixed to use perface forwarding
	};


	template<class T> 
	static auto _OpGuard(T&& t)-> _OperatorGuard< std::remove_reference_t<T> >
	{
		return _OperatorGuard< std::remove_reference_t<T> >( std::forward<T>(t) );
	}
	//========//========//========//========//=======#//========//========//========//========//===


	template<class Q> 
	static decltype(auto) _impl_cast(Q&& q)
	{
		if constexpr(is_mxiMatrix_v<Q>)
			return std::remove_reference_t<Q>::impl_t::calc(q.data(), q.rows(), q.cols());
		else if constexpr(is_mxiVector_v<Q>)
			return std::remove_reference_t<Q>::impl_t::calc(q.data(), q.size());
		else SGM_COMPILE_FAILED(no implementation available.);
	}


	template<class Q> 
	static auto _RHS_cast(Q&& q)	// should be re-considered for perfact forwarding
	{
		if constexpr(is_mxiMatrix_v<Q> || is_mxiVector_v<Q>)
			return _impl_cast( std::forward<Q>(q) )();
		else if constexpr(is_OpGuard_v<Q>)
			return q.value();
		else
			return std::forward<Q>(q);
	}
	//========//========//========//========//=======#//========//========//========//========//===



	template<class T, int SIZE,	 class VectorOperator, class internalData_t>
	class Vector
	{
		static_assert(std::is_scalar_v<T>, "Matrix element should be scalar type");

	public:
		using elem_t = std::decay_t<T>;
		using impl_t = VectorOperator;

		using container_t 
		=	Provided_t< Has_MemFunc_data<internalData_t>::value, internalData_t >;
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		Vector() : _data(){}


		Vector(elem_t const* pdata, [[maybe_unused]]int size = SIZE)
		:	_data
			(	[pdata, size]()-> container_t
				{
					return 
					_init_by_pointer
					(	pdata, size, _resized_data<container_t>( (size_t)size ) 
					);
				}()
			)
		{}


		template
		<	class CON
		,	class
			=	std::enable_if_t
				<	!std::is_same_v< std::decay_t<CON>, Vector >
				&&	(is_iterable<CON, elem_t>::value || Has_Vector_interface_v<CON>)
				>
		>
		Vector(CON&& con)
		:	_data
			(	[&con]()-> container_t
				{
					auto res = _resized_data<container_t>(con.size());

					return _copied_data( std::forward<CON>(con), std::move(res) );
				}()
			)
		{}


		template<  class Q, class = std::enable_if_t< std::is_scalar_v<Q> >  >
		Vector(std::initializer_list<Q>&& iL)
		:	_data
			(	[&iL]()-> container_t
				{
					auto res = _resized_data<container_t>(iL.size());

					return _copied_data( std::move(iL), std::move(res) );
				}()
			)
		{}


		template<class VEC> 
		Vector(_OperatorGuard<VEC>&& opg) : Vector(opg.value()){}


		Vector(Vector const&) = default;
		Vector(Vector&&) noexcept = default;
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------
		

		auto operator=(Vector const&)-> Vector& = default;
		auto operator=(Vector&&) noexcept-> Vector& = default;


		template
		<	class VEC
		,	class
			=	std::enable_if_t
				<	!std::is_same_v< std::decay_t<VEC>, Vector > && Has_Vector_interface_v<VEC>
				>
		>
		auto operator=(VEC&& v)-> Vector&
		{
			auto res = _resized_data<container_t>(v.size());

			_data = _copied_data( std::forward<VEC>(v), std::move(res) );

			return *this;
		}


		template<class VEC>
		auto operator=(_OperatorGuard<VEC>&& opg)-> Vector&{  return *this = opg.value();  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto size() const-> int			
		{  
			return 
			SIZE == MatSize::DYNAMIC && _data.size() != 0
			?	(int)_data.size()
			:	SIZE;
		}


		auto data() const-> elem_t const*	{  return _data.data();  }
		auto data()-> elem_t*				{  return _data.data();  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto operator()(int d) const-> elem_t	{  return _impl_cast(*this).element(d);  }
		auto operator()(int d)-> elem_t&			{  return _impl_cast(*this).element(d);  }

		decltype(auto) head(int n) const		{  return _impl_cast(*this).head(n);  }
		decltype(auto) head(int n)			{  return _impl_cast(*this).head(n);  }

		decltype(auto) tail(int n) const		{  return _impl_cast(*this).tail(n);  }
		decltype(auto) tail(int n)			{  return _impl_cast(*this).tail(n);  }

		decltype(auto) norm() const		{  return _impl_cast(*this).norm();  }
		decltype(auto) norm_sqr() const	{  return _impl_cast(*this).norm_sqr();  }


		template<class VEC>
		decltype(auto) dot(VEC&& v) const
		{  
			assert(v.size() == size() && L"size dismatched");

			return _impl_cast(*this).dot(  _RHS_cast( std::forward<VEC>(v) )  );
		}


		template<class VEC>
		decltype(auto) cross(VEC&& v) const
		{
			assert(v.size() == 3 && size() == 3 && L"cross product is defined in R^3");

			return _impl_cast(*this).cross(  _RHS_cast( std::forward<VEC>(v) )  );
		}

		decltype(auto) rowvec() const{  return _impl_cast(*this).rowvec();  }
		decltype(auto) colvec() const{  return _impl_cast(*this).colvec();  }


	private:
		container_t _data;
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template<class CON>
		static auto _resized_data(size_t n)-> CON
		{
			CON res{};

			if constexpr (Has_MemFunc_resize<CON, size_t>::value)
				assert(n > 0 && L"invalid size."),
				res.resize(n);
			else
				assert(n == SIZE && L"invalid size.");

			return res;
		}


		template<class res_t, class CON>
		static decltype(auto) _copied_data(CON&& con, res_t&& res)
		{
			if constexpr(is_iterable<CON>::value)
				for
				(	auto[itr, res_itr] = std::pair(con.begin(), res.begin())
				;	itr != con.end()
				;	*res_itr++ = static_cast<elem_t>(*itr++)
				);
			else if constexpr(Has_Vector_interface_v<CON>)
				for(int idx = 0; idx < res.size(); ++idx)
					res[idx] = static_cast<elem_t>( con(idx) );
				
			return std::move(res);
		}


		template<class P, class res_t>
		static decltype(auto) _init_by_pointer(P p, int s, res_t&& res)
		{
			for(auto itr = res.begin(); s-->0; *itr++ = *p++);

			return std::move(res);
		}

	};//	end of class Vector


} // end of namespace sgm::mxi
#endif



#endif // end of #ifndef _SGM_MATRIX_INTERFACE_

#if 0

#endif