/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once

#pragma warning(push)
/*	Case when scalar 0 divides a Matrix / Vector is protected 
*	on s3d::_MatrixAdaptor::operator/(value_type) .
*/
#pragma warning(disable : 4723)
#include "Eigen/Dense"
#pragma warning(pop)


namespace s3d
{
	
	template
	<	class T, size_t ROWS, size_t COLS, Storing_Order STOR
	,	bool = trait::is_complexible< Decay_t<T> >::value
	>
	struct _Seed_Matrix;

	struct _Seed_Helper;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, std::size_t ROWS, std::size_t COLS, s3d::Storing_Order STOR>
struct s3d::_Seed_Matrix<T, ROWS, COLS, STOR, true>
{
private:
	template<size_t N>
	static int constexpr _int_cast()
	{
		if constexpr( N > static_cast<size_t>(std::numeric_limits<int>::max()) )
			return Eigen::Dynamic;
		else
			return static_cast<int>(N);
	}

	static int constexpr 
		egnRows = _int_cast<ROWS>(),
		egnCols = _int_cast<COLS>(),
		egnStor
		=	ROWS == 1  ?  Eigen::RowMajor
		:	COLS == 1  ?  Eigen::ColMajor
		:	STOR == Storing_Order::COL_FIRST  ?  Eigen::ColMajor
		:	/* otherwise */ Eigen::RowMajor;


public:
	using egn_Mat_t = Eigen::Matrix<T, egnRows, egnCols, egnStor>;

	using value_type = Decay_t<decltype( Mock<egn_Mat_t>()(0, 0) )>;
};


template<class T, std::size_t ROWS, std::size_t COLS, s3d::Storing_Order STOR>
struct s3d::_Seed_Matrix<T, ROWS, COLS, STOR, false>
{
	using egn_Mat_t = T;

	using value_type = Decay_t<decltype( Mock<egn_Mat_t>()(0, 0) )>;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct s3d::_Seed_Helper : Unconstructible
{
private:
	SGM_HAS_MEMFUNC(_seed);

	template<class Q>
	static decltype(auto) _calc(Q&& q)
	{
		if constexpr(Has_MemFunc__seed<Q>::value)
			return Move_if< is_Rvalue_Reference<Q&&>::value >(q._seed());
		else if constexpr(trait::is_Matrix<Q>::value)
			return _calc(  _Mat_implementor( Forward<Q>(q) )  );
		else
			return Forward<Q>(q);
	}


public:
	template<class Q>
	static decltype(auto) seed(Referenceless_t<Q>& t){  return _calc(t);  }

	template<class Q>
	static decltype(auto) seed(Referenceless_t<Q>&& t) noexcept{  return _calc( Move(t) );  }	


	template<size_t ROWS, size_t COLS, Storing_Order STOR = DefaultStorOrder, class TEMP>
	static auto lazy(TEMP&& temp) noexcept(is_Rvalue_Reference<TEMP&&>::value)
	->	_MatrixAdaptor< Referenceless_t<TEMP>, ROWS, COLS, STOR >
	{
		return Forward<TEMP>(temp);  
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, std::size_t ROWS, std::size_t COLS, s3d::Storing_Order STOR>
struct s3d::_MatrixAdaptor : public _Seed_Matrix<T, ROWS, COLS, STOR>::egn_Mat_t
{
private:
	using _SeedMat_t = typename _Seed_Matrix<T, ROWS, COLS, STOR>::egn_Mat_t;
	using _Helper = _Seed_Helper;

	template<size_t S>  
	static size_t constexpr _1_or_D = S == 1 ? size_t(1) : DYNAMIC;


public:
	using value_type = typename _SeedMat_t::value_type;
	static size_t constexpr STT_ROW_SIZE = ROWS,  STT_COL_SIZE = COLS;
	static Storing_Order constexpr STORING_ORDER = STOR;


	template
	<	class...ARGS, class = Enable_if_t< Has_Operator_New<_SeedMat_t, ARGS&&...>::value >  
	>
	_MatrixAdaptor(ARGS&&...args) : _SeedMat_t( Forward<ARGS>(args)... ){}

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	_MatrixAdaptor(_MatrixAdaptor<Q, _R, _C, _S> const& ma) 
	:	_SeedMat_t( _Helper::seed<decltype(ma)>(ma) ){}

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	_MatrixAdaptor(_MatrixAdaptor<Q, _R, _C, _S> &&ma) noexcept 
	:	_SeedMat_t( _Helper::seed<decltype(ma)>(ma) ){}

	_MatrixAdaptor(NullMat_t const) noexcept{  invalidate();  }


	template
	<	class RHS
	,	class 
		=	Enable_if_t
			<	Has_Operator_Copy_Assignment<_SeedMat_t, RHS&&>::value
			||	Has_Operator_Move_Assignment<_SeedMat_t, RHS&&>::value
			>
	>
	auto operator=(RHS&& rhs) noexcept(is_Rvalue_Reference<RHS&&>::value)
	->	_MatrixAdaptor&{  return _seed() = Forward<RHS>(rhs),  *this;  }

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	auto operator=(_MatrixAdaptor<Q, _R, _C, _S> const& ma)
	->	_MatrixAdaptor&{  return _seed() = _Helper::seed<decltype(ma)>(ma),  *this;  }

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	auto operator=(_MatrixAdaptor<Q, _R, _C, _S>&& ma) noexcept
	->	_MatrixAdaptor&{  return _seed() = _Helper::seed<decltype(ma)>(ma),  *this;  }

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	auto operator=(s3d::Matrix<Q, _R, _C, _S> const& m)
	->	_MatrixAdaptor&{  return *this = _Mat_implementor(m);  }

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	auto operator=(s3d::Matrix<Q, _R, _C, _S>&& m) noexcept
	->	_MatrixAdaptor&{  return *this = _Mat_implementor( Move(m) );  }

	auto operator=(NullMat_t const)-> _MatrixAdaptor&{  return invalidate();  }


	auto rows() const-> size_t{  return _SeedMat_t::rows();  }
	auto cols() const-> size_t{  return _SeedMat_t::cols();  }
	auto size() const-> size_t{  return _SeedMat_t::size();  }

	auto data() &-> value_type*{  return _SeedMat_t::data();  }
	auto data() const&-> value_type const*{  return _SeedMat_t::data();  }


	decltype(auto) operator()(size_t const idx)
	{
		return _SeedMat_t::operator()( static_cast<int>(idx) );  
	}
	
	decltype(auto) operator()(size_t const idx) const
	{
		return _SeedMat_t::operator()( static_cast<int>(idx) );  
	}

	decltype(auto) operator()(size_t const i, size_t const j)
	{
		return _SeedMat_t::operator()( static_cast<int>(i), static_cast<int>(j) );
	}

	decltype(auto) operator()(size_t const i, size_t const j) const
	{
		return _SeedMat_t::operator()( static_cast<int>(i), static_cast<int>(j) );
	}


	decltype(auto) row(size_t const i)
	{
		return _Helper::template lazy<1, COLS>(  _SeedMat_t::row( static_cast<int>(i) )  );  
	}

	decltype(auto) row(size_t const i) const
	{ 
		return _Helper::template lazy<1, COLS>(  _SeedMat_t::row( static_cast<int>(i) )  );  
	}

	decltype(auto) col(size_t const j)
	{
		return _Helper::template lazy<ROWS, 1>(  _SeedMat_t::col( static_cast<int>(j) )  );  
	}
	
	decltype(auto) col(size_t const j) const
	{
		return _Helper::template lazy<ROWS, 1>(  _SeedMat_t::col( static_cast<int>(j) )  );  
	}


	decltype(auto) block(size_t const i, size_t const j, size_t const rsize, size_t const csize)
	{
		return 
		_Helper::template lazy<DYNAMIC, DYNAMIC>
		(	_SeedMat_t::block
			(	static_cast<int>(i), static_cast<int>(j)
			,	static_cast<int>(rsize), static_cast<int>(csize)
			)
		);
	}

	decltype(auto) block
	(	size_t const i, size_t const j, size_t const rsize, size_t const csize
	)	const
	{
		return 
		_Helper::template lazy<DYNAMIC, DYNAMIC>
		(	_SeedMat_t::block
			(	static_cast<int>(i), static_cast<int>(j)
			,	static_cast<int>(rsize), static_cast<int>(csize)
			)
		);
	}


	decltype(auto) head(size_t const s)
	{
		size_t constexpr _R = _1_or_D<ROWS>,  _C = _1_or_D<COLS>;

		return _Helper::template lazy<_R, _C>( _SeedMat_t::head(s) );   
	}
	
	decltype(auto) head(size_t const s) const
	{
		size_t constexpr _R = _1_or_D<ROWS>,  _C = _1_or_D<COLS>;
		
		return _Helper::template lazy<_R, _C>( _SeedMat_t::head(s) );  
	}
	
	decltype(auto) tail(size_t const s)
	{
		size_t constexpr _R = _1_or_D<ROWS>, _C = _1_or_D<COLS>;

		return _Helper::template lazy<_R, _C>( _SeedMat_t::tail(s) );  
	}
	
	decltype(auto) tail(size_t const s) const
	{
		size_t constexpr _R = _1_or_D<ROWS>, _C = _1_or_D<COLS>;

		return _Helper::template lazy<_R, _C>( _SeedMat_t::tail(s) );  
	}


	auto inv() const{  return _Helper::template lazy<ROWS, COLS>(_SeedMat_t::inverse());  }
	auto transpose() const{  return _Helper::template lazy<COLS, ROWS>(_SeedMat_t::adjoint());  }
	auto det() const-> value_type{  return _SeedMat_t::determinant();  }

	auto dot(_SeedMat_t const& q) const-> value_type{  return _SeedMat_t::dot(q);  }

	auto cross(_SeedMat_t const& q) const
	{
		return _Helper::template lazy<ROWS, COLS>( _SeedMat_t::cross(q) );  
	}


	auto sqr_norm() const-> value_type{  return _SeedMat_t::squaredNorm();  }
	auto norm() const-> value_type{  return _SeedMat_t::norm();  }
	auto normalized() const{  return *this / norm();  }
	auto normalize()-> _MatrixAdaptor&{  return *this = normalized();  }

	auto invalidate()-> _MatrixAdaptor&
	{
		auto at_least_1_f = [](size_t const n) constexpr{  return n == 0 ? 1 : n;  };

		_SeedMat_t::resize( at_least_1_f(rows()), at_least_1_f(cols()) );

		return (*this)(0, 0) = NaN<value_type>,  *this;
	}


	auto operator+() const{  return _Helper::template lazy<ROWS, COLS>(+_seed());  }
	auto operator-() const{  return _Helper::template lazy<ROWS, COLS>(-_seed());  }

	template<class Q>  
	auto operator+(Q&& q) const
	{
		return _Helper::template lazy<ROWS, COLS>( _seed() + _Helper::seed<Q>(q) );  
	}
	
	template<class Q>  
	auto operator-(Q&& q) const
	{
		return _Helper::template lazy<ROWS, COLS>( _seed() - _Helper::seed<Q>(q) );  
	}


	template
	<	class Q
	,	class _E = Decay_t< decltype(Mock<value_type>()/Mock<Q>()) >
	,	class ELEM = std::conditional_t< std::numeric_limits<_E>::is_integer, float, _E >
	>
	auto operator/(Q const q) const-> _MatrixAdaptor<ELEM, ROWS, COLS, STOR>
	{
		assert(size() != 0);

		if(  Are_almost_same<ELEM, 1>( static_cast<ELEM>(q), 0 )  )
			return NULLMAT;
		else
			return _seed() / q;  
	}


	template<class Q>  
	auto operator*(Q&& q) const
	{
		using RHS = Decay_t<Q>;
		
		if constexpr(trait::is_complexible<RHS>::value)
			return _Helper::template lazy<ROWS, COLS>(_seed()*q);
		else
			return 
			_Helper::template lazy<ROWS, RHS::STT_COL_SIZE>( _seed() * _Helper::seed<Q>(q) );
	}


	template<class Q>  
	auto operator+=(Q&& q)-> _MatrixAdaptor&{  return *this = *this + Forward<Q>(q);  }
	
	template<class Q>  
	auto operator-=(Q&& q)-> _MatrixAdaptor&{  return *this = *this - Forward<Q>(q);  }
	
	template<class Q>  
	auto operator*=(Q&& q)-> _MatrixAdaptor&{  return *this = *this * Forward<Q>(q);  }
	
	template<class Q>  
	auto operator/=(Q&& q)-> _MatrixAdaptor&{  return *this = *this / Forward<Q>(q);  }


	static auto Zero(){  return _Helper::template lazy<ROWS, COLS>(_SeedMat_t::Zero());  }

	static auto Zero(size_t const i)
	{
		return _Helper::template lazy<ROWS, COLS>( _SeedMat_t::Zero(i) );  
	}

	static auto Zero(size_t const i, size_t const j)
	{
		return _Helper::template lazy<ROWS, COLS>( _SeedMat_t::Zero(i, j) );  
	}


	static auto Ones(){  return _Helper::template lazy<ROWS, COLS>(_SeedMat_t::Ones());  }
	
	static auto Ones(size_t const i)
	{
		return _Helper::template lazy<ROWS, COLS>( _SeedMat_t::Ones(i) );  
	}

	static auto Ones(size_t const i, size_t const j)
	{
		return _Helper::template lazy<ROWS, COLS>( _SeedMat_t::Ones(i, j) );  
	}


	static auto identity(){  return _Helper::template lazy<ROWS, COLS>(_SeedMat_t::Identity());  }
	
	static auto identity(size_t const i)
	{
		return _Helper::template lazy<ROWS, COLS>( _SeedMat_t::Identity(i, i) );  
	}


private:
	friend struct _Seed_Helper;

	auto _seed()-> _SeedMat_t&{  return *this;  }
	auto _seed() const-> _SeedMat_t const&{  return *this;  }
};


template
<	class T, class U, std::size_t ROWS, std::size_t COLS, s3d::Storing_Order STOR
,	class = sgm::Enable_if_t< s3d::trait::is_complexible<T>::value >
>
static decltype(auto) operator*(T t, s3d::_MatrixAdaptor<U, ROWS, COLS, STOR> const& ma)
{
	return ma*t;  
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#
