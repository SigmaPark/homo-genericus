#pragma once

#ifndef _SGM_REFORM_EIGEN_
#define _SGM_REFORM_EIGEN_

#ifdef _MSC_VER
	static_assert(_MSC_VER >= 1914, "C++17 or higher version language support is required.");
#endif

#include "..\..\..\Eigen_3_3_7\Eigen\Dense"
#include <cassert>
#include <limits>
#include <cmath>
#include <type_traits>
#include <initializer_list>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{
	
	template<typename T, unsigned D>
	using Vector = Eigen::Matrix<T, D, 1>;
	//========//========//========//========//=======#//========//========//========//========//===


	template<typename T, unsigned D>
	class UnitVector
	{
	public:
		UnitVector(std::initializer_list<T>&& iL)
		:	uv
			(	[&iL]
				{
					assert(D == iL.size());

					Vector<T, D> res;

					for
					(	auto [pres, itr] = std::make_pair( &res(0), iL.begin() )
					;	itr != iL.end()
					;	*pres++ = *itr++
					);

					return res.normalized();
				}()
			)
		{}


		template
		<	typename M
		,	typename = std::enable_if_t<  std::is_convertible_v< M, Vector<T, D> >  >
		>
		UnitVector(M&& m) : uv(m.normalized()){}


		auto operator=(UnitVector const&) = delete;


		auto operator()() const-> Vector<T, D> const&{  return uv;  }


		auto adjoint() const-> Eigen::Matrix<T, 1, D>{  return uv.adjoint();  }


		template
		<	typename V
		,	typename std::enable_if_t<  std::is_convertible_v< V, Vector<T, D> >  >
		>
		auto dot(V&& v) const-> T
		{
			return uv.dot( std::forward<V>(v) );
		}


	private:
		Vector<T, D> const uv;
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<typename M, typename T, unsigned D>
	static auto operator*(M const& Rot, UnitVector<T, D> const& uv)-> UnitVector<T, D>
	{
		assert(  abs( Rot.determinant() - T(1) ) < T(1e3) * std::numeric_limits<T>::epsilon()  );

		return Rot * uv();
	}


	template<typename T>
	static auto Ray_dist(Vector<T, 3> const& V, Vector<T, 3> const& P, UnitVector<T, 3> const& L)
	->	T
	{
		return L().cross(V - P).norm();
	}


	template<typename T, unsigned D>
	static auto PL_signed_dist
	(	Vector<T, D> const& V, Vector<T, D> const& P, UnitVector<T, D> const& N
	)->	T
	{
		return N.dot(V- P);
	}


	template<typename T, unsigned D>
	static auto PL_dist(Vector<T, D> const& V, Vector<T, D> const& P, UnitVector<T, D> const& N)
	->	T
	{
		return abs( PL_signed_dist(V, P, N) );
	}


	template<typename T, unsigned D>
	static auto UVecAngle(UnitVector<T, D> const& u1, UnitVector<T, D> const& u2)-> T
	{
		auto bounded
		=	[](auto val, auto minval, auto maxval)
			{
				return
				maxval < val
				?	maxval
				:
				val < minval
				?	minval
				:	val;
			};

		return
		acos
		(	bounded(  abs( T(1) - T(.5)*(u1() - u2()).squaredNorm() ), T(0), T(1)  )
		);
	}


	template<typename T, unsigned D>
	static auto PL_proj(Vector<T, D> const& V, Vector<T, D> const& P, UnitVector<T, D> const& N)
	->	Vector<T, D>
	{
		return V- N()*N.dot(V - P);
	}


	template<typename T, unsigned D>
	static auto Ray_proj
	(	Vector<T, D> const& V, Vector<T, D> const& P, UnitVector<T, D> const& L
	)->	Vector<T, D>
	{
		return L() * L.dot(V - P);
	}


	template<unsigned PoD, typename T, unsigned D>
	static auto Ortho_UVec(UnitVector<T, D> const& U, UnitVector<T, D> const& N)
	->	UnitVector<T, D>
	{
		if(  T(1) - N.dot(U()) > (T)pow(10.0, PoD) * std::numeric_limits<T>::epsilon()  )
			throw false;
		else
			return U - N()*N.dot(U());
	}


	template<typename T, unsigned D>
	static auto Rotation(UnitVector<T, D> const& axis, T theta)-> Eigen::Matrix<T, D, D>
	{
		Eigen::Transform<T, D, Eigen::Affine> Tran;

		Tran.setIdentity(),
		Tran.rotate( Eigen::AngleAxis<T>(theta, axis()) );

		return Tran.matrix().block(0, 0, D, D);
	}

}	// End of namespace sgm

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

#endif