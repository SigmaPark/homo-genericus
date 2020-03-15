#pragma once

#ifndef _SGM_AVATAR_
#define _SGM_AVATAR_

#include <cassert>
#include "..\Type_Analysis\Type_Analysis.hpp"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{
	

	//template<class> class is_Avatar;

	//template
	//<	class T, class NoRef
	//,	class = std::conditional_t< is_Avatar<NoRef>::value, std::true_type, std::false_type >
	//>
	//class Avatar;


	template<class T>
	class Avatar
	{
		enum class State{ YET, OWNING, GONE };


	public:
		//using value_t = NoRef; 

		bool is_yet() const	{  return _state == State::YET;  }
		bool is_owning() const	{  return _state == State::OWNING;  }
		bool has_gone() const	{  return _state == State::GONE;  }


		Avatar() : _pval(nullptr), _state(State::YET){}
		Avatar(T& t) : _pval(&t), _state(State::OWNING){}

		~Avatar(){  _pval = nullptr, _state = State::GONE;  }
		
		
		auto operator()(T& t)-> Avatar&{  return *this = Avatar(t);  }
		

		template
		<	class Q
		,	class 
			=	std::enable_if_t
				<	std::is_convertible<Q, T>::value 
				&&	!std::is_same< std::decay_t<Q>, Avatar >::value
				>
		>
		auto operator=(Q&& q)-> Avatar&	
		{  
			assert(is_owning() && L"Avatar has nothing");

			*_pval = std::forward<Q>(q);

			return *this;  
		}

		auto operator=(Avatar avt)-> Avatar&{  return *this = avt.value();  }


		operator T const&() const				{  return value();  }
		auto value()-> T&				{  return _value_impl(*this);  }
		auto value() const-> T const&	{  return _value_impl(*this);  }


		bool operator==(T const& t) const	{  return *_pval == t;  }
		bool operator==(T&& t) const		{  return *_pval == std::move(t);  }
		bool operator!=(T const& t) const	{  return !(*this == t);  }
		bool operator!=(T&& t) const		{  return !( *this == std::move(t) );  }

		bool operator==(Avatar avt) const
		{  
			assert(!has_gone() && !avt.has_gone() && L"Avatar was released already");

			return *this == *avt._pval;  
		}

		bool operator!=(Avatar avt) const{  return !(*this == avt);  }


	private:
		T* _pval;
		State _state;


		template<class AVT>
		static auto _value_impl(AVT& avt)-> decltype(avt.value())
		{
			assert(avt.is_owning() && L"Avatar has nothing");
				
			return *avt._pval;
		}

	}; // end of class Avatar<T>



} // end of namespace sgm

#endif // end of #ifndef _SGM_AVATAR_


#if 0

template<class T, ...ARGS> 
class Avatar : public DirectProxy<Avatar>
{
//...
	using Self_t = typename DirectProxy<Avatar>::template Proxy<T, ARGS...>;
	using value_t = typename DirectProxy<Avatar>::template Raw<T, ARGS...>;
};

//...

Avatar<  Avatar< Avatar<int> >  > nested1;

static_assert( std::is_same< decltype(nested1), Avatar<int> >::value );

Avatar< Avatar< Avatar<int>&& > const& > nested2;

static_assert( std::is_same< decltype(nested2), Avatar<int> const
>::value );


#endif