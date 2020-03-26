#pragma once

#ifndef _SGM_PINWEIGHT_
#define _SGM_PINWEIGHT_

#include "..\Type_Analysis\Type_Analysis.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


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


		void operator=(T) = delete;
		void operator=(Pinweight<T>) = delete;

		auto value() const-> T const&{  return *_cpval;  }
		operator T const&() const{  return value();  }


		template<class Q>
		bool operator==(Q&& q) const{  return *_cpval == std::forward<Q>(q);  }

		template<class Q>
		bool operator!=(Q&& q) const{  return !( *this == std::forward<Q>(q) );  }

		bool operator==(Pinweight_t pw) const{  return *this == *pw._cpval;  }
		bool operator!=(Pinweight_t pw) const{  return !( *this == pw );  }


		bool share_with(Pinweight_t const& pw) const{  return _cpval == pw._cpval;  }
		size_t share_count() const{  return *_pcount;  }


	private:
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


		auto operator=(Pinweight_t const& pw)-> Pinweight_t&
		{
			if(_cpval == pw._cpval)
				return *this;

			_my_pcount_down(), _pcount_up(pw._pcount);

			_cpval = pw._cpval, _pcount = pw._pcount;

			return *this;
		}

		auto operator=(Pinweight_t&& pw)-> Pinweight_t&
		{
			if(_cpval == pw._cpval)
				return *this;

			_my_pcount_down();

			_cpval = pw._cpval, pw._cpval = nullptr,
			_pcount = pw._pcount, pw._pcount = nullptr;

			return *this;
		}


		friend class Pinweight_t<T, Var>;
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	class Pinweight_t< T, Var, Pinweight_T_Helper<T, Var, false, false, false> >
	:	public Pinweight_t< T, invar, Pinweight_T_Helper<T, invar, false, false, false> >
	{
	private:
		using const_Pinweight_t = constPinweight<T>;


		template<class Q>
		auto _substitute(Q&& q)-> Pinweight_t&
		{
			_my_pcount_down();

			_cpval = new T const( std::forward<Q>(q) ), _pcount = new size_t(1);

			return *this; 			
		}


	public:	
		Pinweight_t() : const_Pinweight_t(){}
		Pinweight_t(T const& t) : const_Pinweight_t(t){}
		Pinweight_t(T&& t) : const_Pinweight_t( std::move(t) ){}

		Pinweight_t(const_Pinweight_t const& pw) : const_Pinweight_t(pw){}
		Pinweight_t(const_Pinweight_t&& pw) : const_Pinweight_t( std::move(pw) ){}


		auto operator=(T const& t)-> Pinweight_t&{  return _substitute(t);  }
		auto operator=(T&& t)-> Pinweight_t&{  return _substitute( std::move(t) );  }


		auto operator=(const_Pinweight_t const& pw)-> const_Pinweight_t&
		{
			return const_Pinweight_t::operator=(pw);
		}

		auto operator=(const_Pinweight_t&& pw)-> const_Pinweight_t&
		{
			return const_Pinweight_t::operator=( std::move(pw) );
		}

	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	static auto make_Pinweight(T&& t)-> Pinweight< std::remove_reference_t<T> >
	{
		return Pinweight< std::remove_reference_t<T> >( std::forward<T>(t) );
	}


} // end of namespace sgm

#endif // end of #ifndef _SGM_PINWEIGHT_