#pragma once

#ifndef _SGM_HIGH_TEMPLAR11_
#define _SGM_HIGH_TEMPLAR11_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

#include "..\indexable\Serial.hpp"
#include "..\Concurrency\Concurrency.hpp"
#include "..\Type_Decorator\Type_Decorator.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#


namespace sgm
{
	namespace ht
	{
		struct _implementation : No_Making
		{
			
		};


	#if 0

		template
		<	class...FLAGS, class CON, class FUNC
		,	class = Guaranteed_t< is_iterable<CON>::value >
		>
		static auto Morph(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
		(
			_implementation<>
		)


		template
		<	class...ARGS, class FLAGSET, class CON, class FUNC
		,	class = Guaranteed_t< sizeof...(ARGS) <= 2 >
		>
		static auto Morph(FLAGSET fset, CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
		(

		)


		template
		<	class...FLAGS, class CON, class FUNC
		,	class = Guaranteed_t< is_iterable<CON>::value >
		>
		static auto Filter(CON&& con, FUNC&& func);


		template
		<	class...ARGS, class FLAGSET, class CON, class FUNC
		,	class = Guaranteed_t< sizeof...(ARGS) <=2 >
		>
		static auto Filter(FLAGSET fset, CON&& con, FUNC&& func);
	#endif




	}
}


#if 0

namespace sgm
{


	class HT_implementation : No_Making
	{
	
		struct _Branch : No_Making
		{
			enum class _Sequancial;
			enum class _Parallel;

			template<class FLAG>
			using impl_t
			=	std::conditional_t
				<	std::is_base_of< Parallel_Proc, std::decay_t<FLAG> >::value
				,	_Parallel, _Sequancial
				>;
		};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template< class FLAG, class = typename _Branch::impl_t<FLAG> > struct _Morph_Helper;


		template<class FLAG>
		struct _Morph_Helper<FLAG, _Branch::_Sequancial> : _Branch
		{
			template
			<	class CON, class FUNC
			,	class y_t = decltype( Declval<FUNC>()(*Declval<CON>().begin()) )
			,	class elem_t = typename FLAG::template type<y_t>
			>
			static auto Morph(FLAG, CON&& con, FUNC&& func)-> indexable<elem_t>
			{
				indexable<elem_t> res(con.size());

				for(auto const& x : con)
					res >> func(x);

				return res;
			}
		};


		template<class FLAG>
		struct _Morph_Helper<FLAG, _Branch::_Parallel> : _Branch
		{
			template
			<	class CON, class FUNC
			,	class y_t = decltype( Declval<FUNC>()(*Declval<CON>().begin()) )
			,	class elem_t = typename FLAG::template type<y_t>
			>
			static auto Morph(FLAG flag, CON&& con, FUNC&& func)-> indexable<elem_t>
			{
				static_assert
				(	sgm::is_random_access_iterator<decltype(con.begin())>::value
				,	"the iterable should have random-access iterator type for parallelization."
				);

				if(con.size() == 0)
					return indexable<elem_t>{};

				indexable<elem_t> res( con.size(), func(*con.begin()) );

				flag.process(con.size() - 1).calc
				(	[&func, &res, &con](ixSize_t idx, ixSize_t idx_end)-> std::nullptr_t
					{
						for(; idx < idx_end; ++idx)
							res[idx] = func(con.begin()[idx]);

						return nullptr;
					}
				,	1
				);

				return res;
			}
		};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------




		template< class FLAG, class = typename _Branch::impl_t<FLAG> > struct _Filter_Helper;


		template<class FLAG>
		struct _Filter_Helper<FLAG, _Branch::_Sequancial> : _Branch
		{
			template
			<	class CON, class FUNC
			,	class x_t = std::remove_reference_t< decltype(*Declval<CON>().begin()) >
			,	class elem_t = typename FLAG::template type<x_t>
			>
			static auto Filter(FLAG, CON&& con, FUNC&& func)-> indexable<elem_t>
			{
				indexable<elem_t> res(con.size());

				for(auto const& x : con)
					if( func(x) )
						res >> x;

				return res;
			}
		};


		template<class FLAG>
		struct _Filter_Helper<FLAG, _Branch::_Parallel> : _Branch
		{	
			template
			<	class CON, class FUNC
			,	class x_t = std::remove_reference_t< decltype(*Declval<CON>().begin()) >
			,	class elem_t = typename FLAG::template type<x_t>
			>
			static auto Filter(FLAG flag, CON&& con, FUNC&& func)-> indexable<elem_t>
			{
				indexable<bool> const truth_table
				=	Morph( flag, con, std::forward<FUNC>(func) );

				auto const nof_true
				=	[](indexable<bool> const& tt)-> ixSize_t
					{
						int res = 0, i = 0;

					//#pragma omp parallel for default(shared) private(i) reduction(+:res)
					//	for(i = 0; i < tt.size(); i++)
					//		if(tt[i])
					//			res = res + 1;

						return static_cast<ixSize_t>(res);
					}(truth_table);

				indexable<elem_t> res(nof_true);

				for(ixSize_t idx = 0; idx < con.size(); ++idx)
					if(truth_table[idx])
						res >> con.begin()[idx];

				return res;
			}
		};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		struct _Fold_Helper : No_Making
		{

			template<class FLAG> struct Accumulate;

			template<>
			struct Accumulate<_Branch::_Sequancial> : No_Making
			{
				template<class ITR, class FUNC, class res_t>
				static auto calc(ITR itr, ITR const ei, FUNC&& func, res_t&& res)-> res_t
				{
					while(itr != ei)
						res = func(res, *itr++);

					return res;
				}
			};


			template<>
			struct Accumulate<_Branch::_Parallel> : No_Making
			{
				template<class ITR, class FUNC, class res_t>
				static auto calc(ITR itr, ITR const ei, FUNC&& func, res_t&& res)-> res_t
				{
					//while(itr != ei)
					//	res = func(res, *itr++);

					//return res;
				}
			};


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
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template<bool HAS_INIT, bool FORWARD, class FLAG> struct _Fold_impl;


		template<bool FWD, class FLAG>
		struct _Fold_impl<true, FWD, FLAG> : _Fold_Helper
		{
			template<class CON, class FUNC, class res_t>
			static auto fold(CON&& con, FUNC&& func, res_t&& res)-> res_t
			{
				return
				Accumulate< _Branch::impl_t<FLAG> >::calc
				(	itrMethod<FWD>::begin(con), itrMethod<FWD>::end(con)
				,	std::forward<FUNC>(func)
				,	std::forward<res_t>(res)
				);
			}
		};


		template<bool FWD, class FLAG>
		struct _Fold_impl<false, FWD, FLAG> : _Fold_Helper
		{
			template<class CON, class FUNC>
			static auto fold(CON&& con, FUNC&& func, std::nullptr_t)
			->	std::decay_t< decltype( *itrMethod<FWD>::begin(con) ) >
			{
				auto bi = itrMethod<FWD>::begin(con);
				auto ei = itrMethod<FWD>::end(con);

				assert(bi != ei && L"the container has nothing to fold.\n");

				return
				Accumulate< _Branch::impl_t<FLAG> >
				::	calc( ++bi, ei, std::forward<FUNC>(func), *bi );
			}
		};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


	};
	//========//========//========//========//=======#//========//========//========//========//===


//	
//#ifndef _SGM_MORPH_AND_FILTER_INTERFACE
//	#define _SGM_MORPH_AND_FILTER_INTERFACE(NAME)	\
//		template		\
//		<	class DECO = No_Deco, class CON, class FUNC		\
//		,	class = Guaranteed_t< is_iterable<CON>::value >	\
//		>	\
//		static auto NAME(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO		\
//		(	\
//			HT_implementation	\
//			::	template NAME<DECO>( std::forward<CON>(con), std::forward<FUNC>(func) )	\
//		)	\
//		\
//		\
//		template		\
//		<	class...ARGS, class DECO, class CON, class FUNC	\
//		,	class = Guaranteed_t< sizeof...(ARGS) <= 2 >	\
//		>	\
//		static auto NAME(DECO, CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO	\
//		(	\
//			NAME<DECO>	\
//			(	std::conditional_t		\
//				<	sizeof...(ARGS) >= 1, First_t<ARGS...>, decltype(con)	\
//				>	\
//				(con)	\
//			,	std::conditional_t		\
//				<	sizeof...(ARGS) >= 2, Nth_t<1, ARGS...>, decltype(func) \
//				>	\
//				(func)	\
//			)	\
//		)
//
//		_SGM_MORPH_AND_FILTER_INTERFACE(Morph)
//		_SGM_MORPH_AND_FILTER_INTERFACE(Filter)
//
//	#undef _SGM_MORPH_AND_FILTER_INTERFACE
//#else
//	#error _SGM_MORPH_AND_FILTER_INTERFACE was already defined somewhere else.
//#endif


}// end of namespace sgm
#endif

#endif// end of #ifndef _SGM_HIGH_TEMPLAR11_