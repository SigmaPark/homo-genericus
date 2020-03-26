#pragma once

#ifndef _SGM_AVATAR_
#define _SGM_AVATAR_

#include <cassert>
#include "..\Type_Analysis\Type_Analysis.hpp"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	SGM_DECL_PROXY_TEMPLATE_CLASS(Avatar);


	template<class T>
	class Avatar_t< T, invar, Avatar_T_Helper<T, invar, false, false, false> >
	{
	protected:
		T const* _cpval;
		enum class State { YET, OWNING, GONE } _state;


	public:
		using value_t = T;

		bool is_yet() const	{  return _state == State::YET;  }
		bool is_owning() const	{  return _state == State::OWNING;  }
		bool has_gone() const	{  return _state == State::GONE;  }

		Avatar_t() : _cpval(nullptr), _state(State::YET){}
		Avatar_t(T const& t) : _cpval(&t), _state(State::OWNING){}
		Avatar_t(T&&) = delete;

		~Avatar_t(){  _cpval = nullptr, _state = State::GONE;  }


		auto operator=(Avatar_t)-> Avatar_t = delete;
		auto operator=(T)-> Avatar_t = delete;


		auto operator()(Avatar_t avt)-> Avatar_t
		{
			_cpval = avt._cpval, _state = avt._state;  

			return *this;
		}

		auto operator()(T const& t)-> Avatar_t{  _cpval = &t;  return *this;  }
		void operator()(T&&) = delete;


		auto value() const-> T const&	{  return *this;  }
		operator T const&() const		{  return value();  }


		template<class Q>
		bool operator==(Q&& q) const{  return *_cpval == std::forward<Q>(q);  }

		template<class Q>
		bool operator!=(Q&& q) const{  return !( *this == std::forward<Q>(q) );  }

		bool operator==(Avatar_t avt) const
		{  
			assert(!has_gone() && !avt.has_gone() && L"Avatar_t was released already");

			return *this == *avt._cpval;  
		}

		bool operator!=(Avatar_t avt) const{  return !(*this == avt);  }
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T>
	class Avatar_t< T, Var, Avatar_T_Helper<T, Var, false, false, false> >
	:	public Avatar_t< T, invar, Avatar_T_Helper<T, invar, false, false, false> >
	{
	private:
		T* _pval;
		

		using const_Avatar_t = constAvatar<T>;

		static auto _avt_state(Avatar_t avt)-> typename const_Avatar_t::State
		{
			return 
			avt.is_owning() 
			?	const_Avatar_t::State::OWNING
			:	
			avt.is_yet()
			?	const_Avatar_t::State::YET
			:	const_Avatar_t::State::GONE;
		}


	public:	
		Avatar_t() : const_Avatar_t(), _pval(nullptr){}
		Avatar_t(T const& t) : const_Avatar_t(t), _pval(nullptr){}
		Avatar_t(T& t) : const_Avatar_t(t), _pval(&t){}
		Avatar_t(T&&) = delete;

		~Avatar_t(){  _pval = nullptr;  }

		template
		<	class Q
		,	class 
			=	std::enable_if_t
				<	std::is_convertible<Q, T>::value 
				&&	!std::is_same< std::decay_t<Q>, Avatar_t >::value
				>
		>
		auto operator=(Q&& q)-> Avatar_t
		{  
			assert(is_owning() && L"Avatar_t has nothing");

			*_pval = std::forward<Q>(q);

			return *this; 
		}

		auto operator=(const_Avatar_t cavt)-> Avatar_t{  return *this = cavt.value();  }


		auto operator()(Avatar_t avt)-> Avatar_t
		{
			_cpval = _pval = avt._pval,
			_state = _avt_state(avt);
			
			return *this;
		}

		auto operator()(T& t)-> Avatar_t
		{
			_cpval = _pval = &t, 
			_state = State::OWNING;

			return *this;
		}


		auto value()-> T&{  return *_pval;  }
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
	

	template<class T> 
	static auto make_Avatar(T&& t)-> Avatar< std::remove_reference_t<T> >
	{
		static_assert(!is_Avatar<T>::value, "no nested Avatar_t");

		return Avatar< std::remove_reference_t<T> >( std::forward<T>(t) );
	}


} // end of namespace sgm

#endif // end of #ifndef _SGM_AVATAR_