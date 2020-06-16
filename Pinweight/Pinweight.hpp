#pragma once

#ifndef _SGM_PINWEIGHT_
#define _SGM_PINWEIGHT_

#include "..\Type_Analysis\Type_Analysis.hpp"
#include <atomic>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	SGM_DECL_PROXY_TEMPLATE_CLASS(Pinweight);


	template<class T>
	class _Pinweight_interface
	{
	public:
		using count_t = std::atomic<size_t>;
		using value_t = T;


		_Pinweight_interface() : _cpval(new T const()), _pcount( new count_t(1) ){}

		_Pinweight_interface(T const& t) : _cpval( new T const(t) ), _pcount( new count_t(1) ){}

		_Pinweight_interface(T&& t)
		:	_cpval(  new T const( std::move(t) )  ), _pcount( new count_t(1) )
		{}

		_Pinweight_interface(_Pinweight_interface const& pwimpl)
		:	_cpval(pwimpl._cpval), _pcount( _pcount_up(pwimpl._pcount) )
		{}

		_Pinweight_interface(_Pinweight_interface&& pwimpl) 
		:	_cpval(pwimpl._cpval), _pcount(pwimpl._pcount)
		{
			pwimpl._cpval = nullptr, pwimpl._pcount = nullptr;
		}

		~_Pinweight_interface(){  _my_pcount_down();  }

		auto value() const-> T const&{  return *_cpval;  }
		operator T const&() const{  return value();  }


		template<class Q>
		bool operator==(Q&& q) const{  return *_cpval == std::forward<Q>(q);  }

		template<class Q>
		bool operator!=(Q&& q) const{  return !( *this == std::forward<Q>(q) );  }

		bool operator==(_Pinweight_interface pw) const{  return *this == *pw._cpval;  }
		bool operator!=(_Pinweight_interface pw) const{  return !( *this == pw );  }


		bool share_with(_Pinweight_interface const& pw) const{  return _cpval == pw._cpval;  }
		size_t share_count() const{  return *_pcount;  }


	protected:
		T const* _cpval;
		count_t mutable* _pcount;


		static auto _pcount_up(count_t* pn)-> count_t*
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
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T>
	class Pinweight_t< T, invar, Pinweight_T_Helper<T, invar, false, false, false> >
	:	public _Pinweight_interface<T>
	{
		using impl_t = _Pinweight_interface<T>;

		friend class Pinweight_t<T, Var>;

	public:
		Pinweight_t() = default;

		Pinweight_t(T const& t) : impl_t(t){}
		Pinweight_t(T&& t) : impl_t( std::move(t) ){}

		Pinweight_t(_Pinweight_interface<T> const& pwimpl) : impl_t(pwimpl){}
		Pinweight_t(_Pinweight_interface<T>&& pwimpl) : impl_t( std::move(pwimpl) ){}

		Pinweight_t(Pinweight_t const& pw) : impl_t(pw){}
		Pinweight_t(Pinweight_t&& pw) : impl_t( std::move(pw) ){}

		~Pinweight_t() = default;


		void operator=(T) = delete;
		void operator=(_Pinweight_interface<T>) = delete;
		auto operator=(Pinweight_t const&)-> Pinweight_t& = delete;
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	class Pinweight_t< T, Var, Pinweight_T_Helper<T, Var, false, false, false> >
	:	public _Pinweight_interface<T>
	{
		using impl_t = _Pinweight_interface<T>;


		template<class Q>
		auto _substitute(Q&& q)-> Pinweight_t&
		{
			impl_t::_my_pcount_down();

			this->_cpval = new T const( std::forward<Q>(q) ), 
			this->_pcount = new typename impl_t::count_t(1);

			return *this; 			
		}


	public:
		Pinweight_t() = default;

		Pinweight_t(T const& t) : impl_t(t){}
		Pinweight_t(T&& t) : impl_t( std::move(t) ){}

		Pinweight_t(_Pinweight_interface<T> const& pwimpl) : impl_t(pwimpl){}
		Pinweight_t(_Pinweight_interface<T>&& pwimpl) : impl_t( std::move(pwimpl) ){}

		Pinweight_t(Pinweight_t const& pw) : impl_t(pw){}
		Pinweight_t(Pinweight_t&& pw) : impl_t( std::move(pw) ){}

		~Pinweight_t() = default;


		auto operator=(T const& t)-> Pinweight_t&{  return _substitute(t);  }
		auto operator=(T&& t)-> Pinweight_t&{  return _substitute( std::move(t) );  }

	
		auto operator=(Pinweight_t<T, invar> const& pw)-> Pinweight_t&
		{
			if(this->_cpval == pw._cpval)
				return *this;

			impl_t::_my_pcount_down(), impl_t::_pcount_up(pw._pcount);

			this->_cpval = pw._cpval, this->_pcount = pw._pcount;

			return *this;
		}

		auto operator=(Pinweight_t<T, invar>&& pw)-> Pinweight_t&
		{
			if(this->_cpval == pw._cpval)
				return *this;

			impl_t::_my_pcount_down();

			this->_cpval = pw._cpval, pw._cpval = nullptr,
			this->_pcount = pw._pcount, pw._pcount = nullptr;

			return *this;
		}

		auto operator=(Pinweight_t const& pw)-> Pinweight_t&
		{
			return *this = static_cast< Pinweight_t<T, invar> const& >(pw);
		}

		auto operator=(Pinweight_t&& pw)-> Pinweight_t&
		{
			return *this = static_cast< Pinweight_t<T, invar>&& >( std::move(pw) );
		}

	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T>
	static auto make_Pinweight(T&& t)-> Pinweight< std::remove_reference_t<T> >
	{
		return Pinweight< std::remove_reference_t<T> >( std::forward<T>(t) );
	}


} // end of namespace sgm

#endif // end of #ifndef _SGM_PINWEIGHT_