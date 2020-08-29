#pragma once

#ifndef _SGM_MATRIX_INTERFACE_
#define _SGM_MATRIX_INTERFACE_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif

#include "..\interface_Traits\interface_Traits.hpp"
#include <cassert>
#include <iterator>

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
		static size_t constexpr DYNAMIC = Size_info<size_t>::MAXIMUM;
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
	<	class S, class T, MxSize_t R, MxSize_t C
	,	class = std::enable_if_t< std::is_scalar_v<S> >
	>
	static auto operator*(S s, Matrix<T, R, C> const& m);


	template
	<	class S, class T, MxSize_t SIZE, class = std::enable_if_t< std::is_scalar_v<S> >  
	>
	static auto operator*(S s, Vector<T, SIZE> const& v);

}
//========//========//========//========//=======#//========//========//========//========//=======#


class sgm::mxi::_Temporary : No_Making
{
private:
	template<class, MxSize_t, MxSize_t>		friend class Matrix;
	template<class, MxSize_t>				friend class Vector;


	template< class T>
	static auto Mx(T&& t)
	{
		return Matrix<T, MxSize::TEMPORARY, MxSize::TEMPORARY>( std::move(t) );
	}

	template< class T>
	static auto Vt(T&& t)
	{
		return Vector<T, MxSize::TEMPORARY>( std::move(t) );
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::mxi::MxTraits : No_Making
{
private:
	template<class> struct is_mxiMatrix : std::false_type{};

	template<class T, MxSize_t R, MxSize_t C>
	struct is_mxiMatrix< Matrix<T, R, C> > : std::true_type{};


	template<class> struct is_mxiVector : std::false_type{};

	template<class T, MxSize_t S>
	struct is_mxiVector< Vector<T, S> > : std::true_type{};


	template<class> struct is_Dynamic{  SGM_COMPILE_FAILED(is not a Matrix);  };

	template<class T, MxSize_t R, MxSize_t C>
	struct is_Dynamic< Matrix<T, R, C> > : No_Making
	{
		static bool constexpr value = MxSize::is_dynamic_v<R> || MxSize::is_dynamic_v<C>;
	};


	template<class> struct is_Static{  SGM_COMPILE_FAILED(is not a Matrix);  };

	template<class T, MxSize_t R, MxSize_t C>
	struct is_Static< Matrix<T, R, C> > : No_Making
	{
		static bool constexpr value = MxSize::is_static_v<R> && MxSize::is_static_v<C>;
	};


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


template<class S, class T, sgm::mxi::MxSize_t R, sgm::mxi::MxSize_t C, class>
auto sgm::mxi::operator*(S s, Matrix<T, R, C> const& m){  return m * s;  }
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
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	auto normalized() const{  return _Temporary::Vt(impl_t::normalized());  }

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


template<class S, class T, sgm::mxi::MxSize_t SIZE, class>
auto sgm::mxi::operator*(S s, Vector<T, SIZE> const& v){  return v * s;  }
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

