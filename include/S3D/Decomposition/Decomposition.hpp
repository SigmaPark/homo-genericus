/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _S3D_DECOMPOSITION_
#define _S3D_DECOMPOSITION_


#include "../Hamilton/Hamilton.hpp"
#include "SGM/Flag_Set/Flag_Set.hpp"


namespace s3d
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
//========//========//========//========//=======#//========//========//========//========//=======#


namespace s3d::flag
{
	
	enum class Value_Only; // for ED, SVD

	enum class Real_Symmetric; // for ED

	// for SVD
	enum class UMat_Only;  enum class VMat_Only;

	enum class FullMat;
	enum class ThinMat;


	template<class T = float>  
	struct Truncated;
	
	template<class T = float>  
	struct RelativelyTrunc;
	
	template<class T = float>  
	struct AbsolutelyTrunc;


	SGM_USER_DEFINED_TYPE_CHECK
	(	class FLAG
	,	Truncated, <FLAG>
	);

}
//========//========//========//========//=======#//========//========//========//========//=======#


namespace s3d::trait
{

	template<class T>  
	struct is_Decomposition;
	

	template<class ITR, class COMP>
	static auto is_Sorted(ITR bi, ITR const ei, COMP&& comp)-> bool;


}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class ITR, class COMP>
auto s3d::trait::is_Sorted(ITR bi, ITR const ei, COMP&& comp)-> bool
{
	if(bi != ei)
		for( auto itr = Next(bi);  itr != ei;  ++bi,  ++itr )
			if( !comp(*bi, *itr) )
				return false;

	return true;
}


class s3d::Decomposition{  protected:  Decomposition() = default;  };


template<class T>  
struct s3d::trait::is_Decomposition 
:	Boolean<  is_inherited< Decomposition, Decay_t<T> >::value  >{};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template
<	class T, std::size_t ROWS, std::size_t COLS, s3d::Storing_Order STOR, bool IS_REAL_SYMMETRIC
>
class s3d::Eigen_Decomposition : public Decomposition
{
private:
	class _impl_t;

	_impl_t _impl;

	using _Default_Flag_Set = Flag_Set<>;

public:
	static_assert(!IS_REAL_SYMMETRIC || trait::is_real<T>::value);

	template
	<	class MAT, class FS = _Default_Flag_Set
	,	class = Enable_if_t< trait::Has_Matrix_interface<MAT>::value && is_Flag_Set<FS>::value >  
	>
	Eigen_Decomposition(MAT&& m, FS&& fs = {}){  (*this)( Forward<MAT>(m), Forward<FS>(fs) );  }


	template
	<	class MAT, class FS = _Default_Flag_Set
	,	class = Enable_if_t< trait::Has_Matrix_interface<MAT>::value && is_Flag_Set<FS>::value >  
	>
	auto operator()(MAT&& m, FS&& fs = {})->	Eigen_Decomposition&
	{
		return _impl( Forward<MAT>(m), Forward<FS>(fs) ),  *this;
	}

	auto size() const{  return _impl.size();  }
	decltype(auto) eigenval(size_t const idx) const{  return _impl.eigenval(idx);  }
	decltype(auto) eigenvec(size_t const idx) const{  return _impl.eigenvec(idx);  }

	decltype(auto) diagmat() const{  return _impl.diagmat();  }
	decltype(auto) basemat() const{  return _impl.basemat();  }
};


namespace s3d
{

	template< class MAT, class FS = Flag_Set<>, class M = Decay_t<MAT> >
	Eigen_Decomposition(MAT&&, FS&& = {})
	->	Eigen_Decomposition
		<	typename M::value_type, M::STT_ROW_SIZE, M::STT_COL_SIZE, M::STORING_ORDER
		,	Has_Flag<flag::Real_Symmetric, FS>::value
		>;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, std::size_t ROWS, std::size_t COLS, s3d::Storing_Order STOR>
class s3d::Singular_Value_Decomposition : public Decomposition
{
private:
	class _impl_t;

	_impl_t _impl;

	using _Default_Flag_Set = Flag_Set<flag::ThinMat>;

public:
	template
	<	class MAT, class FS = _Default_Flag_Set
	,	class = Enable_if_t< trait::Has_Matrix_interface<MAT>::value && is_Flag_Set<FS>::value >  
	>
	Singular_Value_Decomposition(MAT&& m, FS&& fs = {})
	{
		(*this)( Forward<MAT>(m), Forward<FS>(fs) );  
	}


	template
	<	class MAT, class FS = _Default_Flag_Set
	,	class = Enable_if_t< trait::Has_Matrix_interface<MAT>::value && is_Flag_Set<FS>::value >  
	>
	auto operator()(MAT&& m, FS&& fs = {})-> Singular_Value_Decomposition&
	{  
		return _impl( Forward<MAT>(m), Forward<FS>(fs) ),  *this;
	}


	decltype(auto) Ucol(size_t const idx) const{  return _impl.Ucol(idx);  }
	decltype(auto) Vcol(size_t const idx) const{  return _impl.Vcol(idx);  }

	auto nof_singularvals() const-> size_t{  return _impl.nof_singularvals();  }
	auto singularval(size_t const idx) const-> T{  return _impl.singularval(idx);  }
	
	decltype(auto) diagmat() const{  return _impl.diagmat();  }
	decltype(auto) Umat() const{  return _impl.Umat();  }
	decltype(auto) Vmat() const{  return _impl.Vmat();  }
};


namespace s3d
{

	template< class MAT, class FS = Flag_Set<flag::ThinMat>, class M = Decay_t<MAT> >
	Singular_Value_Decomposition(MAT&&, FS&& = {})
	->	Singular_Value_Decomposition
		<	typename M::value_type, M::STT_ROW_SIZE, M::STT_COL_SIZE, M::STORING_ORDER
		>;

}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T>
struct s3d::flag::Truncated
{
protected:
	template<Storing_Order STOR>
	static void _cut
	(	size_t const nof_valid, Vector<T>& values
	,	DynamicMat<T, STOR>& U, DynamicMat<T, STOR>& V
	)
	{
		Vector<T> _values(nof_valid);
		DynamicMat<T, STOR> _U(U.rows(), nof_valid), _V(V.rows(), nof_valid);

		for(size_t idx = 0;  idx < nof_valid;  ++idx)
			_values(idx) = values(idx),
			_U.col(idx) = U.col(idx),  _V.col(idx) = V.col(idx);

		values = Move(_values),  U = Move(_U),  V = Move(_V);
	}


	static auto _nof_valid(Vector<T> const& values, T const cutoff_value)-> size_t
	{
		auto deceasing_order_f = [](T const _1, T const _2){  return _1 > _2;  };
		T const *const p0 = values.cdata(), *const p1 = p0 + values.size();

		assert( s3d::trait::is_Sorted(p0, p1, deceasing_order_f) );

		return _Upper_bound(p0, p1, cutoff_value, deceasing_order_f) - p0;
	}


private:
	template<class ITR, class Q, class COMP>
	static auto _Upper_bound(ITR bi, ITR const ei, Q const& val, COMP&& comp)-> ITR
	{
		for(auto count = Difference(bi, ei);  count > 0;)
		{
			auto itr = bi;
			auto const step = count/2;

			itr = Next(itr, step);

			if( !comp(val, *itr) )
				bi = ++itr,
				count -= step + 1;
			else
				count = step;
		}

		return bi;
	}
};


template<class T>
struct s3d::flag::RelativelyTrunc : Truncated<T>
{
	RelativelyTrunc(T const cr) : cutoff_ratio(cr){}


	template<Storing_Order STOR>
	void cut(Vector<T>& values, DynamicMat<T, STOR>& U, DynamicMat<T, STOR>& V) const
	{
		assert( cutoff_ratio >= T(0) && cutoff_ratio < T(1) );

		size_t const nof_valid = Truncated<T>::_nof_valid( values, cutoff_ratio*values(0) );

		Truncated<T>::_cut(nof_valid, values, U, V);
	}


	T cutoff_ratio;
};


template<class T>
struct s3d::flag::AbsolutelyTrunc : Truncated<T>
{
	AbsolutelyTrunc(T const cv) : cutoff_value(cv){}


	template<Storing_Order STOR>
	void cut(Vector<T>& values, DynamicMat<T, STOR>& U, DynamicMat<T, STOR>& V) const
	{
		size_t const nof_valid = Truncated<T>::_nof_valid(values, cutoff_value);

		Truncated<T>::_cut(nof_valid, values, U, V);
	}


	T cutoff_value;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


enum class s3d::Solving_Mode{SVD, QR};


struct s3d::Least_Square_Problem : Unconstructible
{
	template
	<	Solving_Mode SM = Solving_Mode::SVD, class AMAT, class BVEC
	,	class A_t = Decay_t<AMAT>
	,	class XVEC 
		=	_MatrixAdaptor
			<	trait::value_t<A_t>
			,	A_t::STT_COL_SIZE
			,	1
			,	Storing_Order::COL_FIRST 
			>
	>
	static auto solution(AMAT const& A, BVEC const& b)-> XVEC
	{
		assert( b.cols() == 1 && A.rows() == b.rows() );

		return _Least_Square_Solution_Helper<SM>::calc(A, b);
	}
};


#include "_Decomposition_by_Eigen.hpp"


#endif // end of #ifndef _S3D_DECOMPOSITION_