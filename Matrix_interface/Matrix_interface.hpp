#pragma once

#ifndef _SGM_MATRIX_INTERFACE_
#define _SGM_MATRIX_INTERFACE_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif

#include "..\interface_Traits\interface_Traits17.hpp"
#include "..\idiom\idiom.hpp"
#include <cassert>


#ifndef _READ_WRITE_MEMFUNC
	#define _READ_WRITE_MEMFUNC(signature, ...)		\
		signature const __VA_ARGS__		\
		signature __VA_ARGS__
#else
	#error _READ_WRITE_MEMFUNC was already defined somewhere else.
#endif
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm::mxi
{
	
	struct MxSize : No_Making 
	{ 
		static size_t constexpr DYNAMIC = ULLONG_MAX;
		static auto constexpr TEMPORARY = DYNAMIC - 1;
		
		template<size_t N> static bool constexpr is_dynamic_v = N == DYNAMIC;
		template<size_t N> static bool constexpr is_temporary_v = N == TEMPORARY;
	
		template<size_t N>
		static bool constexpr is_static_v = !(is_dynamic_v<N> || is_temporary_v<N>);
	};

	using MxSize_t = size_t;
	using index_t = size_t;


	struct MxTraits;


	template<class elem_t, MxSize_t ROW_SIZE, MxSize_t COLUMN_SIZE>
	class Mx_implementation;	// should be defined on other file


	template<class elem_t, MxSize_t SIZE>
	class Vt_implementation;	// should be defined on other file


	template
	<	class elem_t = float
	,	MxSize_t ROW_SIZE = MxSize::DYNAMIC, MxSize_t COLUMN_SIZE = MxSize::DYNAMIC
	>
	class Matrix;


	template<class elem_t = float, MxSize_t SIZE = MxSize::DYNAMIC>
	class Vector;


	class _Temporary;



	template
	<	MxSize_t S1 = MxSize::DYNAMIC, MxSize_t S2 = MxSize::DYNAMIC
	,	class T
	,	class = std::enable_if_t< MxTraits::is_mxiMatrix_v<T> || MxTraits::is_mxiVector_v<T> >
	>
	static decltype(auto) fetch(T&& t);


	template
	<	class VECS
	,	class
		=	std::enable_if_t
			<	is_iterable<VECS>::value && MxTraits::is_mxiVector_v< Deref_t<VECS> >
			>
	>
	static auto column_space(VECS&& vecs);

	 
	template
	<	class VECS
	,	class
		=	std::enable_if_t
			<	is_iterable<VECS>::value && MxTraits::is_mxiVector_v< Deref_t<VECS> >
			>
	>
	static auto row_space(VECS&& vecs);


	template<class T = float, MxSize_t S = MxSize::DYNAMIC>
	class UnitVector;


	template<class elem_t = float,MxSize_t N = MxSize::DYNAMIC>
	class OrthonormalMatrix;

}
//========//========//========//========//=======#//========//========//========//========//=======#


class sgm::mxi::_Temporary : No_Making
{
private:
	template<class, MxSize_t, MxSize_t>		friend class Matrix;
	template<class, MxSize_t>				friend class Vector;


	template<class T>
	static auto Mx(T&& t)
	{
		return Matrix<T, MxSize::TEMPORARY, MxSize::TEMPORARY>( std::move(t) );
	}

	template<class T>
	static auto Vt(T&& t)
	{
		return Vector<T, MxSize::TEMPORARY>( std::move(t) );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::mxi::MxTraits : No_Making
{
private:
	template<class> struct is_mxiMatrix : False_t{};

	template<class T, MxSize_t R, MxSize_t C>
	struct is_mxiMatrix< Matrix<T, R, C> > : True_t{};


	template<class> struct is_mxiVector : False_t{};

	template<class T, MxSize_t S>
	struct is_mxiVector< Vector<T, S> > : True_t{};


	template<class> struct is_Dynamic{  SGM_COMPILE_FAILED(is not a Matrix);  };

	template<class T, MxSize_t R, MxSize_t C>
	struct is_Dynamic< Matrix<T, R, C> > 
	:	Boolean_type< MxSize::is_dynamic_v<R> || MxSize::is_dynamic_v<C> >
	{};


	template<class> struct is_Static{  SGM_COMPILE_FAILED(is not a Matrix);  };

	template<class T, MxSize_t R, MxSize_t C>
	struct is_Static< Matrix<T, R, C> > 
	:	Boolean_type< MxSize::is_static_v<R>&& MxSize::is_static_v<C> >
	{};


	template<class>
	struct is_UnitVector : False_t{};
	
	template<class T, size_t N>
	struct is_UnitVector< sgm::mxi::UnitVector<T, N> > : True_t{};
	
	
	template<class>
	struct is_OrthonormalMatrix : False_t{};
	
	template<class T, size_t N>
	struct is_OrthonormalMatrix< sgm::mxi::OrthonormalMatrix<T, N> > : True_t{};


public:
	template<class M>
	static bool constexpr is_mxiMatrix_v = is_mxiMatrix< std::decay_t<M> >::value;

	template<class V>
	static bool constexpr is_mxiVector_v = is_mxiVector< std::decay_t<V> >::value;

	template<class M>
	static bool constexpr is_DynamicMx_v = is_Dynamic< std::decay_t<M> >::value;

	template<class M> 
	static bool constexpr is_StaticMx_v = is_Static< std::decay_t<M> >::value;

	template<class M>
	static bool constexpr is_Temporary_v
	=	[]() constexpr
		{	
			if constexpr(is_mxiMatrix_v<M>)
				return MxSize::is_temporary_v<M::ROW_SIZE>;
			else if constexpr(is_mxiVector_v<M>)
				return MxSize::is_temporary_v<M::VEC_SIZE>;
		}();


	template<class T>
	static bool constexpr is_UnitVector_v = is_UnitVector< std::decay_t<T> >::value;

	template<class T>
	static bool constexpr is_OrthonormalMatrix_v = is_OrthonormalMatrix< std::decay_t<T> >::value;

};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, sgm::mxi::MxSize_t R, sgm::mxi::MxSize_t C>
class sgm::mxi::Matrix : Mx_implementation<T, R, C>
{
	using impl_t = Mx_implementation<T, R, C>;


public:
	using elem_t = typename impl_t::elem_t;
	static MxSize_t constexpr ROW_SIZE = R, COL_SIZE = C;


	Matrix() = default;


	template
	<	class = std::enable_if_t< !MxSize::is_static_v<R> && !MxSize::is_static_v<C> > 
	>
	Matrix(MxSize_t r, MxSize_t c) : Matrix(){  this->resize(r, c);  }


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
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiMatrix_v<Q> >  >
	auto operator=(Q&& q)-> Matrix&
	{
		impl_t::core() = q.core();

		return *this;
	}


	template<class Q, MxSize_t _S>
	auto operator=(Vector<Q, _S>) = delete;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	_READ_WRITE_MEMFUNC
	(	SGM_MACROPACK( decltype(auto) operator()(index_t i, index_t j) )
	,	{
			return impl_t::element(i, j);
		}
	)


	auto rows() const-> MxSize_t{  return impl_t::rows();  }
	auto cols() const-> MxSize_t{  return impl_t::cols();  }

	auto size() const-> MxSize_t{  return impl_t::size();  }
	auto data(){  return impl_t::data();  }
	auto data() const{  return impl_t::data();  }


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
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	_READ_WRITE_MEMFUNC
	(	SGM_MACROPACK( auto block(index_t i, index_t j, MxSize_t r, MxSize_t c) )
	,	{
			return _Temporary::Mx( impl_t::block(i, j, r, c) );
		}
	)

	_READ_WRITE_MEMFUNC
	(	auto row(index_t idx), {  return _Temporary::Mx( impl_t::row(idx) );  }
	)

	_READ_WRITE_MEMFUNC
	(	auto col(index_t idx), {  return _Temporary::Mx(  impl_t::col(idx)  );  }
	)


	_READ_WRITE_MEMFUNC
	(	auto transposed(), {  return _Temporary::Mx(impl_t::transposed());  }
	)

	
	auto det() const{  return impl_t::det();  }
	auto inversed() const{  return _Temporary::Mx(impl_t::inversed());  }
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	auto operator-() const{  return _Temporary::Mx(-impl_t::core());  }

	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiMatrix_v<Q> >  >
	auto operator+(Q&& q) const{  return _Temporary::Mx(impl_t::core() + q.core());  }

	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiMatrix_v<Q> >  >
	auto operator+=(Q&& q)-> Matrix&{  return *this = *this + std::forward<Q>(q);  }

	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiMatrix_v<Q> >  >
	auto operator-(Q&& q) const{  return _Temporary::Mx(impl_t::core() - q.core());  }

	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiMatrix_v<Q> >  >
	auto operator-=(Q&& q)-> Matrix&{  return *this = *this - std::forward<Q>(q);  }

	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiMatrix_v<Q> >  >
	auto operator*(Q&& q) const{  return _Temporary::Mx(impl_t::core() * q.core());  }

	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiMatrix_v<Q> >  >
	auto operator*=(Q&& q)-> Matrix&{  return *this = *this * std::forward<Q>(q);  }

	template<  class S, class = std::enable_if_t< std::is_scalar_v<S> >  >
	auto operator*(S s) const{  return _Temporary::Mx(impl_t::core() * s);  }

	template<  class S, class = std::enable_if_t< std::is_scalar_v<S> >  >
	auto operator*=(S s) const{  return *this = *this * s;  }

	template<  class S, class = std::enable_if_t< std::is_scalar_v<S> >  >
	auto operator/(S s) const{  return _Temporary::Mx(impl_t::core() / s);  }

	template<  class S, class = std::enable_if_t< std::is_scalar_v<S> >  >
	auto operator/=(S s) const{  return *this = *this / s;  }
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<  class = std::enable_if_t< !MxTraits::is_StaticMx_v<Matrix> || R==C >  >
	static auto identity(){  return _Temporary::Mx(impl_t::identity());  }

	template
	<	class = std::enable_if_t< MxSize::is_dynamic_v<R> && MxSize::is_dynamic_v<C> >
	>
	static auto identity(MxSize_t size)
	{
		return _Temporary::Mx( impl_t::identity(size) );  
	}


	template<  class = std::enable_if_t< MxTraits::is_StaticMx_v<Matrix> >  >
	static auto zero(){  return _Temporary::Mx(impl_t::zero());  }

	template
	<	class = std::enable_if_t< MxSize::is_dynamic_v<R> && MxSize::is_dynamic_v<C> >
	>
	static auto zero(MxSize_t r, MxSize_t c)
	{
		return _Temporary::Mx( impl_t::zero(r, c) );
	}

};// end of Matrix class


template
<	class S, class T, sgm::mxi::MxSize_t R, sgm::mxi::MxSize_t C
,	class = std::enable_if_t< std::is_scalar_v<S> >
>
static auto operator*(S s, sgm::mxi::Matrix<T, R, C> const& m){  return m * s;  }


template<class VECS, class>
auto sgm::mxi::column_space(VECS&& vecs)
{
	using elem_t = typename std::decay_t< Deref_t<VECS> >::elem_t;

	Matrix<elem_t> res(vecs.begin()->size(), vecs.size());

	for
	(	auto[itr, c] = Dual_iteration( vecs.begin(), MxSize_t(0) )
	;	c < res.cols()
	;	res.col(c) = itr->colVec(),  c++,  itr++
	);
		
	return res;
}


template<class VECS, class>
auto sgm::mxi::row_space(VECS&& vecs)
{
	using elem_t = typename std::decay_t< Deref_t<VECS> >::elem_t;

	Matrix<elem_t> res(vecs.size(), vecs.begin()->size());

	for
	(	auto[itr, r] = Dual_iteration( vecs.begin(), MxSize_t(0) )
	;	r < res.rows()
	;	res.row(r) = itr->rowVec(),  r++,  itr++
	);
	
	return res;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, sgm::mxi::MxSize_t S>
class sgm::mxi::Vector : Vt_implementation<T, S>
{
	using impl_t = Vt_implementation<T, S>;

public:
	using elem_t = typename impl_t::elem_t;
	static MxSize_t constexpr VEC_SIZE = S;


	Vector() = default;


	template<  class Q, class = std::enable_if_t< std::is_scalar_v<Q> >  >
	Vector(std::initializer_list<Q>&& iL) : impl_t( std::move(iL) ){}


	template<class Q>
	Vector(Q&& q) : impl_t( std::forward<Q>(q) ){}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiVector_v<Q> >  >
	auto operator=(Q&& q)-> Vector&
	{
		impl_t::core() = q.core();

		return *this;
	}


	template<class Q, MxSize_t _R, MxSize_t _C>
	auto operator=(Matrix<Q, _R, _C>) = delete;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	_READ_WRITE_MEMFUNC
	(	decltype(auto) operator()(index_t idx), {  return impl_t::element(idx);  }
	)

	auto size() const-> MxSize_t{  return impl_t::size();  }

	decltype(auto) core() const	{  return impl_t::core();  }


	auto resize(MxSize_t size)-> Vector&
	{
		static_assert(MxSize::is_dynamic_v<S>, "static size Matrix cannot be resized.");

		assert
		(	(MxSize::is_dynamic_v<S> || size == S)  && L"static size cannot be modified.\n"
		);

		impl_t::resize(size);

		return *this;
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	_READ_WRITE_MEMFUNC
	(	auto head(MxSize_t n), {  return _Temporary::Vt( impl_t::head(n) );  }
	)

	_READ_WRITE_MEMFUNC
	(	auto tail(MxSize_t n), {  return _Temporary::Vt( impl_t::tail(n) );  }
	)

	_READ_WRITE_MEMFUNC
	(	auto rowVec(), {  return _Temporary::Mx( impl_t::rowVec() );  }
	)

	_READ_WRITE_MEMFUNC
	(	auto colVec(), {  return _Temporary::Mx( impl_t::colVec() );  }
	)


	template<MxSize_t L = 2>
	auto norm() const{  return impl_t::template norm<L>();  }

	auto normalized() const
	{
		assert(norm() > 0.000001 && L"cannot normalize zero vector.\n");

		return _Temporary::Vt(impl_t::normalized());
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	auto operator-() const{  return _Temporary::Vt(-impl_t::core());  }

	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiVector_v<Q> >  >
	auto operator+(Q&& q) const{  return _Temporary::Vt(impl_t::core() + q.core());  }

	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiVector_v<Q> >  >
	auto operator+=(Q&& q)-> Vector&{  return *this = *this + std::forward<Q>(q);  }

	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiVector_v<Q> >  >
	auto operator-(Q&& q) const{  return _Temporary::Vt(impl_t::core() - q.core());  }

	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiVector_v<Q> >  >
	auto operator-=(Q&& q)-> Vector&{  return *this = *this - std::forward<Q>(q);  }

	template<  class U, class = std::enable_if_t< std::is_scalar_v<U> >  >
	auto operator*(U u) const{  return _Temporary::Vt(impl_t::core() * u);  }

	template<  class U, class = std::enable_if_t< std::is_scalar_v<U> >  >
	auto operator*=(U u)-> Vector&{  return *this = *this * u;  }

	template<  class U, class = std::enable_if_t< std::is_scalar_v<U> >  >
	auto operator/(U u) const{  return _Temporary::Vt(impl_t::core() / u);  }

	template<  class U, class = std::enable_if_t< std::is_scalar_v<U> >  >
	auto operator/=(U u)-> Vector&{  return *this = *this / u;  }

	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiVector_v<Q> >  >
	auto dot(Q&& q) const{  return impl_t::dot(q.core());  }

	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiVector_v<Q> >  >
	auto cross(Q&& q) const{  return _Temporary::Vt( impl_t::cross(q.core()) );  }

	template<  class Q, class = std::enable_if_t< MxTraits::is_mxiVector_v<Q> >  >
	auto dyad(Q&& q) const{  return _Temporary::Mx( impl_t::dyad(q.core()) );  }
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<  class = std::enable_if_t< MxSize::is_static_v<S> >  >
	static auto zero(){  return _Temporary::Vt(impl_t::zero());  }

	template<  class = std::enable_if_t< MxSize::is_dynamic_v<S> >  >
	static auto zero(MxSize_t s){  return _Temporary::Vt( impl_t::zero(s) );  }

	template<  class = std::enable_if_t< MxSize::is_static_v<S> >  >
	static auto ones(){  return _Temporary::Vt(impl_t::ones());  }

	template<  class = std::enable_if_t< MxSize::is_dynamic_v<S> >  >
	static auto ones(MxSize_t s){  return _Temporary::Vt( impl_t::ones(s) );  }

};// end of class Vector



template
<	class S, class T, sgm::mxi::MxSize_t SIZE, class = std::enable_if_t< std::is_scalar_v<S> >  
>
static auto operator*(S s, sgm::mxi::Vector<T, SIZE> const& v){  return v * s;  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, sgm::mxi::MxSize_t S>
class sgm::mxi::UnitVector
{
	using Vec_t = Vector<T, S>;


public:
	UnitVector() : _vec(Vec_t::zero())
	{
		static_assert(MxSize::is_static_v<S>);

		_vec(0) = 1;
	}


	UnitVector(size_t dim) : _vec( Vec_t::zero(dim) ){  _vec(0) = 1;  }


	template<class Q>
	UnitVector(Q&& q) : _vec(  _ctor( std::forward<Q>(q) )  )
	{
		if constexpr(!MxTraits::is_UnitVector_v<Q>)
			_normalize_myself();
	}


	template<  class Q, class = std::enable_if_t< std::is_scalar_v<Q> >  >
	UnitVector(std::initializer_list<Q>&& iL) : _vec( std::move(iL) )
	{
		_normalize_myself();
	}


	template
	<	class Q
	,	class = std::enable_if_t<  !std::is_same_v< std::decay_t<Q>, UnitVector >  >
	>
	auto operator=(Q&& q)-> UnitVector&
	{
		_vec = _ctor( std::forward<Q>(q) );

		return _normalize_myself();
	}


	auto vec() const-> Vec_t const&{  return _vec;  }

	auto size() const{  return vec().size();  }
	decltype(auto) data() const{  return vec().data();  }

	decltype(auto) operator()(index_t idx) const{  return vec()(idx);  }
	decltype(auto) head(MxSize_t n) const{  return vec().head(n);  }
	decltype(auto) tail(MxSize_t n) const{  return vec().tail(n);  }

	decltype(auto) rowVec() const{  return vec().rowVec();  }
	decltype(auto) colVec() const{  return vec().colVec();  }

	auto operator-() const
	{
		auto cpy = *this;
		
		cpy._vec *= -1;

		return cpy;
	}


	template<class Q> 
	decltype(auto) operator*(Q&& q) const{  return vec() * std::forward<Q>(q);  }

	template<class Q>
	decltype(auto) operator/(Q&& q) const{  return vec() / std::forward<Q>(q);  }


	template<class Q>
	decltype(auto) dot(Q&& q) const{  return vec().dot( std::forward<Q>(q) );  }

	template<class Q>
	decltype(auto) cross(Q&& q) const{  return vec().cross( std::forward<Q>(q) );  }

	template<class Q>
	decltype(auto) dyad(Q&& q) const{  return vec().dyad( std::forward<Q>(q) );  }


private:
	Vec_t _vec;


	auto _normalize_myself()-> UnitVector&
	{
		_vec = _vec.normalized();

		return *this;
	}


	template<class Q>
	static decltype(auto) _ctor(Q&& q)
	{
		if constexpr(MxTraits::is_UnitVector_v<Q>)
			return q.vec();
		else
			return std::forward<Q>(q);
	}

};// end of class UnitVector


template
<	class S, class T
,	sgm::mxi::MxSize_t SIZE, class = std::enable_if_t< std::is_scalar_v<S> >  
>
static auto operator*(S s, sgm::mxi::UnitVector<T, SIZE> const& v){  return v * s;  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, sgm::mxi::MxSize_t N>
class sgm::mxi::OrthonormalMatrix
{
	using Mat_t = Matrix<T, N, N>;


public:
	OrthonormalMatrix() : _mat(Mat_t::identity()){  static_assert(MxSize::is_static_v<N>);  }
	OrthonormalMatrix(size_t dim) : _mat( Mat_t::identity(dim) ){}

	template<class Q>
	OrthonormalMatrix(Q&& q) : _mat(  _ctor( std::forward<Q>(q) )  )
	{
		if constexpr(!MxTraits::is_OrthonormalMatrix_v<Q>)
			_orthonormalize_myself();
	}


	template<  class Q, class = std::enable_if_t< std::is_scalar_v<Q> >  >
	OrthonormalMatrix(std::initializer_list<Q>&& iL) : _mat( std::move(iL) )
	{
		_orthonormalize_myself();
	}


	template
	<	class Q
	,	class = std::enable_if_t<  !std::is_same_v< std::decay_t<Q>, OrthonormalMatrix >  >
	>
	auto operator=(Q&& q)-> OrthonormalMatrix&
	{
		_mat = _ctor( std::forward<Q>(q) );

		return _orthonormalize_myself();
	}


	auto mat() const-> Mat_t const&{  return _mat;  }

	decltype(auto) data() const{  return mat().data();  }
	decltype(auto) operator()(index_t r, index_t c) const{  return mat()(r, c);  }

	auto rows() const{  return mat().rows();  }
	auto cols() const{  return mat().cols();  }
	auto size() const{  return mat().size();  }

	decltype(auto) row(index_t r) const{  return mat().row(r);  }
	decltype(auto) col(index_t c) const{  return mat().col(c);  }

	auto operator-() const
	{
		auto cpy = *this;

		cpy._mat *= -1;

		return cpy;
	}


	auto transposed() const
	{
		auto cpy = *this;

		cpy._mat = cpy._mat.transposed();

		return cpy;
	}


	auto inversed() const{  return transposed();  }

	template<class Q> 
	decltype(auto) operator*(Q&& q) const{  return mat() * std::forward<Q>(q);  }

	template<class Q>
	decltype(auto) operator/(Q&& q) const{  return mat() / std::forward<Q>(q);  }


private:
	Matrix<T, N, N> _mat;
	

	auto _orthonormalize_myself()-> OrthonormalMatrix&
	{
		auto const size = mat().cols();
		auto v = [this](MxSize_t j)-> Vector<T, N>{  return mat().col(j);  };

		for(MxSize_t j = 0;  j < size;  ++j)
		{
			auto s = Vector<T>::zero(size);

			for(MxSize_t k = 0;  k < j;  ++k)
				s += v(k) * v(k).dot( v(j) );

			_mat.col(j) = UnitVector<T, N>( v(j) - s ).colVec();
		}

		return *this;
	}


	template<class Q>
	static decltype(auto) _ctor(Q&& q)
	{
		if constexpr(MxTraits::is_UnitVector_v<Q>)
			return q.mat();
		else
			return std::forward<Q>(q);
	}

};// end of class OrthonormalMatrix


template
<	class S, class T, sgm::mxi::MxSize_t N, class = std::enable_if_t< std::is_scalar_v<S> >
>
static auto operator*(S s, sgm::mxi::OrthonormalMatrix<T, N> const& m){  return m * s;  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<sgm::mxi::MxSize_t S1, sgm::mxi::MxSize_t S2, class T, class>
decltype(auto) sgm::mxi::fetch(T&& t)
{
	if constexpr(!MxTraits::is_Temporary_v<T>)
		return std::forward<T>(t);
	else if constexpr(MxTraits::is_mxiMatrix_v<T>)
		return static_cast< Matrix<typename T::elem_t, S1, S2> >(t);
	else if constexpr(MxTraits::is_mxiVector_v<T>)
		return static_cast< Vector<typename T::elem_t, S1> >(t);
	else SGM_COMPILE_FAILED(no suitable method was found);
}


#undef _READ_WRITE_MEMFUNC


#endif // end of #ifndef _SGM_MATRIX_INTERFACE_

