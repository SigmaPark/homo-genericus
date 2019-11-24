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
#include <type_traits>
#include <initializer_list>
#include <optional>

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
		template<	  typename...ARGS, typename = std::enable_if_t< sizeof...(ARGS) >= 2 >  >
		UnitVector(ARGS&&...args)
		:	uv(  Vector<T, D>( static_cast<T>(args)... ).normalized()  )
		{}
		

		template
		<	typename M
		,	typename = std::enable_if_t<  std::is_convertible_v< M, Vector<T, D> >  >
		>
		UnitVector(M&& m) : uv(m.normalized()){}


		auto operator=(UnitVector const&) = delete;

		auto operator()() const-> Vector<T, D> const&{  return uv;  }

		auto operator()(unsigned dim) const-> T{  return uv(dim);  }

		auto adjoint() const-> Eigen::Matrix<T, 1, D>{  return uv.adjoint();  }


		template
		<	typename V
		,	typename = std::enable_if_t<  std::is_same_v< std::decay_t<V>, Vector<T, D> >  >
		>
		auto dot(V&& v) const-> T
		{
			return uv.dot( std::forward<V>(v) );
		}


	private:
		Vector<T, D> const uv;
	};


	template<typename M, typename T, unsigned D>
	static auto operator*(M const& Rot, UnitVector<T, D> const& uv)-> UnitVector<T, D>
	{
		assert(  abs( Rot.determinant() - T(1) ) < T(1e3) * std::numeric_limits<T>::epsilon()  );

		return Rot * uv();
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<typename T, unsigned D>
	struct Plane
	{
		template<	typename Pos_t, typename Nml_t>
		Plane(Pos_t&& position, Nml_t&& normal) 
		:	pos( std::forward<Pos_t>(position) ), nml( std::forward<Nml_t>(normal) )
		{
			static_assert
			(	(	std::is_convertible_v< Pos_t, Vector<T, D> >
				&&	std::is_convertible_v< Nml_t, UnitVector<T, D> >
				)
			,	"sgm::Plane construction failed."
			);
		}

		using elem_t = T;
		enum {  DIMENSION = D  };

		Vector<T, D> const pos;
		UnitVector<T, D> const nml;
	};


	template<typename T, unsigned D>
	struct Line
	{
		template<	typename Pos_t, typename Dir_t>
		Line(Pos_t&& position, Dir_t&& direction) 
		:	pos( std::forward<Pos_t>(position) ), dir( std::forward<Dir_t>(direction) )
		{
			static_assert
			(	(	std::is_convertible_v< Pos_t, Vector<T, D> >
				&&	std::is_convertible_v< Dir_t, UnitVector<T, D> >
				)
			,	"sgm::Line construction failed."
			);
		}

		using elem_t = T;
		enum {  DIMENSION = D  };

		Vector<T, D> const pos;
		UnitVector<T, D> const dir;
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template< template<typename, unsigned> class TC >
	class did_temclass
	{
	public:
		did_temclass() = delete;

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
	

	template<typename res_t, typename V, typename G, typename dist_t = unsigned>
	auto Projected(V&& query_point, G&& geometric_object, dist_t = unsigned())
	->	res_t
	{
		using std::decay_t, std::forward;

		static_assert
		(	(	did_temclass<Vector>::create_v<V>
			&&	did_temclass<Plane>::create_v<G> || did_temclass<Line>::create_v<G>
			&&	std::is_same_v< decay_t<V>, decay_t<decltype(geometric_object.pos)> >
			)
		,	"undefined operation."
		);

		using elem_t = typename V::elem_t;
		unsigned constexpr dimension = V::DIMENSION;

		auto&& qpt = forward<V>(query_point);
		auto&& gobj = forward<G>(geometric_object);

		if constexpr(std::is_scalar_v< decay_t<res_t> >)
			if constexpr(did_temclass<Plane>::create_v<G> && std::is_signed_v<dist_t>)
				return gobj.nml.dot(qpt - gobj.pos);
			else if constexpr(did_temclass<Plane>::create_v<G> && std::is_unsigned_v<dist_t>)	
				return abs(  Projected<res_t>( forward<V>(qpt), forward<G>(gobj), signed() )  );
			else if constexpr(did_temclass<Line>::create_v<G> && dimension == 2)	
				return 
				Projected<res_t>
				(	forward<V>(qpt)
				,	Plane<elem_t, 2>
					(	gobj.pos, UnitVector<elem_t, 2>( -gobj.dir(1), gobj.dir(0) )
					)
				);
			else if constexpr(did_temclass<Line>::create_v<G> && dimension == 3)	
				return gobj.dir().cross(qpt - gobj.pos).norm();
			else
				static_assert(false, "no suitable method was found.");
		else if constexpr(did_temclass<Vector>::create_v<res_t>)
			if constexpr (did_temclass<Plane>::create_v<G>)
				return 
				qpt 
				-	gobj.nml()
					*	Projected<elem_t>( forward<V>(qpt), forward<G>(gobj), signed() );
			else if constexpr(did_temclass<Line>::create_v<G>)
				return
				gobj.nml()
				*	Projected<elem_t>
					(	forward<V>(qpt)
					,	Plane<elem_t, dimension>(gobj.pos, gobj.nml)
					,	signed()
					);
			else
				static_assert(false, "no suitable method was found.");
		else
			static_assert(false, "no suitable method was found.");
			
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---





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
	->	std::optional< UnitVector<T, D> >
	{
		return 
		abs( T(1) - N.dot(U()) ) < (T)pow(10.0, PoD) * std::numeric_limits<T>::epsilon()
		?	std::optional< UnitVector<T, D> >{}
		:	std::optional< UnitVector<T, D> >( U() - N()*N.dot(U()) );
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