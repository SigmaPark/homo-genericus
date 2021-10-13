#pragma once

#ifndef _V3D_DECOMPOSITION_
#define _V3D_DECOMPOSITION_


namespace v3d
{

	class Decomposition;

	
	template<class T, size_t ROWS, size_t COLS, Storing_Order STOR, bool IS_REAL_SYMMETRIC>
	class Eigen_Decomposition;
	
	template<class T, size_t ROWS, size_t COLS, Storing_Order STOR>
	class Singular_Value_Decomposition;

	
	struct Least_Square_Problem;
	enum class Solving_Mode;

	template<Solving_Mode>
	class _Least_Square_Solution_Helper;

}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


namespace v3d::flag
{
	
	enum class Value_Only; // for ED, SVD

	enum class Real_Symmetric; // for ED

	// for SVD
	enum class UMat_Only;  enum class VMat_Only;

	enum class FullMat;
	enum class ThinMat;

	template<class T = float>  struct Truncated;
	template<class T = float>  struct RelativelyTrunc;
	template<class T = float>  struct AbsolutelyTrunc;

	template<class F>  struct is_Truncated;

}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


#include "..\Hamilton\Hamilton.hpp"


namespace v3d::trait
{

	template<class T>  struct is_Decomposition;
	template<class T>  static bool constexpr is_Decomposition_v = is_Decomposition<T>::value;

}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


template<class T>  
struct v3d::trait::is_Decomposition 
:	sgm::Boolean_type<  std::is_base_of_v< v3d::Decomposition, std::decay_t<T> >  >{};


class v3d::Decomposition{  protected:  Decomposition() = default;  };
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#include "Sigma\Flags\Flags.hpp"


template<class T, size_t ROWS, size_t COLS, v3d::Storing_Order STOR, bool IS_REAL_SYMMETRIC>
class v3d::Eigen_Decomposition : public Decomposition
{
private:
	class _impl_t;

	_impl_t _impl;

	using _Default_FlagSet = sgm::FlagSet<>;

public:
	static_assert(!IS_REAL_SYMMETRIC || trait::is_real_v<T>);

	template
	<	class MAT, class FS = _Default_FlagSet
	,	class = std::enable_if_t< trait::has_Matrix_interface_v<MAT> && sgm::is_FlagSet_v<FS> >  
	>
	Eigen_Decomposition(MAT &&m, FS &&fs = {}){  (*this)( std::forward<MAT>(m), std::forward<FS>(fs) );  }


	template
	<	class MAT, class FS = _Default_FlagSet
	,	class = std::enable_if_t< trait::has_Matrix_interface_v<MAT> && sgm::is_FlagSet_v<FS> >  
	>
	auto operator()(MAT&& m, FS&& fs = {})->	Eigen_Decomposition&
	{
		return _impl( std::forward<MAT>(m), std::forward<FS>(fs) ),  *this;
	}

	auto size() const{  return _impl.size();  }
	decltype(auto) eigenval(size_t const idx) const{  return _impl.eigenval(idx);  }
	decltype(auto) eigenvec(size_t const idx) const{  return _impl.eigenvec(idx);  }

	decltype(auto) diagmat() const{  return _impl.diagmat();  }
	decltype(auto) basemat() const{  return _impl.basemat();  }
};


namespace v3d
{
	template< class MAT, class FS = sgm::FlagSet<>, class M = std::decay_t<MAT> >
	Eigen_Decomposition(MAT&&, FS&& = {})
	->	Eigen_Decomposition
		<	typename M::value_type, M::ROW_SIZE, M::COL_SIZE, M::STORING_ORDER
		,	sgm::is_Selected_Flag<flag::Real_Symmetric, FS>::value
		>;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class T, size_t ROWS, size_t COLS, v3d::Storing_Order STOR>
class v3d::Singular_Value_Decomposition : public Decomposition
{
private:
	class _impl_t;

	_impl_t _impl;

	using _Default_FlagSet = sgm::FlagSet<flag::ThinMat>;

public:
	template
	<	class MAT, class FS = _Default_FlagSet
	,	class = std::enable_if_t< trait::has_Matrix_interface_v<MAT> && sgm::is_FlagSet_v<FS> >  
	>
	Singular_Value_Decomposition(MAT &&m, FS &&fs = {}){  (*this)( std::forward<MAT>(m), std::forward<FS>(fs) );  }


	template
	<	class MAT, class FS = _Default_FlagSet
	,	class = std::enable_if_t< trait::has_Matrix_interface_v<MAT> && sgm::is_FlagSet_v<FS> >  
	>
	auto operator()(MAT &&m, FS &&fs = {})-> Singular_Value_Decomposition&
	{  
		return _impl( std::forward<MAT>(m), std::forward<FS>(fs) ),  *this;
	}


	decltype(auto) Ucol(size_t const idx) const{  return _impl.Ucol(idx);  }
	decltype(auto) Vcol(size_t const idx) const{  return _impl.Vcol(idx);  }

	auto nof_singularvals() const-> size_t{  return _impl.nof_singularvals();  }
	auto singularval(size_t const idx) const-> T{  return _impl.singularval(idx);  }
	
	decltype(auto) diagmat() const{  return _impl.diagmat();  }
	decltype(auto) Umat() const{  return _impl.Umat();  }
	decltype(auto) Vmat() const{  return _impl.Vmat();  }
};


namespace v3d
{
	template< class MAT, class FS = sgm::FlagSet<flag::ThinMat>, class M = std::decay_t<MAT> >
	Singular_Value_Decomposition(MAT&&, FS&& = {})
	->	Singular_Value_Decomposition<typename M::value_type, M::ROW_SIZE, M::COL_SIZE, M::STORING_ORDER>;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class F>
struct v3d::flag::is_Truncated
{
private:
	template<class...> /* Declaration Only */  static auto _calc(void*)-> sgm::False_t;
	template<class Q> /* Declaration Only */  static auto _calc(Truncated<Q>*)-> sgm::True_t;

public:
	static bool constexpr value = decltype( _calc(std::declval< std::decay_t<F>* >()) )::value;
	using type = sgm::Boolean_type<value>;
};


#include <algorithm>


template<class T>
struct v3d::flag::Truncated
{
protected:
	template<Storing_Order STOR>
	static void _cut(size_t const nof_valid, Vector<T> &values, DynamicMat<T, STOR> &U, DynamicMat<T, STOR> &V)
	{
		Vector<T> _values(nof_valid);
		DynamicMat<T, STOR> _U(U.rows(), nof_valid), _V(V.rows(), nof_valid);

		for(size_t idx = 0;  idx < nof_valid;  ++idx)
			_values(idx) = values(idx),
			_U.col(idx) = U.col(idx),  _V.col(idx) = V.col(idx);

		values = std::move(_values),  U = std::move(_U),  V = std::move(_V);
	}

	static auto _nof_valid(Vector<T> const &values, T const cutoff_value)-> size_t
	{
		auto deceasing_order_f = [](T const _1, T const _2){  return _1 > _2;  };
		T const *const p0 = values.cdata(), *const p1 = p0 + values.size();

		assert( std::is_sorted(p0, p1, deceasing_order_f) );

		return std::upper_bound(p0, p1, cutoff_value, deceasing_order_f) - p0;
	}
};


template<class T>
struct v3d::flag::RelativelyTrunc : Truncated<T>
{
	RelativelyTrunc(T const cr) : cutoff_ratio(cr){}

	template<Storing_Order STOR>
	void cut(Vector<T> &values, DynamicMat<T, STOR> &U, DynamicMat<T, STOR> &V) const
	{
		assert( cutoff_ratio >= T(0) && cutoff_ratio < T(1) );

		size_t const nof_valid = Truncated<T>::_nof_valid( values, cutoff_ratio*values(0) );

		Truncated<T>::_cut(nof_valid, values, U, V);
	}

	T cutoff_ratio;
};


template<class T>
struct v3d::flag::AbsolutelyTrunc : Truncated<T>
{
	AbsolutelyTrunc(T const cv) : cutoff_value(cv){}

	template<Storing_Order STOR>
	void cut(Vector<T> &values, DynamicMat<T, STOR> &U, DynamicMat<T, STOR> &V) const
	{
		size_t const nof_valid = Truncated<T>::_nof_valid(values, cutoff_value);

		Truncated<T>::_cut(nof_valid, values, U, V);
	}

	T cutoff_value;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


enum class v3d::Solving_Mode{SVD, QR};


struct v3d::Least_Square_Problem : sgm::No_Making
{

	template
	<	Solving_Mode SM = Solving_Mode::SVD, class AMAT, class BVEC
	,	class A_t = std::decay_t<AMAT>
	,	class XVEC = _MatrixAdaptor< trait::value_t<A_t>, A_t::COL_SIZE, 1, Storing_Order::COL_FIRST >
	>
	static auto solution(AMAT const &A, BVEC const &b)-> XVEC
	{
		assert( b.cols() == 1 && A.rows() == b.rows() );

		return _Least_Square_Solution_Helper<SM>::calc(A, b);
	}
};


#include "_Decomposition_by_Eigen.hpp"


#endif // end of #ifndef _V3D_DECOMPOSITION_