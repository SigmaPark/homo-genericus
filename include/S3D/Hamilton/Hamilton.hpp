/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _S3D_HAMILTON_
#define _S3D_HAMILTON_


#include "SGM/High_Templar/Countable.hpp"
#include "SGM/High_Templar/High_Templar.hpp"
#include <type_traits>
#include <climits>
#include <complex>
#include <cmath>


//	 C++17 or higher version of language support is required.


namespace s3d
{

	using namespace sgm;

	using std::size_t;
	
	enum class Storing_Order{COL_FIRST, ROW_FIRST};
	static auto constexpr DefaultStorOrder = Storing_Order::COL_FIRST;


	enum class FixedSize{} constexpr FIXED_SIZE{};

	size_t constexpr 
		DYNAMIC = std::numeric_limits<size_t>::max(),  
		UNKNOWN = DYNAMIC - 1;


	template<class T, bool SIGNALING> 
	static T constexpr _NaN_Helper() noexcept;
	
	template<class T, bool SIGNALING = false>  
	static T constexpr NaN = _NaN_Helper<T, SIGNALING>();

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace s3d::trait
{
	
	template<class T>  
	struct is_real;

	template<  class T, bool = is_Same< T, Decay_t<T> >::value  >  
	struct is_complex;

	template<class T>  
	struct is_complexible;


	template<class MAT>  
	using value_t
	=	Guaranteed_t< Has_NestedType_value_type<MAT>::value, typename MAT::value_type >;


	template<class CON>
	using Deref_t = decltype( *Begin(Declval<CON>()) );


	template<class MAT>  
	struct Has_Matrix_interface;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace s3d
{

	template
	<	class T, size_t ROWS = DYNAMIC, size_t COLS = DYNAMIC
	,	Storing_Order STOR = DefaultStorOrder
	>
	class Matrix;


	template<class T, size_t ROWS, size_t COLS, Storing_Order STOR>
	struct _MatrixAdaptor;


	template<class T, Storing_Order STOR = DefaultStorOrder>
	using DynamicMat = Matrix<T, DYNAMIC, DYNAMIC, STOR>;


	struct NullMat_t;


	template<class T, size_t SIZE = DYNAMIC>
	using ColVec = Matrix<T, SIZE, 1>;

	template<class T, size_t SIZE = DYNAMIC>
	using RowVec = Matrix<T, 1, SIZE>;

	template<class T, size_t SIZE = DYNAMIC>
	using Vector 
	=	Selective_t
		<	DefaultStorOrder == Storing_Order::COL_FIRST, ColVec<T, SIZE>, RowVec<T, SIZE> 
		>;


	template<class T, size_t SIZE = DYNAMIC>
	class UnitVec;


	template<class T, size_t SIZE = DYNAMIC, Storing_Order STOR = DefaultStorOrder>
	class OrthogonalMat;


	template<class MAT>
	static decltype(auto) Eval(MAT&&) noexcept(is_Rvalue_Reference<MAT&&>::value);


	template<class T, int DP = 3>  
	static auto Are_almost_same(T t1, T t2)-> bool;


	template<class MAT>  
	static auto is_valid(MAT const&) noexcept-> bool;


	template<class MAT>  
	static auto Has_Vector_interface(MAT const&) noexcept-> bool;


	template<class MAT>  
	static auto is_Square_Matrix(MAT const&) noexcept-> bool;


	template
	<	class RES, class...ARGS
	,	class = Enable_if_t< Has_Operator_New<RES, ARGS&&...>::value >  
	>
	static auto Skipped(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value);


	template<  class MAT, class = Enable_if_t< trait::Has_Matrix_interface<MAT>::value >  >
	static auto as_col_space(MAT& mat);

	template<  class MAT, class = Enable_if_t< trait::Has_Matrix_interface<MAT>::value >  >
	static auto as_row_space(MAT& mat);

	template<  class MAT, class = Enable_if_t< trait::Has_Matrix_interface<MAT>::value >  >
	static auto as_icol_space(MAT& mat);

	template<  class MAT, class = Enable_if_t< trait::Has_Matrix_interface<MAT>::value >  >
	static auto as_irow_space(MAT& mat);


	class _iterable_conversion_Helper;

	template<class T, size_t SIZE>  
	class _iterable_to_Vector;

	template<class T, size_t ROWS, size_t COLS, Storing_Order STOR>  
	class _iterable_to_Matrix;


	template
	<	class CON, class MAT
	,	class 
		=	Enable_if_t
			<	is_iterable<CON>::value
			&&	is_Convertible< trait::Deref_t<CON>, trait::value_t<MAT> >::value
			>
	>
	static void _MatCopy(CON&& container, MAT& target_matrix) 
	noexcept(is_Rvalue_Reference<CON&&>::value);


	template<size_t BASE, size_t POWER>  
	static auto constexpr _CX_POW() noexcept-> size_t;


	template<class MAT>  
	static decltype(auto) _Mat_implementor(MAT&& m);


	struct _Eval_Helper;

	enum class _ExemptionTag{};

	template<class MAT, int DIREC>  
	struct _VecSpace_Helper;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace s3d::trait
{
	
	SGM_USER_DEFINED_TYPE_CHECK
	(	SGM_MACROPACK(class T, size_t ROWS, size_t COLS, Storing_Order STOR)
	,	Matrix, <T, ROWS, COLS, STOR>
	);


	template<size_t SIZE>
	struct is_DynamicSize;

	template<size_t SIZE>
	struct is_UnknownSize;

	template<size_t SIZE>
	struct is_StaticSize;


	template<class MAT>
	struct is_FixedSizeMat;

	template<class MAT>
	struct is_DynamicSizeMat;

	template<class MAT>
	struct is_StrictMat;

	template<class MAT>
	struct is_StrictVec;


	SGM_USER_DEFINED_TYPE_CHECK
	(	SGM_MACROPACK(class T, size_t SIZE)
	,	UnitVec, <T, SIZE> 
	);
	

	SGM_USER_DEFINED_TYPE_CHECK
	(	SGM_MACROPACK(class T, size_t SIZE, Storing_Order STOR)
	,	OrthogonalMat, <T, SIZE, STOR>
	);

}


template
<	class T, class U, std::size_t ROWS, std::size_t COLS, s3d::Storing_Order STOR
,	class = sgm::Enable_if_t< s3d::trait::is_complexible<T>::value >
>
static decltype(auto) operator*(T t, s3d::Matrix<U, ROWS, COLS, STOR> const& m);
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>  
struct s3d::trait::is_real : is_Convertible< Decay_t<T>, long double >{};


template<class T>  
struct s3d::trait::is_complex<T, false> : is_complex< Decay_t<T> >{};

template<class T>  
struct s3d::trait::is_complex<T, true> : False_t{};

template<class T>  
struct s3d::trait::is_complex< std::complex<T>, true > : is_real<T>{};


template<class T>  
struct s3d::trait::is_complexible : Boolean_Or< is_complex<T>, is_real<T> >{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class MAT>
struct s3d::trait::Has_Matrix_interface 
{
private:
	SGM_HAS_MEMFUNC(rows);
	SGM_HAS_MEMFUNC(cols);

	template<class Q>
	static bool constexpr _calc()
	{
		if constexpr
		(	Has_Operator_invocation<Q, size_t, size_t>::value
		&&	Has_MemFunc_rows<Q>::value && Has_MemFunc_cols<Q>::value
		)
			return 
			is_complexible<  Decay_t< invocation_Result_t<Q, size_t, size_t> >  >::value;
		else
			return false;
	}


public:
	static bool constexpr value = _calc<MAT>();

	using type = Boolean<value>;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<std::size_t SIZE>
struct s3d::trait::is_DynamicSize : Boolean<SIZE == DYNAMIC>{};


template<std::size_t SIZE>
struct s3d::trait::is_UnknownSize : Boolean<SIZE == UNKNOWN>{};


template<std::size_t SIZE>
struct s3d::trait::is_StaticSize
:	Boolean< !is_DynamicSize<SIZE>::value && !is_UnknownSize<SIZE>::value >{};


template<class MAT>
struct s3d::trait::is_FixedSizeMat : Unconstructible
{
private:
	template<class _M>
	static bool constexpr _calc()
	{
		if constexpr(Has_Matrix_interface<_M>::value)
			return 
			is_StaticSize<_M::STT_ROW_SIZE>::value && is_StaticSize<_M::STT_COL_SIZE>::value;
		else
			return false;
	}


public:
	static bool constexpr value = _calc< Decay_t<MAT> >();

	using type = Boolean<value>;
};


template<class MAT>
struct s3d::trait::is_DynamicSizeMat : Unconstructible
{
private:
	template<class _M>
	static bool constexpr _calc()
	{
		if constexpr(Has_Matrix_interface<_M>::value)
			return 
			is_DynamicSize<_M::STT_ROW_SIZE>::value && is_DynamicSize<_M::STT_COL_SIZE>::value;
		else
			return false;	
	}


public:
	static bool constexpr value = _calc< Decay_t<MAT> >();

	using type = Boolean<value>;
};


template<class MAT>
struct s3d::trait::is_StrictMat : Unconstructible
{
private:
	template<class _M>
	static bool constexpr _calc()
	{
		if constexpr(Has_Matrix_interface<_M>::value)
			return _M::STT_ROW_SIZE >= 2 && _M::STT_COL_SIZE >= 2;
		else
			return false;
	}


public:
	static bool constexpr value = _calc< Decay_t<MAT> >();

	using type = Boolean<value>;	
};


template<class MAT>
struct s3d::trait::is_StrictVec : Unconstructible
{
private:
	template<class _M>
	static bool constexpr _calc()
	{
		if constexpr(Has_Matrix_interface<_M>::value)
			return _M::STT_ROW_SIZE == 1 || _M::STT_COL_SIZE == 1;
		else
			return false;
	}


public:
	static bool constexpr value = _calc< Decay_t<MAT> >();

	using type = Boolean<value>;	
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct s3d::_Eval_Helper : Unconstructible
{
private:
	template<class MAT>
	friend decltype(auto) s3d::Eval(MAT&& mat) noexcept(sgm::is_Rvalue_Reference<MAT&&>::value);


	template< class T, class MAT, class _M = Decay_t<MAT> >
	using _eval_t
	=	Selective_t
		<	is_Same<T, typename _M::value_type>::value
		,	MAT
		,	Matrix
			<	typename _M::value_type
			,	_M::STT_ROW_SIZE
			,	_M::STT_COL_SIZE
			,	_M::STORING_ORDER
			>
		>;


	template
	<	template<class, size_t, size_t, Storing_Order> class TM
	,	class T, size_t _R, size_t _C, Storing_Order _S
	>
	static auto _Eval(TM<T, _R, _C, _S>& lazy)
	->	_eval_t<T, decltype(lazy)>{  return lazy;  }
	
	
	template
	<	template<class, size_t, size_t, Storing_Order> class TM
	,	class T, size_t _R, size_t _C, Storing_Order _S
	>
	static auto _Eval(TM<T, _R, _C, _S> const& lazy)
	->	_eval_t<T, decltype(lazy)>{  return lazy;  }
	
	
	template
	<	template<class, size_t, size_t, Storing_Order> class TM
	,	class T, size_t _R, size_t _C, Storing_Order _S
	>
	static auto _Eval(TM<T, _R, _C, _S>&& lazy)
	->	_eval_t<T, decltype(lazy)>{  return Move(lazy);  }


	template
	<	template<class, size_t, size_t, Storing_Order> class TM
	,	class T, size_t _R, size_t _C, Storing_Order _S
	>
	static auto _Eval(TM<T, _R, _C, _S> const&& lazy)
	->	_eval_t<T, decltype(lazy)>{  return Move(lazy);  }
};


namespace s3d
{

	template<class MAT>
	decltype(auto) Eval(MAT&& mat) noexcept(is_Rvalue_Reference<MAT&&>::value)
	{
		return _Eval_Helper::_Eval( Forward<MAT>(mat) );
	}

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct s3d::NullMat_t
{
	using value_type = None;
	static size_t constexpr STT_ROW_SIZE = UNKNOWN,  STT_COL_SIZE = UNKNOWN;
	static Storing_Order constexpr STORING_ORDER = DefaultStorOrder;
};


namespace s3d
{

	static NullMat_t constexpr NULLMAT{};  
	
}


namespace s3d
{

	template<class T, bool SIGNALING> 
	T constexpr _NaN_Helper() noexcept
	{
		static_assert(!std::is_integral_v<T>);
	
		if constexpr(std::is_floating_point_v<T>)
			return 
			SIGNALING 
			?	std::numeric_limits<T>::signaling_NaN() 
			:	std::numeric_limits<T>::quiet_NaN();
		else if constexpr(trait::is_complex<T>::value)
		{
			using real_t = trait::value_t<T>;
	
			return {_NaN_Helper<real_t, SIGNALING>(), _NaN_Helper<real_t, SIGNALING>()};
		}
		else 
			return Compile_Fails(); //	no method to generate NaN object .
	}

}


namespace std
{

	template<class T>
	static auto constexpr isnan(std::complex<T> const c) noexcept
	->	bool{  return isnan(c.real()) || isnan(c.imag());  }
	
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace s3d
{

	template<class MAT>
	decltype(auto) _Mat_implementor(MAT&& m)
	{
		if constexpr(trait::is_Matrix<MAT>::value)
		{
			using impl_t = typename Decay_t<MAT>::_impl_t;
			using res_t = Selective_t< is_immutable<MAT>::value, impl_t const, impl_t >&;
			
			return static_cast<res_t>(m._impl);
		}
		else
			return Forward<MAT>(m);
	}
	
	
	template<class T, int DP>  
	auto Are_almost_same(T t1, T t2)-> bool
	{
		if constexpr(std::is_integral_v<T>)
			return t1 == t2;
		else if constexpr(std::is_floating_point_v<T>)
			return 
			(	std::abs(t1 - t2) 
			<	static_cast<T>(_CX_POW<10, DP>()) * std::numeric_limits<T>::epsilon()
			);
		else if constexpr(trait::is_complex<T>::value)
		{
			using real_t = trait::value_t<T>;
	
			return
			(	Are_almost_same<real_t, DP>(t1.real(), t2.real()) 
			&&	Are_almost_same<real_t, DP>(t1.imag(), t2.imag())
			);
		}
		else 
			return Compile_Fails(); // no method to compare them.
	}
	
	
	template<class MAT>
	auto is_valid(MAT const& mat) noexcept-> bool
	{
		if constexpr(trait::Has_Matrix_interface<MAT>::value)
			return mat.size() == 0 || !std::isnan( mat(0, 0) );
		else 
			return Compile_Fails(); // no method to judge it if valid.
	}
	
	
	template<class MAT>  
	auto Has_Vector_interface(MAT const& mat) noexcept-> bool
	{
		if constexpr(trait::Has_Matrix_interface<MAT>::value)
			return (mat.rows() == 1 || mat.cols() == 1) && mat.size() > 1;
		else
			return false;
	}
	
	
	template<class MAT>
	auto is_Square_Matrix(MAT const& mat) noexcept-> bool
	{
		if constexpr(trait::Has_Matrix_interface<MAT>::value)
			return mat.rows() == mat.cols() && mat.cols() > 1;
		else
			return Compile_Fails();	 // Not a Matrix.
	}
	
	
	template<size_t BASE, std::size_t POWER>  
	auto constexpr _CX_POW() noexcept-> std::size_t
	{
		if constexpr(POWER == 0)
			return 1;
		else if constexpr(BASE == 0)
			return 0;
		else
			return BASE*_CX_POW<BASE, POWER - 1>();
	}

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class s3d::_iterable_conversion_Helper : Unconstructible
{
public:
	template<class T, size_t ROWS, size_t COLS, Storing_Order STOR, class CON>
	static decltype(auto) substituted_by_iterable(Matrix<T, ROWS, COLS, STOR>& Lhs, CON&& con)
	{
		static_assert(is_iterable<CON>::value);

		if constexpr(ROWS != 1 && COLS != 1)
			_iterable_conversion_Helper::_toMatrix<T, ROWS, COLS, STOR>
			(	Lhs, Forward<CON>(con) 
			);
		else if constexpr(is_Convertible< trait::Deref_t<CON>, T >::value)
			_iterable_conversion_Helper::_toVector<T, ROWS, COLS>( Lhs, Forward<CON>(con) );
		else 
			Compile_Fails(); // no suitable method was found.

		return Lhs;
	}


private:
	template<class T, size_t ROWS, size_t COLS, class LHS, class RHS>
	static decltype(auto) _toVector(LHS& Lhs, RHS&& rhs)
	{
		auto constexpr VEC_SIZE = ROWS != 1 ? ROWS : COLS;

		return _iterable_to_Vector<T, VEC_SIZE>::calc( Lhs, Forward<RHS>(rhs) );
	}


	template<class T, size_t ROWS, size_t COLS, Storing_Order STOR, class LHS, class RHS>
	static decltype(auto) _toMatrix(LHS& Lhs, RHS&& rhs)
	{
		using Helper = _iterable_to_Matrix<T, ROWS, COLS, STOR>;

		if constexpr(is_iterable<RHS, T>::value)
		{
			Helper::resize_if_needed( Lhs, Size(rhs) );
			Helper::with_elems(Lhs, rhs);
		}
		else if constexpr
		(	is_iterable<RHS>::value 
		&&	trait::Has_Matrix_interface< trait::Deref_t<RHS> >::value
		)
		{
			auto itr = Begin(rhs);

			assert( Has_Vector_interface(*itr) );

			Helper::resize_if_needed( Lhs, *itr, Size(rhs) );
			Helper::copy_baseVectors(Lhs, itr);
		}

		return Lhs;
	}
};


template<class T, std::size_t SIZE>
class s3d::_iterable_to_Vector : Unconstructible
{
private:
	template<class, size_t, size_t, class LHS, class RHS>
	friend decltype(auto) _iterable_conversion_Helper::_toVector(LHS& Lhs, RHS&& rhs);


	template<class LHS, class RHS>
	static auto calc(LHS& Lhs, RHS&& rhs)-> LHS&
	{
		if constexpr(trait::is_DynamicSize<SIZE>::value)
			Lhs.resize( Size(rhs) );

		assert( Size(Lhs) == Size(rhs) );

		_MatCopy( Forward<RHS>(rhs), Lhs );

		return Lhs;
	}
};


template<class T, std::size_t ROWS, std::size_t COLS, s3d::Storing_Order STOR>
class s3d::_iterable_to_Matrix : Unconstructible
{
private:
	template<class, size_t, size_t, Storing_Order, class LHS, class RHS>
	friend decltype(auto) _iterable_conversion_Helper::_toMatrix(LHS& Lhs, RHS&& rhs);


	template<class CON>
	static void with_elems(Matrix<T, ROWS, COLS, STOR>& Lhs, CON const& con)
	{
		if constexpr
		(	STOR == Storing_Order::ROW_FIRST 
		&&	is_Same<  T, Decay_t< trait::Deref_t<CON> >  >::value
		)
			_MatCopy(con, Lhs);
		else
			for(  auto[itr, i] = std::pair( Begin(con), size_t(0) );  i < Lhs.rows();  ++i  )
				for(size_t j = 0;  j < Lhs.cols();  ++j,  itr++)
					Lhs(i, j) = static_cast<T const&>(*itr);
	}


	template<class VEC>
	static void resize_if_needed
	(	Matrix<T, ROWS, COLS, STOR>& Lhs, VEC const& baseVector, size_t const nof_baseVector
	)
	{
		[[maybe_unused]] auto const base_size = baseVector.size();
		[[maybe_unused]] bool const is_column_space = baseVector.rows() > 1;

		if constexpr(trait::is_DynamicSizeMat<decltype(Lhs)>::value)
		{
			if( is_column_space && (Lhs.cols() != nof_baseVector || Lhs.rows() != base_size) )
				Lhs.resize(base_size, nof_baseVector);
			else if
			(	!is_column_space 
			&&	(Lhs.rows() != nof_baseVector || Lhs.cols() != base_size) 
			)
				Lhs.resize(nof_baseVector, base_size);
		}
		else if constexpr(trait::is_DynamicSize<ROWS>::value)
		{
			assert(!is_column_space && base_size == COLS);

			Lhs.resize(nof_baseVector, FIXED_SIZE);
		}
		else if constexpr(trait::is_DynamicSize<COLS>::value)
		{
			assert(is_column_space && base_size == ROWS);

			Lhs.resize(FIXED_SIZE, nof_baseVector);
		}
		else
			assert
			(	is_column_space ? ROWS == base_size && COLS == nof_baseVector 
			:	/* otherwise */ COLS == base_size && ROWS == nof_baseVector
			);
	}


	static void resize_if_needed(Matrix<T, ROWS, COLS, STOR>& Lhs, size_t const nof_elements)
	{
		if constexpr( !trait::is_FixedSizeMat<decltype(Lhs)>::value )
		{
			if constexpr(trait::is_StaticSize<COLS>::value)
				Lhs.resize(nof_elements/COLS, FIXED_SIZE);
			else if constexpr(trait::is_StaticSize<ROWS>::value)
				Lhs.resize(FIXED_SIZE, nof_elements/ROWS);
		}
		
		assert(Lhs.rows()*Lhs.cols() == nof_elements);
	}


	template<class ITR>
	static void copy_baseVectors(Matrix<T, ROWS, COLS, STOR>& Lhs, ITR itr)
	{
		if(bool const is_column_space = itr->rows() > 1;  is_column_space)
			for(size_t j = 0;  j < Lhs.cols();  ++j,  itr++)
				for(size_t i = 0;  i < itr->size();  ++i)
					Lhs(i, j) = static_cast<T const&>( (*itr)(i) );
		else
			for(size_t i = 0;  i < Lhs.rows();  ++i,  itr++)
				for(size_t j = 0;  j < itr->size();  ++j)
					Lhs(i, j) = static_cast<T const&>( (*itr)(j) );
	}
};


namespace s3d
{

	template<class CON, class MAT, class>
	void _MatCopy(CON&& con, MAT& mat) noexcept(is_Rvalue_Reference<CON&&>::value)
	{
		using elem1_t = trait::value_t<MAT>;
	
		using elem2_t 
		=	Selective_t
			<	is_Same<  elem1_t, Decay_t< trait::Deref_t<CON> >  >::value
			,	elem1_t const&
			,	elem1_t   
			>;
	
	
		elem1_t* p = &mat(0, 0);
	
		for(auto const& x : con)
			*p++ = static_cast<elem2_t>(x);
	}

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, std::size_t ROWS, std::size_t COLS, s3d::Storing_Order STOR>
class s3d::Matrix
{
private:
	template<class MAT>  
	friend decltype(auto) s3d::_Mat_implementor(MAT&& m);	

	using _impl_t = _MatrixAdaptor<T, ROWS, COLS, STOR>;

	_impl_t _impl;
	

public:
	using value_type = trait::value_t<_impl_t>;
	static size_t constexpr STT_ROW_SIZE = ROWS, STT_COL_SIZE = COLS;
	static Storing_Order constexpr STORING_ORDER = STOR;


	Matrix() : Matrix(_init_default()){}
	Matrix(size_t const r, size_t const c) : Matrix( Matrix::Zero(r, c) ){}

	Matrix(size_t const r, FixedSize const) : Matrix(r, COLS)
	{  
		static_assert(!trait::is_StaticSize<ROWS>::value, "CANNOT resize fixed size part."); 
	}

	Matrix(FixedSize const, size_t const c) : Matrix(ROWS, c)
	{
		static_assert(!trait::is_StaticSize<COLS>::value, "CANNOT resize fixed size part."); 
	}

	Matrix(size_t const n) : Matrix( Matrix::Zero(n) )
	{
		static_assert(ROWS == 1 || COLS == 1, "this method is only for Vector cass."); 
	}


	template
	<	class Q, size_t _R, size_t _C, Storing_Order _S
	,	class
		=	Enable_if_t
			<	(	!trait::is_StaticSize<ROWS>::value 
				||	!trait::is_StaticSize<_R>::value 
				||	ROWS == _R
				)
			&&	(	!trait::is_StaticSize<COLS>::value 
				||	!trait::is_StaticSize<_C>::value 
				||	COLS == _C
				)
			&&	!is_Same< Matrix<Q, _R, _C, _S>, Matrix >::value
			>
	>
	Matrix(Matrix<Q, _R, _C, _S> const& m) 
	:	_impl( _Mat_implementor(m) ){  assert(m.rows() == rows() && m.cols() == cols());  }


	template
	<	class Q, size_t _R, size_t _C, Storing_Order _S
	,	class
		=	Enable_if_t
			<	(	!trait::is_StaticSize<ROWS>::value 
				||	!trait::is_StaticSize<_R>::value 
				||	ROWS == _R
				)
			&&	(	!trait::is_StaticSize<COLS>::value 
				||	!trait::is_StaticSize<_C>::value 
				||	COLS == _C
				)
			&&	!is_Same< Matrix<Q, _R, _C, _S>, Matrix >::value
			>
	>
	Matrix(Matrix<Q, _R, _C, _S>&& m) noexcept : _impl(  _Mat_implementor( Move(m) )  )
	{  
		assert(m.rows() == rows() && m.cols() == cols());  
	}


	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	Matrix(_MatrixAdaptor<Q, _R, _C, _S> const& ma) : _impl(ma){}

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	Matrix(_MatrixAdaptor<Q, _R, _C, _S>&& temp) : _impl( Move(temp) ){}

	template<  class CON, class = Enable_if_t< is_iterable<CON>::value >  >	
	Matrix(CON&& con) noexcept(is_Rvalue_Reference<CON&&>::value) 
	:	Matrix(  _init_by_iterable( Forward<CON>(con) )  ){}


	template<  class Q, class = Enable_if_t< is_Convertible<Q, T>::value >  >
	Matrix(std::initializer_list<Q>&& iL) noexcept 
	:	Matrix(  _init_by_iterable( Move(iL) )  ){}

	Matrix(NullMat_t const) noexcept{  _impl.invalidate();  }


	template<   class Q, class = Enable_if_t<  !is_Same< Decay_t<Q>, Matrix >::value  >   >
	auto operator=([[maybe_unused]] Q&& q)-> Matrix&
	{
		if constexpr(is_iterable<Q>::value)
		{
			using deref_t = trait::Deref_t<Q>;

			static_assert
			(	is_Convertible<deref_t, value_type>::value 
			||	trait::Has_Matrix_interface<deref_t>::value
			);

			return _iterable_conversion_Helper::substituted_by_iterable( *this, Forward<Q>(q) );  
		}
		else if constexpr(is_Same< Decay_t<Q>, NullMat_t >::value)
			return _impl.invalidate(),  *this;
		else
			return _impl = _Mat_implementor( Forward<Q>(q) ),  *this;
	}

	template<class Q>
	auto operator=(std::initializer_list<Q>&& iL) noexcept-> Matrix&
	{
		return _iterable_conversion_Helper::substituted_by_iterable( *this, Move(iL) );
	}


	operator _impl_t const&() const{  return _impl;  }
	operator _impl_t&(){  return _impl;  }


	auto resize(size_t const i, size_t const j)-> Matrix&
	{
		static_assert
		(	!trait::is_StaticSize<ROWS>::value && !trait::is_StaticSize<COLS>::value
		,	"CANNOT resize fixed size matrix"
		);

		return i == rows() && j == cols() ? *this : *this = Matrix(i, j);
	}

	auto resize(FixedSize const, size_t const j)
	->	Matrix&{  return j == cols() ? *this : *this = Matrix(FIXED_SIZE, j);  }

	auto resize(size_t const i, FixedSize const)
	->	Matrix&{  return i == rows() ? *this : *this = Matrix(i, FIXED_SIZE);  }
	
	auto resize(size_t const s)-> Matrix&{  return s == size() ? *this : *this = Matrix(s);  }


	auto rows() const-> size_t{  return _impl.rows();  }
	auto cols() const-> size_t{  return _impl.cols();  }
	auto size() const-> size_t{  return _impl.size();  }

	auto cdata() const-> value_type const*{  return _impl.data();  }
	auto data() const-> value_type const*{  return cdata();  }
	auto data()-> value_type*{  return _impl.data();  }

	decltype(auto) operator()(size_t const idx){  return _impl(idx);  }
	decltype(auto) operator()(size_t const idx) const{  return _impl(idx);  }
	decltype(auto) operator()(size_t const i, size_t const j){  return _impl(i, j);  }
	decltype(auto) operator()(size_t const i, size_t const j) const{  return _impl(i, j);  }

	decltype(auto) operator+() const{  return +_impl;  }
	decltype(auto) operator-() const{  return -_impl;  }

	template<class Q>  
	auto operator+(Q&& q) const{  return _impl + _Mat_implementor( Forward<Q>(q) );  }
	
	template<class Q>  
	auto operator-(Q&& q) const{  return _impl - _Mat_implementor( Forward<Q>(q) );  }
	
	template<class Q>  
	auto operator*(Q&& q) const{  return _impl * _Mat_implementor( Forward<Q>(q) );  }

	template<  class Q, class = Enable_if_t< trait::is_complexible<Q>::value >  >  
	auto operator/(Q const q) const{  return _impl/q;  }

	template<class Q>  
	auto operator+=(Q&& q)
	->	Matrix&{  return _impl += _Mat_implementor( Forward<Q>(q) ),  *this;  }
	
	template<class Q>  
	auto operator-=(Q&& q)
	->	Matrix&{  return _impl -= _Mat_implementor( Forward<Q>(q) ),  *this;  }
	
	template<class Q>  
	auto operator*=(Q&& q)
	->	Matrix&{  return _impl *= _Mat_implementor( Forward<Q>(q) ),  *this;  }
	
	template<class Q>  
	auto operator/=(Q&& q)
	->	Matrix&{  return _impl /= _Mat_implementor( Forward<Q>(q) ),  *this;  }


	decltype(auto) row(size_t const i){  return _impl.row(i);  }
	decltype(auto) row(size_t const i) const{  return _impl.row(i);  }
	decltype(auto) col(size_t const j){  return _impl.col(j);  }
	decltype(auto) col(size_t const j) const{  return _impl.col(j);  }


	decltype(auto) block(size_t const i, size_t const j, size_t const rsize, size_t const csize)
	{
		return _impl.block(i, j, rsize, csize);
	}

	decltype(auto) block
	(	size_t const i, size_t const j, size_t const rsize, size_t const csize
	)	const
	{
		return _impl.block(i, j, rsize, csize);
	}

	decltype(auto) head(size_t const s)
	{
		assert( Has_Vector_interface(*this) );  
		
		return _impl.head(s);  
	}

	decltype(auto) head(size_t const s) const
	{
		assert( Has_Vector_interface(*this) );  
		
		return _impl.head(s);  
	}

	decltype(auto) tail(size_t const s)
	{
		assert( Has_Vector_interface(*this) );  
		
		return _impl.tail(s);  
	}

	decltype(auto) tail(size_t const s) const
	{
		assert( Has_Vector_interface(*this) );  
		
		return _impl.tail(s);  
	}


	auto inv() const
	{
		assert( is_Square_Matrix(*this) );  
		
		return _impl.inv();  
	}


	auto transpose() const{  return _impl.transpose();  }
	
	auto det() const
	{	
		assert( is_Square_Matrix(*this) );  
		
		return _impl.det();  
	}


	auto sqr_norm() const-> T{  return _impl.sqr_norm();  }
	auto norm() const-> T{  return _impl.norm();  }
	auto normalized() const{  return _impl.normalized();  }
	decltype(auto) normalize(){  return _impl.normalize();  }

	auto dot(Matrix const& m) const-> T{  return _impl.dot(m._impl);  }
	auto cross(Matrix const& m) const{  return _impl.cross(m._impl);  }


	template
	<	size_t VSIZE
		=	trait::is_StaticSize<ROWS>::value && ROWS != 1 ? ROWS
		:	trait::is_StaticSize<COLS>::value && COLS != 1 ? COLS
		:	/* otherwise */ DYNAMIC
	>
	auto dyadic(Vector<T, VSIZE> const& q) const
	{
		assert( Has_Vector_interface(*this) && cols() == q.cols() && rows() == q.rows() );

		if constexpr(is_Same< Vector<T, VSIZE>, ColVec<T, VSIZE> >::value)
			return *this*q.transpose();
		else if constexpr(is_Same< Vector<T, VSIZE>, RowVec<T, VSIZE> >::value)
			return this->transpose()*q;
		else 
			return 
			(Matrix<T, VSIZE, VSIZE>)Compile_Fails(); // no method to do dyadic product with.
	}
	

	auto skew() const-> s3d::Matrix<T, 3, 3>
	{
		static_assert
		(	!trait::is_FixedSizeMat<Matrix>::value
		||	ROWS == 3 || COLS == 3
		);
		
		assert( Has_Vector_interface(*this) && size() == 3 );

		T constexpr _0 = T(0);
		T const x = (*this)(0), y = (*this)(1), z = (*this)(2);

		return
		{	_0, -z, y
		,	z, _0, -x
		,	-y, x, _0
		};
	}


	static auto Zero()
	{
		static_assert(trait::is_FixedSizeMat<Matrix>::value);
		
		return _impl_t::Zero();  
	}

	static auto Zero(size_t const i){  return _impl_t::Zero(i);  }
	static auto Zero(size_t const i, size_t const j){  return _impl_t::Zero(i, j);  }


	static auto Ones()
	{
		static_assert(trait::is_FixedSizeMat<Matrix>::value);
		
		return _impl_t::Ones();  
	}

	static auto Ones(size_t const i){  return _impl_t::Ones(i);  }
	static auto Ones(size_t const i, size_t const j){  return _impl_t::Ones(i, j);  }


	static auto identity()
	{
		static_assert(trait::is_StrictMat<Matrix>::value && ROWS == COLS);
		
		return _impl_t::identity();  
	}

	static auto identity(size_t const i){  return _impl_t::identity(i);  }


private:
	template<class CON>	
	static auto _init_by_iterable(CON&& con, Matrix res = {})-> Matrix
	{
		return _iterable_conversion_Helper::substituted_by_iterable( res, Forward<CON>(con) );  
	}


	static auto _init_default()
	{
		if constexpr(trait::is_FixedSizeMat<Matrix>::value)
			return Matrix::Zero();
		else if constexpr(Has_Operator_New<_impl_t>::value)
			return _impl_t();
		else 
			return (Matrix)Compile_Fails(); // no way for default initialization.
	}
};


template<class T, class U, std::size_t ROWS, std::size_t COLS, s3d::Storing_Order STOR, class>
static decltype(auto) operator*(T t, s3d::Matrix<U, ROWS, COLS, STOR> const& m){  return m*t;  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace s3d
{

	template<class RES, class...ARGS, class>
	auto Skipped(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	{
		return RES( _ExemptionTag{}, Forward<ARGS>(args)... );
	}

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, std::size_t SIZE>
class s3d::UnitVec : public Vector<T, SIZE>
{
	using _Vec = Vector<T, SIZE>;


public:
	template
	<	class...ARGS
	,	class 
		=	Enable_if_t
			<	Has_Operator_New< _Vec, ARGS&&...>::value 
			&&	!std::is_integral_v< First_t<ARGS...> >
			>
	>
	UnitVec(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value) 
	:	_Vec( Forward<ARGS>(args)... )
	{
		if constexpr( sizeof...(ARGS) != 1 || !trait::is_UnitVec< First_t<ARGS...> >::value )
			_normalize();
	}

	template<class Q>
	UnitVec(std::initializer_list<Q>&& iL) noexcept : _Vec( Move(iL) ){  _normalize();  };

	UnitVec() : UnitVec(Axis<SIZE - 1>()){  static_assert(trait::is_StaticSize<SIZE>::value);  }

	UnitVec(size_t const dim) : _Vec( _Vec::Zero(dim) )
	{
		static_assert(trait::is_DynamicSize<SIZE>::value);

		assert(dim != 0);

		_Vec::operator()(dim - 1) = 1;
	}

	UnitVec(NullMat_t const) : UnitVec(_ExemptionTag{}, NULLMAT){}


	template
	<	class Q
	,	class 
		=	Enable_if_t
			<	Has_Operator_Copy_Assignment<_Vec, Q&&>::value
			||	Has_Operator_Move_Assignment<_Vec, Q&&>::value
			>
	>
	auto operator=(Q&& q) noexcept(is_Rvalue_Reference<Q&&>::value)-> UnitVec&
	{
		static_cast<_Vec&>(*this) = Forward<Q>(q);

		if constexpr(!trait::is_UnitVec<Q>::value && !is_Same< Decay_t<Q>, NullMat_t >::value)
			_normalize();

		return *this;
	}

	auto vec() const noexcept-> _Vec const&{  return *this;  }

	auto resize(size_t const dim)-> UnitVec&{  return *this = UnitVec(dim);  }

	auto operator+() const noexcept-> UnitVec const&{  return *this;  }
	auto operator-() const-> UnitVec{  return Skipped<UnitVec>(-vec());  }

	void operator+(_Vec) const = delete;
	void operator-(_Vec) const = delete;

	void operator+=(_Vec) = delete;
	void operator-=(_Vec) = delete;
	void operator*=(_Vec) = delete;
	void operator/=(_Vec) = delete;

	auto operator()(size_t const idx) const{  return _Vec::operator()(idx);  }
	auto operator()(size_t const i, size_t const j) const{  return _Vec::operator()(i, j);  }

	auto head(size_t const size) const{  return _Vec::head(size);  }
	auto tail(size_t const size) const{  return _Vec::tail(size);  }
	decltype(auto) row(size_t const idx) const{  return _Vec::row(idx);  }
	decltype(auto) col(size_t const idx) const{  return _Vec::col(idx);  }

	decltype(auto) block
	(	size_t const i, size_t const j, size_t const rsize, size_t const csize
	)	const
	{
		return _Vec::block(i, j, rsize, csize);
	}

	auto sqr_norm() const{  return T(1);  }
	auto norm() const{  return T(1);  }
	auto normalized() const-> UnitVec const&{  return *this;  };
	auto normalize()-> UnitVec&{  return *this;  };


	template<size_t IDX = DYNAMIC>
	static auto Axis(size_t idx = DYNAMIC)
	->	Selective_t< trait::is_StaticSize<IDX>::value, UnitVec const&, UnitVec >
	{
		static_assert(trait::is_StaticSize<SIZE>::value);

		if constexpr(trait::is_StaticSize<IDX>::value)
		{
			static auto const res 
			=	[](_Vec v){  return v(IDX) = 1,  Skipped<UnitVec>(v);  }(_Vec::Zero());

			assert(idx == IDX || idx == DYNAMIC);

			return res;
		}
		else
		{
			_Vec res = _Vec::Zero();

			assert(idx != DYNAMIC && idx < IDX);

			return res(idx) = 1,  Skipped<UnitVec>(res);
		}
	}


private:
	auto _normalize()-> UnitVec&{  return _Vec::normalize(),  *this;  }


	template<class res_t, class...ARGS, class>
	friend auto s3d::Skipped(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value);


	template<class...ARGS>
	UnitVec(_ExemptionTag, ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value) 
	:	_Vec( Forward<ARGS>(args)... ){}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, std::size_t SIZE, s3d::Storing_Order STOR>
class s3d::OrthogonalMat : public Matrix<T, SIZE, SIZE, STOR>
{
	using _Mat = Matrix<T, SIZE, SIZE, STOR>;


public:
	template
	<	class...ARGS
	,	class 
		=	Enable_if_t
			<	Has_Operator_New< _Mat, ARGS&&...>::value 
			&&	!std::is_integral_v< First_t<ARGS...> >
			>
	>
	OrthogonalMat(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value)
	:	_Mat( Forward<ARGS>(args)... )
	{
		if constexpr
		(	sizeof...(ARGS) != 1 
		||	!trait::is_OrthogonalMat< First_t<ARGS...> >::value 
		)
			_orthonormalize();
	}


	template<class Q>
	OrthogonalMat(std::initializer_list<Q>&& iL) noexcept 
	:	_Mat( Move(iL) ){  _orthonormalize();  };

	OrthogonalMat() 
	:	_Mat(_Mat::identity()){  static_assert(trait::is_StaticSize<SIZE>::value);  }

	explicit OrthogonalMat(size_t const dim) : _Mat( _Mat::identity(dim) )
	{
		static_assert(trait::is_DynamicSize<SIZE>::value);

		assert(dim != 0);
	}

	OrthogonalMat(size_t, size_t) = delete;

	OrthogonalMat(NullMat_t const) : OrthogonalMat(_ExemptionTag{}, NULLMAT){}


	template
	<	class Q
	,	class 
		=	Enable_if_t
			<	Has_Operator_Copy_Assignment<_Mat, Q&&>::value
			||	Has_Operator_Move_Assignment<_Mat, Q&&>::value
			>  
	>
	auto operator=(Q&& q) noexcept(is_Rvalue_Reference<Q&&>::value)-> OrthogonalMat&
	{
		static_cast<_Mat&>(*this) = Forward<Q>(q);

		if constexpr
		(	!trait::is_OrthogonalMat<Q>::value 
		&&	!is_Same< Decay_t<Q>, NullMat_t >::value
		)
			_orthonormalize();

		return *this;
	}

	auto mat() const noexcept-> _Mat const&{  return *this;  }

	auto resize(size_t const s)-> OrthogonalMat&{  return *this = OrthogonalMat(s);  }

	auto operator+() const noexcept-> OrthogonalMat const&{  return *this;  }
	auto operator-() const-> OrthogonalMat{  return Skipped<OrthogonalMat>(-mat());  }

	void operator+(_Mat) const = delete;
	void operator-(_Mat) const = delete;
	void operator+=(_Mat) = delete;
	void operator-=(_Mat) = delete;
	void operator/=(_Mat) = delete;


	template<class Q>  
	auto operator*(Q const& m) const{  return mat()*m;  }
	

	template<class Q>  
	decltype(auto) operator*=(Q const& m){  return *this = *this * m;  }


	auto operator()(size_t const idx) const{  return mat()(idx);  }
	auto operator()(size_t const i, size_t const j) const{  return mat()(i, j);  }

	decltype(auto) row(size_t const idx) const{  return mat().row(idx);  }
	decltype(auto) col(size_t const idx) const{  return mat().col(idx);  }

	decltype(auto) block
	(	size_t const i, size_t const j, size_t const rsize, size_t const csize
	)	const
	{
		return mat().block(i, j, rsize, csize);
	}

	auto inv() const{  return transpose();  }
	auto transpose() const{  return Skipped<OrthogonalMat>(mat().transpose());  }


private:
	auto _orthonormalize()-> OrthogonalMat&
	{
		assert( mat().size() != 0 && is_valid(*this) );

		Vector<T, SIZE> const zerovec = Vector<T, SIZE>::Zero(mat().cols());

		for(size_t j = 0;  j < mat().cols();  ++j)
		{
			auto s = zerovec;

			for(size_t k = 0;  k < j;  ++k)
				s += col(k) * col(k).dot( col(j) );

			_Mat::col(j) = UnitVec<T, SIZE>( col(j) - s );
		}

		return *this;
	}


	template<class res_t, class...ARGS, class>
	friend auto s3d::Skipped(ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value);


	template<class...ARGS>
	OrthogonalMat(_ExemptionTag, ARGS&&...args) noexcept(Aleph_Check<ARGS&&...>::value) 
	:	_Mat( Forward<ARGS>(args)... ){}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace s3d
{

	template<class MAT, class>
	auto as_col_space(MAT& mat)
	{
		return Morph( Countable(mat.cols()), _VecSpace_Helper<MAT, 1>(mat) );
	}
	
	
	template<class MAT, class>
	auto as_row_space(MAT& mat)
	{
		return Morph( Countable(mat.rows()), _VecSpace_Helper<MAT, 2>(mat) );	
	}
	
	
	template<class MAT, class>
	auto as_icol_space(MAT& mat)
	{
		return Morph( Countable(mat.cols()), _VecSpace_Helper<MAT, -1>(mat) );
	}
	
	
	template<class MAT, class>
	auto as_irow_space(MAT& mat)
	{
		return Morph( Countable(mat.rows()), _VecSpace_Helper<MAT, -2>(mat) );
	}

}


template<class MAT, int DIREC>
struct s3d::_VecSpace_Helper
{
	_VecSpace_Helper(MAT& m) : _p(&m){}

	decltype(auto) operator()(size_t const idx)
	{
		if constexpr(DIREC == 1)
			return _p->col(idx);
		else if constexpr(DIREC == 2)
			return _p->row(idx);
		else if constexpr(DIREC == -1)
			return _p->col(_p->cols() - 1 - idx);
		else if constexpr(DIREC == -2)
			return _p->row(_p->rows() - 1 - idx);
		else 
			//_VecSpace_Helper got wrong flag number DIREC .
			return (DynamicMat< trait::value_t<MAT> >)Compile_Fails(); 
	}


private:
	MAT* _p;
};
//========//========//========//========//=======#//========//========//========//========//=======#


#include "_Hamilton_by_Eigen.hpp"


#endif // end of #ifndef _S3D_HAMILTON_