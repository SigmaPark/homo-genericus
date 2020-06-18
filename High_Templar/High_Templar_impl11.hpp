#pragma once

#ifndef _SGM_HIGH_TEMPLAR_IMPL11_
#define _SGM_HIGH_TEMPLAR_IMPL11_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

#include "High_Templar_interface.hpp"

#define USE_STL_VECTOR_AND_ARRAY
#include "..\indexable\indexable.hpp"
#undef USE_STL_VECTOR_AND_ARRAY
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	class HT_implementation : No_Making
	{
		
		struct _Fold_Helper : No_Making
		{
			template<class ITR, class FUNC, class res_t>
			static auto calc(ITR itr, ITR const ei, FUNC&& func, res_t&& res)-> res_t
			{
				while(itr != ei)
					res = func(res, *itr++);

				return res;
			}


			template<bool FORWARD> struct itrMethod;

			template<>
			struct itrMethod<true>
			{
				template<class CON>
				static auto begin(CON&& con) SGM_DECLTYPE_AUTO(  con.begin()  )

				template<class CON>
				static auto end(CON&& con) SGM_DECLTYPE_AUTO(  con.end()  )
			};

			template<>
			struct itrMethod<false>
			{
				template<class CON>
				static auto begin(CON&& con) SGM_DECLTYPE_AUTO(  con.rbegin()  )

				template<class CON>
				static auto end(CON&& con) SGM_DECLTYPE_AUTO(  con.rend()  )
			};

		};


		template<bool HAS_INIT, bool FORWARD> struct _Fold_impl;

		template<bool FWD>
		struct _Fold_impl<true, FWD> : _Fold_Helper
		{
			template<class CON, class FUNC, class res_t>
			static auto fold(CON&& con, FUNC&& func, res_t&& res)-> res_t
			{
				return
				calc
				(	itrMethod<FWD>::begin(con), itrMethod<FWD>::end(con)
				,	std::forward<FUNC>(func)
				,	std::forward<res_t>(res)
				);
			}
		};

		template<bool FWD>
		struct _Fold_impl<false, FWD> : _Fold_Helper
		{
			template<class CON, class FUNC>
			static auto fold(CON&& con, FUNC&& func, std::nullptr_t)
			->	std::decay_t< decltype( *itrMethod<FWD>::begin(con) ) >
			{
				auto bi = itrMethod<FWD>::begin(con);
				auto ei = itrMethod<FWD>::end(con);

				assert(bi != ei && L"the container has nothing to fold.\n");

				return calc( ++bi, ei, std::forward<FUNC>(func), *bi );
			}
		};


	public:
		template<class T>
		static auto indices(size_t N, T offset)-> indexable<T>
		{
			indexable<T> res(N);

			while(N-->0)
				res >> offset++;

			return res;
		}


		template
		<	class DECO, class CON, class FUNC
		,	class y_t = decltype( Declval<FUNC>()(*Declval<CON>().begin()) )
		,	class elem_t = typename DECO::template type<y_t>
		>
		static auto Morph(CON&& con, FUNC&& func)-> indexable<elem_t>
		{
			indexable<elem_t> res(con.size());

			for(auto const& x : con)
				res >> func(x);

			return res;
		}


		template
		<	class DECO, class CON, class FUNC
		,	class x_t = std::remove_reference_t< decltype(*Declval<CON>().begin()) >
		,	class elem_t = typename DECO::template type<x_t>
		>
		static auto Filter(CON&& con, FUNC&& func)-> indexable<elem_t>
		{
			indexable<elem_t> res(con.size());

			for(auto const& x : con)
				if( func(x) )
					res >> x;

			return res;
		}


	#ifndef _SGM_FORWARD_AND_REVERSE_IMPL
		#define _SGM_FORWARD_AND_REVERSE_IMPL(NAME, IS_FORWARD_DIRECTION, BEGIN)	\
			template		\
			<	class DECO, class CON, class FUNC, class init_t	\
			,	class No_init_t = std::is_same< std::decay_t<init_t>, std::nullptr_t >	\
			,	class res_t	\
				=	typename DECO::template type	\
					<	std::conditional_t		\
						<	No_init_t::value	\
						,	std::remove_reference_t< decltype(*Declval<CON>().BEGIN()) >	\
						,	init_t	\
						>	\
					>	\
			>	\
			static auto NAME(CON&& con, FUNC&& func, init_t&& init)-> res_t	\
			{	\
				return	\
				_Fold_impl<!No_init_t::value, IS_FORWARD_DIRECTION>::fold	\
				(	std::forward<CON>(con), std::forward<FUNC>(func)	\
				,	std::forward<init_t>(init)	\
				);	\
			}
			
			_SGM_FORWARD_AND_REVERSE_IMPL(Fold, true, begin)
			_SGM_FORWARD_AND_REVERSE_IMPL(rFold, false, rbegin)

		#undef _SGM_FORWARD_AND_REVERSE_IMPL
	#else
		#error _SGM_FORWARD_AND_REVERSE_IMPL was already defined somewhere else.
	#endif

	};


}

#endif // end of #ifndef _SGM_HIGH_TEMPLAR_IMPL11_