#pragma once

#include "Matrix_interface/Matrix_interface.hpp"

#include "Reform/ReformEigen.h"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm::mxi
{


	using egnSize_t = int;


	template<class Q>
	using Maybe_reference_t
	=	std::conditional_t< is_immutable<Q>::value, std::decay_t<Q>, std::decay_t<Q>& >;


	template<class T, MxSize_t R, MxSize_t C>
	class Mx_implementation
	{
		using core_t 
		=	std::conditional_t
			<	MxSize::is_temporary_v<R>
			,	std::remove_reference_t<T>
			,	Eigen::Matrix
				<	std::decay_t<T>
				,	MxSize::is_dynamic_v<R> ? Eigen::Dynamic : static_cast<egnSize_t>(R)
				,	MxSize::is_dynamic_v<C> ? Eigen::Dynamic : static_cast<egnSize_t>(C)
				>
			>;

		core_t _core;


	protected:
		using elem_t = std::decay_t< decltype( _core(0) ) >;


		template
		<	class CON
		,	class = std::enable_if_t< is_iterable<CON>::value && !is_mxiMatrix_v<CON> >
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

					for(  auto[itr, i] = std::pair( con.begin(), MxSize_t(0) ); i < r; ++i  )
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
					if constexpr(is_mxiMatrix_v<Q>)
						return q.core();
					else if constexpr(MxSize::is_temporary_v<R>)
						return
						std::conditional_t
						<	is_immutable<Q>::value, decltype(q), std::decay_t<Q>&&
						>
						(q);
					else SGM_COMPILE_FAILED(no suitable method was found.);
				}()
			)
		{}


		Mx_implementation() : _core(){}


		auto core()-> Maybe_reference_t<core_t>	{  return _core;  }
		auto core() const-> core_t const&		{  return _core;  }

		decltype(auto) element(index_t i, index_t j)		{  return _core(i, j);  }
		auto element(index_t i, index_t j) const-> elem_t	{  return _core(i, j);  }

		auto rows() const{  return _core.rows();  }
		auto cols() const{  return _core.cols();  }
		auto size() const{  return _core.size();  }

		void resize(MxSize_t r, MxSize_t c){  _core.resize(r, c);  }

		auto transposed() const{  return _core.adjoint();  }
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

	};


	template<class elem_t, egnSize_t S>
	using egnVector_t = Eigen::Matrix<elem_t, S, 1>;


	template<class T, MxSize_t S>
	class Vt_implementation
	{
		using core_t 
		=	std::conditional_t
			<	MxSize::is_temporary_v<S>
			,	std::remove_reference_t<T>
			,	egnVector_t
				<	std::decay_t<T>
				,	MxSize::is_dynamic_v<S> ? Eigen::Dynamic : static_cast<egnSize_t>(S)
				>
			>;

		core_t _core;

	protected:
		using elem_t = std::decay_t< decltype( _core(0) ) >;


		template<class Q>
		Vt_implementation(Q&& q)
		:	_core
			(	[&q]()-> core_t
				{
					if constexpr(is_mxiMatrix_v<Q>)
					{
						assert
						(	(q.rows() == 1 || q.cols() == 1)
						&&	L"Not a row/column Vector.\n"
						);

						return 
						q.rows() == 1
						?	static_cast<core_t>(q.core().adjoint())
						:	static_cast<core_t>(q.core());
					}
					else if constexpr(is_mxiVector_v<Q>)
						return q.core();
					else if constexpr(MxSize::is_temporary_v<S>)
						return
						std::conditional_t
						<	is_immutable<Q>::value, decltype(q), std::decay_t<Q>&&
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
						(	auto[itr, p] = std::pair( q.begin(), &res(0) )
						;	itr != q.end()
						;	*p++ = static_cast<elem_t>(*itr++);
						);
							
						return res;
					}
					else SGM_COMPILE_FAILED(no suitable method was found.);
				}()
			)
		{}


		Vt_implementation() : _core(){}


		auto core()-> Maybe_reference_t<core_t>	{  return _core;  }
		auto core() const-> core_t const&		{  return _core;  }

		decltype(auto) element(index_t idx)		{  return _core(idx);  }
		auto element(index_t idx) const-> elem_t	{  return _core(idx);  }

		auto size() const{  return _core.size();  }

		void resize(MxSize_t size){  _core.resize(size, 1);  }
	};


}
