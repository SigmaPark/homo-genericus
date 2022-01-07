/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#include "Eigen/Dense"


template
<	class T, std::size_t ROWS, std::size_t COLS, s3d::Storing_Order STOR, bool IS_REAL_SYMMETRIC
>
class s3d::Eigen_Decomposition<T, ROWS, COLS, STOR, IS_REAL_SYMMETRIC>::_impl_t
{
private:
	using _Mat_t = typename _Seed_Matrix<T, ROWS, COLS, STOR>::egn_Mat_t;


	Selective_t
	<	IS_REAL_SYMMETRIC
	,	Eigen::SelfAdjointEigenSolver<_Mat_t>
	,	Eigen::EigenSolver<_Mat_t> 
	> 
		_solver;
	
	bool _has_computed_eigenvectors = false;

	using _Elem_t = typename Decay_t<decltype(_solver.eigenvectors())>::value_type;


public:
	template<class MAT, class FS>
	void operator()(MAT&& m, FS&&)
	{
		_has_computed_eigenvectors = !Has_Flag<flag::Value_Only, FS>::value;

		if constexpr(IS_REAL_SYMMETRIC)
			_solver.compute
			(	_Mat_implementor( Forward<MAT>(m) )
			,	_has_computed_eigenvectors 
				?	Eigen::ComputeEigenvectors 
				:	Eigen::EigenvaluesOnly
			);
		else
			_solver.compute(  _Mat_implementor( Forward<MAT>(m) ), _has_computed_eigenvectors  );
	}


	auto size() const-> size_t{  return _solver.eigenvalues().size();  }
	
	decltype(auto) eigenval(size_t const idx) const
	{
		return _solver.eigenvalues()( static_cast<int>(idx) );  
	}

	auto eigenvec(size_t const idx) const-> s3d::_MatrixAdaptor<_Elem_t, ROWS, 1, STOR>
	{
		assert(_has_computed_eigenvectors);

		return _solver.eigenvectors().col( static_cast<int>(idx) );
	}


	auto diagmat() const
	{
		size_t const dim = _solver.eigenvectors().cols();

		s3d::Matrix<_Elem_t, ROWS, COLS, STOR> res 
		=	s3d::Matrix<_Elem_t, ROWS, COLS, STOR>::Zero(dim, dim);

		for(size_t i = 0;  i < dim;  ++i)
			res(i, i) = _solver.eigenvalues()(i);

		return res;
	}


	auto basemat() const
	->	s3d::_MatrixAdaptor<_Elem_t, ROWS, COLS, STOR>{  return _solver.eigenvectors();  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, std::size_t ROWS, std::size_t COLS, s3d::Storing_Order STOR>
class s3d::Singular_Value_Decomposition<T, ROWS, COLS, STOR>::_impl_t
{
private:
	DynamicMat<T, STOR> _U{}, _V{};
	Vector<T> _values{};

	using _MA_t = _MatrixAdaptor<T, DYNAMIC, DYNAMIC, STOR>;


public:
	template<class MAT, class FS>
	void operator()(MAT&& m, FS&& fs)
	{
		Eigen::BDCSVD< Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> > const svd
		(	_Mat_implementor( Forward<MAT>(m) ), _bit_flag<FS>()
		);

		_values = _MA_t(svd.singularValues());

		assert
		(	s3d::trait::is_Sorted
			(	_values.cdata(), _values.cdata() + _values.size()
			,	[](T const _1, T const _2){  return _1 > _2;  }
			)
		);

		if constexpr(Has_Flag<flag::Value_Only, FS>::value)
			_clear();		// clear _U and _V but leave _values unchanged. 
		else
			_calc_UV( svd, Forward<FS>(fs) );
	}


	decltype(auto) Umat() const{  return _U;  }
	decltype(auto) Vmat() const{  return _V;  }
	
	decltype(auto) Ucol(size_t const idx) const{  return Umat().col(idx);  }
	decltype(auto) Vcol(size_t const idx) const{  return Vmat().col(idx);  }

	auto nof_singularvals() const-> size_t{  return _values.size();  }
	auto singularval(size_t const idx) const-> T{  return _values(idx);  }


	auto diagmat() const-> DynamicMat<T, STOR>
	{
		bool const is_full_mode
		=	(	Umat().size() != 0 && Vmat().size() != 0 
			&&	is_Square_Matrix(Umat()) && is_Square_Matrix(Vmat())
			);

		size_t const
			r = is_full_mode ? Umat().cols() : nof_singularvals(),
			c = is_full_mode ? Vmat().rows() : nof_singularvals();

		DynamicMat<T, STOR> res = DynamicMat<T, STOR>::Zero(r, c);

		for(size_t i = 0;  i < nof_singularvals();  ++i)
			res(i, i) = singularval(i);

		return res;
	}


private:
	template<class FS>
	static unsigned constexpr _bit_flag()
	{
		if constexpr(Has_Flag<flag::Value_Only, FS>::value)
			return 0;
		else
		{
			unsigned constexpr
				MODE = Has_Flag<flag::FullMat, FS>::value ? 1 : 0,
				UB 
				=	Has_Flag<flag::VMat_Only, FS>::value ? 0
				:	MODE == 1 ? Eigen::ComputeFullU
				:	/* otherwise */ Eigen::ComputeThinU,
				VB
				=	Has_Flag<flag::UMat_Only, FS>::value ? 0
				:	MODE == 1 ? Eigen::ComputeFullV
				:	/* otherwise */ Eigen::ComputeThinV;

			return UB | VB;
		}
	}


	template<class M = sgm::None const>
	void _clear(M& m = {})
	{
		if constexpr(is_None<M>::value)
			_clear(_U),  _clear(_V);
		else
			m = Decay_t<decltype(m)>{};
	}


	template<class FS, class SVD>
	void _calc_UV(SVD const& svd, FS&& fs)
	{
		if constexpr(Has_Flag<flag::VMat_Only, FS>::value)
			_clear(_U);
		else
			_U = _MA_t(svd.matrixU());


		if constexpr(Has_Flag<flag::UMat_Only, FS>::value)
			_clear(_V);
		else
			_V = _MA_t(svd.matrixV());


		if constexpr(Has_Satisfying_Flag<flag::is_Truncated, FS>::value)
			Satisfying_Flag<flag::is_Truncated>(fs).cut(_values, _U, _V);
	}
};
//========//========//========//========//=======#//========//========//========//========//=======#


template<>
class s3d::_Least_Square_Solution_Helper<s3d::Solving_Mode::SVD>
{
	friend struct Least_Square_Problem;


	template<class AMAT, class BVEC>
	static auto calc(AMAT const& amat, BVEC const& bvec)
	{
		auto& A = _Mat_implementor(amat);
		auto& b = _Mat_implementor(bvec);

		return A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b).eval();
	}
};


template<>
class s3d::_Least_Square_Solution_Helper<s3d::Solving_Mode::QR>
{
	friend struct Least_Square_Problem;


	template<class AMAT, class BVEC>
	static auto calc(AMAT const& amat, BVEC const& bvec)
	{
		auto& A = _Mat_implementor(amat);
		auto& b = _Mat_implementor(bvec);

		return A.colPivHouseholderQr().solve(b).eval();
	}
};