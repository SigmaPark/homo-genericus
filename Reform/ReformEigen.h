#pragma once

#ifndef _SGM_REFORM_EIGEN_
#define _SGM_REFORM_EIGEN_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif

#include "..\..\..\Eigen_3_3_7\Eigen\Dense"
#include <cassert>
#include <limits>
#include <cmath>
#include <tuple>
#include <type_traits>
#include <optional>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{

	template<typename T, unsigned D>
    using Vector = Eigen::Matrix<T, D, 1>;
	//========//========//========//========//=======#//========//========//========//========//===
	

	template< template<typename, unsigned> class TC >
	class did_tem_TN_class
	{
	public:
		did_tem_TN_class() = delete;

		template<typename T>
		static bool constexpr create_v 
		=	_create( static_cast< std::decay_t<T> const* >(nullptr) );

	private:
		template<	typename T, unsigned D>
		static bool constexpr _create(TC<T, D> const*){  return true;  }
			
		template<typename T>
		static bool constexpr _create(T const*) {  return false;  }
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
	

	template<typename T, unsigned D>
	class UnitVector
	{
	public:
		using elem_t = T;
		enum {  DIMENSION = D  };


	    template<  typename...ARGS, typename = std::enable_if_t< sizeof...(ARGS) >= 2 >  >
		UnitVector(ARGS&&...args)
		:   uv(  Vector<T, D>( static_cast<T>(args)... ).normalized()  )
		{
			if( uv.squaredNorm() < T(.5) )
				throw false;
		}
		

		template
		<	typename M
		,	typename 
			=	std::enable_if_t
				<	std::is_convertible_v< M, Vector<T, D> >
				&&	!did_tem_TN_class<UnitVector>::create_v<M>
				>
		>
		UnitVector(M&& m) : uv(m.normalized())
		{	
			if( uv.squaredNorm() < T(.5) )
				throw false;
		}


		auto operator=(UnitVector const& rhs)-> UnitVector<T, D> const&
		{
			uv = rhs();

			return *this;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		explicit operator Vector<T, D>() const{  return uv;  }

		auto operator()() const-> Vector<T, D> const&{  return uv;  }

		auto operator()(unsigned dim) const-> T{  return uv(dim);  }

		auto operator-() const{  return UnitVector<T, D>(-uv);  }

		auto adjoint() const-> Eigen::Matrix<T, 1, D>{  return uv.adjoint();  }


		template<typename V>
		auto dot(V&& v) const-> T
		{
			static_assert
			(	std::is_convertible_v< V, Vector<T, D> >, "expected type is sgm::Vector<T, D>."
			);

			return uv.dot( std::forward<V>(v) );
		}


		template<typename V>
		auto angle(V&& v) const-> T
		{
			if constexpr(std::is_same_v< std::decay_t<V>, UnitVector<T, D> >)
				return UVecAngle( *this, std::forward<V>(v) );
			else if constexpr(std::is_same_v< std::decay_t<V>, Vector<T, D> >)
				return UVecAngle( *this, UnitVector<T, D>( std::forward<V>(v) ) );
			else static_assert(false, "unexpected type input.");
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template<unsigned PoD = 2, typename UV>
		bool is_parallel(UV&& u) const
		{
			static_assert
			(	std::is_same_v< std::decay_t<UV>, UnitVector<T, D> >
			,	"expected type is sgm::UnitVector<T, D>"
			);

			return
			T(1) - abs( uv.dot(u()) ) < (T)pow(10, PoD) * std::numeric_limits<T>::epsilon();
		}


		template<unsigned PoD = 2, typename UV>
		bool is_orthogonal(UV&& u) const
		{
			static_assert
			(	std::is_same_v< std::decay_t<UV>, UnitVector<T, D> >
			,	"expected type is sgm::UnitVector<T, D>"
			);

			return abs( uv.dot(u()) ) < (T)pow(10, PoD) * std::numeric_limits<T>::epsilon();
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template<typename UV>
		auto rotated(UV&& axis, T theta) const-> UnitVector<T, D>
		{
			static_assert
			(	std::is_same_v< std::decay_t<UV>, UnitVector<T, D> >
			,	"expected type is sgm::UnitVector<T, D>"
			);

			Eigen::Transform<T, D, Eigen::Affine> Tran;

			Tran.setIdentity(), Tran.rotate( Eigen::AngleAxis<T>(theta, axis()) );

			return Tran.matrix().block(0, 0, D, D) * uv;
		}


		template<typename UV>
		auto rotate(UV&& axis, T theta) &-> UnitVector<T, D> const&
		{
			uv = this->rotated(std::forward<UV>(axis), theta)();

			return *this;
		}


	private:
		 Vector<T, D> mutable uv;
	};	// End of class sgm::UnitVector<T, D>
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<unsigned PoD = 2, typename M, typename T, unsigned D>
	static auto operator*(M&& m, UnitVector<T, D> const& uv)-> UnitVector<T, D>
	{
		static_assert
		(	std::is_convertible_v< std::decay_t<M>, Eigen::Matrix<T, D, D> >
		,	"undefined matrix multiplication."
		);

		assert
		(	abs( m.determinant() - T(1) ) 
		<	(T)pow(10, PoD) * std::numeric_limits<T>::epsilon()  
		);

		return std::forward<M>(m) * uv();
	}


	template<typename T, unsigned D>
	static auto Rotation(UnitVector<T, D> const& axis, T theta)-> Eigen::Matrix<T, D, D>
	{
		Eigen::Transform<T, D, Eigen::Affine> Tran;

		Tran.setIdentity(), Tran.rotate( Eigen::AngleAxis<T>(theta, axis()) );

		return Tran.matrix().block(0, 0, D, D);
	}


	template<typename U1, typename U2>
	static decltype(auto) UVecAngle(U1&& u1, U2&& u2)
	{
		static_assert
		(	(	did_tem_TN_class<UnitVector>::create_v<U1>
			&&	std::is_same_v< std::decay_t<U1>, std::decay_t<U2> >
			)
		,	"U1 and U2 should be sgm::UnitVector<T, D> type."
		);

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

		using T = typename std::decay_t<U1>::elem_t;

		return acos(  bounded( T(1) - T(.5)*(u1() - u2()).squaredNorm(), T(-1), T(1) )  );
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<typename T, unsigned D>
	struct Plane
	{
		template<	typename pos_t, typename nml_t>
		Plane(pos_t&& position, nml_t&& normal) 
		:	pos( std::forward<pos_t>(position) ), nml( std::forward<nml_t>(normal) )
		{
			static_assert
			(	(	std::is_convertible_v< pos_t, Vector<T, D> >
				&&	std::is_convertible_v< nml_t, UnitVector<T, D> >
				)
			,	"sgm::Plane construction failed."
			);
		}

		using elem_t = T;
		enum {  DIMENSION = D  };

		Vector<T, D> pos;
		UnitVector<T, D> nml;
	};


	template<typename T, unsigned D>
	struct Line
	{
		template<	typename pos_t, typename dir_t>
		Line(pos_t&& position, dir_t&& direction) 
		:	pos( std::forward<pos_t>(position) ), dir( std::forward<dir_t>(direction) )
		{
			static_assert
			(	(	std::is_convertible_v< pos_t, Vector<T, D> >
				&&	std::is_convertible_v< dir_t, UnitVector<T, D> >
				)
			,	"sgm::Line construction failed."
			);
		}

		using elem_t = T;
		enum {  DIMENSION = D  };

		Vector<T, D> pos;
		UnitVector<T, D> dir;
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
	

	template<typename res_t, typename dist_t = unsigned, typename V, typename G>
	auto Projected(V&& query_point, G&& geometric_object)-> res_t
	{
		using std::decay_t, std::forward;
		using elem_t = typename decay_t<G>::elem_t;
		unsigned constexpr dimension = decay_t<G>::DIMENSION;


		if constexpr
		(	auto&& [qpt, gobj]
			=	std::forward_as_tuple( forward<V>(query_point), forward<G>(geometric_object) )
		;	std::is_scalar_v< decay_t<res_t> >
		)
			if constexpr(did_tem_TN_class<Plane>::create_v<G> && std::is_signed_v<dist_t>)
				return gobj.nml.dot(qpt - gobj.pos);
			else if constexpr(did_tem_TN_class<Plane>::create_v<G> && std::is_unsigned_v<dist_t>)
				return abs(  Projected<res_t, signed>( forward<V>(qpt), forward<G>(gobj) )  );
			else if constexpr(did_tem_TN_class<Line>::create_v<G> && dimension == 2)	
				return 
				Projected<res_t>
				(	forward<V>(qpt)
				,	Plane<elem_t, 2>
					(	gobj.pos, UnitVector<elem_t, 2>( -gobj.dir(1), gobj.dir(0) )
					)
				);
			else if constexpr(did_tem_TN_class<Line>::create_v<G> && dimension == 3)	
				return gobj.dir().cross(qpt - gobj.pos).norm();
			else static_assert(false, "no suitable method was found.");
		else if constexpr(did_tem_TN_class<Vector>::create_v<res_t>)
			if constexpr (did_tem_TN_class<Plane>::create_v<G>)
				return 
				qpt 
				-	gobj.nml() * Projected<elem_t, signed>( forward<V>(qpt), forward<G>(gobj) );
			else if constexpr(did_tem_TN_class<Line>::create_v<G>)
				return
				gobj.dir()
				*	Projected<elem_t, signed>
					(	forward<V>(qpt), Plane<elem_t, dimension>(gobj.pos, gobj.dir)
					);
			else static_assert(false, "no suitable method was found.");
		else static_assert(false, "no suitable method was found.");
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---



	template<typename T>
	[[deprecated]] static auto Ray_dist
	(Vector<T, 3> const& V, Vector<T, 3> const& P, UnitVector<T, 3> const& L)-> T
	{
		return L().cross(V - P).norm();
	}


	template<typename T, unsigned D>
	[[deprecated]] static auto PL_signed_dist
	(Vector<T, D> const& V, Vector<T, D> const& P, UnitVector<T, D> const& N)-> T
	{
		return N.dot(V- P);
	}


	template<typename T, unsigned D>
	[[deprecated]] static auto PL_dist
	(Vector<T, D> const& V, Vector<T, D> const& P, UnitVector<T, D> const& N)-> T
	{
		return abs( PL_signed_dist(V, P, N) );
	}



	template<typename T, unsigned D>
	[[deprecated]] static auto PL_proj
	(Vector<T, D> const& V, Vector<T, D> const& P, UnitVector<T, D> const& N)-> Vector<T, D>
	{
		return V- N()*N.dot(V - P);
	}


	template<typename T, unsigned D>
	[[deprecated]] static auto Ray_proj
	(Vector<T, D> const& V, Vector<T, D> const& P, UnitVector<T, D> const& L)->	Vector<T, D>
	{
		return L() * L.dot(V - P);
	}


	template<unsigned PoD, typename T, unsigned D>
	[[deprecated]] static auto Ortho_UVec(UnitVector<T, D> const& U, UnitVector<T, D> const& N)
	->	std::optional< UnitVector<T, D> >
	{
		return 
		abs( T(1) - N.dot(U()) ) < (T)pow(10.0, PoD) * std::numeric_limits<T>::epsilon()
		?	std::optional< UnitVector<T, D> >{}
		:	std::optional< UnitVector<T, D> >( U() - N()*N.dot(U()) );
	}

}	// End of namespace sgm

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

#endif