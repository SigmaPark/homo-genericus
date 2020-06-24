#pragma once

#ifndef _SGM_INDEXABLE_INTERFACE_
#define _SGM_INDEXABLE_INTERFACE_

#include "..\interface_Traits\interface_Traits.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#


namespace sgm
{
	

	struct ixSize : No_Making
	{
		using type = size_t;

		enum : type{DYNAMIC = -1};
	};

	using ixSize_t =  ixSize::type;

	template<class, ixSize_t = ixSize::DYNAMIC> class indexable;


	//	should be defined later
	template<class, ixSize_t> class indexable_impl;

	//	should be defined later
	template<class, ixSize_t> struct ix_iterator;
	template<class, ixSize_t> struct ix_const_iterator;
	template<class, ixSize_t> struct ix_riterator;
	template<class, ixSize_t> struct ix_const_riterator;
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T, ixSize_t S>
	class indexable : indexable_impl<T, S>
	{
		using impl_t = indexable_impl<T, S>;
		using impl_t::at;

	public:
		enum : ixSize_t{SIZE = S};


		indexable() = default;
		indexable(indexable const&) = default;
		indexable(indexable&& ix) : impl_t( std::move(ix) ){}


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
		,	class = std::enable_if_t< std::is_integral<S>::value && SIZE == ixSize::DYNAMIC >
		>
		explicit indexable(S size, ARGS const&...args) : impl_t(size, args...){}


		template<  class Q, class = std::enable_if_t< std::is_convertible<Q, T>::value >  >
		indexable(std::initializer_list<Q>&& con) : impl_t(con.begin(), con.end()){}


		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	!std::is_same< std::decay_t<CON>, indexable >::value  
				>
		>
		indexable(CON&& con) : impl_t(con.begin(), con.end()){}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto operator=(indexable const&)-> indexable& = default;
		
		auto operator=(indexable&& ix)-> indexable&
		{
			impl_t::operator=( std::move(ix) );

			return *this;
		}


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


		using iter_t = typename ix_iterator<T, SIZE>::type;
		using citer_t = typename ix_const_iterator<T, SIZE>::type;
		using riter_t = typename ix_riterator<T, SIZE>::type;
		using criter_t = typename ix_const_riterator<T, SIZE>::type;


		auto cbegin() const-> citer_t	{  return impl_t::cbegin();  }
		auto begin() const-> citer_t	{  return cbegin();  }
		auto begin()-> iter_t			{  return impl_t::begin();  }

		auto cend() const-> citer_t	{  return impl_t::cend();  }
		auto end() const-> citer_t		{  return cend();  }
		auto end()-> iter_t			{  return impl_t::end();  }

		auto crbegin() const-> criter_t	{  return impl_t::crbegin();  }
		auto rbegin() const-> criter_t	{  return crbegin();  }
		auto rbegin()-> riter_t		{  return impl_t::rbegin();  }

		auto crend() const-> criter_t	{  return impl_t::crend();  }
		auto rend() const-> criter_t	{  return crend();  }
		auto rend()-> riter_t			{  return impl_t::rend();  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto size() const-> ixSize_t{  return impl_t::size();  }
		auto empty() const-> bool{  return impl_t::empty();  }

		auto cdata() const-> T const*{  return impl_t::data();  }
		auto data() const-> T const*{  return cdata();  }
		auto data()-> T*{  return impl_t::data();  }

		auto front() const		SGM_DECLTYPE_AUTO(  *cbegin()  )
		auto front()			SGM_DECLTYPE_AUTO(  *begin()  )
		auto back() const		SGM_DECLTYPE_AUTO(  *crbegin()  )
		auto back()			SGM_DECLTYPE_AUTO(  *rbegin()  )

		auto operator[](ixSize_t idx) const	SGM_DECLTYPE_AUTO(  at(idx)  )
		auto operator[](ixSize_t idx)		SGM_DECLTYPE_AUTO(  at(idx)  )



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


	#ifndef _WHEN_DYNAMIC
		#define _WHEN_DYNAMIC class = std::enable_if_t<SIZE == ixSize::DYNAMIC>


		template<_WHEN_DYNAMIC>
		auto capacity() const-> ixSize_t{  return impl_t::capacity();  }


		template<class...ARGS, _WHEN_DYNAMIC>
		auto emplace_back(ARGS&&...args)-> indexable&
		{
			impl_t::emplace_back( std::forward<ARGS>(args)... );

			return *this;
		}


		template<_WHEN_DYNAMIC>
		auto pop_back(ixSize_t n = 1)-> indexable&
		{
			return pop_back_from(end() - n);
		}


		template
		<	class ITR
		,	class = std::enable_if_t< is_iterator<ITR>::value && SIZE == ixSize::DYNAMIC >
		>
		auto pop_back_from(ITR itr)-> indexable&
		{
			impl_t::pop_back_from(itr);

			return *this;
		}


		template<_WHEN_DYNAMIC>
		auto clear()-> indexable&
		{
			impl_t::clear();

			return *this;
		}


		template<class Q, _WHEN_DYNAMIC>
		auto operator>>(Q&& q)-> indexable&{  return emplace_back( std::forward<Q>(q) );  }


		#undef _WHEN_DYNAMIC
	#else
		#error _WHEN_DYNAMIC was already defined somewhere else.
	#endif

	};// end of class indexable


}// end of namespace sgm


#endif// end of #ifndef _SGM_INDEXABLE_INTERFACE_