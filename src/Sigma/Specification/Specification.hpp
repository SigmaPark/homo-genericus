#pragma once

#ifndef _SGM_SPECIFICATION_
#define _SGM_SPECIFICATION_

#include "..\interface_Traits\interface_Traits.hpp"
#include <iostream>
#include <functional>
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	namespace spec
	{
		

		struct Exception{};

		
		inline static void is_True(bool const b)	noexcept(false)
		{
			if(!b)
				throw Exception();
		}

		inline static void is_False(bool const b) noexcept(false){  is_True(!b);  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------	


		struct Judge : No_Making
		{
			template
			<	class CON1, class CON2
			,	class 
				=	Enable_if_t< is_iterable<CON1>::value && is_iterable<CON2>::value >
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
			<	class ITR, class T, class = Enable_if_t< is_iterator<ITR>::value >  
			>
			static bool are_all_same_to(ITR const bi, size_t const n, T const& t)
			{
				return
				(	n == 0
				||	(  *bi == t && are_all_same_to( Next(bi), n - 1, t )  )
				);
			}


			template
			<	class CON, class T, class = Enable_if_t< is_iterable<CON>::value >  
			>
			static bool are_all_same_to(CON const& con, T const& t)
			{
				return are_all_same_to(con.begin(), con.size(), t);
			}


			template
			<	class ITR, class T, class = Enable_if_t< is_iterator<ITR>::value >  
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
			};


			static void Notice(State const s)
			{
				auto show_f = [](wchar_t const* str){  std::wcout << str << L'\n';  };

			#define _SHOW_STATE_AND_BREAK(STATE)  case State::STATE:  show_f(L#STATE);  break

				switch(s)
				{
				_SHOW_STATE_AND_BREAK(DEFAULT_CONSTRUCTION);
				_SHOW_STATE_AND_BREAK(MANUAL_CONSTRUCTION);
				_SHOW_STATE_AND_BREAK(COPY_CONSTRUCTION);
				_SHOW_STATE_AND_BREAK(MOVE_CONSTRUCTION);
				_SHOW_STATE_AND_BREAK(COPY_ASSIGNMENT);
				_SHOW_STATE_AND_BREAK(MOVE_ASSIGNMENT);
				_SHOW_STATE_AND_BREAK(DESTRUCTED);
				}

			#undef _SHOW_STATE_AND_BREAK
			}


			State state;
			int val;
			bool msg[7] = {0, };
			std::function<void(State)> notice = Notice;


			static auto create
			(	int val_
			,	std::initializer_list<size_t> &&msg = {}
			,	std::function<void(State)> notice_ = Notice
			)->	Specimen
			{
				Specimen res{val_};

				for(auto m : msg)
					res.msg[m] = true;

				res.notice = notice_;

				if(res.msg[1])
					res.notice(State::MANUAL_CONSTRUCTION);

				return res;
			}


			Specimen() : state(DEFAULT_CONSTRUCTION), val(0){}

			Specimen(int const n, int const n2 = 0) : state(MANUAL_CONSTRUCTION), val(n + n2){}

			Specimen(Specimen const& spec) 
			:	state(COPY_CONSTRUCTION), val(spec.val), notice(spec.notice)
			{
				for( size_t i = 0;  i < std::size(msg);  ++i )
					msg[i] = spec.msg[i];

				if(msg[2])
					notice(state);
			}
			
			Specimen(Specimen&& spec) throw() 
			:	state(MOVE_CONSTRUCTION), val(spec.val), notice( Move(spec.notice) )
			{
				
				for( size_t i = 0;  i < std::size(msg);  ++i )
					msg[i] = spec.msg[i];

				spec.~Specimen();

				if(msg[3])
					notice(state);
			}

			~Specimen()
			{
				state = DESTRUCTED,  val = 0;

				if(msg[6])
					notice(state);
			}

			auto operator=(Specimen const& spec)-> Specimen&
			{
				state = COPY_ASSIGNMENT,  val = spec.val;

				if(msg[4])
					notice(state);
				
				return *this;
			}

			auto operator=(Specimen&& spec) throw()-> Specimen&
			{
				state = MOVE_ASSIGNMENT,  val = spec.val;
				
				spec.~Specimen();

				if(msg[5])
					notice(state);

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