#pragma once

#include "indexable_interface.hpp"
#include <vector>
#include <array>
#include <cassert>
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#


namespace sgm
{


#ifndef _IX_ITERATOR_METHOD
	#define _IX_ITERATOR_METHOD(C1, R1, C2, R2)	\
		template<class T, ixSize_t S>	\
		struct ix_##C1##R1##iterator : No_Making	\
		{	\
			using type = typename std::array<T, S>::C2##R2##iterator;	\
		};	\
		\
		template<class T>	\
		struct ix_##C1##R1##iterator<T, ixSize::DYNAMIC> : No_Making	\
		{	\
			using type = typename std::vector<T>::C2##R2##iterator;		\
		}


		_IX_ITERATOR_METHOD(,,,);
		_IX_ITERATOR_METHOD(const_,,const_,);
		_IX_ITERATOR_METHOD(,r,,reverse_);
		_IX_ITERATOR_METHOD(const_,r,const_,reverse_);


	#undef _IX_ITERATOR_METHOD
#else
	#error _IX_ITERATOR_METHOD was already defined somewhere else.
#endif
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T, ixSize_t SIZE>
	class indexable_impl
	{
		using core_t 
		=	std::conditional_t
			<	SIZE == ixSize::DYNAMIC, std::vector<T>, std::array<T, SIZE>
			>;

		core_t _core;


	protected:
		indexable_impl() = default;
		indexable_impl(indexable_impl const&) = default;
		indexable_impl(indexable_impl&& ix_impl) : _core( std::move(ix_impl._core) ){}


		template<  class ITR, class = std::enable_if_t< is_iterator<ITR>::value >  >
		indexable_impl(ITR bi, ITR ei) 
		:	_core( _init<SIZE, ITR>::calc(bi, ei) )
		{}


		template
		<	class S
		,	class = std::enable_if_t< std::is_integral<S>::value && SIZE == ixSize::DYNAMIC >
		>
		indexable_impl(S capa) 
		:	_core
			(	[capa]()-> core_t
				{
					core_t res;

					res.reserve(capa);

					return res;
				}()
			)
		{}


		template
		<	class S, class...ARGS
		,	class 
			=	std::enable_if_t
				<	std::is_integral<S>::value && SIZE == ixSize::DYNAMIC 
				&&	sizeof...(ARGS) != 0
				>
		>
		indexable_impl(S size, ARGS const&...args) : _core( size, T(args...) ){}


		auto operator=(indexable_impl const&)-> indexable_impl& = default;

		auto operator=(indexable_impl&& ix_impl)-> indexable_impl&
		{
			_core = std::move(ix_impl._core);

			return *this;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto size() const-> ixSize_t{  return _core.size();  }
		auto empty() const-> bool{  return _core.empty();  }

		auto data() const-> T const*	{  return _core.data();  }
		auto data()-> T*				{  return _core.data();  }

		auto at(ixSize_t idx) const-> T const&	{  return _core.at(idx);  }
		auto at(ixSize_t idx)-> T&				{  return _core.at(idx);  } 
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------
		
		
		auto begin() const		SGM_DECLTYPE_AUTO(  _core.begin()  )
		auto begin()			SGM_DECLTYPE_AUTO(  _core.begin()  )
		auto cbegin() const	SGM_DECLTYPE_AUTO(  _core.cbegin()  )

		auto end() const		SGM_DECLTYPE_AUTO(  _core.end()  )
		auto end()			SGM_DECLTYPE_AUTO(  _core.end()  )
		auto cend() const		SGM_DECLTYPE_AUTO(  _core.cend()  )

		auto rbegin() const	SGM_DECLTYPE_AUTO(  _core.rbegin()  )
		auto rbegin()			SGM_DECLTYPE_AUTO(  _core.rbegin()  )
		auto crbegin() const	SGM_DECLTYPE_AUTO(  _core.crbegin()  )

		auto rend() const		SGM_DECLTYPE_AUTO(  _core.rend()  )
		auto rend()			SGM_DECLTYPE_AUTO(  _core.rend()  )
		auto crend() const		SGM_DECLTYPE_AUTO(  _core.crend()  )
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template< class = std::enable_if_t<SIZE == ixSize::DYNAMIC> >
		auto capacity() const-> ixSize_t{  return _core.capacity();  }


		template< class...ARGS, class = std::enable_if_t<SIZE == ixSize::DYNAMIC> >
		void emplace_back(ARGS&&...args)
		{  
			_core.emplace_back( std::forward<ARGS>(args)... );  
		}


		template
		<	class ITR
		,	class = std::enable_if_t< is_iterator<ITR>::value && SIZE == ixSize::DYNAMIC >
		>
		void pop_back_from(ITR itr){  _core.erase(itr, end());  }


		template< class = std::enable_if_t<SIZE == ixSize::DYNAMIC> >
		void clear(){  _core.clear();  }


	private:
		template
		<	ixSize_t S, class ITR
		,	bool 
			=	std::is_same
				<	std::decay_t< decltype(*Declval<ITR>()) >, std::decay_t<T>
				>::value
		>
		struct _init
		{
			static auto calc(ITR bi, ITR ei)-> core_t
			{
				assert( S == std::distance(bi, ei) && L"array size dismatched.\n" );

				core_t res;

				for
				(	auto itr = res.begin()
				;	bi != ei
				;	*itr++ = static_cast<T>(*bi++)
				);

				return res;
			}
		};

		template<class ITR>
		struct _init<ixSize::DYNAMIC, ITR, false>
		{
			static auto calc(ITR bi, ITR ei)-> core_t
			{
				core_t res;
				
				for
				(	res.reserve( std::distance(bi, ei) )
				;	bi != ei
				;	res.emplace_back( static_cast<T>(*bi++) )
				);

				return res;
			}			
		};

		template<class ITR>
		struct _init<ixSize::DYNAMIC, ITR, true>
		{
			static auto calc(ITR bi, ITR ei)-> core_t{  return core_t(bi, ei);  }
		};


	};// end of class indexable_impl


}// end of namespace sgm