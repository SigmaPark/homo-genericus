#pragma once
#ifndef _V3D_HAMILTON_
#define _V3D_HAMILTON_


#include "Sigma\interface_Traits\interface_Traits.hpp"
#include <type_traits>
#include <climits>


//	minimum versions for C++11 main syntex support.
#ifndef SGM_SYNTEX_VERSION_CPP17
	#error C++17 or higher version of language support is required.
#endif


namespace v3d
{

	using std::size_t;
	
	enum class Storing_Order{COL_FIRST, ROW_FIRST};

	static auto constexpr DefaultStorOrder = Storing_Order::COL_FIRST;
	size_t constexpr DYNAMIC = 0xffff'ffff'ffff'ffffULL,  UNKNOWN = DYNAMIC - 1;

	enum class FixedSize{} constexpr FIXED_SIZE{};

	template<class T, bool SIGNALING> static T constexpr _NaN_Helper() noexcept;
	template<class T, bool SIGNALING = false>  static T constexpr NaN = _NaN_Helper<T, SIGNALING>();


	template<class T, size_t ROWS = DYNAMIC, size_t COLS = DYNAMIC, Storing_Order STOR = DefaultStorOrder>
	class Matrix;

	template<class T, Storing_Order STOR = DefaultStorOrder>
	using DynamicMat = Matrix<T, DYNAMIC, DYNAMIC, STOR>;

	struct NullMat_t;


	template<class T, size_t SIZE = DYNAMIC>
	using ColVec = Matrix<T, SIZE, 1>;

	template<class T, size_t SIZE = DYNAMIC>
	using RowVec = Matrix<T, 1, SIZE>;

	template<class T, size_t SIZE = DYNAMIC>
	using Vector 
	=	std::conditional_t< DefaultStorOrder == Storing_Order::COL_FIRST, ColVec<T, SIZE>, RowVec<T, SIZE> >;


	template<class T, size_t SIZE = DYNAMIC>
	class UnitVec;


	template<class T, size_t SIZE = DYNAMIC, Storing_Order STOR = DefaultStorOrder>
	class OrthogonalMat;


	template<class T, size_t ROWS, size_t COLS, Storing_Order STOR>
	struct _MatrixAdaptor;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


	template<class MAT>  static bool is_valid(MAT const&) noexcept;
	template<class MAT>  static bool Has_Vector_interface(MAT const&) noexcept;
	template<class MAT>  static bool is_Square_Matrix(MAT const&) noexcept;
	template<class T, int DP = 3>  static bool are_almost_same(T t1, T t2);

	template<  class res_t, class...ARGS, class = std::enable_if_t< std::is_constructible_v<res_t, ARGS&&...> >  >
	static auto Skipped(ARGS&&...args) noexcept(sgm::Check_All<std::is_rvalue_reference>::for_any<ARGS&&...>::value);


	template
	<	template<class, size_t, size_t, Storing_Order> class TM
	,	class T, size_t ROWS, size_t COLS, Storing_Order STOR
	>
	decltype(auto) Eval(TM<T, ROWS, COLS, STOR> const &lazy);

	template
	<	template<class, size_t, size_t, Storing_Order> class TM
	,	class T, size_t ROWS, size_t COLS, Storing_Order STOR
	>
	decltype(auto) Eval(TM<T, ROWS, COLS, STOR> &&lazy);
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


	template<  class MAT, class = std::enable_if_t< trait::has_Matrix_interface_v<MAT> >  >
	static auto as_col_space(MAT &mat);

	template<  class MAT, class = std::enable_if_t< trait::has_Matrix_interface_v<MAT> >  >
	static auto as_row_space(MAT &mat);

	template<  class MAT, class = std::enable_if_t< trait::has_Matrix_interface_v<MAT> >  >
	static auto as_icol_space(MAT &mat);

	template<  class MAT, class = std::enable_if_t< trait::has_Matrix_interface_v<MAT> >  >
	static auto as_irow_space(MAT &mat);


	template<class MAT, int DIREC>  struct _VecSpace_Helper;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


	template
	<	class CON, class MAT
	,	class 
		=	std::enable_if_t
			<	sgm::is_iterable<CON>::value
			&&	std::is_convertible_v< sgm::Deref_t<CON>, trait::value_t<MAT> >
			>
	>
	static void _MatCopy(CON &&container, MAT &target_matrix) noexcept(std::is_rvalue_reference_v<CON&&>);


	template<class MAT>  static decltype(auto) _Mat_implementor(MAT &&m);
	template<class M>  static decltype(auto) _Mat_impl(std::remove_reference_t<M> &m);
	template<class M>  static decltype(auto) _Mat_impl(std::remove_reference_t<M> &&m);


	class _iterable_conversion_Helper;
	template<class T, size_t SIZE>  class _iterable_to_Vector;
	template<class T, size_t ROWS, size_t COLS, Storing_Order STOR>  class _iterable_to_Matrix;


	enum class _ExemptionTag{};

	template<size_t BASE, size_t POWER>  static size_t constexpr _CX_POW() noexcept;

}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


namespace v3d::trait
{

	template<class T>  struct is_real;
	template<class T>  static bool constexpr is_real_v = is_real<T>::value;

	template<  class T, bool = std::is_same_v< T, std::decay_t<T> >  >  struct is_complex;
	template<class T>  static bool constexpr is_complex_v = is_complex<T>::value;

	template<class T>  struct is_complexible;
	template<class T>  static bool constexpr is_complexible_v = is_complexible<T>::value;

	template<class MAT>  struct has_Matrix_interface;
	template<class MAT>  static bool constexpr has_Matrix_interface_v = has_Matrix_interface<MAT>::value;

	template<class MAT>  struct is_v3dMat;
	template<class MAT>  static bool constexpr is_v3dMat_v = is_v3dMat< std::decay_t<MAT> >::value;

	template<class TM>  struct is_TempMat;
	template<class TM>  static bool constexpr is_TempMat_v = is_TempMat<TM>::value;  

	template<class MAT>  struct is_v3dVec;
	template<class MAT>  static bool constexpr is_v3dVec_v = is_v3dVec< std::decay_t<MAT> >::value;

	SGM_HAS_NESTED_TYPE(value_type);

	template<class MAT>  
	using value_t = sgm::Guaranteed_t< Has_NestedType_value_type<MAT>::value, typename MAT::value_type >;


	template<size_t SIZE>  static bool constexpr is_Dynamic_v = SIZE == DYNAMIC;
	template<size_t SIZE>  static bool constexpr is_Unknown_v = SIZE == UNKNOWN;
	template<size_t SIZE>  static bool constexpr is_Static_v = !is_Dynamic_v<SIZE> && !is_Unknown_v<SIZE>;
	
	template<Storing_Order STOR>  static bool constexpr is_ColFirst_v = STOR == Storing_Order::COL_FIRST;
	template<Storing_Order STOR>  static bool constexpr is_RowFirst_v = STOR == Storing_Order::ROW_FIRST;


	template<class MAT>  static auto constexpr storing_order_v 
	=	[]() constexpr
		{
			static_assert(is_v3dMat_v<MAT> || is_TempMat_v<MAT>);

			return std::decay_t<MAT>::STORING_ORDER;
		}();


	template<class MAT>  static bool constexpr is_FixedSizeMat_v
	=	[]() constexpr	
		{
			if constexpr(is_v3dMat_v<MAT> || is_TempMat_v<MAT>)
				return is_Static_v< std::decay_t<MAT>::ROW_SIZE > && is_Static_v< std::decay_t<MAT>::COL_SIZE >;
			else
				return false;
		}();

	
	template<class MAT> static bool constexpr is_DynamicSizeMat_v
	=	[]() constexpr
		{
			if constexpr(has_Matrix_interface_v<MAT> || is_TempMat_v<MAT>)
				return is_Dynamic_v< std::decay_t<MAT>::ROW_SIZE > && is_Dynamic_v< std::decay_t<MAT>::COL_SIZE >;
			else
				return false;
		}();


	template<class MAT>  static bool constexpr is_StrictMat_v
	=	[]() constexpr
		{
			using _M = std::decay_t<MAT>;

			if constexpr(is_v3dMat_v<_M> || is_TempMat_v<MAT>)
				return is_FixedSizeMat_v<_M> && _M::ROW_SIZE >= 2 && _M::COL_SIZE >= 2;
			else
				return false;
		}();


	SGM_USER_DEFINED_TYPE_CHECK14( _, UnitVec, SGM_MACROPACK(class T, size_t SIZE), SGM_MACROPACK(T, SIZE) );
	SGM_USER_DEFINED_TYPE_CHECK14( _, OrthogonalMat, SGM_MACROPACK(class T, size_t SIZE), SGM_MACROPACK(T, SIZE) );

};


template
<	class T, class U, size_t ROWS, size_t COLS, v3d::Storing_Order STOR
,	class = std::enable_if_t< v3d::trait::is_complexible_v<T> >
>
static decltype(auto) operator*(T t, v3d::Matrix<U, ROWS, COLS, STOR> const &m);
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


#include <complex>


template<class T, bool SIGNALING> 
T constexpr v3d::_NaN_Helper() noexcept
{
	static_assert(!std::is_integral_v<T>);

	if constexpr(std::is_floating_point_v<T>)
		return SIGNALING ? std::numeric_limits<T>::signaling_NaN() : std::numeric_limits<T>::quiet_NaN();
	else if constexpr(trait::is_complex_v<T>)
	{
		using real_t = trait::value_t<T>;

		return {_NaN_Helper<real_t, SIGNALING>(), _NaN_Helper<real_t, SIGNALING>()};
	}
	else SGM_COMPILE_FAILED(no method to generate NaN object .);
}


namespace std
{

	template<class T>
	static bool constexpr isnan(std::complex<T> const &c) noexcept{  return isnan(c.real()) || isnan(c.imag());  }
	
	template<class T>
	static bool constexpr isnan(std::complex<T> &&c) noexcept{  return isnan(c.real()) || isnan(c.imag());  }

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class T>  
struct v3d::trait::is_real : sgm::Boolean_type<  std::is_convertible_v< std::decay_t<T>, long double >  >{};


template<class T>  struct v3d::trait::is_complex<T, false> : is_complex< std::decay_t<T> >{};
template<class T>  struct v3d::trait::is_complex<T, true> : sgm::False_t{};
template<class T>  struct v3d::trait::is_complex< std::complex<T>, true > : is_real<T>{};


template<class T>  
struct v3d::trait::is_complexible : sgm::Boolean_type< v3d::trait::is_complex_v<T> || is_real_v<T> >{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class MAT>
struct v3d::trait::has_Matrix_interface 
{
	static bool constexpr value
	=	[]() constexpr
		{
			if constexpr(std::is_invocable_v<MAT, size_t, size_t>)
				return std::is_convertible_v<  MAT, v3d::Matrix< std::invoke_result_t<MAT, size_t, size_t> >  >;
			else
				return false;
		}();

	using type = sgm::Boolean_type<value>;
};


template<class MAT>  
struct v3d::trait::is_v3dMat
{
private:
	template<class T, size_t ROWS, size_t COLS, Storing_Order STOR> /* Declaration Only */
	static auto _calc(v3d::Matrix<T, ROWS, COLS, STOR> const volatile*)-> sgm::True_t;

	template<class...> /* Declaration Only */  static auto _calc(void const volatile*)-> sgm::False_t;

public:
	using type = decltype( _calc(std::declval< std::remove_reference_t<MAT>* >()) );
	static bool constexpr value = type::value;
};


template<class TM>  
struct v3d::trait::is_TempMat
{
private:
	template<class T, size_t ROWS, size_t COLS, Storing_Order STOR> /* Declaration Only */
	static auto _calc(_MatrixAdaptor<T, ROWS, COLS, STOR> const volatile*)
	->	sgm::Boolean_type< !is_complexible_v<T> >;

	template<class...> /* Declaration Only */  static auto _calc(void const volatile*)-> sgm::False_t;
	
public:
	using type = decltype( _calc(std::declval< std::remove_reference_t<TM>* >()) );
	static bool constexpr value = type::value;
};


template<class MAT>
struct v3d::trait::is_v3dVec
{
	static bool constexpr value
	=	[]() constexpr
		{
			if constexpr(trait::is_v3dMat_v<MAT> || is_TempMat_v<MAT>)
				return std::decay_t<MAT>::ROW_SIZE == 1 || std::decay_t<MAT>::COL_SIZE == 1;
			else
				return false;
		}();

	using type = sgm::Boolean_type<value>;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#include <cassert>


class v3d::_iterable_conversion_Helper : sgm::No_Making
{
public:
	template<class T, size_t ROWS, size_t COLS, Storing_Order STOR, class CON>
	static decltype(auto) substituted_by_iterable(Matrix<T, ROWS, COLS, STOR> &Lhs, CON &&con)
	{
		static_assert(sgm::is_iterable<CON>::value);

		if constexpr(ROWS != 1 && COLS != 1)
			_iterable_conversion_Helper::_toMatrix<T, ROWS, COLS, STOR>( Lhs, std::forward<CON>(con) );
		else if constexpr(std::is_convertible_v< sgm::Deref_t<CON>, T >)
			_iterable_conversion_Helper::_toVector<T, ROWS, COLS>( Lhs, std::forward<CON>(con) );
		else SGM_COMPILE_FAILED(no suitable method was found.);		

		return Lhs;
	}


private:
	template<class T, size_t ROWS, size_t COLS, class LHS, class RHS>
	static decltype(auto) _toVector(LHS &Lhs, RHS &&rhs)
	{
		auto constexpr VEC_SIZE = ROWS != 1 ? ROWS : COLS;

		return _iterable_to_Vector<T, VEC_SIZE>::calc( Lhs, std::forward<RHS>(rhs) );
	}


	template<class T, size_t ROWS, size_t COLS, Storing_Order STOR, class LHS, class RHS>
	static decltype(auto) _toMatrix(LHS &Lhs, RHS &&rhs)
	{
		using Helper = _iterable_to_Matrix<T, ROWS, COLS, STOR>;

		if constexpr(sgm::is_iterable<RHS, T>::value)
		{
			Helper::resize_if_needed( Lhs, sgm::Size(rhs) );
			Helper::with_elems(Lhs, rhs);
		}
		else if constexpr(sgm::is_iterable<RHS>::value && trait::has_Matrix_interface_v< sgm::Deref_t<RHS> >)
		{
			auto itr = sgm::Begin(rhs);

			assert( Has_Vector_interface(*itr) );

			Helper::resize_if_needed( Lhs, *itr, sgm::Size(rhs) );
			Helper::copy_baseVectors(Lhs, itr);
		}

		return Lhs;
	}
};


template<class T, size_t SIZE>
class v3d::_iterable_to_Vector : sgm::No_Making
{
	template<class T, size_t ROWS, size_t COLS, class LHS, class RHS>
	friend decltype(auto) _iterable_conversion_Helper::_toVector(LHS &Lhs, RHS &&rhs);


	template<class LHS, class RHS>
	static auto calc(LHS &Lhs, RHS &&rhs)-> LHS&
	{
		if constexpr(trait::is_Dynamic_v<SIZE>)
			Lhs.resize( sgm::Size(rhs) );

		assert( sgm::Size(Lhs) == sgm::Size(rhs) );

		_MatCopy( std::forward<RHS>(rhs), Lhs );

		return Lhs;
	}
};


template<class T, size_t ROWS, size_t COLS, v3d::Storing_Order STOR>
class v3d::_iterable_to_Matrix : sgm::No_Making
{
	template<class T, size_t ROWS, size_t COLS, Storing_Order STOR, class LHS, class RHS>
	friend decltype(auto) _iterable_conversion_Helper::_toMatrix(LHS &Lhs, RHS &&rhs);


	template<class CON>
	static void with_elems(Matrix<T, ROWS, COLS, STOR> &Lhs, CON const &con)
	{
		if constexpr( trait::is_RowFirst_v<STOR> && std::is_same_v<  T, std::decay_t< sgm::Deref_t<CON> >  > )
			_MatCopy(con, Lhs);
		else
			for( auto[itr, i] = std::pair( sgm::Begin(con), size_t(0) );  i < Lhs.rows();  ++i )
				for(size_t j = 0;  j < Lhs.cols();  ++j,  itr++)
					Lhs(i, j) = static_cast<T const&>(*itr);
	}


	template<class VEC>
	static void resize_if_needed
	(	Matrix<T, ROWS, COLS, STOR> &Lhs, VEC const &baseVector, size_t const nof_baseVector
	)
	{
		[[maybe_unused]] auto const base_size = baseVector.size();
		[[maybe_unused]] bool const is_column_space = baseVector.rows() > 1;

		if constexpr( trait::is_DynamicSizeMat_v<decltype(Lhs)> )
		{
			if( is_column_space && (Lhs.cols() != nof_baseVector || Lhs.rows() != base_size) )
				Lhs.resize(base_size, nof_baseVector);
			else if( !is_column_space && (Lhs.rows() != nof_baseVector || Lhs.cols() != base_size) )
				Lhs.resize(nof_baseVector, base_size);
		}
		else if constexpr(trait::is_Dynamic_v<ROWS>)
		{
			assert(!is_column_space && base_size == COLS);

			Lhs.resize(nof_baseVector, FIXED_SIZE);
		}
		else if constexpr(trait::is_Dynamic_v<COLS>)
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


	static void resize_if_needed(Matrix<T, ROWS, COLS, STOR> &Lhs, size_t const nof_elements)
	{
		if constexpr(!trait::is_FixedSizeMat_v<decltype(Lhs)>)
		{
			if constexpr(trait::is_Static_v<COLS>)
				Lhs.resize(nof_elements/COLS, FIXED_SIZE);
			else if constexpr(trait::is_Static_v<ROWS>)
				Lhs.resize(FIXED_SIZE, nof_elements/ROWS);
		}
		
		assert(Lhs.rows()*Lhs.cols() == nof_elements);
	}


	template<class ITR>
	static void copy_baseVectors(Matrix<T, ROWS, COLS, STOR> &Lhs, ITR itr)
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
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class CON, class MAT, class>
void v3d::_MatCopy(CON &&con, MAT &mat) noexcept(std::is_rvalue_reference_v<CON&&>)
{
	using T1 = trait::value_t<MAT>;

	T1 *p = &mat(0, 0);

	using T2 = std::conditional_t<   std::is_same_v<  T1, std::decay_t< sgm::Deref_t<CON> >  >, T1 const&, T1   >;

	for(auto const &x : con)
		*p++ = static_cast<T2>(x);
}


template<class MAT>
decltype(auto) v3d::_Mat_implementor(MAT &&m)
{
	if constexpr(trait::is_v3dMat_v<MAT>)
	{
		using impl_t = typename std::decay_t<MAT>::_impl_t;

		return 
		static_cast<  std::conditional_t< sgm::is_immutable<MAT>::value, impl_t const, impl_t >&  >(m._impl);
	}
	else
		return std::forward<MAT>(m);
}

template<class M>  
decltype(auto) v3d::_Mat_impl(std::remove_reference_t<M> &m){  return _Mat_implementor(m);  }

template<class M>  
decltype(auto) v3d::_Mat_impl(std::remove_reference_t<M> &&m){  return _Mat_implementor( std::move(m) );  }


template<class res_t, class...ARGS, class>
auto v3d::Skipped(ARGS&&...args) noexcept(sgm::Check_All<std::is_rvalue_reference>::for_any<ARGS&&...>::value)
{
	return res_t( _ExemptionTag{}, std::forward<ARGS>(args)... );
}


template
<	template<class, size_t, size_t, v3d::Storing_Order> class TM
,	class T, size_t ROWS, size_t COLS, v3d::Storing_Order STOR
>
decltype(auto) v3d::Eval(TM<T, ROWS, COLS, STOR> const &t)
{
	using res_t
	=	Matrix
		<	std::decay_t<decltype( t(0, 0) )>
		,	trait::is_Static_v<ROWS> ? ROWS : DYNAMIC, trait::is_Static_v<COLS> ? COLS : DYNAMIC 
		>;

	return std::conditional_t<  std::is_same_v< TM<T, ROWS, COLS, STOR>, res_t >, decltype(t), res_t  >(t);
}


template
<	template<class, size_t, size_t, v3d::Storing_Order> class TM
,	class T, size_t ROWS, size_t COLS, v3d::Storing_Order STOR
>
decltype(auto) v3d::Eval(TM<T, ROWS, COLS, STOR> &&t)
{
	using res_t
	=	Matrix
		<	std::decay_t<decltype( t(0, 0) )>
		,	trait::is_Static_v<ROWS> ? ROWS : DYNAMIC, trait::is_Static_v<COLS> ? COLS : DYNAMIC 
		>;

	if constexpr(std::is_same_v< TM<T, ROWS, COLS, STOR>, res_t >)
		return std::move(t);
	else
		return res_t( std::move(t) );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class MAT>
bool v3d::is_valid(MAT const &mat) noexcept
{
	if constexpr(trait::has_Matrix_interface_v<MAT>)
		return mat.size() == 0 || !std::isnan( mat(0, 0) );
	else SGM_COMPILE_FAILED(no method to judge it if valid.);
}


template<class MAT>  
bool v3d::Has_Vector_interface(MAT const &mat) noexcept
{
	if constexpr(trait::has_Matrix_interface_v<MAT>)
		return (mat.rows() == 1 || mat.cols() == 1) && mat.size() > 1;
	else
		return false;
}


template<class MAT>
bool v3d::is_Square_Matrix(MAT const &mat) noexcept
{
	if constexpr(trait::has_Matrix_interface_v<MAT>)
		return mat.rows() == mat.cols() && mat.cols() > 1;
	else SGM_COMPILE_FAILED(Not a Matrix.);
}


template<class T, int DP>  
bool v3d::are_almost_same(T t1, T t2)
{
	if constexpr(std::is_integral_v<T>)
		return t1 == t2;
	else if constexpr(std::is_floating_point_v<T>)
		return abs(t1 - t2) < static_cast<T>(_CX_POW<10, DP>()) * std::numeric_limits<T>::epsilon();
	else if constexpr(trait::is_complex_v<T>)
	{
		using real_t = trait::value_t<T>;

		return
		are_almost_same<real_t, DP>(t1.real(), t2.real()) && are_almost_same<real_t, DP>(t1.imag(), t2.imag());
	}
	else SGM_COMPILE_FAILED(no method to compare them.);
}

template<size_t BASE, size_t POWER>  
size_t constexpr v3d::_CX_POW() noexcept
{
	if constexpr(POWER == 0)
		return 1;
	else if constexpr(BASE == 0)
		return 0;
	else
		return BASE*_CX_POW<BASE, POWER - 1>();
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#include <initializer_list>
#include <cmath>


template<class T, size_t ROWS, size_t COLS, v3d::Storing_Order STOR>
class v3d::Matrix
{
private:
	template<class MAT>  friend decltype(auto) v3d::_Mat_implementor(MAT &&m);	

	using _impl_t = _MatrixAdaptor<T, ROWS, COLS, STOR>;

	_impl_t _impl;
	

public:
	using value_type = trait::value_t<_impl_t>;
	static size_t constexpr ROW_SIZE = ROWS, COL_SIZE = COLS;
	static Storing_Order constexpr STORING_ORDER = STOR;


	Matrix() : Matrix(_init_default()){}
	Matrix(size_t const r, size_t const c) : Matrix( Matrix::Zero(r, c) ){}

	Matrix(size_t const n) 
	:	Matrix( Matrix::Zero(n) ){  static_assert(ROWS == 1 || COLS == 1, "this method is only for Vector cass."); }

	Matrix(size_t const r, FixedSize const)
	:	Matrix(r, COLS){  static_assert(!trait::is_Static_v<ROWS>, "CANNOT resize fixed size part."); }

	Matrix(FixedSize const, size_t const c)
	:	Matrix(ROWS, c){  static_assert(!trait::is_Static_v<COLS>, "CANNOT resize fixed size part."); }

	template
	<	class T, size_t _R, size_t _C, Storing_Order _S
	,	class
		=	std::enable_if_t
			<	(!trait::is_Static_v<ROWS> || !trait::is_Static_v<_R> || ROWS == _R)
			&&	(!trait::is_Static_v<COLS> || !trait::is_Static_v<_C> || COLS == _C)
			&&	!std::is_same_v< Matrix<T, _R, _C, _S>, Matrix >
			>
	>
	Matrix(Matrix<T, _R, _C, _S> const &m) 
	:	_impl( _Mat_implementor(m) ){  assert(m.rows() == rows() && m.cols() == cols());  }


	template
	<	class T, size_t _R, size_t _C, Storing_Order _S
	,	class
		=	std::enable_if_t
			<	(!trait::is_Static_v<ROWS> || !trait::is_Static_v<_R> || ROWS == _R)
			&&	(!trait::is_Static_v<COLS> || !trait::is_Static_v<_C> || COLS == _C)
			&&	!std::is_same_v< Matrix<T, _R, _C, _S>, Matrix >
			>
	>
	Matrix(Matrix<T, _R, _C, _S> &&m) noexcept 
	:	_impl(  _Mat_implementor( std::move(m) )  ){  assert(m.rows() == rows() && m.cols() == cols());  }


	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	Matrix(_MatrixAdaptor<Q, _R, _C, _S> &&temp) : _impl( std::move(temp) ){}

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	Matrix(_MatrixAdaptor<Q, _R, _C, _S> const &ma) : _impl(ma){}


	template
	<	class CON
	,	class 
		=	std::enable_if_t
			<	sgm::is_iterable<CON, value_type>::value && (trait::is_Static_v<ROWS> || trait::is_Static_v<COLS>)
			||	sgm::is_iterable<CON>::value && trait::has_Matrix_interface_v< sgm::Deref_t<CON> >
			>
	>	
	Matrix(CON &&con) noexcept(std::is_rvalue_reference_v<CON&&>) 
	:	Matrix(  _init_by_iterable( std::forward<CON>(con) )  ){}


	template<  class Q, class = std::enable_if_t< std::is_convertible_v<Q, T> >  >
	Matrix(std::initializer_list<Q> &&iL) noexcept : Matrix(  _init_by_iterable( std::move(iL) )  ){}

	Matrix(NullMat_t const) noexcept{  _impl.invalidate();  }


	template<   class Q, class = std::enable_if_t<  !std::is_same_v< std::decay_t<Q>, Matrix >  >   >
	auto operator=(Q &&q)-> Matrix&
	{
		if constexpr(sgm::is_iterable<Q>::value)
		{
			using deref_t = sgm::Deref_t<Q>;

			static_assert(std::is_convertible_v<deref_t, value_type> || trait::has_Matrix_interface_v<deref_t>);

			return _iterable_conversion_Helper::substituted_by_iterable( *this, std::forward<Q>(q) );  
		}
		else if constexpr(std::is_same_v< std::decay_t<Q>, NullMat_t >)
			return _impl.invalidate(),  *this;
		else
			return _impl = _Mat_impl<Q>(q),  *this;
	}

	template<class Q>
	auto operator=(std::initializer_list<Q> &&iL) noexcept-> Matrix&
	{
		return _iterable_conversion_Helper::substituted_by_iterable( *this, std::move(iL) );
	}


	operator _impl_t const&() const{  return _impl;  }
	operator _impl_t&(){  return _impl;  }


	auto resize(size_t const i, size_t const j)-> Matrix&
	{
		static_assert(!trait::is_Static_v<ROWS> && !trait::is_Static_v<COLS>, "CANNOT resize fixed size matrix");

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

	template<class Q>  auto operator+(Q &&q) const{  return _impl + _Mat_impl<Q>(q);  }
	template<class Q>  auto operator-(Q &&q) const{  return _impl - _Mat_impl<Q>(q);  }
	template<class Q>  auto operator*(Q &&q) const{  return _impl * _Mat_impl<Q>(q);  }

	template<  class Q, class = std::enable_if_t< trait::is_complexible_v<Q> >  >  
	auto operator/(Q const q) const{  return _impl/q;  }

	template<class Q>  auto operator+=(Q &&q)-> Matrix&{  return _impl += _Mat_impl<Q>(q),  *this;  }
	template<class Q>  auto operator-=(Q &&q)-> Matrix&{  return _impl -= _Mat_impl<Q>(q),  *this;  }
	template<class Q>  auto operator*=(Q &&q)-> Matrix&{  return _impl *= _Mat_impl<Q>(q),  *this;  }
	template<class Q>  auto operator/=(Q &&q)-> Matrix&{  return _impl /= _Mat_impl<Q>(q),  *this;  }

	decltype(auto) row(size_t const i){  return _impl.row(i);  }
	decltype(auto) row(size_t const i) const{  return _impl.row(i);  }
	decltype(auto) col(size_t const j){  return _impl.col(j);  }
	decltype(auto) col(size_t const j) const{  return _impl.col(j);  }

	decltype(auto) block(size_t const i, size_t const j, size_t const rsize, size_t const csize)
	{
		return _impl.block(i, j, rsize, csize);
	}

	decltype(auto) block(size_t const i, size_t const j, size_t const rsize, size_t const csize) const
	{
		return _impl.block(i, j, rsize, csize);
	}

	decltype(auto) head(size_t const s){  assert( Has_Vector_interface(*this) );  return _impl.head(s);  }
	decltype(auto) head(size_t const s) const{  assert( Has_Vector_interface(*this) );  return _impl.head(s);  }
	decltype(auto) tail(size_t const s){  assert( Has_Vector_interface(*this) );  return _impl.tail(s);  }
	decltype(auto) tail(size_t const s) const{  assert( Has_Vector_interface(*this) );  return _impl.tail(s);  }

	auto inv() const{  assert( is_Square_Matrix(*this) );  return _impl.inv();  }
	auto transpose() const{  return _impl.transpose();  }
	auto det() const{  assert( is_Square_Matrix(*this) );  return _impl.det();  }

	auto sqr_norm() const-> T{  return _impl.sqr_norm();  }
	auto norm() const-> T{  return _impl.norm();  }
	auto normalized() const{  return _impl.normalized();  }
	decltype(auto) normalize(){  return _impl.normalize();  }

	auto dot(Matrix const &m) const-> T{  return _impl.dot(m._impl);  }
	auto cross(Matrix const &m) const{  return _impl.cross(m._impl);  }


	template
	<	size_t VSIZE
		=	trait::is_Static_v<ROWS> && ROWS != 1 ? ROWS
		:	trait::is_Static_v<COLS> && COLS != 1 ? COLS
		:	/* otherwise */ DYNAMIC
	>
	auto dyadic(Vector<T, VSIZE> const &q) const
	{
		assert( Has_Vector_interface(*this) && cols() == q.cols() && rows() == q.rows() );

		if constexpr(std::is_same_v< Vector<T, VSIZE>, ColVec<T, VSIZE> >)
			return *this*q.transpose();
		else if constexpr(std::is_same_v< Vector<T, VSIZE>, RowVec<T, VSIZE> >)
			return this->transpose()*q;
		else SGM_COMPILE_FAILED(no method to do dyadic product with .);
	}
	

	auto skew() const-> v3d::Matrix<T, 3, 3>
	{
		static_assert(!trait::is_FixedSizeMat_v<Matrix> || ROWS == 3 || COLS == 3);
		assert( Has_Vector_interface(*this) && size() == 3 );

		T constexpr _0 = T(0);
		T const x = (*this)(0), y = (*this)(1), z = (*this)(2);

		return
		{	_0, -z, y
		,	z, _0, -x
		,	-y, x, _0
		};
	}

	static auto Zero(){  static_assert(trait::is_FixedSizeMat_v<Matrix>);  return _impl_t::Zero();  }
	static auto Zero(size_t const i){  return _impl_t::Zero(i);  }
	static auto Zero(size_t const i, size_t const j){  return _impl_t::Zero(i, j);  }

	static auto Ones(){  static_assert(trait::is_FixedSizeMat_v<Matrix>);  return _impl_t::Ones();  }
	static auto Ones(size_t const i){  return _impl_t::Ones(i);  }
	static auto Ones(size_t const i, size_t const j){  return _impl_t::Ones(i, j);  }

	static auto identity(){  static_assert(trait::is_StrictMat_v<Matrix>);  return _impl_t::identity();  }
	static auto identity(size_t const i){  return _impl_t::identity(i);  }


private:
	template<class CON>	
	static auto _init_by_iterable(CON &&con, Matrix res = {})
	->	Matrix{  return _iterable_conversion_Helper::substituted_by_iterable( res, std::forward<CON>(con) );  }


	static auto _init_default()
	{
		if constexpr(trait::is_FixedSizeMat_v<Matrix>)
			return Matrix::Zero();
		else if constexpr(std::is_default_constructible_v<_impl_t>)
			return _impl_t();
		else SGM_COMPILE_FAILED(no way for default initialization .);
	}
};


template<class T, class U, size_t ROWS, size_t COLS, v3d::Storing_Order STOR, class>
static decltype(auto) operator*(T t, v3d::Matrix<U, ROWS, COLS, STOR> const &m){  return m*t;  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


struct v3d::NullMat_t
{
	using value_type = sgm::None;
	static size_t constexpr ROW_SIZE = UNKNOWN,  COL_SIZE = UNKNOWN;
	static Storing_Order constexpr STORING_ORDER = DefaultStorOrder;
};


namespace v3d{  static NullMat_t constexpr NULLMAT{};  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class T, size_t SIZE>
class v3d::UnitVec : public Vector<T, SIZE>
{
	using _Vec = Vector<T, SIZE>;

public:
	template
	<	class...ARGS
	,	class 
		=	std::enable_if_t
			<	std::is_constructible_v< _Vec, ARGS&&...> && !std::is_integral_v< sgm::First_t<ARGS...> >
			>  
	>
	UnitVec(ARGS&&...args) noexcept(sgm::Check_All<std::is_rvalue_reference>::for_any<ARGS&&...>::value)
	:	_Vec( std::forward<ARGS>(args)... )
	{
		if constexpr( sizeof...(ARGS) != 1 || !trait::is_UnitVec_v< sgm::First_t<ARGS...> > )
			_normalize();  
	}

	template<class Q>
	UnitVec(std::initializer_list<Q> &&iL) noexcept : _Vec( std::move(iL) ){  _normalize();  };

	UnitVec() : UnitVec(Axis<SIZE - 1>()){  static_assert(trait::is_Static_v<SIZE>);  }

	UnitVec(size_t const dim) : _Vec( _Vec::Zero(dim) )
	{
		static_assert(trait::is_Dynamic_v<SIZE>);

		assert(dim != 0);

		_Vec::operator()(dim - 1) = 1;
	}

	UnitVec(NullMat_t const) : UnitVec(_ExemptionTag{}, NULLMAT){}


	template<  class Q, class = std::enable_if_t< std::is_assignable_v<_Vec, Q&&> >  >
	auto operator=(Q &&q) noexcept(std::is_rvalue_reference_v<Q&&>)-> UnitVec&
	{
		static_cast<_Vec&>(*this) = std::forward<Q>(q);

		if constexpr(!trait::is_UnitVec_v<Q> && !std::is_same_v< std::decay_t<Q>, NullMat_t >)
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

	decltype(auto) block(size_t const i, size_t const j, size_t const rsize, size_t const csize) const
	{
		return _Vec::block(i, j, rsize, csize);
	}

	auto sqr_norm() const{  return T(1);  }
	auto norm() const{  return T(1);  }
	auto normalized() const-> UnitVec const&{  return *this;  };
	auto normalize()-> UnitVec&{  return *this;  };


	template<size_t IDX = DYNAMIC>
	static auto Axis(size_t idx = DYNAMIC)-> std::conditional_t< trait::is_Static_v<IDX>, UnitVec const&, UnitVec >
	{
		static_assert(trait::is_Static_v<SIZE>);

		if constexpr(trait::is_Static_v<IDX>)
		{
			static auto const res = [](_Vec v){  return v(IDX) = 1,  Skipped<UnitVec>(v);  }(_Vec::Zero());

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
	friend auto v3d::Skipped(ARGS&&...args) 
	noexcept(sgm::Check_All<std::is_rvalue_reference>::for_any<ARGS&&...>::value);


	template<class...ARGS>
	UnitVec(_ExemptionTag, ARGS&&...args)
	noexcept(sgm::Check_All<std::is_rvalue_reference>::for_any<ARGS&&...>::value) 
	:	_Vec( std::forward<ARGS>(args)... ){}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class T, size_t SIZE, v3d::Storing_Order STOR>
class v3d::OrthogonalMat : public Matrix<T, SIZE, SIZE, STOR>
{
	using _Mat = Matrix<T, SIZE, SIZE, STOR>;

public:
	template
	<	class...ARGS
	,	class 
		=	std::enable_if_t
			<	std::is_constructible_v< _Mat, ARGS&&...> && !std::is_integral_v< sgm::First_t<ARGS...> >
			>  
	>
	OrthogonalMat(ARGS&&...args) noexcept(sgm::Check_All<std::is_rvalue_reference>::for_any<ARGS&&...>::value)
	:	_Mat( std::forward<ARGS>(args)... )
	{
		if constexpr( sizeof...(ARGS) != 1 || !trait::is_OrthogonalMat_v< sgm::First_t<ARGS...> > )
			_orthonormalize();
	}

	template<class Q>
	OrthogonalMat(std::initializer_list<Q> &&iL) noexcept : _Mat( std::move(iL) ){  _orthonormalize();  };

	OrthogonalMat() : _Mat(_Mat::identity()){  static_assert(trait::is_Static_v<SIZE>);  }

	explicit OrthogonalMat(size_t const dim) : _Mat( _Mat::identity(dim) )
	{
		static_assert(trait::is_Dynamic_v<SIZE>);

		assert(dim != 0);
	}

	OrthogonalMat(size_t, size_t) = delete;

	OrthogonalMat(NullMat_t const) : OrthogonalMat(_ExemptionTag{}, NULLMAT){}


	template<  class Q, class = std::enable_if_t< std::is_assignable_v<_Mat, Q&&> >  >
	auto operator=(Q &&q) noexcept(std::is_rvalue_reference_v<Q&&>)-> OrthogonalMat&
	{
		static_cast<_Mat&>(*this) = std::forward<Q>(q);

		if constexpr(!trait::is_OrthogonalMat_v<Q> && !std::is_same_v< std::decay_t<Q>, NullMat_t >)
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

	template<class Q>  auto operator*(Q const &m) const{  return mat()*m;  }
	template<class Q>  decltype(auto) operator*=(Q const &m){  return *this = *this * m;  }

	auto operator()(size_t const idx) const{  return mat()(idx);  }
	auto operator()(size_t const i, size_t const j) const{  return mat()(i, j);  }

	decltype(auto) row(size_t const idx) const{  return mat().row(idx);  }
	decltype(auto) col(size_t const idx) const{  return mat().col(idx);  }

	decltype(auto) block(size_t const i, size_t const j, size_t const rsize, size_t const csize) const
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
	friend auto v3d::Skipped(ARGS&&...args) 
	noexcept(sgm::Check_All<std::is_rvalue_reference>::for_any<ARGS&&...>::value);


	template<class...ARGS>
	OrthogonalMat(_ExemptionTag, ARGS&&...args) 
	noexcept(sgm::Check_All<std::is_rvalue_reference>::for_any<ARGS&&...>::value) 
	:	_Mat( std::forward<ARGS>(args)... ){}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#include "Sigma\Functional_iterator\Functional_iterator.hpp"


template<class MAT, class>
auto v3d::as_col_space(MAT &mat){  return sgm::as_iterable( _VecSpace_Helper<MAT, 1>(&mat), mat.cols() );  }


template<class MAT, class>
auto v3d::as_row_space(MAT &mat){  return sgm::as_iterable( _VecSpace_Helper<MAT, 2>(&mat), mat.rows() );  }


template<class MAT, class>
auto v3d::as_icol_space(MAT &mat){  return sgm::as_iterable( _VecSpace_Helper<MAT, -1>(&mat), mat.cols() );  }


template<class MAT, class>
auto v3d::as_irow_space(MAT &mat){  return sgm::as_iterable( _VecSpace_Helper<MAT, -2>(&mat), mat.rows() );  }


template<class MAT, int DIREC>
struct v3d::_VecSpace_Helper
{
	_VecSpace_Helper(MAT *p) : _p(p){}

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
		else SGM_COMPILE_FAILED(_VecSpace_Helper got wrong flag number DIREC .);
	}

private:
	MAT *_p;
};
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


#include "_Hamilton_by_Eigen.hpp"


#endif // end of #ifndef _V3D_HAMILTON_