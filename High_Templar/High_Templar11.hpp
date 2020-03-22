/**	Higher order functions in FP paradigm as template function format
*/
#pragma once

#ifndef _SGM_HIGH_TEMPLAR11_
#define _SGM_HIGH_TEMPLAR11_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

#include "..\Carrier\Carrier.hpp"
#include "..\Pinweight\Pinweight.hpp"
#include "..\Avatar\Avatar.hpp"
#include "..\Flags\Flags.hpp" 
#include "..\Compound\Compound.hpp"
//#include <omp.h>


////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	struct CONST : Flag<CONST>{};
	struct PWT : Flag<PWT>{};
	struct AVT : Flag<AVT>{};

#ifdef _OMPIMP
	struct PAR : Flag<PAR>{};


	#ifdef _ITERATOR_
		template<class CON>
		struct has_random_access_iterator
		{
			enum : bool
			{
				value
				=	std::is_same
					<	typename std::iterator_traits
						<	decltype(Declval<CON>().begin())
						>::	iterator_category
					,	std::random_access_iterator_tag
					>::value
			};
		};
	#endif
#endif

	template<class F>
	using _HT11_FlagMatching
	=	FlagMatching
		<	F
		,	Flag<>, Flag<PWT>, Flag<AVT>
		,	Flag<CONST>, Flag<PWT, CONST>, Flag<AVT, CONST>
	#ifdef _OMPIMP
		,	Flag<PAR>, Flag<PWT, PAR>, Flag<AVT, PAR>
		,	Flag<PAR, CONST>, Flag<PWT, PAR, CONST>, Flag<AVT, PAR, CONST>
	#endif
		>;
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T = size_t>
	static auto indices(size_t N, T offset = 0)-> Carrier<T>
	{
		Carrier<T> res(N);

		while(N-->0)
			res >> offset++;

		return res;
	}
	//========//========//========//========//=======#//========//========//========//========//===


	struct _Morph_Branch : Branch<_HT11_FlagMatching, _Morph_Branch>
	{
		SGM_FLAG_SWITCH;
	
	private:
		template<class y_t, class CON, class FUNC>
		static auto _calc(CON&& con, FUNC&& func)-> Carrier<y_t>
		{
			Carrier<y_t> res(con.size());

			for (auto const& x : con)
				res >> func(x);

			return res;
		}

		SGM_FLAG_CASE()
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc<  std::result_of_t< FUNC(Elem_t<CON>) >  >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};

		SGM_FLAG_CASE(PWT)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc<   Pinweight<  std::result_of_t< FUNC(Elem_t<CON>) >  >   >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};

		SGM_FLAG_CASE(PWT, CONST)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc<   Pinweight<  std::result_of_t< FUNC(Elem_t<CON>) >  > const   >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};


	#ifdef _OMPIMP
		template<class y_t, class CON, class FUNC>
		static auto _calc2(CON&& con, FUNC&& func)-> Carrier<y_t>
		{
		#ifdef _ITERATOR_
			static_assert
			(	has_random_access_iterator<CON>::value
			,	"random access iterator is expected for parallelization."
			);
		#endif

			if(con.begin() == con.end())
				return Carrier<y_t>();

			Carrier<y_t> res( con.size(), func(*con.begin()) );
		

		#pragma omp parallel for
			for(int i = 1; i < con.size(); ++i)
				res[i] = func(con.begin()[i]);

			return res;
		}


		SGM_FLAG_CASE(PAR)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc2<  std::result_of_t< FUNC(Elem_t<CON>) >  >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};


		SGM_FLAG_CASE(PAR, PWT)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc2<   Pinweight<  std::result_of_t< FUNC(Elem_t<CON>) >  >   >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};

		SGM_FLAG_CASE(PAR, PWT, CONST)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc2<   Pinweight<  std::result_of_t< FUNC(Elem_t<CON>) >  > const   >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};
	#endif

	};


	template<class...FLAGS, class CON, class FUNC>
	static auto Morph(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
	(
		_Morph_Branch::calc<FLAGS...>( std::forward<CON>(con), std::forward<FUNC>(func) )
	)


	template<class...ARGS, class FSET, class CON, class FUNC>
	static auto Morph(FSET fset, CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
	(
		_Morph_Branch::calc<ARGS...>( fset, std::forward<CON>(con), std::forward<FUNC>(func) )
	)
	//========//========//========//========//=======#//========//========//========//========//===
	
	
	struct _Filter_Branch : Branch<_HT11_FlagMatching, _Filter_Branch>
	{
		SGM_FLAG_SWITCH;
	
	private:
		template<class x_t, class CON, class FUNC>
		static auto _calc(CON&& con, FUNC&& func)-> Carrier<x_t>
		{
			Carrier<x_t> res(con.size());

			for(auto& x : con)
				if( func(x) )
					res >> x;

			return res;
		}

		SGM_FLAG_CASE()
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc<  std::decay_t< Elem_t<CON> >  >
				(	std::forward<CON>(con), std::forward<FUNC>(func) 
				)
			)
		};

		SGM_FLAG_CASE(PWT)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc<   Pinweight<  std::decay_t< Elem_t<CON> >  >   >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};

		SGM_FLAG_CASE(PWT, CONST)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc<   Pinweight<  std::decay_t< Elem_t<CON> >  > const   >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};

		SGM_FLAG_CASE(AVT)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc<   Avatar<  std::remove_reference_t< Elem_t<CON> >  >   >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};

		SGM_FLAG_CASE(AVT, CONST)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc<   Avatar<  std::remove_reference_t< Elem_t<CON> >  > const   >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};


	#ifdef _OMPIMP
		template<class x_t, class CON, class FUNC>
		static auto _calc2(CON&& con, FUNC&& func)-> Carrier<x_t>
		{
			Carrier<bool> const truth_table = Morph<PAR>(con, func);

			Carrier<x_t> res
			(	Fold
				(	truth_table
				,	size_t(0), [](size_t n, bool b)-> size_t{  return b ? n + 1 : n;  }
				)
			);

			for(size_t i = 0; i < truth_table.size(); ++i)
				if(truth_table[i])
					res >> con.begin()[i];

			return res;
		}


		SGM_FLAG_CASE(PAR)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc2<  std::decay_t< Elem_t<CON> >  >
				(	std::forward<CON>(con), std::forward<FUNC>(func) 
				)
			)
		};


		SGM_FLAG_CASE(PAR, PWT)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc2<   Pinweight<  std::decay_t< Elem_t<CON> >  >   >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};

		SGM_FLAG_CASE(PAR, PWT, CONST)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc2<   Pinweight<  std::decay_t< Elem_t<CON> >  > const   >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};

		SGM_FLAG_CASE(PAR, AVT)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc2<   Avatar<  std::remove_reference_t< Elem_t<CON> >  >   >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};

		SGM_FLAG_CASE(PAR, AVT, CONST)
		{
			template<class CON, class FUNC>
			static auto calc(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
			(
				_calc2<   Avatar<  std::remove_reference_t< Elem_t<CON> >  > const   >
				(	std::forward<CON>(con), std::forward<FUNC>(func)
				)
			)
		};
	#endif

	};


	template<class...FLAGS, class CON, class FUNC>
	static auto Filter(CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
	(
		_Filter_Branch::calc<FLAGS...>( std::forward<CON>(con), std::forward<FUNC>(func) )
	)


	template<class...ARGS, class FSET, class CON, class FUNC>
	static auto Filter(FSET fset, CON&& con, FUNC&& func) SGM_DECLTYPE_AUTO
	(
		_Filter_Branch::calc<ARGS...>( fset, std::forward<CON>(con), std::forward<FUNC>(func) )
	)
	//========//========//========//========//=======#//========//========//========//========//===



	struct _Fold_Helper : No_Making
	{
		template<class ITR, class FUNC, class res_t>
		static auto Accumulate(ITR itr, ITR const ei, FUNC&& func, res_t res)
		->	res_t
		{
			while(itr != ei)
				res = func(res, *itr++);
	
			return res;
		}
	};


	template<class CON, class FUNC, class res_t>
	static auto Fold(CON&& con, res_t&& init, FUNC&& func)-> res_t
	{
		return 
		_Fold_Helper::Accumulate
		(	con.begin(), con.end(), std::forward<FUNC>(func), std::forward<res_t>(init)
		);
	}
	
	template<class CON, class FUNC>
	static auto Fold(CON&& con, FUNC&& func)-> std::decay_t< decltype(*Declval<CON>().begin()) >
	{
		assert(con.begin() != con.end() && L"the container has nothing to fold.");

		return 
		_Fold_Helper::Accumulate
		(	++con.begin(), con.end(), std::forward<FUNC>(func), *con.begin() 
		);
	}


	template<class CON, class FUNC, class res_t>
	static auto rFold(CON&& con, res_t&& init, FUNC&& func)-> res_t
	{
		return 
		_Fold_Helper::Accumulate
		(	con.rbegin(), con.rend(), std::forward<FUNC>(func), std::forward<res_t>(init)
		);
	}
	
	template<class CON, class FUNC>
	static auto rFold(CON&& con, FUNC&& func)
	->	std::decay_t< decltype(*Declval<CON>().rbegin()) >
	{
		assert(con.rbegin() != con.rend() && L"the container has nothing to fold.");

		return 
		_Fold_Helper::Accumulate
		(	++con.rbegin(), con.rend(), std::forward<FUNC>(func), *con.rbegin() 
		);
	}
	//========//========//========//========//=======#//========//========//========//========//===


	struct _Zip_Helper : Branch<_HT11_FlagMatching, _Zip_Helper>
	{
		SGM_FLAG_SWITCH;
	
	private:
		//template< template<class> class wrap_tc, class...CS >
		//static auto _calc(CS&&...cs)-> Compound<  wrap_tc< Elem_t<CS> >...  >
		//{
		//	
		//}
	};

	//template<class...CS> static auto Zip(CS&&...cs)-> Compound< Elem_t<CS>... >;
	//========//========//========//========//=======#//========//========//========//========//===


}

#endif // end of #ifndef _SGM_HIGH_TEMPLAR11_