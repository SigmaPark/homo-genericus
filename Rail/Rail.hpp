#pragma once

#ifndef _SGM_RAIL_
#define _SGM_RAIL_

#include "..\Type_Analysis\Type_Analysis.hpp"


namespace sgm
{
	

	struct RailSize : No_Making{  enum : size_t{DYNAMIC = size_t(-1)};  };

	template<class T, size_t SIZE = RailSize::DYNAMIC> class Rail;

	template<class T, size_t SIZE> class Static_Rail_impl;
	template<class T> class Dynamic_Rail_impl;
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T, size_t SIZE>
	class Rail : Static_Rail_impl< std::remove_reference_t<T>, SIZE >
	{
		using impl = Static_Rail_impl< std::remove_reference_t<T>, SIZE >;

	public:
		template<class...ARGS>
		Rail(ARGS&&...args) : impl( std::forward<ARGS>(args)... ){};

	#ifdef _INITIALIZER_LIST_
		template<class Q> Rail(std::initializer_list<Q>&& iL) : impl( std::move(iL) ){}
	#endif

		template<class RHS>
		auto operator=(RHS&& rhs)-> Rail&
		{  
			impl::operator=( std::forward<RHS>(rhs) );

			return *this;
		}

		auto operator[](size_t idx) const	SGM_DECLTYPE_AUTO( impl::operator[](idx) )
		auto operator[](size_t idx)			SGM_DECLTYPE_AUTO( impl::operator[](idx) )

		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	!std::is_same< std::decay_t<CON>, Rail >::value  
				> 
		> 
		operator CON() const{  return std::decay_t<CON>(begin(), end());  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		auto size() const-> size_t{  return impl::size();  }
		auto empty() const-> bool{  return impl::empty();  }

		auto data() const		SGM_DECLTYPE_AUTO( impl::data() )
		auto data()			SGM_DECLTYPE_AUTO( impl::data() )
		auto cdata() const		SGM_DECLTYPE_AUTO( impl::cdata() )
		auto cdata()			SGM_DECLTYPE_AUTO( impl::cdata() )

		auto begin() const		SGM_DECLTYPE_AUTO( impl::begin() )
		auto begin()			SGM_DECLTYPE_AUTO( impl::begin() )
		auto cbegin() const	SGM_DECLTYPE_AUTO( impl::cbegin() )
		auto cbegin()			SGM_DECLTYPE_AUTO( impl::cbegin() )

		auto end() const		SGM_DECLTYPE_AUTO( impl::end() )
		auto end()			SGM_DECLTYPE_AUTO( impl::end() )
		auto cend() const		SGM_DECLTYPE_AUTO( impl::cend() )
		auto cend()			SGM_DECLTYPE_AUTO( impl::cend() )

	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T>
	class Rail<T, RailSize::DYNAMIC> : Dynamic_Rail_impl< std::remove_reference_t<T> >
	{
		using impl = Dynamic_Rail_impl< std::remove_reference_t<T> >;

	public:
		template<class...ARGS>
		Rail(ARGS&&...args) : impl( std::forward<ARGS>(args)... ){};

	#ifdef _INITIALIZER_LIST_
		template<class Q> Rail(std::initializer_list<Q>&& iL) : impl( std::move(iL) ){}
	#endif

		template<class RHS>
		auto operator=(RHS&& rhs)-> Rail&
		{  
			impl::operator=( std::forward<RHS>(rhs) );

			return *this;
		}

		auto operator[](size_t idx) const	SGM_DECLTYPE_AUTO( impl::operator[](idx) )
		auto operator[](size_t idx)			SGM_DECLTYPE_AUTO( impl::operator[](idx) )

		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	!std::is_same< std::decay_t<CON>, Rail >::value  
				> 
		> 
		operator CON() const{  return std::decay_t<CON>(begin(), end());  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		auto size() const-> size_t{  return impl::size();  }
		auto empty() const-> bool{  return impl::empty();  }

		auto data() const		SGM_DECLTYPE_AUTO( impl::data() )
		auto data()			SGM_DECLTYPE_AUTO( impl::data() )
		auto cdata() const		SGM_DECLTYPE_AUTO( impl::cdata() )
		auto cdata()			SGM_DECLTYPE_AUTO( impl::cdata() )

		auto begin() const		SGM_DECLTYPE_AUTO( impl::begin() )
		auto begin()			SGM_DECLTYPE_AUTO( impl::begin() )
		auto cbegin() const	SGM_DECLTYPE_AUTO( impl::cbegin() )
		auto cbegin()			SGM_DECLTYPE_AUTO( impl::cbegin() )

		auto end() const		SGM_DECLTYPE_AUTO( impl::end() )
		auto end()			SGM_DECLTYPE_AUTO( impl::end() )
		auto cend() const		SGM_DECLTYPE_AUTO( impl::cend() )
		auto cend()			SGM_DECLTYPE_AUTO( impl::cend() )
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		auto capacity() const-> size_t{  return impl::capacity();  }
		auto no_capacity() const-> bool{  return impl::no_capacity();  }

		auto resize(size_t n)-> Rail&
		{  
			impl::resize(n);  
			
			return *this;  
		}

		auto reserve(size_t n)-> Rail&
		{
			impl::reserve(n);

			return *this;
		}

		template<class...ARGS>
		auto emplace_back(ARGS&&...args)-> Rail&
		{
			impl::emplace_back( std::forward<ARGS>(args)... );  
			
			return *this;
		}

		template<class Q>
		auto operator>>(Q&& q)-> Rail&{  return emplace_back( std::forward<Q>(q) );  }

		auto pop_back(size_t n = 1)-> Rail&
		{
			impl::pop_back(n);

			return *this;
		}

		template<class ITR>
		auto pop_back_from(ITR ei)-> Rail&
		{
			impl::pop_back_from(ei);

			return *this;
		}

		auto clear()-> Rail&
		{
			impl::clear();

			return *this;
		}

		auto shrink_to_fit()-> Rail&
		{
			impl::shrink_to_fit();

			return *this;
		}

	};
	//========//========//========//========//=======#//========//========//========//========//===


}

#endif