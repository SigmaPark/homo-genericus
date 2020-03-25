#pragma once

#ifndef _SGM_PINWEIGHT_
#define _SGM_PINWEIGHT_

#include "..\Type_Analysis\Type_Analysis.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{
	
#if 0
	template<class T> class is_Pinweight;

	template
	<	class T, class NoRef = std::remove_reference_t<T>
	,	class = std::conditional_t< is_Pinweight<NoRef>::value, std::true_type, std::false_type >
	> 
	class Pinweight;


	template<class T>
	class Remove_Ref_and_Pinweight : public No_Making
	{
		template<class Q> struct _Raw{  using type = Q;  };

		template<class Q> struct _Raw<Q&>{  using type = typename _Raw<Q>::type;  };
		template<class Q> struct _Raw<Q&&>{  using type = typename _Raw<Q>::type;  };
		template<class Q> struct _Raw<Q const>{  using type = typename _Raw<Q>::type const;  };
		template<class Q> struct _Raw<Q const&>{  using type = typename _Raw<Q>::type const;  };
		
		template<class Q, class N, class C> 
		struct _Raw< Pinweight<Q, N, C> >{  using type = typename _Raw<N>::type;  };
	
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



	template<class PIN_t, class NoRef>
	class Pinweight<PIN_t, NoRef, std::true_type> : public NoRef
	{
	public: template<class Q> Pinweight(Q&& q) : NoRef( std::forward<Q>(q) ){}
	};

	 
	template<class T, class NoRef>
	class Pinweight<T, NoRef, std::false_type>
	{
	public:
		using value_t = NoRef;


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


		auto operator=(value_t const& t)-> Pinweight&
		{  
			_my_pcount_down();

			pval = new value_t(t),
			pcount = new size_t(1);

			return *this;  	
		}

		auto operator=(value_t&& t)-> Pinweight&
		{
			_my_pcount_down();

			pval = new value_t( static_cast<value_t&&>(t) ),
			pcount = new size_t(1);

			return *this;			
		}


		auto operator=(Pinweight const& pw)-> Pinweight&
		{
			if(pval == pw.pval)
				return *this;

			_my_pcount_down(), _pcount_up(pw.pcount);

			pval = pw.pval, pcount = pw.pcount;

			return *this;
		}

		auto operator=(Pinweight&& pw)-> Pinweight&
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
				++(*pn);

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
#else

	SGM_DECL_PROXY_TEMPLATE_CLASS(Pinweight);


	template<class T>
	class Pinweight_t< T, invar, Pinweight_T_Helper<T, invar, false, false, false> >
	{
	public:
		using value_t = T;


		Pinweight_t() : _cpval(new T const()), _pcount( new size_t(1) ){}
		Pinweight_t(T const& t) : _cpval( new T const(t) ), _pcount( new size_t(1) ){}
		Pinweight_t(T&& t) : _cpval(  new T const( std::move(t) )  ), _pcount( new size_t(1) ){}


		Pinweight_t(Pinweight_t const& pw) 
		:	_cpval(pw._cpval), _pcount( _pcount_up(pw._pcount) )
		{}

		Pinweight_t(Pinweight_t&& pw) : _cpval(pw._cpval), _pcount(pw._pcount)
		{
			pw._cpval = nullptr, pw._pcount = nullptr;
		}


		~Pinweight_t(){  _my_pcount_down();  }


		auto operator=(Pinweight_t const&)-> Pinweight_t& = delete;


		auto value() const-> T const&{  return *_cpval;  }
		operator T const&() const{  return value();  }


		template<class Q>
		bool operator==(Q&& q) const{  return *_cpval == std::forward<Q>(q);  }

		template<class Q>
		bool operator!=(Q&& q) const{  return !( *this == std::forward<Q>(q) );  }

		bool operator==(Pinweight_t pw) const{  return *this == *pw._cpval;  }
		bool operator!=(Pinweight_t pw) const{  return !( *this == pw );  }

		bool share_with(Pinweight_t const& pw) const{  return _cpval == pw._cpval;  }


	protected:
		T const* _cpval;
		size_t mutable* _pcount;


		static auto _pcount_up(size_t* pn)-> size_t*
		{
			if(pn != nullptr)
				++(*pn);

			return pn;
		}

		void _my_pcount_down()
		{
			if(_pcount == nullptr)
				return;
			else if( --(*_pcount) == 0 )
				delete _cpval, _cpval = nullptr,
				delete _pcount, _pcount = nullptr;
		}


		friend class Pinweight_t<T, Var>;
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	class Pinweight_t< T, Var, Pinweight_T_Helper<T, Var, false, false, false> >
	:	public Pinweight_t< T, invar, Pinweight_T_Helper<T, invar, false, false, false> >
	{
		using const_Pinweight_t = constPinweight<T>;


	public:	
		Pinweight_t() : const_Pinweight_t(){}
		Pinweight_t(T const& t) : const_Pinweight_t(t){}
		Pinweight_t(T&& t) : const_Pinweight_t( std::move(t) ){}


		auto operator=(T const& t)-> Pinweight_t&
		{  
			_my_pcount_down();

			_cpval = new T const(t),
			_pcount = new size_t(1);

			return *this; 
		}

		auto operator=(T&& t)-> Pinweight_t&
		{  
			_my_pcount_down();

			_cpval = new T const( std::move(t) ),
			_pcount = new size_t(1);

			return *this; 
		}


		auto operator=(/*const_*/Pinweight_t const pw)-> Pinweight_t&
		{
			if(_cpval == pw._cpval)
				return *this;

			_my_pcount_down(), _pcount_up(pw._pcount);

			_cpval = pw._cpval, _pcount = pw._pcount;

			return *this;
		}

		auto operator=(/*const_*/Pinweight_t&& pw)-> Pinweight_t&
		{
			if(_cpval == pw._cpval)
				return *this;

			_my_pcount_down();

			_cpval = pw._cpval, pw._cpval = nullptr,
			_pcount = pw._pcount, pw._pcount = nullptr;

			return *this;
		}

	};

#endif


} // end of namespace sgm

#endif // end of #ifndef _SGM_PINWEIGHT_