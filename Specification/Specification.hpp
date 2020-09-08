#pragma once

#ifndef _SGM_SPECIFICATION_
#define _SGM_SPECIFICATION_

#include "..\interface_Traits\interface_Traits.hpp"
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	namespace spec
	{
		

		struct Exception{};


		template<class...>		//	to avoid warning C4505
		static void is_True(){}

		template<class...BOOLS>
		static void is_True(bool const b, BOOLS...bs)	//	have to change name. : not "is"
		{
			if(!b)
				throw Exception();
			else
				is_True(bs...);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------	


		template<class T1, class T2, class...TYPES>
		static void are_Equivalent(T1&& t1, T2&& t2, TYPES&&...types)
		{
			if(t1 != t2)
				throw Exception();
			else
				are_Equivalent( std::forward<T2>(t2), std::forward<TYPES>(types)... );
		}


		template<class T>
		static void are_Equivalent(T&&){}


		template
		<	class COMP, class T1, class T2, class...TYPES
		,	class 
			=	std::enable_if_t
				<	std::is_same
					<	decltype( Declval<COMP>()(Declval<T1>(), Declval<T2>()) ), bool
					>::	value
				>
		>
		static void are_Equivalent_by(COMP&& comp, T1&& t1, T2&& t2, TYPES&&...types)
		{
			if( !comp(t1, t2) )
				throw Exception();
			else
				are_Equivalent_by
				(	std::forward<COMP>(comp)
				,	std::forward<T2>(t2)
				,	std::forward<TYPES>(types)...
				);
		}


		template
		<	class COMP, class T
		,	class 
			=	std::enable_if_t
				<	std::is_same
					<	decltype( Declval<COMP>()(Declval<T>(), Declval<T>()) ), bool
					>::	value
				>
		>
		static void are_Equivalent_by(COMP&&, T&&){}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template<int B = 1, int...BS>
		struct All_True : std::conditional_t< B == 1, All_True<BS...>, Guaranteed_t<B> >{};

		template<> 
		struct All_True<>{};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------	


		struct Judge : No_Making
		{
			template
			<	class CON1, class CON2
			,	class 
				=	std::enable_if_t< is_iterable<CON1>::value && is_iterable<CON2>::value >
			>
			static bool have_the_same(CON1&& con1, CON2&& con2)
			{
				auto itr1 = con1.begin();
				auto itr2 = con2.begin();

				bool res = con1.size() == con2.size();

				while (itr1 != con1.end() && res)
					res = *itr1++ == *itr2++;

				return res;
			}


			template
			<	class ITR, class T, class = std::enable_if_t< is_iterator<ITR>::value >  
			>
			static bool are_all_same_to(ITR const bi, size_t const n, T const& t)
			{
				return
				(	n == 0
				||	(  *bi == t && are_all_same_to( Next(bi), n - 1, t )  )
				);
			}


			template
			<	class CON, class T, class = std::enable_if_t< is_iterable<CON>::value >  
			>
			static bool are_all_same_to(CON const& con, T const& t)
			{
				return are_all_same_to(con.begin(), con.size(), t);
			}


			template
			<	class ITR, class T, class = std::enable_if_t< is_iterator<ITR>::value >  
			>
			static bool are_all_same_to(ITR const bi, ITR const ei, T const& t)
			{
				return
				(	bi == ei
				||	*bi == t && are_all_same_to( Next(bi), ei, t )
				);
			}
		};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------	


		struct Specimen
		{
			enum State
			{	DEFAULT_CONSTRUCTION
			,	MANUAL_CONSTRUCTION
			,	COPY_CONSTRUCTION
			,	MOVE_CONSTRUCTION
			,	COPY_ASSIGNMENT
			,	MOVE_ASSIGNMENT
			,	DESTRUCTED
			} state;

			int val;


			Specimen() : state(DEFAULT_CONSTRUCTION), val(0){}
			Specimen(int const n, int const n2 = 0) : state(MANUAL_CONSTRUCTION), val(n + n2){}
			Specimen(Specimen const& spec) : state(COPY_CONSTRUCTION), val(spec.val){}
			
			Specimen(Specimen&& spec) throw() : state(MOVE_CONSTRUCTION), val(spec.val)
			{
				spec.~Specimen();
			}

			~Specimen(){  state = DESTRUCTED,  val = 0;  }

			auto operator=(Specimen const& spec)-> Specimen&
			{
				state = COPY_ASSIGNMENT,  val = spec.val;
				
				return *this;
			}

			auto operator=(Specimen&& spec) throw()-> Specimen&
			{
				state = MOVE_ASSIGNMENT,  val = spec.val;
				
				spec.~Specimen();

				return *this;
			}

			bool operator==(Specimen const& spec) const{  return val == spec.val;  }
			bool operator==(int const n) const{  return val == n;  }
			bool operator==(State const s) const{  return state == s;  }

			template<class T>
			bool operator!=(T const& t) const{  return !(*this == t);  }
		};
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------	


	}
}

#endif // end of #ifndef _SGM_SPECIFICATION_