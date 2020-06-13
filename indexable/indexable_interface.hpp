#pragma once

#ifndef _SGM_INDEXABLE_
#define _SGM_INDEXABLE_

#include "..\interface_Traits\interface_Traits.hpp"


namespace sgm
{
	

	struct DxSize : No_Making
	{
		using type = size_t;

		enum : type{DYNAMIC = -1};
	};

	using DxSize_t =  DxSize::type;

	template<class T, DxSize_t SIZE = DxSize::DYNAMIC> class indexable;

	//	should be defined later
	template<class T, DxSize_t SIZE> class indexable_impl;
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T, DxSize_t SIZE>
	class indexable : indexable_impl<T, SIZE>
	{
		using impl_t = indexable_impl<T, SIZE>;


	public:
		indexable() = default;
		indexable(indexable const&) = default;


		template
		<	class ITR
		,	class 
			=	std::enable_if_t
				<	is_iterator<ITR>::value 
				&&	std::is_same< T, std::decay_t<decltype(*Declval<ITR>())> >::value
				>
		>
		indexable(ITR bi, ITR ei) : impl_t(bi, ei){}


		template
		<	class S, class...ARGS
		,	class = std::enable_if_t< std::is_integral<S>::value && SIZE == DxSize::DYNAMIC >
		>
		explicit indexable(S size, ARGS const&...args) : impl_t(size, args...){}


		template<  class Q, class = std::enable_if_t< std::is_convertible<Q, T>::value >  >
		indexable(std::initializer_list<Q>&& con) : impl_t( std::move(con) ){}


		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	!std::is_same< std::decay_t<CON>, indexable >::value  
				>
		>
		indexable(CON&& con) : impl_t( std::forward<CON>(con) ){}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		auto operator=(indexable const&)-> indexable& = default;



		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	!std::is_same< std::decay_t<CON>, indexable >::value  
				>  
		>
		auto operator=(CON&& con)-> indexable&
		{
			return *this = indexable(con.begin(), con.end());
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto size() const-> DxSize_t{  return impl_t::size();  }
		auto empty() const-> bool{  return impl_t::empty();  }

		auto cdata() const-> T const*{  return impl_t::data();  }
		auto data() const-> T const*{  return cdata();  }
		auto data()-> T*{  return impl_t::data();  }

		auto operator[](size_t idx) const-> T const&{  return impl_t::at(idx);  }
		auto operator[](size_t idx)-> T&{  return impl_t::at(idx);  } 


		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	!std::is_same< std::decay_t<CON>, indexable >::value  
				>
		> 
		operator CON() const{  return std::decay_t<CON>(begin(), end());  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto begin() const		SGM_DECLTYPE_AUTO(  impl_t::begin()  )
		auto begin()			SGM_DECLTYPE_AUTO(  impl_t::begin()  )
		auto cbegin() const		SGM_DECLTYPE_AUTO(  impl_t::cbegin()  )

		auto end() const		SGM_DECLTYPE_AUTO(  impl_t::end()  )
		auto end()				SGM_DECLTYPE_AUTO(  impl_t::end()  )
		auto cend() const		SGM_DECLTYPE_AUTO(  impl_t::cend()  )

		auto rbegin() const		SGM_DECLTYPE_AUTO(  impl_t::rbegin()  )
		auto rbegin()			SGM_DECLTYPE_AUTO(  impl_t::rbegin()  )
		auto crbegin() const	SGM_DECLTYPE_AUTO(  impl_t::crbegin()  )

		auto rend() const		SGM_DECLTYPE_AUTO(  impl_t::rend()  )
		auto rend()				SGM_DECLTYPE_AUTO(  impl_t::rend()  )
		auto crend() const		SGM_DECLTYPE_AUTO(  impl_t::crend()  )
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


	#ifndef _WHEN_DYNAMIC
		#define _WHEN_DYNAMIC class = std::enable_if_t<SIZE == DxSize::DYNAMIC>

		template<_WHEN_DYNAMIC>
		auto capacity() const-> DxSize_t;

		template<class...ARGS, _WHEN_DYNAMIC>
		auto emplace_back(ARGS&&...args)-> indexable&;

		template<_WHEN_DYNAMIC>
		auto pop_back(DxSize_t n = 1)-> indexable&;

		template<_WHEN_DYNAMIC>
		auto clear()-> indexable&;

		template<class Q, _WHEN_DYNAMIC>
		auto operator>>(Q&& q)-> indexable&{  return emplace_back( std::forward<Q>(q) );  }



	#else
		#error _WHEN_DYNAMIC was already defined somewhere else.
	#endif
	#undef _WHEN_DYNAMIC
	};




#if 1
#else


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
#endif

}

#endif