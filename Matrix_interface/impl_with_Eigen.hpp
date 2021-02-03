#pragma once

#include "Matrix_interface.hpp"
#include "..\interface_Traits\interface_Traits.hpp"

#include "..\3rd_Party_Libraries\Eigen_ver_3.3.9\Dense"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm::mxi
{

	using egnSize_t = int;


	template<class Q>
	using Maybe_reference_t
	=	Selective_t< is_immutable<Q>::value, Decay_t<Q>, Decay_t<Q>& >;


	template<class T>
	using egnDynamicMatrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

	template<class elem_t, egnSize_t S>
	using egnVector_t = Eigen::Matrix<elem_t, S, 1>;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T, sgm::mxi::MxSize_t R, sgm::mxi::MxSize_t C>
class sgm::mxi::Mx_implementation
{
	using core_t
	=	Selective_t
		<	MxSize::is_temporary_v<R>
		,	Referenceless_t<T>
		,	Eigen::Matrix
			<	Decay_t<T>
			,	MxSize::is_dynamic_v<R> ? Eigen::Dynamic : static_cast<egnSize_t>(R)
			,	MxSize::is_dynamic_v<C> ? Eigen::Dynamic : static_cast<egnSize_t>(C)
			>
		>;

	core_t _core;


	SGM_HAS_MEMFUNC(data);


protected:
	using elem_t = Decay_t< decltype( _core(0) ) >;


	template
	<	class CON
	,	class = Enable_if_t< is_iterable<CON>::value && !MxTraits::is_mxiMatrix_v<CON> >
	>
	Mx_implementation(CON&& con, MxSize_t r, MxSize_t c)
	:	_core
		(	[&con, r, c]() mutable-> core_t
			{	
				core_t res;

				if(r == MxSize::DYNAMIC || c == MxSize::DYNAMIC)
					(r == MxSize::DYNAMIC ? r : c)
					=	(MxSize_t)con.size() / (r == MxSize::DYNAMIC ? c : r),
					res.resize(r, c);
				else if
				(	res.rows() != static_cast<egnSize_t>(r) 
				||	res.cols() != static_cast<egnSize_t>(c)
				)
					res.resize(r, c);

				for(  auto[itr, i] = Dual_iteration( con.begin(), MxSize_t(0) );  i < r;  ++i  )
					for(index_t j = 0; j < c; ++j, ++itr)
						res(i, j) = static_cast<elem_t>(*itr);
					
				return res;
			}()
		)
	{}


	template<class Q>
	Mx_implementation(Q&& q)
	:	_core
		(	[&q]()-> core_t
			{
				if constexpr(MxTraits::is_mxiMatrix_v<Q>)
					return q.core();
				else if constexpr(MxSize::is_temporary_v<R>)
					return
					Selective_t
					<	is_immutable<Q>::value, decltype(q), Decay_t<Q>&&
					>
					(q);
				else SGM_COMPILE_FAILED(no suitable method was found.);
			}()
		)
	{}


	Mx_implementation() = default;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	auto core() const-> core_t const&		{  return _core;  }
	auto core()-> Maybe_reference_t<core_t>	{  return _core;  }

	auto element(index_t i, index_t j) const-> elem_t	{  return _core(i, j);  }
	decltype(auto) element(index_t i, index_t j)		{  return _core(i, j);  }

	auto rows() const{  return _core.rows();  }
	auto cols() const{  return _core.cols();  }
	
	auto size() const{  return _core.size();  }
	
	auto data()
	{
		if constexpr(Has_MemFunc_data<core_t>::value)
			return _core.data();
		else
			return static_cast< egnDynamicMatrix<elem_t> >(_core).data();
	}

	auto data() const
	{
		if constexpr(Has_MemFunc_data<core_t>::value)
			return _core.data();
		else
			return static_cast< egnDynamicMatrix<elem_t> >(_core).data();
	}

	void resize(MxSize_t r, MxSize_t c){  _core.resize(r, c);  }

	auto transposed() const{  return _core.adjoint();  }
	decltype(auto) transposed(){  return _core.transpose();  }

	auto det() const{  return _core.determinant();  }
	auto inversed() const{  return _core.inverse();  }


	auto block(index_t i, index_t j, MxSize_t r, MxSize_t c) const
	{
		return _core.block(i, j, r, c);
	}

	auto block(index_t i, index_t j, MxSize_t r, MxSize_t c)
	{
		return _core.block(i, j, r, c);
	}

	
	auto row(index_t idx) const	{  return _core.row(idx);  }
	auto row(index_t idx)		{  return _core.row(idx);  }
	auto col(index_t idx) const	{  return _core.col(idx);  }
	auto col(index_t idx)		{  return _core.col(idx);  }


	static auto identity(){  return core_t::Identity();  }

	static auto identity(MxSize_t s)
	{	
		return egnDynamicMatrix<elem_t>(s, s).setIdentity();
	}


	static auto zero(){  return core_t::Zero();  }

	static auto zero(MxSize_t r, MxSize_t c)
	{
		return egnDynamicMatrix<elem_t>(r, c).setZero();
	}

};// end of class Mx_implementation
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, sgm::mxi::MxSize_t S>
class sgm::mxi::Vt_implementation
{
	using core_t
	=	Selective_t
		<	MxSize::is_temporary_v<S>
		,	Referenceless_t<T>
		,	egnVector_t
			<	Decay_t<T>
			,	MxSize::is_dynamic_v<S> ? Eigen::Dynamic : static_cast<egnSize_t>(S)
			>
		>;

	core_t _core;


protected:
	using elem_t = Decay_t< decltype( _core(0) ) >;


	template<class Q>
	Vt_implementation(Q&& q)
	:	_core
		(	[&q]()-> core_t
			{
				if constexpr(MxSize::is_dynamic_v<S> && is_Convertible_v< Decay_t<Q>, int >)
					return core_t(q, 1);
				else if constexpr(MxTraits::is_mxiMatrix_v<Q>)
				{
					assert
					(	(q.rows() == 1 || q.cols() == 1) && L"Not a row/column Vector.\n" 
					);

					return 
					q.rows() == 1
					?	static_cast<core_t>(q.core().adjoint())
					:	static_cast<core_t>(q.core());
				}
				else if constexpr(MxTraits::is_mxiVector_v<Q>)
					return q.core();
				else if constexpr(MxSize::is_temporary_v<S>)
					return
					Selective_t
					<	is_immutable<Q>::value, decltype(q), Decay_t<Q>&&
					>
					(q);
				else if constexpr(is_iterable<Q>::value)
				{
					core_t res;
					auto const qsize = q.size();

					assert
					(	(MxSize::is_dynamic_v<S> || qsize == S)
					&&	L"static size cannot be modified.\n"
					);

					if constexpr(MxSize::is_dynamic_v<S>)
						res.resize(qsize, 1);

					for
					(	auto[itr, p] = Dual_iteration( q.begin(), &res(0) )
					;	itr != q.end()
					;	*p++ = static_cast<elem_t>(*itr++)
					);
						
					return res;
				}
				else SGM_COMPILE_FAILED(no suitable method was found.);
			}()
		)
	{}


	Vt_implementation() = default;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------


	auto core()-> Maybe_reference_t<core_t>	{  return _core;  }
	auto core() const-> core_t const&		{  return _core;  }

	auto element(index_t idx) const-> elem_t	{  return _core(idx);  }
	decltype(auto) element(index_t idx)		{  return _core(idx);  }

	auto size() const{  return _core.size();  }

	void resize(MxSize_t size){  _core.resize(size, 1);  }

	auto head(MxSize_t n) const		{  return _core.head(n);  }
	decltype(auto) head(MxSize_t n)	{  return _core.head(n);  }

	auto tail(MxSize_t n) const		{  return _core.tail(n);  }
	decltype(auto) tail(MxSize_t n)	{  return _core.tail(n);  }

	decltype(auto) rowVec() const	{  return _core.adjoint();  }
	decltype(auto) rowVec()			{  return _core.transpose();  }
	decltype(auto) colVec() const	{  return _core;  }
	decltype(auto) colVec()			{  return _core;  }


	template<MxSize_t L>
	auto norm() const
	{
		if constexpr(L == 2)
			return _core.norm();
		else if constexpr(L == 0)
			SGM_COMPILE_FAILED(L0 norm is undefined);
		else
		{
			auto res = elem_t(0);

			for(auto idx = size(); idx-->0;)
				res += static_cast<elem_t>(   pow(  abs( _core(idx) ), L  )   );

			return pow<elem_t>( res, float(1)/float(L) );
		}
	}


	auto normalized() const{  return _core.normalized();  }


	template<class Q>
	auto dot(Q&& q) const{  return _core.dot( Forward<Q>(q) );  }


	template<class Q>
	decltype(auto) cross(Q&& q) const
	{  
		return
		static_cast< egnVector_t<elem_t, 3> >(_core)
		.	cross( static_cast< egnVector_t<elem_t, 3> >(q) );
	}

	template<class Q>
	auto dyad(Q&& q) const{  return _core * q.adjoint();  }


	static auto zero(){  return core_t::Zero();  }
	static auto zero(MxSize_t s){  return egnDynamicMatrix<elem_t>(s, 1).setZero();  }

	static auto ones(){  return core_t::Ones();  }
	static auto ones(MxSize_t s){  return egnDynamicMatrix<elem_t>(s, 1).setOnes();  }


};// end of class Vt_implementation
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#
