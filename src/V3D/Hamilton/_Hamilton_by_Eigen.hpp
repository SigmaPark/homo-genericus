#pragma once

#pragma warning(push)
//	Case when scalar 0 divides a Matrix/Vector is protected on v3d::_MatrixAdaptor::operator/(value_type) .
#pragma warning(disable : 4723)
#include "Eigen\Dense"
#pragma warning(pop)


namespace v3d
{
	
	template
	<	class T, size_t ROWS, size_t COLS, Storing_Order STOR, bool = trait::is_complexible_v< std::decay_t<T> >
	>
	struct _Seed_Matrix;

	class _Seed_Helper;

}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


template<class T, size_t ROWS, size_t COLS, v3d::Storing_Order STOR>
struct v3d::_Seed_Matrix<T, ROWS, COLS, STOR, true>
{
	using type 
	=	Eigen::Matrix
		<	T, static_cast<int>(ROWS), static_cast<int>(COLS)
		,	static_cast<int>
			(	ROWS == 1 ? Storing_Order::ROW_FIRST
			:	COLS == 1 ? Storing_Order::COL_FIRST
			:	/* otherwise */ STOR
			)
		>;

	using value_type = std::decay_t<decltype( std::declval<type>()(0, 0) )>;
};


template<class T, size_t ROWS, size_t COLS, v3d::Storing_Order STOR>
struct v3d::_Seed_Matrix<T, ROWS, COLS, STOR, false>
{
	using type = T;  

	using value_type = std::decay_t<decltype( std::declval<type>()(0, 0) )>;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


class v3d::_Seed_Helper : sgm::No_Making
{
private:
	SGM_HAS_MEMFUNC(seed);

	template<class Q>
	static decltype(auto) _calc(Q &&q)
	{
		if constexpr(Has_MemFunc_seed<Q>::value)
			return sgm::Move_if< std::is_rvalue_reference_v<Q&&> >(q._seed());
		else if constexpr(trait::is_v3dMat_v<Q>)
			return _calc( _Mat_impl<Q>(q) );
		else
			return std::forward<Q>(q);
	}


public:
	template<class Q>
	static decltype(auto) seed(std::remove_reference_t<Q> &t){  return _calc(t);  }

	template<class Q>
	static decltype(auto) seed(std::remove_reference_t<Q> &&t) noexcept{  return _calc( std::move(t) );  }	


	template<size_t ROWS, size_t COLS, Storing_Order STOR = DefaultStorOrder, class TEMP>
	static auto lazy(TEMP &&temp) noexcept(std::is_rvalue_reference_v<TEMP&&>)
	->	_MatrixAdaptor< std::remove_reference_t<TEMP>, ROWS, COLS, STOR >{  return std::forward<TEMP>(temp);  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


template<class T, size_t ROWS, size_t COLS, v3d::Storing_Order STOR>
struct v3d::_MatrixAdaptor : public _Seed_Matrix<T, ROWS, COLS, STOR>::type
{
private:
	using _SeedMat_t = typename _Seed_Matrix<T, ROWS, COLS, STOR>::type;
	using _Helper = _Seed_Helper;

	template<size_t S>  static int constexpr _1_or_D = static_cast<int>(S == 1 ? 1 : DYNAMIC);


public:
	using value_type = trait::value_t<_SeedMat_t>;
	static size_t constexpr ROW_SIZE = ROWS,  COL_SIZE = COLS;
	static Storing_Order constexpr STORING_ORDER = STOR;


	template<  class...ARGS, class = std::enable_if_t< std::is_constructible_v<_SeedMat_t, ARGS&&...> >  >
	_MatrixAdaptor(ARGS&&...args) : _SeedMat_t( std::forward<ARGS>(args)... ){}

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	_MatrixAdaptor(_MatrixAdaptor<Q, _R, _C, _S> const &ma) : _SeedMat_t( _Helper::seed<decltype(ma)>(ma) ){}

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	_MatrixAdaptor(_MatrixAdaptor<Q, _R, _C, _S> &&ma) noexcept : _SeedMat_t( _Helper::seed<decltype(ma)>(ma) ){}

	_MatrixAdaptor(NullMat_t const) noexcept{  invalidate();  };


	template<  class RHS, class = std::enable_if_t< std::is_assignable_v<_SeedMat_t, RHS&&> >  >
	auto operator=(RHS &&rhs) noexcept(std::is_rvalue_reference_v<RHS&&>)-> _MatrixAdaptor&
	{
		return _seed() = std::forward<RHS>(rhs),  *this;
	}

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	auto operator=(_MatrixAdaptor<Q, _R, _C, _S> const &ma)-> _MatrixAdaptor&
	{
		return _seed() = _Helper::seed<decltype(ma)>(ma),  *this;
	}

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	auto operator=(_MatrixAdaptor<Q, _R, _C, _S> &&ma) noexcept-> _MatrixAdaptor&
	{
		return _seed() = _Helper::seed<decltype(ma)>(ma),  *this;
	}

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	auto operator=(v3d::Matrix<Q, _R, _C, _S> const &m)-> _MatrixAdaptor&{  return *this = _Mat_implementor(m);  }

	template<class Q, size_t _R, size_t _C, Storing_Order _S>
	auto operator=(v3d::Matrix<Q, _R, _C, _S> &&m) noexcept
	->	_MatrixAdaptor&{  return *this = _Mat_implementor( std::move(m) );  }

	auto operator=(NullMat_t const)-> _MatrixAdaptor&{  return invalidate();  }


	auto rows() const-> size_t{  return _SeedMat_t::rows();  }
	auto cols() const-> size_t{  return _SeedMat_t::cols();  }
	auto size() const-> size_t{  return _SeedMat_t::size();  }

	auto data() &-> value_type*{  return _SeedMat_t::data();  }
	auto data() const&-> value_type const*{  return _SeedMat_t::data();  }


	decltype(auto) operator()(size_t const idx){  return _SeedMat_t::operator()( static_cast<int>(idx) );  }
	decltype(auto) operator()(size_t const idx) const{  return _SeedMat_t::operator()( static_cast<int>(idx) );  }

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
		return _Helper::lazy<1, COLS>(  _SeedMat_t::row( static_cast<int>(i) )  );  
	}

	decltype(auto) row(size_t const i) const
	{ 
		return _Helper::lazy<1, COLS>(  _SeedMat_t::row( static_cast<int>(i) )  );  
	}

	decltype(auto) col(size_t const j)
	{
		return _Helper::lazy<ROWS, 1>(  _SeedMat_t::col( static_cast<int>(j) )  );  
	}
	
	decltype(auto) col(size_t const j) const
	{
		return _Helper::lazy<ROWS, 1>(  _SeedMat_t::col( static_cast<int>(j) )  );  
	}


	decltype(auto) block(size_t const i, size_t const j, size_t const rsize, size_t const csize)
	{
		return 
		_Helper::lazy<DYNAMIC, DYNAMIC>
		(	_SeedMat_t::block
			(	static_cast<int>(i), static_cast<int>(j), static_cast<int>(rsize), static_cast<int>(csize)
			)
		);
	}

	decltype(auto) block(size_t const i, size_t const j, size_t const rsize, size_t const csize) const
	{
		return 
		_Helper::lazy<DYNAMIC, DYNAMIC>
		(	_SeedMat_t::block
			(	static_cast<int>(i), static_cast<int>(j), static_cast<int>(rsize), static_cast<int>(csize)
			)
		);
	}


	decltype(auto) head(size_t const s)
	{
		return _Helper::lazy< _1_or_D<ROWS>, _1_or_D<COLS> >( _SeedMat_t::head(s) );   
	}
	
	decltype(auto) head(size_t const s) const
	{  
		return _Helper::lazy< _1_or_D<ROWS>, _1_or_D<COLS> >( _SeedMat_t::head(s) );  
	}
	
	decltype(auto) tail(size_t const s)
	{
		return _Helper::lazy< _1_or_D<ROWS>, _1_or_D<COLS> >( _SeedMat_t::tail(s) );  
	}
	
	decltype(auto) tail(size_t const s) const
	{
		return _Helper::lazy< _1_or_D<ROWS>, _1_or_D<COLS> >( _SeedMat_t::tail(s) );  
	}


	auto inv() const{  return _Helper::lazy<ROWS, COLS>(_SeedMat_t::inverse());  }
	auto transpose() const{  return _Helper::lazy<COLS, ROWS>(_SeedMat_t::adjoint());  }
	auto det() const-> value_type{  return _SeedMat_t::determinant();  }

	auto dot(_SeedMat_t const &q) const-> value_type{  return _SeedMat_t::dot(q);  }
	auto cross(_SeedMat_t const &q) const{  return _Helper::lazy<ROWS, COLS>( _SeedMat_t::cross(q) );  }

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


	auto operator+() const{  return _Helper::lazy<ROWS, COLS>(+_seed());  }
	auto operator-() const{  return _Helper::lazy<ROWS, COLS>(-_seed());  }

	template<class Q>  
	auto operator+(Q &&q) const{  return _Helper::lazy<ROWS, COLS>( _seed() + _Helper::seed<Q>(q) );  }
	
	template<class Q>  
	auto operator-(Q &&q) const{  return _Helper::lazy<ROWS, COLS>( _seed() - _Helper::seed<Q>(q) );  }


	template
	<	class Q
	,	class _E = std::decay_t< decltype(std::declval<value_type>()/std::declval<Q>()) >
	,	class elem_t = std::conditional_t< std::is_integral_v<_E>, float, _E >
	,	class res_t = _MatrixAdaptor<elem_t, ROWS, COLS, STOR>
	>
	auto operator/(Q const q) const-> res_t
	{
		assert(size() != 0);

		if(  are_almost_same<elem_t, 1>( static_cast<elem_t>(q), 0 )  )
			return NULLMAT;
		else
			return _seed() / q;  
	}


	template<class Q>  
	auto operator*(Q &&q) const
	{
		using RHS = std::decay_t<Q>;

		if constexpr(trait::is_complexible_v<RHS>)
			return _Helper::lazy<ROWS, COLS>(_seed()*q);
		else
			return _Helper::lazy<ROWS, RHS::COL_SIZE>( _seed() * _Helper::seed<Q>(q) );
	}


	template<class Q>  auto operator+=(Q &&q)-> _MatrixAdaptor&{  return *this = *this + std::forward<Q>(q);  }
	template<class Q>  auto operator-=(Q &&q)-> _MatrixAdaptor&{  return *this = *this - std::forward<Q>(q);  }
	template<class Q>  auto operator*=(Q &&q)-> _MatrixAdaptor&{  return *this = *this * std::forward<Q>(q);  }
	template<class Q>  auto operator/=(Q &&q)-> _MatrixAdaptor&{  return *this = *this / std::forward<Q>(q);  }


	static auto Zero(){  return _Helper::lazy<ROWS, COLS>(_SeedMat_t::Zero());  }
	static auto Zero(size_t const i){  return _Helper::lazy<ROWS, COLS>( _SeedMat_t::Zero(i) );  }
	static auto Zero(size_t const i, size_t const j){  return _Helper::lazy<ROWS, COLS>( _SeedMat_t::Zero(i, j) );  }

	static auto Ones(){  return _Helper::lazy<ROWS, COLS>(_SeedMat_t::Ones());  }
	static auto Ones(size_t const i){  return _Helper::lazy<ROWS, COLS>( _SeedMat_t::Ones(i) );  }
	static auto Ones(size_t const i, size_t const j){  return _Helper::lazy<ROWS, COLS>( _SeedMat_t::Ones(i, j) );  }

	static auto identity(){  return _Helper::lazy<ROWS, COLS>(_SeedMat_t::Identity());  }
	static auto identity(size_t const i){  return _Helper::lazy<ROWS, COLS>( _SeedMat_t::Identity(i, i) );  }


private:
	friend class _Helper;

	auto _seed()-> _SeedMat_t&{  return *this;  }
	auto _seed() const-> _SeedMat_t const&{  return *this;  }
};


template
<	class T, class U, size_t ROWS, size_t COLS, v3d::Storing_Order STOR
,	class = std::enable_if_t< v3d::trait::is_complexible_v<T> >
>
static decltype(auto) operator*(T t, v3d::_MatrixAdaptor<U, ROWS, COLS, STOR> const &ma){  return ma*t;  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------
