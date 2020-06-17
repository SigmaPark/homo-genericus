#pragma once

#ifndef _SGM_HIGH_TEMPLAR_IMPL11_
#define _SGM_HIGH_TEMPLAR_IMPL11_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

#include "High_Templar_interface.hpp"
#include "..\indexable\indexable.hpp"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	class HT_implementation : No_Making
	{
		
		struct _Accumulation_impl : No_Making
		{
		protected:
			template<class ITR, class FUNC, class res_t>
			static auto calc(ITR itr, ITR const ei, FUNC&& func, res_t&& res)-> res_t
			{
				while(itr != ei)
					res = func(res, *itr++);

				return res;
			}
		};

		template<class>
		struct _Fold_impl : _Accumulation_impl
		{
			template<class CON, class FUNC, class res_t>
			static auto fold(CON&& con, FUNC&& func, res_t&& res)-> res_t
			{
				return
				calc
				(	con.begin(), con.end(), std::forward<FUNC>(func)
				,	std::forward<res_t>(res)
				);
			}

			template<class CON, class FUNC, class res_t>
			static auto rfold(CON&& con, FUNC&& func, res_t&& res)-> res_t
			{
				return
				calc
				(	con.rbegin(), con.rend(), std::forward<FUNC>(func)
				,	std::forward<res_t>(res)
				);
			}
		};

		template<>
		struct _Fold_impl<std::nullptr_t> : _Accumulation_impl
		{
			template<class CON, class FUNC>
			static auto fold(CON&& con, FUNC&& func, std::nullptr_t)
			->	std::decay_t< decltype(*Declval<CON>().begin()) >
			{
				assert(con.begin() != con.end() && L"the container has nothing to fold.\n");

				return 
				calc( ++con.begin(), con.end(), std::forward<FUNC>(func), *con.begin() );
			}

			template<class CON, class FUNC>
			static auto rfold(CON&& con, FUNC&& func, std::nullptr_t)
			->	std::decay_t< decltype(*Declval<CON>().rbegin()) >
			{
				assert(con.rbegin() != con.rend() && L"the container has nothing to fold.\n");

				return
				calc( ++con.rbegin(), con.rend(), std::forward<FUNC>(func), *con.rbegin() );
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
		#define _SGM_FORWARD_AND_REVERSE_IMPL(NAME, IMPL_NAME, BEGIN)	\
			template		\
			<	class DECO, class CON, class FUNC, class init_t	\
			,	class res_t	\
				=	typename DECO::template type	\
					<	std::conditional_t		\
						<	std::is_same< std::decay_t<init_t>, std::nullptr_t >::value	\
						,	std::remove_reference_t< decltype(*Declval<CON>().BEGIN()) >		\
						,	init_t	\
						>	\
					>	\
			>	\
			static auto NAME(CON&& con, FUNC&& func, init_t&& init)-> res_t		\
			{	\
				return	\
				_Fold_impl<init_t>::IMPL_NAME	\
				(	std::forward<CON>(con), std::forward<FUNC>(func)	\
				,	std::forward<init_t>(init)	\
				);	\
			}		
			
			_SGM_FORWARD_AND_REVERSE_IMPL(Fold, fold, begin)
			_SGM_FORWARD_AND_REVERSE_IMPL(rFold, rfold, rbegin)

		#undef _SGM_FORWARD_AND_REVERSE_IMPL
	#else
		#error _SGM_FORWARD_AND_REVERSE_IMPL was already defined somewhere else.
	#endif

	};


}

#endif // end of #ifndef _SGM_HIGH_TEMPLAR_IMPL11_