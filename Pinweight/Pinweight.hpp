#pragma once

#ifndef _SGM_PINWEIGHT_
#define _SGM_PINWEIGHT_

#include "..\Type_Analysis\Type_Analysis.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{
	
	template<class T> class is_Pinweight;
	template<class T, bool = is_Pinweight<T>::value> class Pinweight;


	template<class T>
	class Remove_Ref_and_Pinweight : public No_Making
	{
		template<class Q> struct _Raw{  using type = Q;  };
		template<class Q> struct _Raw<Q&>{  using type = typename _Raw<Q>::type;  };
		template<class Q> struct _Raw<Q&&>{  using type = typename _Raw<Q>::type;  };
		template<class Q> struct _Raw<Q const>{  using type = typename _Raw<Q>::type const;  };
		template<class Q> struct _Raw<Q const&>{  using type = typename _Raw<Q>::type const;  };
		
		template<class Q, bool B> 
		struct _Raw< Pinweight<Q, B> >{  using type = typename _Raw<Q>::type;  };
	
	public: using type = typename _Raw<T>::type;
	};

	template<class T>
	using NoRef_NoPW_t = typename Remove_Ref_and_Pinweight<T>::type;


	template<class T>
	class is_Pinweight : public No_Making
	{
	public: enum : bool{value = !std::is_same<std::decay_t<T>, NoRef_NoPW_t<T> >::value};
	};
	//========//========//========//========//=======#//========//========//========//========//===




	template<class T>
	class Pinweight<T, false>
	{
	public:
		using value_t = NoRef_NoPW_t<T>;


		Pinweight() : pval(new value_t()), pcount( new size_t(1) ){}
		Pinweight(value_t const& t) : pval( new value_t(t) ), pcount( new size_t(1) ){}
		
		Pinweight(value_t&& t) 
		:	pval(  new value_t( static_cast<value_t&&>(t) )  ), pcount( new size_t(1) )
		{}


		Pinweight(Pinweight const& pw) : pval(pw.pval), pcount( _pcount_up(pw.pcount) ){}

		Pinweight(Pinweight&& pw) : pval(pw.pval), pcount(pw.pcount)
		{
			pw.pval = nullptr, pw.pcount = nullptr;
		}


		~Pinweight(){  _my_pcount_down();  }


		auto operator=(value_t const& t)-> Pinweight const&
		{  
			_my_pcount_down();

			pval = new value_t(t),
			pcount = new size_t(1);

			return *this;  	
		}

		auto operator=(value_t&& t)-> Pinweight const&
		{
			_my_pcount_down();

			pval = new value_t( static_cast<value_t&&>(t) ),
			pcount = new size_t(1);

			return *this;			
		}


		auto operator=(Pinweight const& pw)-> Pinweight const&
		{
			if(pval == pw.pval)
				return *this;

			_my_pcount_down(), _pcount_up(pw.pcount);

			pval = pw.pval, pcount = pw.pcount;

			return *this;
		}

		auto operator=(Pinweight&& pw)-> Pinweight const&
		{
			if(pval == pw.pval)
				return *this;

			_my_pcount_down();

			pval = pw.pval, pw.pval = nullptr,
			pcount = pw.pcount, pw.pcount = nullptr;

			return *this;
		}


		operator value_t const&() const{  return *pval;  }
		
		auto value() const-> value_t const&{  return *pval;  }

		auto value()-> value_t&
		{
			if(*pcount > 1)
				_my_pcount_down(),
				pval = new value_t(*pval),
				pcount = new size_t(1);
			
			return *pval;
		}

		bool share_with(Pinweight const& pw) const{  return pval == pw.pval;  }

	
	private:
		value_t* pval;
		size_t* pcount;


		static auto _pcount_up(size_t* pn)-> size_t*
		{
			if(pn != nullptr)
				(*pn)++;

			return pn;
		}

		void _my_pcount_down()
		{
			if(pcount == nullptr)
				return;
			else if( --(*pcount) == 0 )
				delete pval, pval = nullptr,
				delete pcount, pcount = nullptr;
		}

	};



	template<class PIN_t>
	class Pinweight<PIN_t, true> : public std::decay_t<PIN_t>
	{
	public:
		template<class Q>
		Pinweight(Q&& q) : std::decay_t<PIN_t>( std::forward<Q>(q) ){}
	};


} // end of namespace sgm

#endif // end of #ifndef _SGM_PINWEIGHT_